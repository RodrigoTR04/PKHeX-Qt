#pragma once

#include <QDialog>
#include <memory>

class QByteArray;
class QEvent;

namespace Ui
{
class QR;
}

class QrWindow : public QDialog
{
    Q_OBJECT

public:
    explicit QrWindow(QWidget *parent = nullptr);
    ~QrWindow() override;

    void setImage(const QByteArray &png);
    void setBoxSlotCopiesVisible(bool visible);
    int box() const;
    int slot() const;
    int copies() const;

signals:
    void refreshRequested();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void copyImage();

    std::unique_ptr<Ui::QR> _ui;
};
