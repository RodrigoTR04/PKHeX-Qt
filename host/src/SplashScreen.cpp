#include "SplashScreen.h"

#include "ui_SplashScreen.h"

#include <QCloseEvent>

SplashScreen::SplashScreen(QWidget *parent)
    : QDialog(parent)
    , _ui(std::make_unique<Ui::SplashScreen>())
{
    _ui->setupUi(this);
    setWindowFlags(Qt::SplashScreen | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setModal(false);
}

SplashScreen::~SplashScreen() = default;

void SplashScreen::forceClose()
{
    _canClose = true;
    close();
}

void SplashScreen::closeEvent(QCloseEvent *event)
{
    if (!_canClose)
    {
        event->ignore();
        return;
    }
    QDialog::closeEvent(event);
}
