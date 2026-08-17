#pragma once

#include "EditorBridge.h"

#include <QMainWindow>
#include <memory>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(EditorBridge &editor, QWidget *parent = nullptr);
    ~MainWindow() override;

    bool openPath(const QString &path);
    bool savePath(const QString &path);

private slots:
    void onMenuOpen();
    void onMenuExportSav();

private:
    void updateExportEnabled();

    EditorBridge &_editor;
    std::unique_ptr<Ui::MainWindow> _ui;
};
