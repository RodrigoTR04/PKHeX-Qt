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

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void onMenuOpen();
    void onMenuExportSav();
    void onMenuExit();
    void onBoxSelected(int index);
    void onBoxLeft();
    void onBoxRight();

private:
    void updateExportEnabled();
    void applyEnglishStrings();
    void fillSavChrome();
    void fillSlotChrome();
    void refreshStorage();
    void refreshBoxSelector(const StorageLayout &layout);
    void refreshBoxSlots(const StorageLayout &layout);
    void refreshPartySlots();

    EditorBridge &_editor;
    std::unique_ptr<Ui::MainWindow> _ui;
};
