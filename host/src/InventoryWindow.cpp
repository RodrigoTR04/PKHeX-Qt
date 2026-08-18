#include "InventoryWindow.h"

#include "LangCatalog.h"
#include "ComboChrome.h"
#include "ui_SAV_Inventory.h"

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QDir>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTabWidget>
#include <QVariant>
#include <QVBoxLayout>

namespace
{
constexpr int kColItem = 0;
constexpr int kColCount = 1;

QString sampleInventory()
{
    return QStringLiteral(
        "{\"itemColumnReadOnly\":false,\"hasFavorite\":false,\"hasNew\":false,"
        "\"hasFreeSpace\":false,\"hasFreeSpaceIndex\":false,\"hasNewShop\":false,"
        "\"hasHeld\":false,\"currentPouch\":0,\"giveCount\":995,"
        "\"pouches\":[{\"type\":\"Medicine\",\"maxCount\":999,\"giveDisabled\":false,"
        "\"choices\":[\"None\",\"Potion\"],"
        "\"rows\":[{\"itemId\":0,\"item\":\"None\",\"count\":0}]}]}");
}

int addFlagColumns(QTableWidget *table, const QJsonObject &root)
{
    int columns = 2;
    if (root.value(QStringLiteral("hasFavorite")).toBool())
        ++columns;
    if (root.value(QStringLiteral("hasNew")).toBool())
        ++columns;
    if (root.value(QStringLiteral("hasFreeSpace")).toBool())
        ++columns;
    if (root.value(QStringLiteral("hasFreeSpaceIndex")).toBool())
        ++columns;
    if (root.value(QStringLiteral("hasNewShop")).toBool())
        ++columns;
    if (root.value(QStringLiteral("hasHeld")).toBool())
        ++columns;
    table->setColumnCount(columns);
    QStringList headers{QStringLiteral("Item"), QStringLiteral("Count")};
    if (root.value(QStringLiteral("hasFavorite")).toBool())
        headers << QStringLiteral("Fav");
    if (root.value(QStringLiteral("hasNew")).toBool())
        headers << QStringLiteral("New");
    if (root.value(QStringLiteral("hasFreeSpace")).toBool())
        headers << QStringLiteral("Free");
    if (root.value(QStringLiteral("hasFreeSpaceIndex")).toBool())
        headers << QStringLiteral("Free");
    if (root.value(QStringLiteral("hasNewShop")).toBool())
        headers << QStringLiteral("Shop");
    if (root.value(QStringLiteral("hasHeld")).toBool())
        headers << QStringLiteral("Held");
    table->setHorizontalHeaderLabels(headers);
    return columns;
}

QJsonObject rowObject(QTableWidget *table, int row, const QJsonObject &root)
{
    QJsonObject out;
    QString itemName;
    int itemId = 0;
    if (auto *combo = qobject_cast<QComboBox *>(table->cellWidget(row, kColItem)))
        itemName = combo->currentText();
    else if (auto *cell = table->item(row, kColItem))
    {
        itemName = cell->text();
        itemId = cell->data(Qt::UserRole).toInt();
    }
    int count = 0;
    if (auto *spin = qobject_cast<QSpinBox *>(table->cellWidget(row, kColCount)))
        count = spin->value();
    out.insert(QStringLiteral("item"), itemName);
    out.insert(QStringLiteral("itemId"), itemId);
    out.insert(QStringLiteral("count"), count);

    int col = 2;
    if (root.value(QStringLiteral("hasFavorite")).toBool())
    {
        bool on = false;
        if (auto *box = qobject_cast<QCheckBox *>(table->cellWidget(row, col++)))
            on = box->isChecked();
        out.insert(QStringLiteral("favorite"), on);
    }
    if (root.value(QStringLiteral("hasNew")).toBool())
    {
        bool on = false;
        if (auto *box = qobject_cast<QCheckBox *>(table->cellWidget(row, col++)))
            on = box->isChecked();
        out.insert(QStringLiteral("isNew"), on);
    }
    if (root.value(QStringLiteral("hasFreeSpace")).toBool())
    {
        bool on = false;
        if (auto *box = qobject_cast<QCheckBox *>(table->cellWidget(row, col++)))
            on = box->isChecked();
        out.insert(QStringLiteral("freeSpace"), on);
    }
    if (root.value(QStringLiteral("hasFreeSpaceIndex")).toBool())
    {
        uint value = 0;
        if (auto *spin = qobject_cast<QSpinBox *>(table->cellWidget(row, col++)))
            value = static_cast<uint>(spin->value());
        out.insert(QStringLiteral("freeSpaceIndex"), static_cast<int>(value));
    }
    if (root.value(QStringLiteral("hasNewShop")).toBool())
    {
        bool on = false;
        if (auto *box = qobject_cast<QCheckBox *>(table->cellWidget(row, col++)))
            on = box->isChecked();
        out.insert(QStringLiteral("newShop"), on);
    }
    if (root.value(QStringLiteral("hasHeld")).toBool())
    {
        bool on = false;
        if (auto *box = qobject_cast<QCheckBox *>(table->cellWidget(row, col++)))
            on = box->isChecked();
        out.insert(QStringLiteral("held"), on);
    }
    return out;
}
}

InventoryWindow::InventoryWindow(QWidget *parent)
    : QDialog(parent)
    , _ui(std::make_unique<Ui::SAV_Inventory>())
{
    _ui->setupUi(this);
    configureComboBoxes(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    setWindowFlag(Qt::WindowMaximizeButtonHint, false);
    buildMenus();
    connect(_ui->B_Save, &QPushButton::clicked, this, &QDialog::accept);
    connect(_ui->B_Cancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(_ui->tabControl1, &QTabWidget::currentChanged, this, [this](int) { applyPouchChrome(); });
    LangCatalog catalog;
    catalog.loadFromFile(QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("lang/lang_en.txt")));
    catalog.apply(this, QStringLiteral("SAV_Inventory"));
}

InventoryWindow::~InventoryWindow() = default;

void InventoryWindow::buildMenus()
{
    auto *sortMenu = new QMenu(this);
    sortMenu->setObjectName(QStringLiteral("sortMenu"));
    const auto addSort = [&](const char *name, const char *label) {
        auto *action = sortMenu->addAction(QString::fromLatin1(label));
        action->setObjectName(QString::fromLatin1(name));
        connect(action, &QAction::triggered, this, [this, name] { emit modifyRequested(QString::fromLatin1(name)); });
    };
    addSort("mnuSortName", "Name");
    addSort("mnuSortNameReverse", "Name (Reverse)");
    sortMenu->addSeparator();
    addSort("mnuSortCount", "Count");
    addSort("mnuSortCountReverse", "Count (Reverse)");
    sortMenu->addSeparator();
    addSort("mnuSortIndex", "Index");
    addSort("mnuSortIndexReverse", "Index (Reverse)");
    _ui->B_Sort->setMenu(sortMenu);

    auto *giveMenu = new QMenu(this);
    giveMenu->setObjectName(QStringLiteral("giveMenu"));
    const auto addGive = [&](const char *name, const char *label) {
        auto *action = giveMenu->addAction(QString::fromLatin1(label));
        action->setObjectName(QString::fromLatin1(name));
        connect(action, &QAction::triggered, this, [this, name] { emit modifyRequested(QString::fromLatin1(name)); });
    };
    addGive("giveAll", "All");
    addGive("giveNone", "None");
    addGive("giveModify", "Modify");
    _ui->B_GiveAll->setMenu(giveMenu);
}

void InventoryWindow::loadDocument(const QString &json)
{
    const auto parsed = QJsonDocument::fromJson(json.toUtf8());
    const QJsonObject root = parsed.isObject() ? parsed.object() : QJsonDocument::fromJson(sampleInventory().toUtf8()).object();
    _ui->NUD_Count->setValue(qMax(1, root.value(QStringLiteral("giveCount")).toInt(1)));
    fillPouches(root);
    const int current = root.value(QStringLiteral("currentPouch")).toInt();
    if (current >= 0 && current < _ui->tabControl1->count())
        _ui->tabControl1->setCurrentIndex(current);
    applyPouchChrome();
    setProperty("inventoryRoot", QVariant::fromValue(root));
}

void InventoryWindow::fillPouches(const QJsonObject &root)
{
    auto *tabs = _ui->tabControl1;
    while (tabs->count() > 0)
    {
        QWidget *page = tabs->widget(0);
        tabs->removeTab(0);
        delete page;
    }

    const bool readOnlyItem = root.value(QStringLiteral("itemColumnReadOnly")).toBool();
    const auto pouches = root.value(QStringLiteral("pouches")).toArray();
    for (const auto &entry : pouches)
    {
        const auto pouch = entry.toObject();
        const QString type = pouch.value(QStringLiteral("type")).toString();
        auto *page = new QWidget(tabs);
        auto *layout = new QVBoxLayout(page);
        layout->setContentsMargins(0, 0, 0, 0);
        auto *table = new QTableWidget(page);
        table->setObjectName(QStringLiteral("DGV_%1").arg(type));
        table->setProperty("pouchType", type);
        table->setProperty("maxCount", pouch.value(QStringLiteral("maxCount")).toInt());
        table->setProperty("giveDisabled", pouch.value(QStringLiteral("giveDisabled")).toBool());
        table->verticalHeader()->setVisible(false);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        addFlagColumns(table, root);
        const auto choices = pouch.value(QStringLiteral("choices")).toArray();
        QStringList choiceList;
        for (const auto &choice : choices)
            choiceList << choice.toString();
        const auto rows = pouch.value(QStringLiteral("rows")).toArray();
        table->setRowCount(rows.size());
        for (int r = 0; r < rows.size(); ++r)
        {
            const auto row = rows.at(r).toObject();
            if (readOnlyItem)
            {
                auto *item = new QTableWidgetItem(row.value(QStringLiteral("item")).toString());
                item->setData(Qt::UserRole, row.value(QStringLiteral("itemId")).toInt());
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                table->setItem(r, kColItem, item);
            }
            else
            {
                auto *combo = new QComboBox(table);
                configureComboBox(combo);
                combo->addItems(choiceList);
                const int at = combo->findText(row.value(QStringLiteral("item")).toString());
                if (at >= 0)
                    combo->setCurrentIndex(at);
                table->setCellWidget(r, kColItem, combo);
            }
            auto *count = new QSpinBox(table);
            count->setMaximum(qMax(1, pouch.value(QStringLiteral("maxCount")).toInt()));
            count->setValue(row.value(QStringLiteral("count")).toInt());
            table->setCellWidget(r, kColCount, count);
            int col = 2;
            auto addCheck = [&](const char *jsonKey) {
                auto *box = new QCheckBox(table);
                box->setChecked(row.value(QLatin1String(jsonKey)).toBool());
                table->setCellWidget(r, col++, box);
            };
            if (root.value(QStringLiteral("hasFavorite")).toBool())
                addCheck("favorite");
            if (root.value(QStringLiteral("hasNew")).toBool())
                addCheck("isNew");
            if (root.value(QStringLiteral("hasFreeSpace")).toBool())
                addCheck("freeSpace");
            if (root.value(QStringLiteral("hasFreeSpaceIndex")).toBool())
            {
                auto *spin = new QSpinBox(table);
                spin->setMaximum(9999);
                spin->setValue(row.value(QStringLiteral("freeSpaceIndex")).toInt());
                table->setCellWidget(r, col++, spin);
            }
            if (root.value(QStringLiteral("hasNewShop")).toBool())
                addCheck("newShop");
            if (root.value(QStringLiteral("hasHeld")).toBool())
                addCheck("held");
        }
        if (readOnlyItem)
            table->sortByColumn(kColItem, Qt::AscendingOrder);
        layout->addWidget(table);
        tabs->addTab(page, type);
        tabs->setTabToolTip(tabs->count() - 1, type);
    }
}

void InventoryWindow::applyPouchChrome()
{
    auto *table = _ui->tabControl1->currentWidget() ? _ui->tabControl1->currentWidget()->findChild<QTableWidget *>() : nullptr;
    const int maxCount = table ? table->property("maxCount").toInt() : 999;
    const bool disable = table && table->property("giveDisabled").toBool();
    _ui->NUD_Count->setMaximum(qMax(1, maxCount));
    _ui->NUD_Count->setVisible(!disable);
    _ui->L_Count->setVisible(!disable);
    _ui->B_GiveAll->setVisible(!disable);
}

QJsonObject InventoryWindow::collectDocument() const
{
    QJsonObject root = property("inventoryRoot").toJsonObject();
    root.insert(QStringLiteral("currentPouch"), _ui->tabControl1->currentIndex());
    root.insert(QStringLiteral("giveCount"), _ui->NUD_Count->value());
    QJsonArray pouches;
    for (int i = 0; i < _ui->tabControl1->count(); ++i)
    {
        auto *page = _ui->tabControl1->widget(i);
        auto *table = page->findChild<QTableWidget *>();
        if (table == nullptr)
            continue;
        QJsonObject pouch;
        pouch.insert(QStringLiteral("type"), table->property("pouchType").toString());
        pouch.insert(QStringLiteral("maxCount"), table->property("maxCount").toInt());
        pouch.insert(QStringLiteral("giveDisabled"), table->property("giveDisabled").toBool());
        QJsonArray rows;
        for (int r = 0; r < table->rowCount(); ++r)
            rows.append(rowObject(table, r, root));
        pouch.insert(QStringLiteral("rows"), rows);
        pouches.append(pouch);
    }
    root.insert(QStringLiteral("pouches"), pouches);
    return root;
}

QString InventoryWindow::document() const
{
    return QString::fromUtf8(QJsonDocument(collectDocument()).toJson(QJsonDocument::Compact));
}
