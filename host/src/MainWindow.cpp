#include "MainWindow.h"

#include "AccessoryWindow.h"
#include "BatchWindow.h"
#include "BoxExportWindow.h"
#include "PkmDatabaseWindow.h"
#include "AboutWindow.h"
#include "EditorBridge.h"
#include "InventoryWindow.h"
#include "PokedexWindow.h"
#include "SaveBlockWindow.h"
#include "LangCatalog.h"
#include "PkmTabChrome.h"
#include "QrWindow.h"
#include "SavToolChrome.h"
#include "SlotChrome.h"
#include "ui_MainWindow.h"

#include <QAbstractSpinBox>
#include <QAction>
#include <QApplication>
#include <QBuffer>
#include <QCheckBox>
#include <QClipboard>
#include <QCloseEvent>
#include <QComboBox>
#include <QCoreApplication>
#include <QDate>
#include <QDateEdit>
#include <QDialog>
#include <QDir>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QGuiApplication>
#include <QIODevice>
#include <QImage>
#include <QImage>
#include <QKeyEvent>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QMouseEvent>
#include <QPushButton>
#include <QSignalBlocker>
#include <QSoundEffect>
#include <QSpinBox>
#include <QUrl>
#include <QVariant>
#include <QWidget>

MainWindow::MainWindow(EditorBridge &editor, QWidget *parent)
    : QMainWindow(parent)
    , _editor(editor)
    , _ui(std::make_unique<Ui::MainWindow>())
    , _cry(std::make_unique<QSoundEffect>())
{
    _ui->setupUi(this);
    fillSavChrome();
    fillSlotChrome();
    fillPkmChrome();
    bindPkmContextMenu();
    applyEnglishStrings();
    applyInGameFont(this);
    setWindowTitle(QStringLiteral("PKHeX Qt"));
    setAcceptDrops(true);

    connect(_ui->Menu_Open, &QAction::triggered, this, &MainWindow::onMenuOpen);
    connect(_ui->Menu_ExportSAV, &QAction::triggered, this, &MainWindow::onMenuExportSav);
    connect(_ui->Menu_Save, &QAction::triggered, this, &MainWindow::onMenuSavePkm);
    connect(_ui->Menu_ExportBAK, &QPushButton::clicked, this, &MainWindow::onMenuExportBak);
    connect(_ui->Menu_Exit, &QAction::triggered, this, &MainWindow::onMenuExit);
    connect(_ui->Menu_ShowdownImportPKM, &QAction::triggered, this, &MainWindow::onShowdownImport);
    connect(_ui->Menu_ShowdownExportPKM, &QAction::triggered, this, &MainWindow::onShowdownExportPkm);
    connect(_ui->Menu_ShowdownExportParty, &QAction::triggered, this, &MainWindow::onShowdownExportParty);
    connect(_ui->Menu_ShowdownExportCurrentBox, &QAction::triggered, this, &MainWindow::onShowdownExportBox);
    connect(_ui->Menu_About, &QAction::triggered, this, &MainWindow::onMenuAbout);
    connect(_ui->Menu_BatchEditor, &QAction::triggered, this, &MainWindow::onOpenBatch);
    connect(_ui->Menu_DumpBoxes, &QAction::triggered, this, &MainWindow::onDumpBoxes);
    connect(_ui->Menu_DumpBox, &QAction::triggered, this, &MainWindow::onDumpBox);
    connect(_ui->Menu_Database, &QAction::triggered, this, &MainWindow::onOpenPkmDatabase);
    updateExportEnabled();
}

MainWindow::~MainWindow() = default;

bool MainWindow::openPath(const QString &path)
{
    if (_editor.pathIsSave(path) && !confirmOpenSave())
        return false;
    const bool ok = _editor.openPath(path);
    if (!ok)
        QMessageBox::warning(this, windowTitle(), tr("Could not open that save."));
    syncSession();
    return ok;
}

void MainWindow::syncSession()
{
    updateExportEnabled();
    refreshStorage();
    if (_editor.hasSession())
        refreshPkmEditor();
    refreshSavTools();
}

bool MainWindow::applyStartup(const QStringList &args)
{
    const bool ok = _editor.applyStartup(args);
    syncSession();
    return ok;
}

void MainWindow::promptBackupFolder()
{
    if (!_editor.takeBackupPrompt())
        return;
    const QString folder = _editor.backupDirectory();
    const QString location = QStringLiteral(
                                 "PKHeX can perform automatic backups if you create a folder with the name '%1' in the same folder as PKHeX's executable.")
                                 .arg(folder);
    if (!confirmYesNo(location, QStringLiteral("Would you like to create the backup folder now?")))
        return;
    if (!_editor.createBackupFolder())
    {
        QMessageBox::warning(this, windowTitle(), tr("Unable to create backup folder."));
        return;
    }
    QMessageBox::information(
        this,
        windowTitle(),
        tr("Backup folder created.") + QLatin1Char('\n') + folder);
}

bool MainWindow::savePath(const QString &path)
{
    if (!confirmExportSave())
        return false;
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

void MainWindow::onMenuSavePkm()
{
    if (!_editor.hasSession())
        return;
    const QByteArray data = _editor.exportEntity();
    if (data.isEmpty())
        return;
    const QString path = QFileDialog::getSaveFileName(
        this,
        tr("Save PKM"),
        _editor.entityFileName(),
        tr("All Files (*)"));
    if (path.isEmpty())
        return;
    if (!_editor.saveEntityPath(path))
        QMessageBox::warning(this, windowTitle(), tr("Could not export that Pokémon."));
}

void MainWindow::onMenuExportBak()
{
    if (!_editor.hasSession())
        return;
    const QString path = QFileDialog::getSaveFileName(
        this,
        tr("Export Backup"),
        _editor.suggestedBackupName(),
        tr("All Files (*)"));
    if (path.isEmpty())
        return;
    if (!_editor.exportBackup(path))
        QMessageBox::warning(this, windowTitle(), tr("Original file has been moved; unable to copy a backup."));
}

void MainWindow::updateExportEnabled()
{
    _ui->Menu_ExportSAV->setEnabled(_editor.isExportable());
    const bool open = _editor.hasSession();
    _ui->Menu_ShowdownImportPKM->setEnabled(open);
    _ui->Menu_ShowdownExportPKM->setEnabled(open);
    _ui->Menu_ShowdownExportParty->setEnabled(open);
    _ui->Menu_ShowdownExportCurrentBox->setEnabled(open);
    _ui->Menu_DumpBoxes->setEnabled(open && _editor.hasBox());
    _ui->Menu_DumpBox->setEnabled(open && _editor.hasBox());
    _ui->Menu_Database->setEnabled(open);
    if (auto *qr = findChild<QAction *>(QStringLiteral("mnuLQR")))
        qr->setEnabled(open);
}

void MainWindow::onMenuExit()
{
    close();
}

void MainWindow::onMenuAbout()
{
    AboutWindow dialog(this);
    dialog.exec();
}

void MainWindow::onOpenBatch()
{
    if (!_editor.hasSession())
        return;
    BatchWindow dialog(this);
    dialog.setProperties(_editor.batchProperties());
    connect(&dialog, &BatchWindow::runRequested, this, [&] {
        if (dialog.scope() == QLatin1String("folder"))
        {
            QMessageBox::information(this, windowTitle(), tr("Folder batch edits are not available yet."));
            return;
        }
        const QString raw = _editor.runBatch(dialog.scope(), dialog.instructions());
        const auto split = raw.indexOf(QLatin1Char('\n'));
        const QString kind = split < 0 ? raw : raw.left(split);
        const QString message = split < 0 ? raw : raw.mid(split + 1);
        if (kind == QLatin1String("ok"))
            QMessageBox::information(this, windowTitle(), message);
        else
            QMessageBox::warning(this, windowTitle(), message.isEmpty() ? tr("Could not run those instructions.") : message);
        refreshStorage();
        refreshPkmEditor();
    });
    dialog.exec();
}

void MainWindow::onDumpBoxes()
{
    openBoxExport(QStringLiteral("All"));
}

void MainWindow::onDumpBox()
{
    openBoxExport(QStringLiteral("Current"));
}

void MainWindow::openBoxExport(const QString &scopeLock)
{
    if (!_editor.hasSession() || !_editor.hasBox())
        return;
    BoxExportWindow dialog(this);
    dialog.loadDocument(_editor.boxExportDocument());
    dialog.lockScope(scopeLock);
    connect(&dialog, &BoxExportWindow::exportRequested, this, [&] {
        const QString folder = QFileDialog::getExistingDirectory(this, tr("Select a folder to export the boxes to."));
        if (folder.isEmpty())
            return;
        const QString raw = _editor.exportBoxes(folder, dialog.document());
        const auto split = raw.indexOf(QLatin1Char('\n'));
        const QString kind = split < 0 ? raw : raw.left(split);
        const QString message = split < 0 ? raw : raw.mid(split + 1);
        if (kind == QLatin1String("ok"))
        {
            if (!message.isEmpty())
                QMessageBox::information(this, windowTitle(), message);
        }
        else
            QMessageBox::warning(this, windowTitle(), message.isEmpty() ? tr("Invalid Box Data, unable to dump.") : message);
    });
    dialog.exec();
    _editor.saveBoxExportSettings(dialog.document());
}

void MainWindow::onOpenPkmDatabase()
{
    if (!_editor.hasSession())
        return;
    PkmDatabaseWindow dialog(this);
    dialog.loadDocument(_editor.pkmDatabaseDocument());
    connect(&dialog, &PkmDatabaseWindow::searchRequested, this, [&] {
        dialog.loadDocument(_editor.searchPkmDatabase(dialog.query()));
    });
    connect(&dialog, &PkmDatabaseWindow::loadRequested, this, [&](int index) {
        if (!_editor.loadPkmDatabaseHit(index))
        {
            QMessageBox::warning(this, windowTitle(), tr("Could not convert that Pokémon to this save."));
            return;
        }
        refreshPkmEditor();
    });
    dialog.exec();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!confirmClose())
    {
        event->ignore();
        return;
    }
    _editor.saveUserConfig();
    event->accept();
}

bool MainWindow::confirmYesNo(const QString &text, const QString &informative)
{
    QMessageBox box(this);
    box.setIcon(QMessageBox::Question);
    box.setWindowTitle(windowTitle());
    box.setText(text);
    box.setInformativeText(informative);
    box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    box.setDefaultButton(QMessageBox::Yes);
    return box.exec() == QMessageBox::Yes;
}

bool MainWindow::confirmOpenSave()
{
    if (!_editor.needsOpenPrompt())
        return true;
    return confirmYesNo(
        QStringLiteral("Any unsaved changes will be lost."),
        QStringLiteral("Are you sure you want to load a new save file?"));
}

bool MainWindow::confirmExportSave()
{
    if (!_editor.needsExportPrompt())
        return true;
    return confirmYesNo(
        QStringLiteral("The Pokémon in the editor has unsaved changes."),
        QStringLiteral("Continue?"));
}

bool MainWindow::confirmClose()
{
    if (!_editor.needsClosePrompt())
        return true;
    return confirmYesNo(
        QStringLiteral("Any unsaved changes will be lost."),
        QStringLiteral("Are you sure you want to close PKHeX?"));
}

void MainWindow::fillSavChrome()
{
    fillSavToolButtons(findChild<QWidget *>(QStringLiteral("FLP_SAVtools")));
    if (auto *items = findChild<QPushButton *>(QStringLiteral("B_OpenItemPouch")))
        connect(items, &QPushButton::clicked, this, &MainWindow::onOpenInventory);
    if (auto *dex = findChild<QPushButton *>(QStringLiteral("B_OpenPokedex")))
        connect(dex, &QPushButton::clicked, this, &MainWindow::onOpenPokedex);
    if (auto *trainer = findChild<QPushButton *>(QStringLiteral("B_OpenTrainerInfo")))
        connect(trainer, &QPushButton::clicked, this, &MainWindow::onOpenSaveBlock);
    if (auto *flags = findChild<QPushButton *>(QStringLiteral("B_OpenEventFlags")))
        connect(flags, &QPushButton::clicked, this, &MainWindow::onOpenEventFlags);
    if (auto *misc = findChild<QPushButton *>(QStringLiteral("B_OpenMiscEditor")))
        connect(misc, &QPushButton::clicked, this, [this] { openSaveBlockPage(QStringLiteral("misc")); });
    if (auto *fashion = findChild<QPushButton *>(QStringLiteral("B_OpenFashion")))
        connect(fashion, &QPushButton::clicked, this, [this] { openSaveBlockPage(QStringLiteral("fashion")); });
    if (auto *puffs = findChild<QPushButton *>(QStringLiteral("B_OpenPokepuffs")))
        connect(puffs, &QPushButton::clicked, this, [this] { openSaveBlockPage(QStringLiteral("pokepuffs")); });
    if (auto *opowers = findChild<QPushButton *>(QStringLiteral("B_OpenOPowers")))
        connect(opowers, &QPushButton::clicked, this, [this] { openSaveBlockPage(QStringLiteral("opowers")); });
}

void MainWindow::onOpenInventory()
{
    if (!_editor.hasSession())
        return;
    InventoryWindow dialog(this);
    dialog.loadDocument(_editor.inventoryDocument());
    connect(&dialog, &InventoryWindow::modifyRequested, this, [&](const QString &action) {
        dialog.loadDocument(_editor.inventoryModify(action, dialog.document()));
    });
    if (dialog.exec() == QDialog::Accepted)
        _editor.saveInventory(dialog.document());
}

void MainWindow::onOpenPokedex()
{
    if (!_editor.hasSession() || !_editor.hasPokedex())
        return;
    PokedexWindow dialog(this);
    dialog.loadDocument(_editor.pokedexDocument());
    connect(&dialog, &PokedexWindow::modifyRequested, this, [&](const QString &action) {
        const QString next = _editor.pokedexModify(action, dialog.document());
        if (!next.isEmpty())
            dialog.loadDocument(next);
    });
    if (dialog.exec() == QDialog::Accepted)
        _editor.savePokedex(dialog.document());
    else
        _editor.cancelPokedex();
}

void MainWindow::onOpenSaveBlock()
{
    openSaveBlockPage(QStringLiteral("trainer"));
}

void MainWindow::onOpenEventFlags()
{
    openSaveBlockPage(QStringLiteral("flags"));
}

void MainWindow::openSaveBlockPage(const QString &page)
{
    if (!_editor.hasSession() || !_editor.hasSaveBlock())
        return;
    const auto pages = _editor.saveBlockPages().split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    if (!pages.contains(page))
        return;
    SaveBlockWindow dialog(this);
    dialog.loadDocument(_editor.saveBlockDocument(page));
    connect(&dialog, &SaveBlockWindow::modifyRequested, this, [&](const QString &action) {
        const QString next = _editor.saveBlockModify(action, dialog.document());
        if (!next.isEmpty())
            dialog.loadDocument(next);
    });
    if (dialog.exec() == QDialog::Accepted)
        _editor.saveSaveBlock(dialog.document());
    else
        _editor.cancelSaveBlock();
}

void MainWindow::refreshSavTools()
{
    const auto pages = _editor.hasSession()
                           ? _editor.saveBlockPages().split(QLatin1Char('\n'), Qt::SkipEmptyParts)
                           : QStringList{};
    const auto show = [&](const char *button, const char *page) {
        if (auto *btn = findChild<QPushButton *>(QString::fromLatin1(button)))
            btn->setVisible(pages.contains(QString::fromLatin1(page)));
    };
    show("B_OpenTrainerInfo", "trainer");
    show("B_OpenEventFlags", "flags");
    show("B_OpenMiscEditor", "misc");
    show("B_OpenFashion", "fashion");
    show("B_OpenPokepuffs", "pokepuffs");
    show("B_OpenOPowers", "opowers");
}

void MainWindow::openAccessoryPage(const QString &page)
{
    if (!_editor.hasSession() || !_editor.hasAccessory())
        return;
    AccessoryWindow dialog(this);
    dialog.loadDocument(_editor.accessoryDocument(page));
    connect(&dialog, &AccessoryWindow::modifyRequested, this, [&](const QString &action) {
        const QString next = _editor.accessoryModify(action, dialog.document());
        if (!next.isEmpty())
            dialog.loadDocument(next);
    });
    if (dialog.exec() == QDialog::Accepted)
        _editor.saveAccessory(dialog.document());
    else
        _editor.cancelAccessory();
    refreshPkmEditor();
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
        {
            slot->installEventFilter(this);
            slot->setAcceptDrops(true);
        }
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

    if (auto *dragout = findChild<QLabel *>(QStringLiteral("dragout")))
        dragout->installEventFilter(this);
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
    if (QApplication::keyboardModifiers() & Qt::ControlModifier)
    {
        select->setCurrentIndex(0);
        return;
    }
    const int next = (select->currentIndex() + select->count() - 1) % select->count();
    select->setCurrentIndex(next);
}

void MainWindow::onBoxRight()
{
    auto *select = findChild<QComboBox *>(QStringLiteral("CB_BoxSelect"));
    if (select == nullptr || select->count() == 0)
        return;
    if (QApplication::keyboardModifiers() & Qt::ControlModifier)
    {
        select->setCurrentIndex(select->count() - 1);
        return;
    }
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

namespace
{
const char *kComboFields[] = {
    "CB_Species", "CB_Form", "CB_Nature", "CB_HeldItem", "CB_Ability", "CB_Language",
    "CB_GameOrigin", "CB_MetLocation", "CB_Ball", "CB_EggLocation", "CB_GroundTile",
    "CB_Move1", "CB_Move2", "CB_Move3", "CB_Move4", "CB_HPType",
    "CB_PPUps1", "CB_PPUps2", "CB_PPUps3", "CB_PPUps4", "UC_Gender", "UC_OTGender",
    "CB_PKRSStrain", "CB_PKRSDays",
};

const char *kEditFields[] = {
    "TB_PID", "TB_Nickname", "TB_EXP", "TB_Level", "TB_MetLevel", "TB_OT", "TB_TID", "TB_SID",
    "TB_Friendship", "TB_IVHP", "TB_IVATK", "TB_IVDEF", "TB_IVSPA", "TB_IVSPD", "TB_IVSPE",
    "TB_EVHP", "TB_EVATK", "TB_EVDEF", "TB_EVSPA", "TB_EVSPD", "TB_EVSPE",
    "TB_PP1", "TB_PP2", "TB_PP3", "TB_PP4", "TB_Cool", "TB_Beauty", "TB_Cute", "TB_Smart",
    "TB_Tough", "TB_Sheen", "TB_BaseHP", "TB_BaseATK", "TB_BaseDEF", "TB_BaseSPA", "TB_BaseSPD",
    "TB_BaseSPE", "Stat_HP", "Stat_ATK", "Stat_DEF", "Stat_SPA", "Stat_SPD", "Stat_SPE",
};

const char *kCheckFields[] = {
    "CHK_NicknamedFlag", "CHK_IsEgg", "CHK_Infected", "CHK_Cured", "CHK_NSparkle",
    "CHK_Fateful", "CHK_AsEgg", "PB_MarkShiny",
};

const char *kWritableChecks[] = {
    "CHK_NicknamedFlag", "CHK_IsEgg", "CHK_NSparkle", "CHK_Fateful", "PB_MarkShiny",
};
}

void MainWindow::fillPkmChrome()
{
    fillPkmTabs(
        findChild<QWidget *>(QStringLiteral("Tab_Main")),
        findChild<QWidget *>(QStringLiteral("Tab_Met")),
        findChild<QWidget *>(QStringLiteral("Tab_Stats")),
        findChild<QWidget *>(QStringLiteral("Tab_Moves")),
        findChild<QWidget *>(QStringLiteral("Tab_Cosmetic")),
        findChild<QWidget *>(QStringLiteral("Tab_OTMisc")));
    bindPkmFields();
    if (auto *ribbons = findChild<QPushButton *>(QStringLiteral("BTN_Ribbons")))
        connect(ribbons, &QPushButton::clicked, this, [this] { openAccessoryPage(QStringLiteral("ribbons")); });
    if (auto *history = findChild<QPushButton *>(QStringLiteral("BTN_History")))
        connect(history, &QPushButton::clicked, this, [this] { openAccessoryPage(QStringLiteral("memories")); });
    if (auto *medals = findChild<QPushButton *>(QStringLiteral("BTN_Medals")))
        connect(medals, &QPushButton::clicked, this, [this] { openAccessoryPage(QStringLiteral("medals")); });
    if (auto *tech = findChild<QPushButton *>(QStringLiteral("B_RelearnFlags")))
        connect(tech, &QPushButton::clicked, this, [this] { openAccessoryPage(QStringLiteral("tech")); });
    if (auto *shop = findChild<QPushButton *>(QStringLiteral("B_MoveShop")))
        connect(shop, &QPushButton::clicked, this, [this] { openAccessoryPage(QStringLiteral("shop")); });
    if (auto *plus = findChild<QPushButton *>(QStringLiteral("B_PlusRecord")))
        connect(plus, &QPushButton::clicked, this, [this] { openAccessoryPage(QStringLiteral("plus")); });
    if (auto *legal = findChild<QLabel *>(QStringLiteral("PB_Legal")))
    {
        legal->installEventFilter(this);
        legal->setProperty("legalityIcon", true);
    }
}

void MainWindow::bindPkmContextMenu()
{
    auto *mnuL = new QMenu(this);
    mnuL->setObjectName(QStringLiteral("mnuL"));
    auto *legality = new QAction(QStringLiteral("Legality"), this);
    legality->setObjectName(QStringLiteral("mnuLLegality"));
    auto *qr = new QAction(QStringLiteral("QR!"), this);
    qr->setObjectName(QStringLiteral("mnuLQR"));
    auto *saveAs = new QAction(QStringLiteral("Save as..."), this);
    saveAs->setObjectName(QStringLiteral("mnuLSave"));
    mnuL->addAction(legality);
    mnuL->addAction(qr);
    mnuL->addAction(saveAs);
    connect(legality, &QAction::triggered, this, &MainWindow::onLegalityClicked);
    connect(qr, &QAction::triggered, this, &MainWindow::clickQr);
    connect(saveAs, &QAction::triggered, this, &MainWindow::onMenuSavePkm);
    if (auto *dragout = findChild<QLabel *>(QStringLiteral("dragout")))
    {
        dragout->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(dragout, &QLabel::customContextMenuRequested, this, [dragout, mnuL](const QPoint &pos) {
            mnuL->exec(dragout->mapToGlobal(pos));
        });
    }
}

void MainWindow::bindPkmFields()
{
    for (const char *name : kEditFields)
    {
        if (auto *box = findChild<QLineEdit *>(QString::fromLatin1(name)))
            connect(box, &QLineEdit::editingFinished, this, &MainWindow::onPkmFieldEdited);
    }
    for (const char *name : kComboFields)
    {
        if (auto *box = findChild<QComboBox *>(QString::fromLatin1(name)))
            connect(box, &QComboBox::currentIndexChanged, this, &MainWindow::onPkmFieldEdited);
    }
    for (const char *name : kWritableChecks)
    {
        if (auto *box = findChild<QCheckBox *>(QString::fromLatin1(name)))
            connect(box, &QCheckBox::toggled, this, &MainWindow::onPkmFieldEdited);
    }
    for (auto *date : findChildren<QDateEdit *>())
        connect(date, &QDateEdit::dateChanged, this, &MainWindow::onPkmFieldEdited);
    if (auto *fame = findChild<QSpinBox *>(QStringLiteral("NUD_PokeStarFame")))
        connect(fame, &QSpinBox::valueChanged, this, &MainWindow::onPkmFieldEdited);
}

void MainWindow::onLegalityClicked()
{
    if (!_editor.hasSession())
        return;
    const QString report = _editor.legalityReport(false);
    QMessageBox box(this);
    box.setWindowTitle(tr("Legality Report"));
    box.setText(_editor.legalityValid() ? tr("Legal") : tr("Invalid"));
    box.setInformativeText(report);
    box.exec();
}

void MainWindow::onPkmFieldEdited()
{
    if (_pkmBusy || !_editor.hasSession())
        return;
    auto *obj = sender();
    if (obj == nullptr)
        return;
    const QString name = obj->objectName();
    if (name.isEmpty())
        return;

    QString value;
    if (auto *box = qobject_cast<QLineEdit *>(obj))
        value = box->text();
    else if (auto *combo = qobject_cast<QComboBox *>(obj))
        value = combo->currentData().isValid() ? combo->currentData().toString() : combo->currentText();
    else if (auto *check = qobject_cast<QCheckBox *>(obj))
        value = check->isChecked() ? QStringLiteral("1") : QStringLiteral("0");
    else if (auto *date = qobject_cast<QDateEdit *>(obj))
        value = date->date().toString(QStringLiteral("yyyy-MM-dd"));
    else if (auto *spin = qobject_cast<QSpinBox *>(obj))
        value = QString::number(spin->value());
    else
        return;

    writeField(name, value);
}

void MainWindow::writeField(const QString &name, const QString &value)
{
    if (!_editor.setField(name, value))
        return;
    _editor.commitCurrent();
    refreshPkmEditor();
}

void MainWindow::refreshPkmEditor()
{
    if (!_editor.hasSession())
        return;
    _pkmBusy = true;
    for (const char *name : kComboFields)
        fillComboChoices(QString::fromLatin1(name));
    for (const char *name : kComboFields)
        applyFieldValue(QString::fromLatin1(name), _editor.getField(QString::fromLatin1(name)));
    for (const char *name : kEditFields)
        applyFieldValue(QString::fromLatin1(name), _editor.getField(QString::fromLatin1(name)));
    for (const char *name : kCheckFields)
        applyFieldValue(QString::fromLatin1(name), _editor.getField(QString::fromLatin1(name)));
    applyFieldValue(QStringLiteral("CAL_MetDate"), _editor.getField(QStringLiteral("CAL_MetDate")));
    applyFieldValue(QStringLiteral("CAL_EggDate"), _editor.getField(QStringLiteral("CAL_EggDate")));
    applyFieldValue(QStringLiteral("NUD_PokeStarFame"), _editor.getField(QStringLiteral("NUD_PokeStarFame")));
    applyFieldValue(QStringLiteral("L_Characteristic"), _editor.getField(QStringLiteral("L_Characteristic")));
    applyFieldValue(QStringLiteral("Label_HiddenPowerPower"), _editor.getField(QStringLiteral("Label_HiddenPowerPower")));
    setLegalityIcon(findChild<QLabel *>(QStringLiteral("PB_Legal")), _editor.legalityValid());
    const auto pages = _editor.accessoryPages().split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    const auto showPage = [&](const char *button, const char *page) {
        if (auto *btn = findChild<QPushButton *>(QString::fromLatin1(button)))
            btn->setVisible(pages.contains(QString::fromLatin1(page)));
    };
    showPage("BTN_Ribbons", "ribbons");
    showPage("BTN_History", "memories");
    showPage("BTN_Medals", "medals");
    showPage("B_RelearnFlags", "tech");
    showPage("B_MoveShop", "shop");
    showPage("B_PlusRecord", "plus");
    _pkmBusy = false;
}

void MainWindow::fillComboChoices(const QString &name)
{
    auto *box = findChild<QComboBox *>(name);
    if (box == nullptr)
        return;
    const QSignalBlocker block(box);
    box->clear();
    if (name == QLatin1String("UC_Gender") || name == QLatin1String("UC_OTGender"))
    {
        box->addItem(QStringLiteral("♂"), 0);
        box->addItem(QStringLiteral("♀"), 1);
        return;
    }
    const auto lines = _editor.fieldChoices(name).split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    for (const auto &line : lines)
    {
        const auto parts = line.split(QLatin1Char('\t'));
        if (parts.size() < 2)
            continue;
        box->addItem(parts[1], parts[0].toInt());
    }
}

void MainWindow::applyFieldValue(const QString &name, const QString &value)
{
    if (auto *box = findChild<QLineEdit *>(name))
    {
        const QSignalBlocker block(box);
        box->setText(value);
        return;
    }
    if (auto *combo = findChild<QComboBox *>(name))
    {
        const QSignalBlocker block(combo);
        const int idx = combo->findData(value.toInt());
        if (idx >= 0)
            combo->setCurrentIndex(idx);
        return;
    }
    if (auto *check = findChild<QCheckBox *>(name))
    {
        const QSignalBlocker block(check);
        check->setChecked(value == QLatin1String("1"));
        return;
    }
    if (auto *date = findChild<QDateEdit *>(name))
    {
        const QSignalBlocker block(date);
        const auto parsed = QDate::fromString(value, QStringLiteral("yyyy-MM-dd"));
        if (parsed.isValid())
            date->setDate(parsed);
        return;
    }
    if (auto *spin = findChild<QSpinBox *>(name))
    {
        const QSignalBlocker block(spin);
        spin->setValue(value.toInt());
        return;
    }
    if (auto *label = findChild<QLabel *>(name))
        label->setText(value);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (handleSlotMouse(watched, event))
        return true;
    if (event->type() == QEvent::MouseButtonPress)
    {
        const auto *mouse = static_cast<QMouseEvent *>(event);
        if (mouse->button() == Qt::LeftButton && watched->property("legalityIcon").toBool())
        {
            onLegalityClicked();
            return true;
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

namespace
{
constexpr auto kEntityMime = "application/x-pkhex-pokemon";
constexpr auto kSlotMime = "application/x-pkhex-slot";
}

void MainWindow::onShowdownImport()
{
    if (!_editor.hasSession())
        return;
    auto *clip = QGuiApplication::clipboard();
    if (clip == nullptr || clip->text().isEmpty())
    {
        QMessageBox::warning(this, windowTitle(), tr("Set data not found in clipboard."));
        return;
    }
    const QString text = clip->text();
    const QString preview = _editor.previewShowdown(text);
    if (preview.isEmpty())
    {
        QMessageBox::warning(this, windowTitle(), tr("Set data not found in clipboard."));
        return;
    }
    const auto answer = QMessageBox::question(this, windowTitle(), tr("Import this set?\n\n%1").arg(preview));
    if (answer != QMessageBox::Yes)
        return;
    if (!_editor.importShowdown(text))
    {
        QMessageBox::warning(this, windowTitle(), tr("Set data not found in clipboard."));
        return;
    }
    refreshPkmEditor();
}

void MainWindow::onShowdownExportPkm()
{
    copyShowdown(QStringLiteral("pkm"), tr("Exported Showdown Set to Clipboard:"));
}

void MainWindow::onShowdownExportParty()
{
    copyShowdown(QStringLiteral("party"), tr("Showdown Team (Party) set to Clipboard."));
}

void MainWindow::onShowdownExportBox()
{
    copyShowdown(QStringLiteral("box"), tr("Showdown Sets copied to Clipboard."));
}

void MainWindow::copyShowdown(const QString &scope, const QString &success)
{
    if (!_editor.hasSession())
        return;
    const QString text = _editor.exportShowdown(scope);
    if (text.trimmed().isEmpty())
        return;
    auto *clip = QGuiApplication::clipboard();
    if (clip == nullptr)
        return;
    clip->setText(text);
    if (scope == QLatin1String("pkm"))
        QMessageBox::information(this, windowTitle(), success + QLatin1Char('\n') + text);
    else
        QMessageBox::information(this, windowTitle(), success);
}

void MainWindow::clickQr()
{
    if (!_editor.hasSession())
        return;
    if (QApplication::keyboardModifiers() & Qt::AltModifier)
    {
        importQrFromClipboard();
        return;
    }
    exportQrWindow();
}

void MainWindow::importQrFromClipboard()
{
    auto *clip = QGuiApplication::clipboard();
    if (clip == nullptr)
        return;
    const QString url = clip->text();
    if (!url.trimmed().isEmpty())
    {
        if (url.startsWith(QLatin1String("http")) && !url.contains(QLatin1Char('\n')))
        {
            if (!_editor.importQrMessage(url))
            {
                QMessageBox::warning(this, windowTitle(), tr("Decoded data not a valid PKM/Gift."));
                return;
            }
            refreshPkmEditor();
            refreshStorage();
            return;
        }
        onShowdownImport();
        return;
    }
    const auto *mime = clip->mimeData();
    if (mime == nullptr || !mime->hasImage())
        return;
    const QImage image = qvariant_cast<QImage>(mime->imageData());
    if (image.isNull())
        return;
    QByteArray png;
    QBuffer buffer(&png);
    if (!buffer.open(QIODevice::WriteOnly) || !image.save(&buffer, "PNG"))
    {
        QMessageBox::warning(this, windowTitle(), tr("Reader could not find QR data in the image."));
        return;
    }
    if (!_editor.importQrPng(png))
    {
        QMessageBox::warning(this, windowTitle(), tr("Decoded data not a valid PKM/Gift."));
        return;
    }
    refreshPkmEditor();
    refreshStorage();
}

void MainWindow::exportQrWindow()
{
    const QString message = _editor.exportQrMessage(0, 0, 1);
    if (message.startsWith(QLatin1String("http://lunarcookies.github.io/b1s1.html#")) && !_qr6Notified)
    {
        QMessageBox::information(
            this,
            windowTitle(),
            tr("QR codes are deprecated in favor of other methods.\n\n"
               "Consider utilizing homebrew or on-the-fly RAM editing custom firmware (PKMN-NTR)."));
        _qr6Notified = true;
    }

    QrWindow dialog(this);
    dialog.setBoxSlotCopiesVisible(_editor.qrHasBoxSlotCopies());
    const auto reload = [this, &dialog] {
        dialog.setImage(_editor.exportQrPng(dialog.box(), dialog.slot(), dialog.copies()));
    };
    connect(&dialog, &QrWindow::refreshRequested, this, reload);
    reload();
    dialog.exec();
}

void MainWindow::copyEntityToClipboard()
{
    if (!_editor.hasSession())
        return;
    const QByteArray data = _editor.exportEntity();
    if (data.isEmpty())
        return;
    auto *clip = QGuiApplication::clipboard();
    if (clip == nullptr)
        return;
    auto *mime = new QMimeData();
    mime->setData(QString::fromLatin1(kEntityMime), data);
    const QString name = _editor.entityFileName();
    if (!name.isEmpty())
    {
        const QString path = QDir::temp().filePath(name);
        QFile file(path);
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            file.write(data);
            file.close();
            mime->setUrls({QUrl::fromLocalFile(path)});
        }
    }
    clip->setMimeData(mime);
}

void MainWindow::pasteEntityFromClipboard()
{
    if (!_editor.hasSession())
        return;
    auto *clip = QGuiApplication::clipboard();
    if (clip == nullptr)
        return;
    const QMimeData *mime = clip->mimeData();
    if (mime == nullptr)
        return;
    QByteArray data;
    if (mime->hasFormat(QString::fromLatin1(kEntityMime)))
        data = mime->data(QString::fromLatin1(kEntityMime));
    else if (mime->hasUrls())
    {
        for (const auto &url : mime->urls())
        {
            const QString path = url.toLocalFile();
            if (path.isEmpty())
                continue;
            QFile file(path);
            if (file.open(QIODevice::ReadOnly))
            {
                data = file.readAll();
                break;
            }
        }
    }
    if (data.isEmpty() || !_editor.importEntity(data))
        return;
    refreshPkmEditor();
}

bool MainWindow::textWidgetHasFocus() const
{
    auto *focus = focusWidget();
    return qobject_cast<QLineEdit *>(focus) != nullptr
        || qobject_cast<QAbstractSpinBox *>(focus) != nullptr;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (!textWidgetHasFocus())
    {
        if (event->matches(QKeySequence::Copy))
        {
            copyEntityToClipboard();
            return;
        }
        if (event->matches(QKeySequence::Paste))
        {
            pasteEntityFromClipboard();
            return;
        }
    }
    QMainWindow::keyPressEvent(event);
}

bool MainWindow::handleSlotMouse(QObject *watched, QEvent *event)
{
    const QString key = watched->property("slotKey").toString();
    const bool dragout = watched->objectName() == QLatin1String("dragout");
    if (key.isEmpty() && !dragout)
        return false;

    switch (event->type())
    {
    case QEvent::Enter:
        if (!key.isEmpty() && _editor.hasSession())
        {
            static_cast<QWidget *>(watched)->setToolTip(_editor.slotPreview(key));
            playSlotCry(key);
        }
        return false;
    case QEvent::Leave:
        stopSlotCry();
        return false;
    case QEvent::DragEnter:
    case QEvent::DragMove:
    {
        auto *drag = static_cast<QDropEvent *>(event);
        const auto *mime = drag->mimeData();
        if (mime != nullptr
            && (mime->hasUrls() || mime->hasFormat(QString::fromLatin1(kEntityMime))
                || mime->hasFormat(QString::fromLatin1(kSlotMime))))
        {
            drag->acceptProposedAction();
            return true;
        }
        return false;
    }
    case QEvent::Drop:
        applyDrop(static_cast<QDropEvent *>(event), key);
        return true;
    case QEvent::MouseButtonPress:
    {
        const auto *mouse = static_cast<QMouseEvent *>(event);
        if (mouse->button() != Qt::LeftButton)
            return false;
        _pressPos = mouse->globalPosition().toPoint();
        _pressKey = dragout ? QString() : key;
        _dragging = false;
        if (dragout)
        {
            const auto mods = mouse->modifiers();
            if ((mods & Qt::AltModifier) || (mods & Qt::ShiftModifier))
            {
                clickQr();
                return true;
            }
            return false;
        }
        const auto mods = mouse->modifiers();
        if (mods & Qt::AltModifier)
        {
            _editor.deleteSlot(key);
            refreshStorage();
            refreshPkmEditor();
            return true;
        }
        if (mods & Qt::ShiftModifier)
        {
            _editor.writeCurrentToSlot(key);
            refreshStorage();
            return true;
        }
        _editor.selectSlot(key);
        refreshPkmEditor();
        return true;
    }
    case QEvent::MouseMove:
    {
        const auto *mouse = static_cast<QMouseEvent *>(event);
        if (!(mouse->buttons() & Qt::LeftButton) || _dragging)
            return false;
        if ((_pressPos - mouse->globalPosition().toPoint()).manhattanLength() < QApplication::startDragDistance())
            return false;
        auto *slot = qobject_cast<QLabel *>(watched);
        if (slot == nullptr)
            return false;
        startSlotDrag(slot, _pressKey);
        return true;
    }
    default:
        return false;
    }
}

void MainWindow::startSlotDrag(QLabel *slot, const QString &key)
{
    if (!_editor.hasSession())
        return;
    if (!key.isEmpty())
        _editor.selectSlot(key);
    const QByteArray data = _editor.exportEntity();
    if (data.isEmpty())
        return;
    _dragging = true;
    auto *mime = new QMimeData();
    mime->setData(QString::fromLatin1(kEntityMime), data);
    if (!key.isEmpty())
        mime->setData(QString::fromLatin1(kSlotMime), key.toUtf8());
    const QString name = _editor.entityFileName();
    if (!name.isEmpty())
    {
        const QString path = QDir::temp().filePath(name);
        QFile file(path);
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            file.write(data);
            file.close();
            mime->setUrls({QUrl::fromLocalFile(path)});
        }
    }
    QDrag drag(this);
    drag.setMimeData(mime);
    if (!slot->pixmap().isNull())
    {
        drag.setPixmap(slot->pixmap());
        drag.setHotSpot(slot->pixmap().rect().center());
    }
    drag.exec(Qt::CopyAction | Qt::MoveAction);
    _dragging = false;
    refreshStorage();
    refreshPkmEditor();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    const auto *mime = event->mimeData();
    if (mime != nullptr && (mime->hasUrls() || mime->hasFormat(QString::fromLatin1(kEntityMime))))
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    applyDrop(event, {});
}

void MainWindow::applyDrop(QDropEvent *event, const QString &destKey)
{
    const auto *mime = event->mimeData();
    if (mime == nullptr)
        return;

    if (!destKey.isEmpty() && mime->hasFormat(QString::fromLatin1(kSlotMime)))
    {
        const QString source = QString::fromUtf8(mime->data(QString::fromLatin1(kSlotMime)));
        if (!source.isEmpty() && source != destKey)
        {
            _editor.swapSlots(source, destKey);
            event->acceptProposedAction();
            refreshStorage();
            refreshPkmEditor();
            return;
        }
    }

    if (!destKey.isEmpty())
    {
        const QByteArray data = entityBytesFromMime(mime);
        if (!data.isEmpty() && _editor.dropOnSlot(destKey, data))
        {
            event->acceptProposedAction();
            refreshStorage();
            refreshPkmEditor();
        }
        return;
    }

    if (mime->hasUrls() && !mime->urls().isEmpty())
    {
        const QString path = mime->urls().front().toLocalFile();
        if (!path.isEmpty())
        {
            openPath(path);
            event->acceptProposedAction();
        }
    }
}

QByteArray MainWindow::entityBytesFromMime(const QMimeData *mime) const
{
    if (mime->hasFormat(QString::fromLatin1(kEntityMime)))
        return mime->data(QString::fromLatin1(kEntityMime));
    for (const auto &url : mime->urls())
    {
        const QString path = url.toLocalFile();
        if (path.isEmpty())
            continue;
        QFile file(path);
        if (file.open(QIODevice::ReadOnly))
            return file.readAll();
    }
    return {};
}

void MainWindow::playSlotCry(const QString &key)
{
    const QString path = _editor.slotCryPath(key);
    if (path.isEmpty())
    {
        stopSlotCry();
        return;
    }
    _cry->setSource(QUrl::fromLocalFile(path));
    _cry->play();
}

void MainWindow::stopSlotCry()
{
    if (_cry->isPlaying())
        _cry->stop();
}
