#include "MainWindow.h"

#include "EditorBridge.h"
#include "LangCatalog.h"
#include "SavToolChrome.h"
#include "SlotChrome.h"
#include "ui_MainWindow.h"

#include <QComboBox>
#include <QCoreApplication>
#include <QDir>
#include <QEvent>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPushButton>
#include <QSignalBlocker>
#include <QWidget>

MainWindow::MainWindow(EditorBridge &editor, QWidget *parent)
    : QMainWindow(parent)
    , _editor(editor)
    , _ui(std::make_unique<Ui::MainWindow>())
{
    _ui->setupUi(this);
    fillSavChrome();
    fillSlotChrome();
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
    refreshStorage();
    return ok;
}

bool MainWindow::savePath(const QString &path)
{
    const bool ok = _editor.savePath(path);
    if (!ok)
        QMessageBox::warning(this, windowTitle(), tr("Could not export that save."));
    return ok;
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        const auto *mouse = static_cast<QMouseEvent *>(event);
        if (mouse->button() == Qt::LeftButton)
        {
            const QString key = watched->property("slotKey").toString();
            if (!key.isEmpty())
            {
                _editor.selectSlot(key);
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
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

void MainWindow::fillSlotChrome()
{
    fillBoxAndPartyChrome(
        findChild<QWidget *>(QStringLiteral("Tab_Box")),
        findChild<QWidget *>(QStringLiteral("Tab_PartyBattle")));

    const auto slotLabels = findChildren<QLabel *>();
    for (auto *slot : slotLabels)
    {
        if (!slot->property("slotKey").toString().isEmpty())
            slot->installEventFilter(this);
    }

    auto *select = findChild<QComboBox *>(QStringLiteral("CB_BoxSelect"));
    auto *left = findChild<QPushButton *>(QStringLiteral("B_BoxLeft"));
    auto *right = findChild<QPushButton *>(QStringLiteral("B_BoxRight"));
    if (select != nullptr)
        connect(select, &QComboBox::currentIndexChanged, this, &MainWindow::onBoxSelected);
    if (left != nullptr)
        connect(left, &QPushButton::clicked, this, &MainWindow::onBoxLeft);
    if (right != nullptr)
        connect(right, &QPushButton::clicked, this, &MainWindow::onBoxRight);
}

void MainWindow::onBoxSelected(int index)
{
    if (!_editor.hasSession() || index < 0)
        return;
    _editor.setCurrentBox(index);
    StorageLayout layout;
    if (!_editor.storageLayout(layout))
        return;
    refreshBoxSlots(layout);
}

void MainWindow::onBoxLeft()
{
    auto *select = findChild<QComboBox *>(QStringLiteral("CB_BoxSelect"));
    if (select == nullptr || select->count() == 0)
        return;
    const int next = (select->currentIndex() + select->count() - 1) % select->count();
    select->setCurrentIndex(next);
}

void MainWindow::onBoxRight()
{
    auto *select = findChild<QComboBox *>(QStringLiteral("CB_BoxSelect"));
    if (select == nullptr || select->count() == 0)
        return;
    const int next = (select->currentIndex() + 1) % select->count();
    select->setCurrentIndex(next);
}

void MainWindow::refreshStorage()
{
    if (!_editor.hasSession())
        return;

    StorageLayout layout;
    if (!_editor.storageLayout(layout))
        return;

    refreshBoxSelector(layout);
    refreshBoxSlots(layout);
    refreshPartySlots();
}

void MainWindow::refreshBoxSelector(const StorageLayout &layout)
{
    auto *select = findChild<QComboBox *>(QStringLiteral("CB_BoxSelect"));
    if (select == nullptr)
        return;

    const QSignalBlocker block(select);
    select->clear();
    for (int i = 0; i < layout.boxCount; ++i)
        select->addItem(tr("Box %1").arg(i + 1));
    if (layout.currentBox >= 0 && layout.currentBox < layout.boxCount)
        select->setCurrentIndex(layout.currentBox);
}

void MainWindow::refreshBoxSlots(const StorageLayout &layout)
{
    auto *grid = findChild<BoxPokeGrid *>(QStringLiteral("BoxPokeGrid"));
    if (grid == nullptr)
        return;

    grid->setWallpaperPng(_editor.slotPng(QStringLiteral("wallpaper:%1").arg(layout.currentBox)));

    int index = 0;
    for (int row = 0; row < 5; ++row)
    {
        for (int column = 0; column < 6; ++column)
        {
            const QString name = QStringLiteral("Pokémon Grid Row %1 Column %2")
                                     .arg(row, 2, 10, QLatin1Char('0'))
                                     .arg(column, 2, 10, QLatin1Char('0'));
            auto *slot = grid->findChild<QLabel *>(name);
            if (slot == nullptr)
                continue;
            const QString key = QStringLiteral("box:%1:%2").arg(layout.currentBox).arg(index);
            slot->setProperty("slotKey", key);
            applySlotPng(slot, _editor.slotPng(key));
            ++index;
        }
    }
}

void MainWindow::refreshPartySlots()
{
    auto *grid = findChild<QWidget *>(QStringLiteral("PartyPokeGrid"));
    if (grid == nullptr)
        return;

    int index = 0;
    for (int row = 0; row < 3; ++row)
    {
        for (int column = 0; column < 2; ++column)
        {
            const QString name = QStringLiteral("Pokémon Grid Row %1 Column %2")
                                     .arg(row, 2, 10, QLatin1Char('0'))
                                     .arg(column, 2, 10, QLatin1Char('0'));
            auto *slot = grid->findChild<QLabel *>(name);
            if (slot == nullptr)
                continue;
            const QString key = QStringLiteral("party:%1").arg(index);
            slot->setProperty("slotKey", key);
            applySlotPng(slot, _editor.slotPng(key));
            ++index;
        }
    }
}

void MainWindow::applyEnglishStrings()
{
    const QString path = QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("lang/lang_en.txt"));
    LangCatalog catalog;
    if (!catalog.loadFromFile(path))
        return;
    catalog.apply(this, QStringLiteral("Main"));
}
