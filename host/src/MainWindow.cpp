#include "MainWindow.h"

#include "EditorBridge.h"
#include "ui_MainWindow.h"

#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(EditorBridge &editor, QWidget *parent)
    : QMainWindow(parent)
    , _editor(editor)
    , _ui(std::make_unique<Ui::MainWindow>())
{
    _ui->setupUi(this);
    setWindowTitle(QStringLiteral("PKHeX Qt"));

    connect(_ui->Menu_Open, &QAction::triggered, this, &MainWindow::onMenuOpen);
    connect(_ui->Menu_ExportSAV, &QAction::triggered, this, &MainWindow::onMenuExportSav);
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
