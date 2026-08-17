#include "DotNetEditorBridge.h"
#include "AppInfo.h"
#include "ErrorWindow.h"
#include "MainWindow.h"
#include "SplashScreen.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDialog>
#include <QEvent>
#include <QObject>
#include <QStringList>
#include <exception>

class PkhexApplication : public QApplication
{
public:
    using QApplication::QApplication;

    bool notify(QObject *receiver, QEvent *event) override
    {
        try
        {
            return QApplication::notify(receiver, event);
        }
        catch (const std::exception &ex)
        {
            const int result = ErrorWindow::showException(
                QStringLiteral("An error occurred in PKHeX Qt. Please report this error to the PKHeX Qt author."),
                QString::fromUtf8(ex.what()),
                true);
            if (result != QDialog::Accepted)
                QApplication::exit(1);
            return true;
        }
    }
};

int main(int argc, char *argv[])
{
    PkhexApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("PKHeX Qt"));
    app.setApplicationDisplayName(QStringLiteral("PKHeX Qt"));
    app.setApplicationVersion(pkhexQtVersion());

    SplashScreen splash;
    splash.show();
    QApplication::processEvents();

    try
    {
        DotNetEditorBridge editor;
        MainWindow window(editor);
        QStringList args = QCoreApplication::arguments();
        if (!args.isEmpty())
            args.removeFirst();
        window.applyStartup(args);
        splash.forceClose();
        window.show();
        window.promptBackupFolder();
        return QApplication::exec();
    }
    catch (const std::exception &ex)
    {
        splash.forceClose();
        ErrorWindow::showException(
            QStringLiteral("PKHeX Qt must now close."),
            QString::fromUtf8(ex.what()),
            false);
        return 1;
    }
}
