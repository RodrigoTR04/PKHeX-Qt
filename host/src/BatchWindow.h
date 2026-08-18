#pragma once

#include <QDialog>
#include <QString>
#include <memory>

namespace Ui
{
class BatchEditor;
}

class BatchWindow : public QDialog
{
    Q_OBJECT

public:
    explicit BatchWindow(QWidget *parent = nullptr);
    ~BatchWindow() override;

    void setProperties(const QString &names);
    QString instructions() const;
    QString scope() const;
    void appendInstruction(const QString &line);

signals:
    void runRequested();

private:
    std::unique_ptr<Ui::BatchEditor> _ui;
};
