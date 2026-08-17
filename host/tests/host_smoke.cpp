#include "EditorBridge.h"
#include "MainWindow.h"

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
#include <QMimeData>
#include <QMouseEvent>
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

    return 0;
}
