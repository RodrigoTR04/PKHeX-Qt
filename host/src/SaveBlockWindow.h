#pragma once

#include <QDialog>
#include <QString>
#include <memory>

class QJsonObject;

namespace Ui
{
class SAV_SimpleTrainer;
}

class SaveBlockWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SaveBlockWindow(QWidget *parent = nullptr);
    ~SaveBlockWindow() override;

    void loadDocument(const QString &json);
    QString document() const;

signals:
    void modifyRequested(const QString &action);

private:
    void fillTrainer(const QJsonObject &root);
    void fillFlags(const QJsonObject &root);
    void fillFields(const QJsonObject &root);
    void setPageKind(const QString &kind);
    QJsonObject collectDocument() const;
    void clearBlockActions();

    std::unique_ptr<Ui::SAV_SimpleTrainer> _ui;
    bool _mapEdited = false;
};
