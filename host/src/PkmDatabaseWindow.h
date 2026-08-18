#pragma once

#include <QDialog>
#include <QString>
#include <memory>

namespace Ui
{
class SAV_Database;
}

class PkmDatabaseWindow : public QDialog
{
    Q_OBJECT

public:
    explicit PkmDatabaseWindow(QWidget *parent = nullptr);
    ~PkmDatabaseWindow() override;

    void loadDocument(const QString &json);
    QString query() const;

signals:
    void searchRequested();
    void loadRequested(int index);

private:
    std::unique_ptr<Ui::SAV_Database> _ui;
};
