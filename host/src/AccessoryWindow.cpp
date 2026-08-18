#include "AccessoryWindow.h"

#include "LangCatalog.h"
#include "ui_RibbonEditor.h"

#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QDir>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLayoutItem>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVariant>
#include <QVBoxLayout>
#include <QWidget>

namespace
{
QSpinBox *memorySpin(QWidget *parent, const QString &name, int value)
{
    auto *spin = new QSpinBox(parent);
    spin->setObjectName(name);
    spin->setRange(0, 255);
    spin->setValue(value);
    return spin;
}

int spinValue(const QObject *root, const QString &name)
{
    auto *spin = root->findChild<QSpinBox *>(name);
    return spin == nullptr ? 0 : spin->value();
}
}

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
    if (_ui->FLP_Ribbons->layout() == nullptr)
        new QVBoxLayout(_ui->FLP_Ribbons);
    connect(_ui->B_Save, &QPushButton::clicked, this, &QDialog::accept);
    connect(_ui->B_Cancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(_ui->B_All, &QPushButton::clicked, this, [this] {
        emit modifyRequested(QStringLiteral("B_All"));
        if (_closeOnBulk)
            accept();
    });
    connect(_ui->B_None, &QPushButton::clicked, this, [this] {
        const QString page = property("accessoryRoot").toJsonObject().value(QStringLiteral("page")).toString();
        if (page == QLatin1String("memories"))
            emit modifyRequested(QStringLiteral("B_ClearAll"));
        else
        {
            emit modifyRequested(QStringLiteral("B_None"));
            if (_closeOnBulk)
                accept();
        }
    });
    LangCatalog catalog;
    catalog.loadFromFile(QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("lang/lang_en.txt")));
    catalog.apply(this, QStringLiteral("RibbonEditor"));
}

AccessoryWindow::~AccessoryWindow() = default;

void AccessoryWindow::loadDocument(const QString &json)
{
    const auto root = QJsonDocument::fromJson(json.toUtf8()).object();
    const QString page = root.value(QStringLiteral("page")).toString();
    const QString kind = root.value(QStringLiteral("kind")).toString();
    _closeOnBulk = page == QLatin1String("tech") || page == QLatin1String("shop") || page == QLatin1String("plus");
    LangCatalog catalog;
    catalog.loadFromFile(QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("lang/lang_en.txt")));
    catalog.apply(this, root.value(QStringLiteral("langForm")).toString(QStringLiteral("RibbonEditor")));
    if (kind == QLatin1String("memories"))
        fillMemories(root);
    else if (root.contains(QStringLiteral("rows")))
        fillFlags(root);
    else
        fillRibbons(root);
    setProperty("accessoryRoot", QVariant::fromValue(root));
}

void AccessoryWindow::clearSidePanel()
{
    auto *layout = _ui->FLP_Ribbons->layout();
    if (layout == nullptr)
        return;
    while (layout->count() > 0)
    {
        QLayoutItem *item = layout->takeAt(0);
        if (item->widget() != nullptr)
            item->widget()->deleteLater();
        delete item;
    }
}

void AccessoryWindow::fillRibbons(const QJsonObject &root)
{
    clearSidePanel();
    _ui->TLP_Ribbons->setVisible(true);
    _ui->B_All->setVisible(true);
    _ui->B_None->setVisible(true);
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

void AccessoryWindow::fillFlags(const QJsonObject &root)
{
    clearSidePanel();
    _ui->CB_Affixed->setVisible(false);
    _ui->B_All->setVisible(true);
    _ui->B_None->setVisible(true);
    _ui->TLP_Ribbons->setVisible(true);
    const bool pair = root.value(QStringLiteral("kind")).toString() == QLatin1String("pair");
    auto *table = _ui->TLP_Ribbons;
    const auto rows = root.value(QStringLiteral("rows")).toArray();
    table->setRowCount(rows.size());
    for (int i = 0; i < rows.size(); ++i)
    {
        const auto row = rows.at(i).toObject();
        if (pair)
        {
            auto *cell = new QWidget(table);
            auto *lay = new QHBoxLayout(cell);
            lay->setContentsMargins(0, 0, 0, 0);
            auto *purchased = new QCheckBox(cell);
            purchased->setObjectName(QStringLiteral("CHK_Shop0_") + QString::number(row.value(QStringLiteral("index")).toInt()));
            purchased->setChecked(row.value(QStringLiteral("flag0")).toBool());
            auto *mastered = new QCheckBox(cell);
            mastered->setObjectName(QStringLiteral("CHK_Shop1_") + QString::number(row.value(QStringLiteral("index")).toInt()));
            mastered->setChecked(row.value(QStringLiteral("flag1")).toBool());
            lay->addWidget(purchased);
            lay->addWidget(mastered);
            table->setCellWidget(i, 0, cell);
        }
        else
        {
            auto *check = new QCheckBox(table);
            check->setObjectName(QStringLiteral("CHK_Flag_") + QString::number(row.value(QStringLiteral("index")).toInt()));
            check->setChecked(row.value(QStringLiteral("flag0")).toBool());
            table->setCellWidget(i, 0, check);
        }
        auto *label = new QTableWidgetItem(row.value(QStringLiteral("label")).toString());
        label->setFlags(label->flags() & ~Qt::ItemIsEditable);
        label->setData(Qt::UserRole, row.value(QStringLiteral("index")).toInt());
        label->setData(Qt::UserRole + 1, pair ? QStringLiteral("pair") : QStringLiteral("flags"));
        table->setItem(i, 1, label);
    }

    if (!root.value(QStringLiteral("hasBag")).toBool() && root.value(QStringLiteral("page")).toString() != QLatin1String("medals"))
        return;
    auto *side = _ui->FLP_Ribbons;
    auto *unlocked = new QCheckBox(QStringLiteral("Secret Unlocked"), side);
    unlocked->setObjectName(QStringLiteral("CHK_SecretUnlocked"));
    unlocked->setChecked(root.value(QStringLiteral("secretUnlocked")).toBool());
    auto *complete = new QCheckBox(QStringLiteral("Secret Complete"), side);
    complete->setObjectName(QStringLiteral("CHK_SecretComplete"));
    complete->setChecked(root.value(QStringLiteral("secretComplete")).toBool());
    side->layout()->addWidget(unlocked);
    side->layout()->addWidget(complete);
    if (!root.value(QStringLiteral("hasBag")).toBool())
        return;
    auto *bag = new QSpinBox(side);
    bag->setObjectName(QStringLiteral("CB_Bag"));
    bag->setRange(0, 255);
    bag->setValue(root.value(QStringLiteral("bag")).toInt());
    auto *hits = new QSpinBox(side);
    hits->setObjectName(QStringLiteral("NUD_BagHits"));
    hits->setRange(0, 255);
    hits->setValue(root.value(QStringLiteral("hits")).toInt());
    side->layout()->addWidget(bag);
    side->layout()->addWidget(hits);
}

void AccessoryWindow::fillMemories(const QJsonObject &root)
{
    clearSidePanel();
    _ui->TLP_Ribbons->setRowCount(0);
    _ui->TLP_Ribbons->setVisible(false);
    _ui->CB_Affixed->setVisible(false);
    _ui->B_All->setVisible(false);
    _ui->B_None->setVisible(true);
    auto *side = _ui->FLP_Ribbons;
    auto *formHost = new QWidget(side);
    auto *form = new QFormLayout(formHost);
    form->addRow(QStringLiteral("OT Friendship"), memorySpin(formHost, QStringLiteral("M_OT_Friendship"), root.value(QStringLiteral("otFriendship")).toInt()));
    form->addRow(QStringLiteral("HT Friendship"), memorySpin(formHost, QStringLiteral("M_CT_Friendship"), root.value(QStringLiteral("htFriendship")).toInt()));
    form->addRow(QStringLiteral("OT Affection"), memorySpin(formHost, QStringLiteral("M_OT_Affection"), root.value(QStringLiteral("otAffection")).toInt()));
    form->addRow(QStringLiteral("HT Affection"), memorySpin(formHost, QStringLiteral("M_CT_Affection"), root.value(QStringLiteral("htAffection")).toInt()));
    form->addRow(QStringLiteral("Fullness"), memorySpin(formHost, QStringLiteral("M_Fullness"), root.value(QStringLiteral("fullness")).toInt()));
    form->addRow(QStringLiteral("Enjoyment"), memorySpin(formHost, QStringLiteral("M_Enjoyment"), root.value(QStringLiteral("enjoyment")).toInt()));
    side->layout()->addWidget(formHost);
}

QJsonObject AccessoryWindow::collectDocument() const
{
    QJsonObject root = property("accessoryRoot").toJsonObject();
    const QString kind = root.value(QStringLiteral("kind")).toString();
    if (kind == QLatin1String("memories"))
    {
        root.insert(QStringLiteral("otFriendship"), spinValue(this, QStringLiteral("M_OT_Friendship")));
        root.insert(QStringLiteral("htFriendship"), spinValue(this, QStringLiteral("M_CT_Friendship")));
        root.insert(QStringLiteral("otAffection"), spinValue(this, QStringLiteral("M_OT_Affection")));
        root.insert(QStringLiteral("htAffection"), spinValue(this, QStringLiteral("M_CT_Affection")));
        root.insert(QStringLiteral("fullness"), spinValue(this, QStringLiteral("M_Fullness")));
        root.insert(QStringLiteral("enjoyment"), spinValue(this, QStringLiteral("M_Enjoyment")));
        return root;
    }
    if (kind == QLatin1String("flags") || kind == QLatin1String("pair"))
    {
        QJsonArray rows;
        auto *table = _ui->TLP_Ribbons;
        for (int i = 0; i < table->rowCount(); ++i)
        {
            const auto *label = table->item(i, 1);
            if (label == nullptr)
                continue;
            QJsonObject row;
            row.insert(QStringLiteral("index"), label->data(Qt::UserRole).toInt());
            if (kind == QLatin1String("pair"))
            {
                auto *cell = table->cellWidget(i, 0);
                const auto checks = cell == nullptr ? QList<QCheckBox *>() : cell->findChildren<QCheckBox *>();
                row.insert(QStringLiteral("flag0"), checks.size() > 0 && checks.at(0)->isChecked());
                row.insert(QStringLiteral("flag1"), checks.size() > 1 && checks.at(1)->isChecked());
            }
            else
            {
                auto *check = qobject_cast<QCheckBox *>(table->cellWidget(i, 0));
                row.insert(QStringLiteral("flag0"), check != nullptr && check->isChecked());
            }
            rows.append(row);
        }
        root.insert(QStringLiteral("rows"), rows);
        if (auto *unlocked = findChild<QCheckBox *>(QStringLiteral("CHK_SecretUnlocked")))
            root.insert(QStringLiteral("secretUnlocked"), unlocked->isChecked());
        if (auto *complete = findChild<QCheckBox *>(QStringLiteral("CHK_SecretComplete")))
            root.insert(QStringLiteral("secretComplete"), complete->isChecked());
        if (auto *bag = findChild<QSpinBox *>(QStringLiteral("CB_Bag")))
            root.insert(QStringLiteral("bag"), bag->value());
        if (auto *hits = findChild<QSpinBox *>(QStringLiteral("NUD_BagHits")))
            root.insert(QStringLiteral("hits"), hits->value());
        return root;
    }

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
