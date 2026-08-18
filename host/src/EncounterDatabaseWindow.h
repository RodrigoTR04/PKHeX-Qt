#pragma once

#include <QDialog>
#include <QString>
#include <memory>

namespace Ui
{
class SAV_Encounters;
}

class EncounterDatabaseWindow : public QDialog
{
    Q_OBJECT

public:
    explicit EncounterDatabaseWindow(QWidget *parent = nullptr);
    ~EncounterDatabaseWindow() override;

    void loadDocument(const QString &json);
    QString query() const;

signals:
    void searchRequested();
    void loadRequested(int index);

private:
    std::unique_ptr<Ui::SAV_Encounters> _ui;
};
