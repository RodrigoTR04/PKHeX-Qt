#include "AccessoryWindow.h"
#include "AboutWindow.h"
#include "AppInfo.h"
#include "EditorBridge.h"
#include "ErrorWindow.h"
#include "InventoryWindow.h"
#include "PokedexWindow.h"
#include "MainWindow.h"
#include "SplashScreen.h"

#include <QAction>
#include <QApplication>
#include <QByteArray>
#include <QClipboard>
#include <QComboBox>
#include <QCoreApplication>
#include <QEvent>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QSpinBox>
#include <QTableWidget>
#include <QMimeData>
#include <QMouseEvent>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSplitter>
#include <QString>
#include <QStringList>
#include <QTabWidget>
#include <iostream>

class FakeEditor : public EditorBridge
{
public:
    QString lastOpened;
    QString lastSaved;
    QString lastSelect;
    int lastBox = -1;
    QString lastWrite;
    QString lastDelete;
    QString lastSwap;
    QString lastDrop;
    QString lastCry;
    QString lastInventory;
    QString lastInventorySave;
    bool session = false;

    bool openPath(const QString &path) override
    {
        lastOpened = path;
        session = true;
        return true;
    }

    bool savePath(const QString &path) override
    {
        lastSaved = path;
        return true;
    }

    bool hasSession() const override
    {
        return session;
    }

    bool selectSlot(const QString &key) override
    {
        lastSelect = key;
        return true;
    }

    bool setCurrentBox(int box) override
    {
        lastBox = box;
        return true;
    }

    bool storageLayout(StorageLayout &out) const override
    {
        out = StorageLayout{};
        if (lastBox >= 0)
            out.currentBox = lastBox;
        return true;
    }

    QByteArray slotPng(const QString &) override
    {
        return {};
    }

    QString getField(const QString &) override
    {
        return {};
    }

    bool setField(const QString &, const QString &) override
    {
        return true;
    }

    bool commitCurrent() override
    {
        return true;
    }

    bool legalityValid() const override
    {
        return true;
    }

    QString legalityReport(bool) override
    {
        return QStringLiteral("Valid.");
    }

    QString fieldChoices(const QString &) override
    {
        return QStringLiteral("1\tBulbasaur");
    }

    QString previewShowdown(const QString &text) override
    {
        return text.contains(QLatin1String("Bulbasaur")) ? QStringLiteral("Bulbasaur") : QString();
    }

    bool importShowdown(const QString &) override
    {
        return true;
    }

    QString exportShowdown(const QString &) override
    {
        return QStringLiteral("Bulbasaur");
    }

    QByteArray exportEntity() override
    {
        return QByteArray(8, 'P');
    }

    QString entityFileName() override
    {
        return QStringLiteral("Bulbasaur.pk5");
    }

    bool importEntity(const QByteArray &) override
    {
        return true;
    }

    QString slotPreview(const QString &key) override
    {
        return QStringLiteral("preview:") + key;
    }

    QString slotCryPath(const QString &key) override
    {
        lastCry = key;
        return {};
    }

    bool qrHasBoxSlotCopies() const override
    {
        return false;
    }

    QString exportQrMessage(int, int, int) override
    {
        return QStringLiteral("null/#qr");
    }

    QByteArray exportQrPng(int, int, int) override
    {
        return QByteArray::fromHex("89504e470d0a1a0a");
    }

    bool importQrMessage(const QString &) override
    {
        return true;
    }

    bool importQrPng(const QByteArray &) override
    {
        return true;
    }

    bool writeCurrentToSlot(const QString &key) override
    {
        lastWrite = key;
        return true;
    }

    bool deleteSlot(const QString &key) override
    {
        lastDelete = key;
        return true;
    }

    bool swapSlots(const QString &source, const QString &destination) override
    {
        lastSwap = source + QLatin1Char('|') + destination;
        return true;
    }

    bool dropOnSlot(const QString &key, const QByteArray &) override
    {
        lastDrop = key;
        return true;
    }

    bool needsClosePrompt() const override
    {
        return false;
    }

    bool needsOpenPrompt() const override
    {
        return false;
    }

    bool needsExportPrompt() const override
    {
        return false;
    }

    bool pathIsSave(const QString &) override
    {
        return true;
    }

    bool saveUserConfig() override
    {
        return true;
    }

    bool exportBackup(const QString &) override
    {
        return true;
    }

    bool saveEntityPath(const QString &) override
    {
        return true;
    }

    QString suggestedBackupName() override
    {
        return QStringLiteral("named.sav.bak");
    }

    bool applyStartup(const QStringList &) override
    {
        return false;
    }

    bool takeBackupPrompt() override
    {
        return false;
    }

    bool createBackupFolder() override
    {
        return true;
    }

    bool isExportable() const override
    {
        return session;
    }

    QString backupDirectory() override
    {
        return QStringLiteral("/tmp/pkhexqt-bak");
    }

    QString inventoryDocument() override
    {
        lastInventory = QStringLiteral("open");
        return QStringLiteral(
            "{\"itemColumnReadOnly\":false,\"hasFavorite\":false,\"hasNew\":false,"
            "\"hasFreeSpace\":false,\"hasFreeSpaceIndex\":false,\"hasNewShop\":false,"
            "\"hasHeld\":false,\"currentPouch\":0,\"giveCount\":995,"
            "\"pouches\":[{\"type\":\"Medicine\",\"maxCount\":999,\"giveDisabled\":false,"
            "\"choices\":[\"None\",\"Potion\"],"
            "\"rows\":[{\"itemId\":0,\"item\":\"None\",\"count\":0}]}]}");
    }

    QString inventoryModify(const QString &action, const QString &json) override
    {
        lastInventory = action;
        return json;
    }

    bool saveInventory(const QString &json) override
    {
        lastInventorySave = json;
        return true;
    }

    bool hasPokedex() const override
    {
        return true;
    }

    QString pokedexDocument() override
    {
        return QStringLiteral(
            "{\"langForm\":\"SAV_Pokedex5\",\"maxSpecies\":2,\"languageCount\":7,"
            "\"hasNationalDex\":true,\"hasSpinda\":true,\"species\":1,\"caught\":false,"
            "\"speciesNames\":[\"001 - Bulbasaur\",\"002 - Ivysaur\"]}");
    }

    QString pokedexModify(const QString &, const QString &json) override
    {
        return json;
    }

    bool savePokedex(const QString &) override
    {
        return true;
    }

    bool cancelPokedex() override
    {
        return true;
    }

    bool hasAccessory() const override
    {
        return true;
    }

    QString accessoryDocument() override
    {
        return QStringLiteral(
            "{\"page\":\"ribbons\",\"langForm\":\"RibbonEditor\",\"hasAffixed\":false,\"affixed\":-1,"
            "\"ribbons\":[{\"name\":\"RibbonChampionSinnoh\",\"label\":\"Sinnoh Champ Ribbon\","
            "\"type\":\"bool\",\"hasRibbon\":false,\"count\":0,\"maxCount\":0}]}");
    }

    QString accessoryModify(const QString &, const QString &json) override
    {
        return json;
    }

    bool saveAccessory(const QString &) override
    {
        return true;
    }

    bool cancelAccessory() override
    {
        return true;
    }
};

int main(int argc, char *argv[])
{
    qputenv("QT_QPA_PLATFORM", "offscreen");
    QApplication app(argc, argv);

    FakeEditor editor;
    MainWindow window(editor);

    if (window.windowTitle() != QLatin1String("PKHeX Qt"))
    {
        std::cerr << "window title was " << window.windowTitle().toStdString() << "\n";
        return 1;
    }
    if (window.windowTitle() == QLatin1String("PKHeX"))
        return 2;

    auto *exportSav = window.findChild<QAction *>(QStringLiteral("Menu_ExportSAV"));
    auto *menuOpen = window.findChild<QAction *>(QStringLiteral("Menu_Open"));
    if (exportSav == nullptr || menuOpen == nullptr)
        return 3;
    if (exportSav->isEnabled())
        return 4;

    if (!window.openPath(QStringLiteral("/tmp/cli.sav")))
        return 5;
    if (editor.lastOpened != QLatin1String("/tmp/cli.sav"))
        return 6;
    if (!exportSav->isEnabled())
        return 7;

    if (!window.savePath(QStringLiteral("/tmp/exported.sav")))
        return 8;
    if (editor.lastSaved != QLatin1String("/tmp/exported.sav"))
        return 9;

    const QStringList required{
        QStringLiteral("PKME_Tabs"),
        QStringLiteral("Tab_Main"),
        QStringLiteral("Tab_Met"),
        QStringLiteral("Tab_Stats"),
        QStringLiteral("Tab_Moves"),
        QStringLiteral("Tab_Cosmetic"),
        QStringLiteral("Tab_OTMisc"),
        QStringLiteral("C_SAV"),
        QStringLiteral("Tab_Box"),
        QStringLiteral("Tab_PartyBattle"),
        QStringLiteral("Tab_Other"),
        QStringLiteral("Tab_SAV"),
        QStringLiteral("B_OpenItemPouch"),
        QStringLiteral("B_OpenTrainerInfo"),
        QStringLiteral("B_OpenPokedex"),
        QStringLiteral("BTN_Ribbons"),
        QStringLiteral("dragout"),
        QStringLiteral("PB_Legal"),
        QStringLiteral("splitContainer1"),
        QStringLiteral("Menu_Tools"),
        QStringLiteral("Menu_Options"),
        QStringLiteral("Menu_Save"),
        QStringLiteral("Menu_About"),
    };
    for (const auto &name : required)
    {
        if (window.findChild<QObject *>(name) == nullptr)
        {
            std::cerr << "missing control " << name.toStdString() << "\n";
            return 10;
        }
    }

    auto *splitter = window.findChild<QSplitter *>(QStringLiteral("splitContainer1"));
    if (splitter == nullptr)
        return 11;

    auto *tabs = window.findChild<QTabWidget *>(QStringLiteral("PKME_Tabs"));
    if (tabs == nullptr || tabs->tabText(0) != QLatin1String("Main"))
        return 12;

    auto *items = window.findChild<QPushButton *>(QStringLiteral("B_OpenItemPouch"));
    if (items == nullptr || items->text() != QLatin1String("Items"))
    {
        std::cerr << "items text was " << (items ? items->text().toStdString() : "null") << "\n";
        return 13;
    }

    auto *about = window.findChild<QAction *>(QStringLiteral("Menu_About"));
    if (about == nullptr || about->text() != QLatin1String("&About PKHeX"))
        return 14;
    if (about->shortcut() != QKeySequence(QStringLiteral("Ctrl+P")))
        return 37;

    if (window.findChild<QObject *>(QStringLiteral("DarkMode")) != nullptr
        || window.findChild<QObject *>(QStringLiteral("chkDarkMode")) != nullptr)
        return 15;

    if (!window.styleSheet().isEmpty())
        return 16;

    auto *box = window.findChild<QWidget *>(QStringLiteral("Box"));
    auto *boxGrid = window.findChild<QWidget *>(QStringLiteral("BoxPokeGrid"));
    auto *boxSelect = window.findChild<QComboBox *>(QStringLiteral("CB_BoxSelect"));
    auto *party = window.findChild<QWidget *>(QStringLiteral("SL_Party"));
    auto *partyGrid = window.findChild<QWidget *>(QStringLiteral("PartyPokeGrid"));
    if (box == nullptr || boxGrid == nullptr || boxSelect == nullptr || party == nullptr || partyGrid == nullptr)
        return 17;

    auto *slot = boxGrid->findChild<QLabel *>(QString::fromUtf8("Pokémon Grid Row 00 Column 00"));
    if (slot == nullptr)
        return 18;

    QMouseEvent press(
        QEvent::MouseButtonPress,
        QPointF(1, 1),
        QPointF(1, 1),
        Qt::LeftButton,
        Qt::LeftButton,
        Qt::NoModifier);
    QCoreApplication::sendEvent(slot, &press);
    if (editor.lastSelect != QLatin1String("box:0:0"))
    {
        std::cerr << "select was " << editor.lastSelect.toStdString() << "\n";
        return 19;
    }

    auto *nickname = window.findChild<QLineEdit *>(QStringLiteral("TB_Nickname"));
    auto *ot = window.findChild<QLineEdit *>(QStringLiteral("TB_OT"));
    if (window.findChild<QComboBox *>(QStringLiteral("CB_Species")) == nullptr || nickname == nullptr || ot == nullptr)
        return 20;
    const auto nickFamily = nickname->font().family();
    if (!nickFamily.contains(QStringLiteral("PGL"), Qt::CaseInsensitive)
        && !nickFamily.contains(QStringLiteral("Dings"), Qt::CaseInsensitive))
    {
        std::cerr << "nickname font was " << nickFamily.toStdString() << "\n";
        return 21;
    }

    auto *importSet = window.findChild<QAction *>(QStringLiteral("Menu_ShowdownImportPKM"));
    auto *exportSet = window.findChild<QAction *>(QStringLiteral("Menu_ShowdownExportPKM"));
    if (importSet == nullptr || exportSet == nullptr)
        return 22;
    if (importSet->shortcut() != QKeySequence(QStringLiteral("Ctrl+T")))
        return 23;
    if (exportSet->shortcut() != QKeySequence(QStringLiteral("Ctrl+Shift+T")))
        return 24;
    if (!importSet->isEnabled())
        return 25;

    auto *mnuLqr = window.findChild<QAction *>(QStringLiteral("mnuLQR"));
    if (mnuLqr == nullptr || !mnuLqr->text().contains(QLatin1String("QR!")))
    {
        std::cerr << "mnuLQR missing text=" << (mnuLqr ? mnuLqr->text().toStdString() : "null") << "\n";
        return 34;
    }
    if (window.findChild<QMenu *>(QStringLiteral("mnuL")) == nullptr)
        return 35;
    if (!mnuLqr->isEnabled())
        return 36;

    window.setFocus(Qt::OtherFocusReason);
    QKeyEvent copyKey(QEvent::KeyPress, Qt::Key_C, Qt::ControlModifier);
    QCoreApplication::sendEvent(&window, &copyKey);
    auto *clip = QGuiApplication::clipboard();
    if (clip == nullptr || clip->mimeData() == nullptr
        || !clip->mimeData()->hasFormat(QStringLiteral("application/x-pkhex-pokemon")))
    {
        std::cerr << "entity clipboard mime missing\n";
        return 26;
    }

    if (!window.acceptDrops() || !slot->acceptDrops())
        return 27;

    QEvent enter(QEvent::Enter);
    QCoreApplication::sendEvent(slot, &enter);
    if (!slot->toolTip().startsWith(QLatin1String("preview:")))
    {
        std::cerr << "tooltip was " << slot->toolTip().toStdString() << "\n";
        return 28;
    }
    if (editor.lastCry != QLatin1String("box:0:0"))
    {
        std::cerr << "cry was " << editor.lastCry.toStdString() << "\n";
        return 33;
    }

    QMouseEvent shiftPress(
        QEvent::MouseButtonPress,
        QPointF(1, 1),
        QPointF(1, 1),
        Qt::LeftButton,
        Qt::LeftButton,
        Qt::ShiftModifier);
    QCoreApplication::sendEvent(slot, &shiftPress);
    if (editor.lastWrite != QLatin1String("box:0:0"))
    {
        std::cerr << "shift-set was " << editor.lastWrite.toStdString() << "\n";
        return 29;
    }

    QMouseEvent altPress(
        QEvent::MouseButtonPress,
        QPointF(1, 1),
        QPointF(1, 1),
        Qt::LeftButton,
        Qt::LeftButton,
        Qt::AltModifier);
    QCoreApplication::sendEvent(slot, &altPress);
    if (editor.lastDelete != QLatin1String("box:0:0"))
        return 30;

    auto *savePkm = window.findChild<QAction *>(QStringLiteral("Menu_Save"));
    if (savePkm == nullptr || savePkm->shortcut() != QKeySequence(QStringLiteral("Ctrl+S")))
        return 31;

    if (window.findChild<QPushButton *>(QStringLiteral("Menu_ExportBAK")) == nullptr)
        return 32;

    AboutWindow aboutDialog(&window);
    auto *disclaimer = aboutDialog.findChild<QLabel *>(QStringLiteral("L_Disclaimer"));
    auto *version = aboutDialog.findChild<QLabel *>(QStringLiteral("L_Version"));
    auto *notices = aboutDialog.findChild<QPlainTextEdit *>(QStringLiteral("RTB_Notices"));
    if (disclaimer == nullptr || !disclaimer->text().contains(QLatin1String("unofficial"), Qt::CaseInsensitive))
    {
        std::cerr << "about disclaimer missing\n";
        return 38;
    }
    if (version == nullptr
        || !version->text().contains(pkhexQtVersion())
        || !version->text().contains(pkhexOracleDate()))
    {
        std::cerr << "about version was " << (version ? version->text().toStdString() : "null") << "\n";
        return 39;
    }
    if (notices == nullptr
        || !notices->toPlainText().contains(QLatin1String("GPL-3"))
        || !notices->toPlainText().contains(QLatin1String("LGPL"))
        || !notices->toPlainText().contains(QLatin1String("QRCoder")))
    {
        std::cerr << "about notices missing licenses\n";
        return 40;
    }
    if (aboutDialog.findChild<QObject *>(QStringLiteral("TC_About")) == nullptr
        || aboutDialog.findChild<QObject *>(QStringLiteral("Tab_Shortcuts")) == nullptr)
        return 41;

    ErrorWindow errorDialog;
    errorDialog.loadException(QStringLiteral("friendly boom"), QStringLiteral("stack-trace-here"), true);
    if (!errorDialog.detailsText().contains(QLatin1String("stack-trace-here"))
        || !errorDialog.detailsText().contains(QLatin1String("friendly boom")))
        return 42;
    errorDialog.copyDetails();
    if (clip == nullptr || !clip->text().contains(QLatin1String("stack-trace-here")))
        return 43;
    if (errorDialog.findChild<QObject *>(QStringLiteral("B_CopyToClipboard")) == nullptr
        || errorDialog.findChild<QObject *>(QStringLiteral("T_ExceptionDetails")) == nullptr)
        return 44;

    SplashScreen splash;
    auto *status = splash.findChild<QLabel *>(QStringLiteral("L_Status"));
    if (splash.objectName() != QLatin1String("SplashScreen")
        || status == nullptr
        || !status->text().contains(QLatin1String("Starting up")))
        return 45;
    splash.show();
    splash.forceClose();

    InventoryWindow inventoryDialog(&window);
    inventoryDialog.loadDocument(editor.inventoryDocument());
    if (inventoryDialog.findChild<QObject *>(QStringLiteral("B_Cancel")) == nullptr
        || inventoryDialog.findChild<QObject *>(QStringLiteral("B_Save")) == nullptr
        || inventoryDialog.findChild<QObject *>(QStringLiteral("tabControl1")) == nullptr
        || inventoryDialog.findChild<QObject *>(QStringLiteral("B_GiveAll")) == nullptr
        || inventoryDialog.findChild<QObject *>(QStringLiteral("B_Sort")) == nullptr
        || inventoryDialog.findChild<QObject *>(QStringLiteral("L_Count")) == nullptr
        || inventoryDialog.findChild<QSpinBox *>(QStringLiteral("NUD_Count")) == nullptr
        || inventoryDialog.findChild<QObject *>(QStringLiteral("tableLayoutPanel1")) == nullptr
        || inventoryDialog.findChild<QMenu *>(QStringLiteral("sortMenu")) == nullptr
        || inventoryDialog.findChild<QMenu *>(QStringLiteral("giveMenu")) == nullptr
        || inventoryDialog.findChild<QAction *>(QStringLiteral("mnuSortName")) == nullptr
        || inventoryDialog.findChild<QTableWidget *>(QStringLiteral("DGV_Medicine")) == nullptr)
    {
        std::cerr << "inventory chrome missing\n";
        return 46;
    }
    if (inventoryDialog.windowTitle() != QLatin1String("Inventory Editor"))
    {
        std::cerr << "inventory title was " << inventoryDialog.windowTitle().toStdString() << "\n";
        return 47;
    }

    PokedexWindow pokedexDialog(&window);
    pokedexDialog.loadDocument(editor.pokedexDocument());
    const QStringList dexNames{
        QStringLiteral("B_Save"),
        QStringLiteral("B_Cancel"),
        QStringLiteral("LB_Species"),
        QStringLiteral("CB_Species"),
        QStringLiteral("CHK_P1"),
        QStringLiteral("B_Modify"),
        QStringLiteral("mnuSeenNone"),
    };
    for (const auto &name : dexNames)
    {
        if (pokedexDialog.findChild<QObject *>(name) == nullptr)
        {
            std::cerr << "pokedex missing " << name.toStdString()
                      << " title=" << pokedexDialog.windowTitle().toStdString() << "\n";
            return 48;
        }
    }
    if (pokedexDialog.windowTitle() != QStringLiteral("Pokédex Editor"))
    {
        std::cerr << "pokedex title was " << pokedexDialog.windowTitle().toStdString() << "\n";
        return 48;
    }

    AccessoryWindow accessoryDialog(&window);
    accessoryDialog.loadDocument(editor.accessoryDocument());
    const QStringList ribbonNames{
        QStringLiteral("B_Save"),
        QStringLiteral("B_Cancel"),
        QStringLiteral("B_All"),
        QStringLiteral("B_None"),
        QStringLiteral("CB_Affixed"),
        QStringLiteral("TLP_Ribbons"),
        QStringLiteral("SPLIT_Ribbons"),
        QStringLiteral("CHK_RibbonChampionSinnoh"),
    };
    for (const auto &name : ribbonNames)
    {
        if (accessoryDialog.findChild<QObject *>(name) == nullptr)
        {
            std::cerr << "ribbons missing " << name.toStdString() << "\n";
            return 49;
        }
    }
    if (accessoryDialog.windowTitle() != QLatin1String("Ribbon Editor"))
    {
        std::cerr << "ribbons title was " << accessoryDialog.windowTitle().toStdString() << "\n";
        return 49;
    }

    return 0;
}
