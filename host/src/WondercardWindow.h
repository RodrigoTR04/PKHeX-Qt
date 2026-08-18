#pragma once

#include <QDialog>
#include <QString>
#include <memory>

namespace Ui
{
class SAV_Wondercard;
}

class WondercardWindow : public QDialog
{
    Q_OBJECT

public:
    explicit WondercardWindow(QWidget *parent = nullptr);
    ~WondercardWindow() override;

    void loadDocument(const QString &json);
    QString document() const;

signals:
    void modifyRequested(const QString &action);
    void importRequested(const QString &path);
    void exportRequested(const QString &path);

private:
    std::unique_ptr<Ui::SAV_Wondercard> _ui;
};
