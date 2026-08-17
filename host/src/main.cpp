#include "DotNetEditorBridge.h"
#include "MainWindow.h"

#include <QApplication>
#include <QMessageBox>
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
        if (argc > 1)
            window.openPath(QString::fromLocal8Bit(argv[1]));
        window.show();
        return QApplication::exec();
    }
    catch (const std::exception &ex)
    {
        QMessageBox::critical(nullptr, QStringLiteral("PKHeX Qt"), QString::fromUtf8(ex.what()));
        return 1;
    }
}
