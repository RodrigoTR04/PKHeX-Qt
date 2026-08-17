#pragma once

#include <QDialog>
#include <memory>

class QCloseEvent;

namespace Ui
{
class SplashScreen;
}

class SplashScreen : public QDialog
{
    Q_OBJECT

public:
    explicit SplashScreen(QWidget *parent = nullptr);
    ~SplashScreen() override;

    void forceClose();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    std::unique_ptr<Ui::SplashScreen> _ui;
    bool _canClose = false;
};
