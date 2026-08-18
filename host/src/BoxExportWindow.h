#pragma once

#include <QDialog>
#include <QString>
#include <memory>

namespace Ui
{
class BoxExporter;
}

class BoxExportWindow : public QDialog
{
    Q_OBJECT

public:
    explicit BoxExportWindow(QWidget *parent = nullptr);
    ~BoxExportWindow() override;

    void loadDocument(const QString &json);
    void lockScope(const QString &scope);
    QString document() const;

signals:
    void exportRequested();

private:
    std::unique_ptr<Ui::BoxExporter> _ui;
};
