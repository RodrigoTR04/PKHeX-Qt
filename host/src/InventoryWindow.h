#pragma once

#include <QDialog>
#include <QString>
#include <memory>

class QJsonObject;

namespace Ui
{
class SAV_Inventory;
}

class InventoryWindow : public QDialog
{
    Q_OBJECT

public:
    explicit InventoryWindow(QWidget *parent = nullptr);
    ~InventoryWindow() override;

    void loadDocument(const QString &json);
    QString document() const;

signals:
    void modifyRequested(const QString &action);

private:
    void buildMenus();
    void applyPouchChrome();
    void fillPouches(const QJsonObject &root);
    QJsonObject collectDocument() const;

    std::unique_ptr<Ui::SAV_Inventory> _ui;
};
