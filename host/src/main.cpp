#include "DotNetEditorBridge.h"
#include "MainWindow.h"

#include <QApplication>
#include <QCoreApplication>
#include <QMessageBox>
#include <QStringList>
#include <exception>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("PKHeX Qt"));
    app.setApplicationDisplayName(QStringLiteral("PKHeX Qt"));

    try
    {
        DotNetEditorBridge editor;
        MainWindow window(editor);
        QStringList args = QCoreApplication::arguments();
        if (!args.isEmpty())
            args.removeFirst();
        window.applyStartup(args);
        window.show();
        window.promptBackupFolder();
        return QApplication::exec();
    }
    catch (const std::exception &ex)
    {
        QMessageBox::critical(nullptr, QStringLiteral("PKHeX Qt"), QString::fromUtf8(ex.what()));
        return 1;
    }
}
