#include "AccessoryWindow.h"

#include "LangCatalog.h"
#include "ui_RibbonEditor.h"

#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QDir>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVariant>

AccessoryWindow::AccessoryWindow(QWidget *parent)
    : QDialog(parent)
    , _ui(std::make_unique<Ui::RibbonEditor>())
{
    _ui->setupUi(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    _ui->TLP_Ribbons->horizontalHeader()->setVisible(false);
    _ui->TLP_Ribbons->verticalHeader()->setVisible(false);
    _ui->TLP_Ribbons->setShowGrid(false);
    _ui->TLP_Ribbons->setColumnWidth(0, 48);
    _ui->TLP_Ribbons->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    connect(_ui->B_Save, &QPushButton::clicked, this, &QDialog::accept);
    connect(_ui->B_Cancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(_ui->B_All, &QPushButton::clicked, this, [this] { emit modifyRequested(QStringLiteral("B_All")); });
    connect(_ui->B_None, &QPushButton::clicked, this, [this] { emit modifyRequested(QStringLiteral("B_None")); });
    LangCatalog catalog;
    catalog.loadFromFile(QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("lang/lang_en.txt")));
    catalog.apply(this, QStringLiteral("RibbonEditor"));
}

AccessoryWindow::~AccessoryWindow() = default;

void AccessoryWindow::loadDocument(const QString &json)
{
    const auto root = QJsonDocument::fromJson(json.toUtf8()).object();
    fillRibbons(root);
    setProperty("accessoryRoot", QVariant::fromValue(root));
}

void AccessoryWindow::fillRibbons(const QJsonObject &root)
{
    auto *table = _ui->TLP_Ribbons;
    table->setRowCount(0);
    const auto rows = root.value(QStringLiteral("ribbons")).toArray();
    table->setRowCount(rows.size());
    for (int i = 0; i < rows.size(); ++i)
    {
        const auto row = rows.at(i).toObject();
        const QString name = row.value(QStringLiteral("name")).toString();
        const QString type = row.value(QStringLiteral("type")).toString();
        if (type == QLatin1String("byte"))
        {
            auto *spin = new QSpinBox(table);
            spin->setObjectName(QStringLiteral("NUD_") + name);
            spin->setMinimum(0);
            spin->setMaximum(row.value(QStringLiteral("maxCount")).toInt(4));
            spin->setValue(row.value(QStringLiteral("count")).toInt());
            table->setCellWidget(i, 0, spin);
        }
        else
        {
            auto *check = new QCheckBox(table);
            check->setObjectName(QStringLiteral("CHK_") + name);
            check->setChecked(row.value(QStringLiteral("hasRibbon")).toBool());
            table->setCellWidget(i, 0, check);
        }
        auto *label = new QTableWidgetItem(row.value(QStringLiteral("label")).toString());
        label->setFlags(label->flags() & ~Qt::ItemIsEditable);
        label->setData(Qt::UserRole, name);
        label->setData(Qt::UserRole + 1, type);
        table->setItem(i, 1, label);
    }

    const bool hasAffixed = root.value(QStringLiteral("hasAffixed")).toBool();
    _ui->CB_Affixed->setVisible(hasAffixed);
    _ui->CB_Affixed->clear();
    if (!hasAffixed)
        return;
    const auto choices = root.value(QStringLiteral("affixedChoices")).toArray();
    const int affixed = root.value(QStringLiteral("affixed")).toInt(-1);
    int select = 0;
    for (int i = 0; i < choices.size(); ++i)
    {
        const auto choice = choices.at(i).toObject();
        _ui->CB_Affixed->addItem(choice.value(QStringLiteral("text")).toString(), choice.value(QStringLiteral("value")).toInt());
        if (_ui->CB_Affixed->itemData(i).toInt() == affixed)
            select = i;
    }
    _ui->CB_Affixed->setCurrentIndex(select);
}

QJsonObject AccessoryWindow::collectDocument() const
{
    QJsonObject root = property("accessoryRoot").toJsonObject();
    QJsonArray ribbons;
    auto *table = _ui->TLP_Ribbons;
    for (int i = 0; i < table->rowCount(); ++i)
    {
        const auto *label = table->item(i, 1);
        if (label == nullptr)
            continue;
        QJsonObject row;
        row.insert(QStringLiteral("name"), label->data(Qt::UserRole).toString());
        const QString type = label->data(Qt::UserRole + 1).toString();
        row.insert(QStringLiteral("type"), type);
        if (type == QLatin1String("byte"))
        {
            auto *spin = qobject_cast<QSpinBox *>(table->cellWidget(i, 0));
            row.insert(QStringLiteral("count"), spin == nullptr ? 0 : spin->value());
            row.insert(QStringLiteral("hasRibbon"), false);
        }
        else
        {
            auto *check = qobject_cast<QCheckBox *>(table->cellWidget(i, 0));
            row.insert(QStringLiteral("hasRibbon"), check != nullptr && check->isChecked());
            row.insert(QStringLiteral("count"), 0);
        }
        ribbons.append(row);
    }
    root.insert(QStringLiteral("ribbons"), ribbons);
    if (_ui->CB_Affixed->isVisible())
        root.insert(QStringLiteral("affixed"), _ui->CB_Affixed->currentData().toInt());
    return root;
}

QString AccessoryWindow::document() const
{
    return QString::fromUtf8(QJsonDocument(collectDocument()).toJson(QJsonDocument::Compact));
}
