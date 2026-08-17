#include "QrWindow.h"

#include "ui_QR.h"

#include <QByteArray>
#include <QClipboard>
#include <QEvent>
#include <QGuiApplication>
#include <QLabel>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPixmap>
#include <QPushButton>
#include <QSplitter>

QrWindow::QrWindow(QWidget *parent)
    : QDialog(parent)
    , _ui(std::make_unique<Ui::QR>())
{
    _ui->setupUi(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    setWindowFlag(Qt::WindowMaximizeButtonHint, false);
    _ui->PB_QR->installEventFilter(this);
    connect(_ui->B_Refresh, &QPushButton::clicked, this, &QrWindow::refreshRequested);
}

QrWindow::~QrWindow() = default;

void QrWindow::setImage(const QByteArray &png)
{
    QPixmap pixmap;
    pixmap.loadFromData(png, "PNG");
    _ui->PB_QR->setPixmap(pixmap);
    if (!pixmap.isNull())
        _ui->PB_QR->setMinimumSize(pixmap.size());
    adjustSize();
}

void QrWindow::setBoxSlotCopiesVisible(bool visible)
{
    _ui->QRPanel1->setVisible(visible);
    if (auto *handle = _ui->splitContainer1->handle(1))
        handle->setEnabled(visible);
}

int QrWindow::box() const
{
    return _ui->NUD_Box->value() - 1;
}

int QrWindow::slot() const
{
    return _ui->NUD_Slot->value() - 1;
}

int QrWindow::copies() const
{
    return _ui->NUD_Copies->value();
}

bool QrWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == _ui->PB_QR && event->type() == QEvent::MouseButtonRelease)
    {
        const auto *mouse = static_cast<QMouseEvent *>(event);
        if (mouse->button() == Qt::LeftButton)
        {
            copyImage();
            return true;
        }
    }
    return QDialog::eventFilter(watched, event);
}

void QrWindow::copyImage()
{
    const QPixmap pixmap = _ui->PB_QR->pixmap();
    if (pixmap.isNull())
        return;
    const auto answer = QMessageBox::question(
        this,
        windowTitle(),
        tr("Copy QR Image to Clipboard?"));
    if (answer != QMessageBox::Yes)
        return;
    if (auto *clip = QGuiApplication::clipboard())
        clip->setPixmap(pixmap);
}
