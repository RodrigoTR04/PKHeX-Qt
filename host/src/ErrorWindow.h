#pragma once

#include <QDialog>
#include <memory>

class QString;

namespace Ui
{
class ErrorWindow;
}

class ErrorWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ErrorWindow(QWidget *parent = nullptr);
    ~ErrorWindow() override;

    void loadException(const QString &friendlyMessage, const QString &details, bool allowContinue);
    QString detailsText() const;
    void copyDetails() const;

    static int showException(const QString &friendlyMessage, const QString &details, bool allowContinue, QWidget *parent = nullptr);

private:
    std::unique_ptr<Ui::ErrorWindow> _ui;
};
