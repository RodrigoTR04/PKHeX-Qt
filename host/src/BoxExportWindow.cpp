#include "BoxExportWindow.h"

#include "LangCatalog.h"
#include "ui_BoxExporter.h"

#include <QComboBox>
#include <QCoreApplication>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QPushButton>

namespace
{
void fillEnum(QComboBox *box, const LangCatalog &catalog, const QString &type, const QStringList &keys)
{
    box->clear();
    for (const auto &key : keys)
        box->addItem(catalog.text(type, key, key), key);
}

void selectData(QComboBox *box, const QString &value)
{
    const int index = box->findData(value);
    if (index >= 0)
        box->setCurrentIndex(index);
}

QString comboData(const QComboBox *box)
{
    return box->currentData().toString();
}
}

BoxExportWindow::BoxExportWindow(QWidget *parent)
    : QDialog(parent)
    , _ui(std::make_unique<Ui::BoxExporter>())
{
    _ui->setupUi(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    LangCatalog catalog;
    catalog.loadFromFile(QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("lang/lang_en.txt")));
    catalog.apply(this, QStringLiteral("BoxExporter"));
    _ui->L_Scope->setText(catalog.text(QStringLiteral("PropertyGrid"), QStringLiteral("Scope"), QStringLiteral("Scope")));
    _ui->L_FolderCreation->setText(catalog.text(QStringLiteral("PropertyGrid"), QStringLiteral("FolderCreation"), QStringLiteral("FolderCreation")));
    _ui->L_FolderPrefix->setText(catalog.text(QStringLiteral("PropertyGrid"), QStringLiteral("FolderPrefix"), QStringLiteral("FolderPrefix")));
    _ui->L_EmptySlots->setText(catalog.text(QStringLiteral("PropertyGrid"), QStringLiteral("EmptySlots"), QStringLiteral("EmptySlots")));
    _ui->L_FileIndexPrefix->setText(catalog.text(QStringLiteral("PropertyGrid"), QStringLiteral("FileIndexPrefix"), QStringLiteral("FileIndexPrefix")));
    _ui->L_Notify->setText(catalog.text(QStringLiteral("PropertyGrid"), QStringLiteral("Notify"), QStringLiteral("Notify")));
    fillEnum(_ui->CB_Scope, catalog, QStringLiteral("BoxExportScope"), {QStringLiteral("All"), QStringLiteral("Current")});
    fillEnum(_ui->CB_FolderCreation, catalog, QStringLiteral("BoxExportFolderMode"), {QStringLiteral("None"), QStringLiteral("FolderEachBox")});
    fillEnum(_ui->CB_FolderPrefix, catalog, QStringLiteral("BoxExportFolderNaming"), {QStringLiteral("BoxName"), QStringLiteral("Index"), QStringLiteral("IndexBoxName")});
    fillEnum(_ui->CB_EmptySlots, catalog, QStringLiteral("BoxExportEmptySlots"), {QStringLiteral("Skip"), QStringLiteral("Include")});
    fillEnum(_ui->CB_FileIndexPrefix, catalog, QStringLiteral("BoxExportIndexPrefix"), {QStringLiteral("None"), QStringLiteral("InBox"), QStringLiteral("InAll"), QStringLiteral("InBoxAndSlot")});
    fillEnum(_ui->CB_Notify, catalog, QStringLiteral("BoxExportNofify"), {QStringLiteral("NotifyResult"), QStringLiteral("Silent")});
    for (auto *box : findChildren<QComboBox *>())
        box->setInsertPolicy(QComboBox::NoInsert);
    connect(_ui->B_Export, &QPushButton::clicked, this, &BoxExportWindow::exportRequested);
}

BoxExportWindow::~BoxExportWindow() = default;

void BoxExportWindow::loadDocument(const QString &json)
{
    const auto doc = QJsonDocument::fromJson(json.toUtf8());
    const auto root = doc.object();
    _ui->CB_Namer->clear();
    const auto namers = root.value(QStringLiteral("namers")).toArray();
    for (const auto &namer : namers)
        _ui->CB_Namer->addItem(namer.toString());
    if (_ui->CB_Namer->count() == 0)
        _ui->CB_Namer->addItem(QStringLiteral("Default"));
    const int namerIndex = _ui->CB_Namer->findText(root.value(QStringLiteral("namer")).toString());
    _ui->CB_Namer->setCurrentIndex(namerIndex >= 0 ? namerIndex : 0);

    const auto settings = root.value(QStringLiteral("settings")).toObject();
    selectData(_ui->CB_Scope, settings.value(QStringLiteral("scope")).toString());
    selectData(_ui->CB_FolderCreation, settings.value(QStringLiteral("folderCreation")).toString());
    selectData(_ui->CB_FolderPrefix, settings.value(QStringLiteral("folderPrefix")).toString());
    selectData(_ui->CB_EmptySlots, settings.value(QStringLiteral("emptySlots")).toString());
    selectData(_ui->CB_FileIndexPrefix, settings.value(QStringLiteral("fileIndexPrefix")).toString());
    selectData(_ui->CB_Notify, settings.value(QStringLiteral("notify")).toString());
}

void BoxExportWindow::lockScope(const QString &scope)
{
    if (scope.isEmpty())
        return;
    selectData(_ui->CB_Scope, scope);
    _ui->CB_Scope->setEnabled(false);
}

QString BoxExportWindow::document() const
{
    QJsonObject settings{
        {QStringLiteral("scope"), comboData(_ui->CB_Scope)},
        {QStringLiteral("folderCreation"), comboData(_ui->CB_FolderCreation)},
        {QStringLiteral("folderPrefix"), comboData(_ui->CB_FolderPrefix)},
        {QStringLiteral("emptySlots"), comboData(_ui->CB_EmptySlots)},
        {QStringLiteral("fileIndexPrefix"), comboData(_ui->CB_FileIndexPrefix)},
        {QStringLiteral("notify"), comboData(_ui->CB_Notify)},
    };
    QJsonObject root{
        {QStringLiteral("namer"), _ui->CB_Namer->currentText()},
        {QStringLiteral("settings"), settings},
    };
    return QString::fromUtf8(QJsonDocument(root).toJson(QJsonDocument::Compact));
}
