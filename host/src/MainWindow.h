#pragma once

#include "EditorBridge.h"

#include <QMainWindow>
#include <memory>

class QKeyEvent;

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
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onMenuOpen();
    void onMenuExportSav();
    void onMenuExit();
    void onBoxSelected(int index);
    void onBoxLeft();
    void onBoxRight();
    void onLegalityClicked();
    void onPkmFieldEdited();
    void onShowdownImport();
    void onShowdownExportPkm();
    void onShowdownExportParty();
    void onShowdownExportBox();

private:
    void updateExportEnabled();
    void applyEnglishStrings();
    void fillSavChrome();
    void fillSlotChrome();
    void fillPkmChrome();
    void bindPkmFields();
    void refreshStorage();
    void refreshBoxSelector(const StorageLayout &layout);
    void refreshBoxSlots(const StorageLayout &layout);
    void refreshPartySlots();
    void refreshPkmEditor();
    void fillComboChoices(const QString &name);
    void applyFieldValue(const QString &name, const QString &value);
    void writeField(const QString &name, const QString &value);
    void copyShowdown(const QString &scope, const QString &success);
    void copyEntityToClipboard();
    void pasteEntityFromClipboard();
    bool textWidgetHasFocus() const;

    EditorBridge &_editor;
    std::unique_ptr<Ui::MainWindow> _ui;
    bool _pkmBusy = false;
};
