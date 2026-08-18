#pragma once

#include <QDialog>
#include <QString>
#include <memory>

class QJsonObject;

namespace Ui
{
class RibbonEditor;
}

class AccessoryWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AccessoryWindow(QWidget *parent = nullptr);
    ~AccessoryWindow() override;

    void loadDocument(const QString &json);
    QString document() const;

signals:
    void modifyRequested(const QString &action);

private:
    void fillRibbons(const QJsonObject &root);
    void fillFlags(const QJsonObject &root);
    void fillMemories(const QJsonObject &root);
    void clearSidePanel();
    QJsonObject collectDocument() const;

    std::unique_ptr<Ui::RibbonEditor> _ui;
    bool _closeOnBulk = false;
};
