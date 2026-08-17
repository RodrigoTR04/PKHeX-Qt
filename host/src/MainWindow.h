#pragma once

#include "EditorBridge.h"

#include <QMainWindow>
#include <QPoint>
#include <QString>
#include <QStringList>
#include <memory>

class QCloseEvent;
class QDragEnterEvent;
class QDropEvent;
class QKeyEvent;
class QLabel;
class QMimeData;
class QMouseEvent;

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
    bool applyStartup(const QStringList &args);
    void promptBackupFolder();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onMenuOpen();
    void onMenuExportSav();
    void onMenuSavePkm();
    void onMenuExportBak();
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
    bool handleSlotMouse(QObject *watched, QEvent *event);
    void startSlotDrag(QLabel *slot, const QString &key);
    void applyDrop(QDropEvent *event, const QString &destKey);
    QByteArray entityBytesFromMime(const QMimeData *mime) const;
    bool confirmYesNo(const QString &text, const QString &informative);
    bool confirmOpenSave();
    bool confirmExportSave();
    bool confirmClose();
    void syncSession();

    EditorBridge &_editor;
    std::unique_ptr<Ui::MainWindow> _ui;
    bool _pkmBusy = false;
    QPoint _pressPos;
    QString _pressKey;
    bool _dragging = false;
};
