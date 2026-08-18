#include "EncounterDatabaseWindow.h"

#include "LangCatalog.h"
#include "ComboChrome.h"
#include "ui_SAV_Encounters.h"

#include <QAbstractItemView>
#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QDir>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QWidget>

namespace
{
void fillCombo(QComboBox *box, const QString &lines)
{
    box->clear();
    const auto parts = lines.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    for (const auto &line : parts)
    {
        const auto cells = line.split(QLatin1Char('\t'));
        if (cells.size() < 2)
            continue;
        box->addItem(cells.at(1), cells.at(0).toInt());
    }
    if (box->count() == 0)
        box->addItem(QStringLiteral("Any"), 0);
}

int comboValue(const QComboBox *box)
{
    return box->currentData().toInt();
}
}

EncounterDatabaseWindow::EncounterDatabaseWindow(QWidget *parent)
    : QDialog(parent)
    , _ui(std::make_unique<Ui::SAV_Encounters>())
{
    _ui->setupUi(this);
    configureComboBoxes(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    _ui->EncounterPokeGrid->horizontalHeader()->setStretchLastSection(true);
    _ui->EncounterPokeGrid->verticalHeader()->setVisible(false);
    _ui->EncounterPokeGrid->setSelectionBehavior(QAbstractItemView::SelectRows);
    _ui->EncounterPokeGrid->setSelectionMode(QAbstractItemView::SingleSelection);
    _ui->EncounterPokeGrid->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _ui->CHK_Shiny->setTristate(true);
    _ui->CHK_Shiny->setCheckState(Qt::PartiallyChecked);
    _ui->CHK_IsEgg->setTristate(true);
    _ui->CHK_IsEgg->setCheckState(Qt::PartiallyChecked);
    LangCatalog catalog;
    catalog.loadFromFile(QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("lang/lang_en.txt")));
    catalog.apply(this, QStringLiteral("SAV_Encounters"));
    connect(_ui->B_Search, &QPushButton::clicked, this, &EncounterDatabaseWindow::searchRequested);
    connect(_ui->B_Reset, &QPushButton::clicked, this, [this] {
        _ui->CB_Species->setCurrentIndex(0);
        _ui->CB_Move1->setCurrentIndex(0);
        _ui->CB_Move2->setCurrentIndex(0);
        _ui->CB_Move3->setCurrentIndex(0);
        _ui->CB_Move4->setCurrentIndex(0);
        _ui->CB_GameOrigin->setCurrentIndex(0);
        _ui->CHK_Shiny->setCheckState(Qt::PartiallyChecked);
        _ui->CHK_IsEgg->setCheckState(Qt::PartiallyChecked);
        _ui->RTB_Instructions->clear();
        for (auto *box : _ui->TypeFilters->findChildren<QCheckBox *>())
            box->setChecked(true);
    });
    connect(_ui->EncounterPokeGrid, &QTableWidget::itemDoubleClicked, this, [this](QTableWidgetItem *item) {
        if (item == nullptr)
            return;
        emit loadRequested(item->data(Qt::UserRole).toInt());
        _ui->L_Viewed->setText(item->toolTip());
    });
}

EncounterDatabaseWindow::~EncounterDatabaseWindow() = default;

void EncounterDatabaseWindow::loadDocument(const QString &json)
{
    const auto root = QJsonDocument::fromJson(json.toUtf8()).object();
    if (_ui->CB_Species->count() == 0)
    {
        fillCombo(_ui->CB_Species, root.value(QStringLiteral("speciesChoices")).toString());
        fillCombo(_ui->CB_Move1, root.value(QStringLiteral("moveChoices")).toString());
        fillCombo(_ui->CB_Move2, root.value(QStringLiteral("moveChoices")).toString());
        fillCombo(_ui->CB_Move3, root.value(QStringLiteral("moveChoices")).toString());
        fillCombo(_ui->CB_Move4, root.value(QStringLiteral("moveChoices")).toString());
        fillCombo(_ui->CB_GameOrigin, root.value(QStringLiteral("versionChoices")).toString());
    }

    const auto hits = root.value(QStringLiteral("hits")).toArray();
    auto *table = _ui->EncounterPokeGrid;
    table->setRowCount(hits.size());
    for (int row = 0; row < hits.size(); ++row)
    {
        const auto hit = hits.at(row).toObject();
        auto *item = new QTableWidgetItem(hit.value(QStringLiteral("fingerprint")).toString());
        item->setData(Qt::UserRole, hit.value(QStringLiteral("index")).toInt());
        item->setToolTip(hit.value(QStringLiteral("summary")).toString());
        table->setItem(row, 0, item);
    }
    _ui->L_Count->setText(QStringLiteral("Count: %1").arg(root.value(QStringLiteral("count")).toInt()));
}

QString EncounterDatabaseWindow::query() const
{
    QJsonArray types;
    for (auto *box : _ui->TypeFilters->findChildren<QCheckBox *>())
    {
        if (box->isChecked())
            types.append(box->objectName());
    }
    QJsonObject root{
        {QStringLiteral("species"), comboValue(_ui->CB_Species)},
        {QStringLiteral("move1"), comboValue(_ui->CB_Move1)},
        {QStringLiteral("move2"), comboValue(_ui->CB_Move2)},
        {QStringLiteral("move3"), comboValue(_ui->CB_Move3)},
        {QStringLiteral("move4"), comboValue(_ui->CB_Move4)},
        {QStringLiteral("version"), comboValue(_ui->CB_GameOrigin)},
        {QStringLiteral("batchInstructions"), _ui->RTB_Instructions->toPlainText()},
        {QStringLiteral("types"), types},
        {QStringLiteral("useTabsAsCriteria"), true},
        {QStringLiteral("useTabsAsCriteriaAnySpecies"), true},
        {QStringLiteral("filterUnavailableSpecies"), true},
        {QStringLiteral("returnNoneIfEmptySearch"), true},
    };
    if (_ui->CHK_Shiny->checkState() != Qt::PartiallyChecked)
        root.insert(QStringLiteral("searchShiny"), _ui->CHK_Shiny->isChecked());
    if (_ui->CHK_IsEgg->checkState() != Qt::PartiallyChecked)
        root.insert(QStringLiteral("searchEgg"), _ui->CHK_IsEgg->isChecked());
    return QString::fromUtf8(QJsonDocument(root).toJson(QJsonDocument::Compact));
}
