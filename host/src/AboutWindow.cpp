#include "AboutWindow.h"

#include "AppInfo.h"
#include "LangCatalog.h"
#include "ui_About.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QIODevice>
#include <QLabel>
#include <QPlainTextEdit>
#include <QTextStream>

namespace
{
QString readSidecar(const QString &relative)
{
    const QString path = QDir(QCoreApplication::applicationDirPath()).filePath(relative);
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};
    return QTextStream(&file).readAll();
}

QString notices()
{
    return QStringLiteral(
        "PKHeX Qt is licensed under the GNU General Public License v3.0 or later (GPL-3.0-or-later).\n"
        "Qt 6 is used under the GNU Lesser General Public License (LGPL).\n"
        "\n"
        "Third-party notices:\n"
        "- PKHeX.Core: GPL-3.0-or-later (kwsch/PKHeX)\n"
        "- QRCoder: MIT\n"
        "- ZXing.Net: Apache-2.0\n"
        "- SkiaSharp: MIT\n");
}
}

AboutWindow::AboutWindow(QWidget *parent)
    : QDialog(parent)
    , _ui(std::make_unique<Ui::About>())
{
    _ui->setupUi(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    _ui->L_Version->setText(
        tr("PKHeX Qt %1  •  Oracle %2").arg(pkhexQtVersion(), pkhexOracleDate()));
    _ui->RTB_Notices->setPlainText(notices());
    const QString shortcuts = readSidecar(QStringLiteral("about/shortcuts.txt"));
    if (!shortcuts.isEmpty())
        _ui->RTB_Shortcuts->setPlainText(shortcuts);
    const QString changelog = readSidecar(QStringLiteral("about/changelog.txt"));
    if (!changelog.isEmpty())
        _ui->RTB_Changelog->setPlainText(changelog);
    LangCatalog catalog;
    catalog.loadFromFile(QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("lang/lang_en.txt")));
    catalog.apply(this, QStringLiteral("About"));
}

AboutWindow::~AboutWindow() = default;
