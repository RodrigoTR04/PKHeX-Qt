#include "PkmDatabaseWindow.h"

#include "LangCatalog.h"
#include "ui_SAV_Database.h"

#include <QAbstractItemView>
#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QDir>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>

PkmDatabaseWindow::PkmDatabaseWindow(QWidget *parent)
    : QDialog(parent)
    , _ui(std::make_unique<Ui::SAV_Database>())
{
    _ui->setupUi(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    _ui->DatabasePokeGrid->horizontalHeader()->setStretchLastSection(true);
    _ui->DatabasePokeGrid->verticalHeader()->setVisible(false);
    _ui->DatabasePokeGrid->setSelectionBehavior(QAbstractItemView::SelectRows);
    _ui->DatabasePokeGrid->setSelectionMode(QAbstractItemView::SingleSelection);
    _ui->DatabasePokeGrid->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _ui->CHK_Shiny->setTristate(true);
    _ui->CHK_Shiny->setCheckState(Qt::PartiallyChecked);
    _ui->CHK_IsEgg->setTristate(true);
    _ui->CHK_IsEgg->setCheckState(Qt::PartiallyChecked);
    LangCatalog catalog;
    catalog.loadFromFile(QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("lang/lang_en.txt")));
    catalog.apply(this, QStringLiteral("SAV_Database"));
    connect(_ui->B_Search, &QPushButton::clicked, this, &PkmDatabaseWindow::searchRequested);
    connect(_ui->B_Reset, &QPushButton::clicked, this, [this] {
        _ui->CB_Species->setCurrentIndex(0);
        _ui->TB_Nickname->clear();
        _ui->CHK_Shiny->setCheckState(Qt::PartiallyChecked);
        _ui->CHK_IsEgg->setCheckState(Qt::PartiallyChecked);
        _ui->RTB_Instructions->clear();
    });
    connect(_ui->Menu_Exit, &QAction::triggered, this, &QDialog::close);
    connect(_ui->DatabasePokeGrid, &QTableWidget::itemDoubleClicked, this, [this](QTableWidgetItem *item) {
        if (item == nullptr)
            return;
        emit loadRequested(item->data(Qt::UserRole).toInt());
    });
}

PkmDatabaseWindow::~PkmDatabaseWindow() = default;

void PkmDatabaseWindow::loadDocument(const QString &json)
{
    const auto root = QJsonDocument::fromJson(json.toUtf8()).object();
    if (_ui->CB_Species->count() == 0)
    {
        const auto lines = root.value(QStringLiteral("speciesChoices")).toString().split(QLatin1Char('\n'), Qt::SkipEmptyParts);
        for (const auto &line : lines)
        {
            const auto parts = line.split(QLatin1Char('\t'));
            if (parts.size() < 2)
                continue;
            _ui->CB_Species->addItem(parts.at(1), parts.at(0).toInt());
        }
        if (_ui->CB_Species->count() == 0)
            _ui->CB_Species->addItem(QStringLiteral("(Any)"), 0);
    }

    const auto hits = root.value(QStringLiteral("hits")).toArray();
    auto *table = _ui->DatabasePokeGrid;
    table->setRowCount(hits.size());
    for (int row = 0; row < hits.size(); ++row)
    {
        const auto hit = hits.at(row).toObject();
        auto *item = new QTableWidgetItem(hit.value(QStringLiteral("identify")).toString());
        item->setData(Qt::UserRole, hit.value(QStringLiteral("index")).toInt());
        table->setItem(row, 0, item);
    }
    const QString count = QStringLiteral("Count: %1").arg(root.value(QStringLiteral("count")).toInt());
    _ui->L_Count->setText(count);
}

QString PkmDatabaseWindow::query() const
{
    QJsonObject root{
        {QStringLiteral("species"), _ui->CB_Species->currentData().toInt()},
        {QStringLiteral("nickname"), _ui->TB_Nickname->text()},
        {QStringLiteral("searchBoxes"), _ui->Menu_SearchBoxes->isChecked()},
        {QStringLiteral("searchDatabase"), _ui->Menu_SearchDatabase->isChecked()},
        {QStringLiteral("searchBackups"), _ui->Menu_SearchBackups->isChecked()},
        {QStringLiteral("batchInstructions"), _ui->RTB_Instructions->toPlainText()},
    };
    if (_ui->CHK_Shiny->checkState() != Qt::PartiallyChecked)
        root.insert(QStringLiteral("searchShiny"), _ui->CHK_Shiny->isChecked());
    if (_ui->CHK_IsEgg->checkState() != Qt::PartiallyChecked)
        root.insert(QStringLiteral("searchEgg"), _ui->CHK_IsEgg->isChecked());
    if (_ui->Menu_SearchLegal->isChecked() != _ui->Menu_SearchIllegal->isChecked())
        root.insert(QStringLiteral("searchLegal"), _ui->Menu_SearchLegal->isChecked());
    return QString::fromUtf8(QJsonDocument(root).toJson(QJsonDocument::Compact));
}
