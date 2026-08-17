#pragma once

#include <QDialog>
#include <memory>

namespace Ui
{
class About;
}

class AboutWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AboutWindow(QWidget *parent = nullptr);
    ~AboutWindow() override;

private:
    std::unique_ptr<Ui::About> _ui;
};
