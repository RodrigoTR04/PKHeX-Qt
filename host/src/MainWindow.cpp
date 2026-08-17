#include "MainWindow.h"

#include "EditorBridge.h"
#include "LangCatalog.h"
#include "SavToolChrome.h"
#include "ui_MainWindow.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QWidget>

MainWindow::MainWindow(EditorBridge &editor, QWidget *parent)
    : QMainWindow(parent)
    , _editor(editor)
    , _ui(std::make_unique<Ui::MainWindow>())
{
    _ui->setupUi(this);
    fillSavChrome();
    applyEnglishStrings();
    setWindowTitle(QStringLiteral("PKHeX Qt"));

    connect(_ui->Menu_Open, &QAction::triggered, this, &MainWindow::onMenuOpen);
    connect(_ui->Menu_ExportSAV, &QAction::triggered, this, &MainWindow::onMenuExportSav);
    connect(_ui->Menu_Exit, &QAction::triggered, this, &MainWindow::onMenuExit);
    updateExportEnabled();
}

MainWindow::~MainWindow() = default;

bool MainWindow::openPath(const QString &path)
{
    const bool ok = _editor.openPath(path);
    if (!ok)
        QMessageBox::warning(this, windowTitle(), tr("Could not open that save."));
    updateExportEnabled();
    return ok;
}

bool MainWindow::savePath(const QString &path)
{
    const bool ok = _editor.savePath(path);
    if (!ok)
        QMessageBox::warning(this, windowTitle(), tr("Could not export that save."));
    return ok;
}

void MainWindow::onMenuOpen()
{
    const QString path = QFileDialog::getOpenFileName(
        this,
        tr("Open"),
        QString(),
        tr("All Files (*)"));
    if (path.isEmpty())
        return;
    openPath(path);
}

void MainWindow::onMenuExportSav()
{
    if (!_editor.hasSession())
        return;

    const QString path = QFileDialog::getSaveFileName(
        this,
        tr("Export SAV"),
        QString(),
        tr("All Files (*)"));
    if (path.isEmpty())
        return;
    savePath(path);
}

void MainWindow::updateExportEnabled()
{
    _ui->Menu_ExportSAV->setEnabled(_editor.hasSession());
}

void MainWindow::onMenuExit()
{
    close();
}

void MainWindow::fillSavChrome()
{
    fillSavToolButtons(findChild<QWidget *>(QStringLiteral("FLP_SAVtools")));
}

void MainWindow::applyEnglishStrings()
{
    const QString path = QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("lang/lang_en.txt"));
    LangCatalog catalog;
    if (!catalog.loadFromFile(path))
        return;
    catalog.apply(this, QStringLiteral("Main"));
}
