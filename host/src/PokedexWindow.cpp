#include "PokedexWindow.h"

#include "LangCatalog.h"
#include "ComboChrome.h"
#include "ui_SAV_Pokedex.h"

#include <QAction>
#include <QCoreApplication>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QListWidget>
#include <QMenu>
#include <QPushButton>
#include <QVariant>

PokedexWindow::PokedexWindow(QWidget *parent)
    : QDialog(parent)
    , _ui(std::make_unique<Ui::SAV_Pokedex5>())
{
    _ui->setupUi(this);
    configureComboBoxes(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    buildMenus();
    connect(_ui->B_Save, &QPushButton::clicked, this, &QDialog::accept);
    connect(_ui->B_Cancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(_ui->B_GiveAll, &QPushButton::clicked, this, [this] { emit modifyRequested(QStringLiteral("B_GiveAll")); });
    LangCatalog catalog;
    catalog.loadFromFile(QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("lang/lang_en.txt")));
    catalog.apply(this, QStringLiteral("SAV_Pokedex5"));
}

PokedexWindow::~PokedexWindow() = default;

void PokedexWindow::buildMenus()
{
    auto *menu = new QMenu(this);
    menu->setObjectName(QStringLiteral("modifyMenu"));
    const auto add = [&](const char *name, const char *label) {
        auto *action = menu->addAction(QString::fromLatin1(label));
        action->setObjectName(QString::fromLatin1(name));
        connect(action, &QAction::triggered, this, [this, name] { emit modifyRequested(QString::fromLatin1(name)); });
    };
    add("mnuSeenNone", "Seen none");
    add("mnuSeenAll", "Seen all");
    add("mnuCaughtNone", "Caught none");
    add("mnuCaughtAll", "Caught all");
    add("mnuComplete", "Complete Dex");
    _ui->B_Modify->setMenu(menu);
}

void PokedexWindow::loadDocument(const QString &json)
{
    const auto root = QJsonDocument::fromJson(json.toUtf8()).object();
    _langForm = root.value(QStringLiteral("langForm")).toString(QStringLiteral("SAV_Pokedex5"));
    _ui->LB_Species->clear();
    _ui->CB_Species->clear();
    const auto names = root.value(QStringLiteral("speciesNames")).toArray();
    for (const auto &name : names)
    {
        const QString text = name.toString();
        _ui->LB_Species->addItem(text);
        _ui->CB_Species->addItem(text);
    }
    const int species = root.value(QStringLiteral("species")).toInt(1);
    if (species > 0 && species <= _ui->LB_Species->count())
        _ui->LB_Species->setCurrentRow(species - 1);
    _ui->CHK_P1->setChecked(root.value(QStringLiteral("caught")).toBool());
    _ui->CHK_NationalDexUnlocked->setChecked(root.value(QStringLiteral("nationalUnlocked")).toBool());
    _ui->CHK_NationalDexActive->setChecked(root.value(QStringLiteral("nationalActive")).toBool());
    _ui->TB_PID->setText(root.value(QStringLiteral("spinda")).toString());
    setProperty("pokedexRoot", QVariant::fromValue(root));
    applyCapabilities();
    LangCatalog catalog;
    catalog.loadFromFile(QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("lang/lang_en.txt")));
    catalog.apply(this, _langForm);
}

void PokedexWindow::applyCapabilities()
{
    const auto root = property("pokedexRoot").toJsonObject();
    const bool national = root.value(QStringLiteral("hasNationalDex")).toBool();
    const bool spinda = root.value(QStringLiteral("hasSpinda")).toBool();
    const int langs = root.value(QStringLiteral("languageCount")).toInt(7);
    _ui->CHK_NationalDexUnlocked->setVisible(national);
    _ui->CHK_NationalDexActive->setVisible(national && _langForm != QStringLiteral("SAV_PokedexBDSP"));
    _ui->L_Spinda->setVisible(spinda);
    _ui->TB_PID->setVisible(spinda);
    _ui->CHK_L8->setVisible(langs >= 8);
    _ui->CHK_L9->setVisible(langs >= 9);
}

QString PokedexWindow::document() const
{
    QJsonObject root = property("pokedexRoot").toJsonObject();
    root.insert(QStringLiteral("species"), _ui->LB_Species->currentRow() + 1);
    root.insert(QStringLiteral("caught"), _ui->CHK_P1->isChecked());
    root.insert(QStringLiteral("nationalUnlocked"), _ui->CHK_NationalDexUnlocked->isChecked());
    root.insert(QStringLiteral("nationalActive"), _ui->CHK_NationalDexActive->isChecked());
    root.insert(QStringLiteral("spinda"), _ui->TB_PID->text());
    return QString::fromUtf8(QJsonDocument(root).toJson(QJsonDocument::Compact));
}
