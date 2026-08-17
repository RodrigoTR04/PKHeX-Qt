#include "EditorBridge.h"
#include "MainWindow.h"

#include <QAction>
#include <QApplication>
#include <QString>
#include <iostream>

class FakeEditor : public EditorBridge
{
public:
    QString lastOpened;
    QString lastSaved;
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

    return 0;
}
