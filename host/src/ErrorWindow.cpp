#include "ErrorWindow.h"

#include "LangCatalog.h"
#include "ui_ErrorWindow.h"

#include <QClipboard>
#include <QCoreApplication>
#include <QDir>
#include <QGuiApplication>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>

ErrorWindow::ErrorWindow(QWidget *parent)
    : QDialog(parent)
    , _ui(std::make_unique<Ui::ErrorWindow>())
{
    _ui->setupUi(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    LangCatalog catalog;
    catalog.loadFromFile(QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("lang/lang_en.txt")));
    catalog.apply(this, QStringLiteral("ErrorWindow"));
    connect(_ui->B_CopyToClipboard, &QPushButton::clicked, this, &ErrorWindow::copyDetails);
    connect(_ui->B_Continue, &QPushButton::clicked, this, &QDialog::accept);
    connect(_ui->B_Abort, &QPushButton::clicked, this, &QDialog::reject);
}

ErrorWindow::~ErrorWindow() = default;

void ErrorWindow::loadException(const QString &friendlyMessage, const QString &details, bool allowContinue)
{
    _ui->L_Message->setText(friendlyMessage);
    QString text;
    text += QStringLiteral("Exception Details:\n");
    text += details;
    text += QStringLiteral("\n\nUser Message:\n");
    text += friendlyMessage;
    _ui->T_ExceptionDetails->setPlainText(text);
    _ui->B_Continue->setVisible(allowContinue);
}

QString ErrorWindow::detailsText() const
{
    return _ui->T_ExceptionDetails->toPlainText();
}

void ErrorWindow::copyDetails() const
{
    if (auto *clip = QGuiApplication::clipboard())
        clip->setText(detailsText());
}

int ErrorWindow::showException(const QString &friendlyMessage, const QString &details, bool allowContinue, QWidget *parent)
{
    ErrorWindow dialog(parent);
    dialog.loadException(friendlyMessage, details, allowContinue);
    return dialog.exec();
}
