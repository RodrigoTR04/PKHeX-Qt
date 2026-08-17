#include "EditorBridge.h"
#include "MainWindow.h"

#include <QAction>
#include <QApplication>
#include <QByteArray>
#include <QComboBox>
#include <QCoreApplication>
#include <QEvent>
#include <QLabel>
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

    return 0;
}
