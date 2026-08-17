#pragma once

#include "EditorBridge.h"

#include <QByteArray>
#include <QString>

class DotNetEditorBridge : public EditorBridge
{
public:
    DotNetEditorBridge();
    ~DotNetEditorBridge() override;

    bool openPath(const QString &path) override;
    bool savePath(const QString &path) override;
    bool hasSession() const override;
    bool selectSlot(const QString &key) override;
    bool setCurrentBox(int box) override;
    bool storageLayout(StorageLayout &out) const override;
    QByteArray slotPng(const QString &key) override;
    QString getField(const QString &name) override;
    bool setField(const QString &name, const QString &value) override;
    bool commitCurrent() override;
    bool legalityValid() const override;
    QString legalityReport(bool verbose) override;
    QString fieldChoices(const QString &name) override;
    QString previewShowdown(const QString &text) override;
    bool importShowdown(const QString &text) override;
    QString exportShowdown(const QString &scope) override;
    QByteArray exportEntity() override;
    QString entityFileName() override;
    bool importEntity(const QByteArray &data) override;
    QString slotPreview(const QString &key) override;
    bool writeCurrentToSlot(const QString &key) override;
    bool deleteSlot(const QString &key) override;
    bool swapSlots(const QString &source, const QString &destination) override;
    bool dropOnSlot(const QString &key, const QByteArray &data) override;
    bool needsClosePrompt() const override;
    bool needsOpenPrompt() const override;
    bool needsExportPrompt() const override;
    bool pathIsSave(const QString &path) override;
    bool saveUserConfig() override;
    bool exportBackup(const QString &path) override;
    bool saveEntityPath(const QString &path) override;
    QString suggestedBackupName() override;
    bool applyStartup(const QStringList &args) override;
    bool takeBackupPrompt() override;
    bool createBackupFolder() override;
    bool isExportable() const override;
    QString backupDirectory() override;

private:
    using component_entry_point_fn = int (*)(void *arg, int arg_size_in_bytes);
    using close_fn = int (*)(void *context);

    bool call(component_entry_point_fn fn, const QString &path) const;
    QString readText(component_entry_point_fn prepare, const QString &key) const;

    QByteArray readBinary(component_entry_point_fn prepare) const;

    void *_hostfxr{};
    void *_context{};
    close_fn _close{};
    component_entry_point_fn _openPath{};
    component_entry_point_fn _savePath{};
    component_entry_point_fn _hasSession{};
    component_entry_point_fn _selectSlot{};
    component_entry_point_fn _setCurrentBox{};
    component_entry_point_fn _getStorageLayout{};
    component_entry_point_fn _prepareSlotPng{};
    component_entry_point_fn _copyPreparedPng{};
    component_entry_point_fn _getField{};
    component_entry_point_fn _setField{};
    component_entry_point_fn _commitCurrent{};
    component_entry_point_fn _legalityValid{};
    component_entry_point_fn _prepareLegalityReport{};
    component_entry_point_fn _getChoices{};
    component_entry_point_fn _copyPreparedText{};
    component_entry_point_fn _previewShowdown{};
    component_entry_point_fn _importShowdown{};
    component_entry_point_fn _prepareShowdownExport{};
    component_entry_point_fn _prepareEntityCopy{};
    component_entry_point_fn _prepareEntityFileName{};
    component_entry_point_fn _importEntity{};
    component_entry_point_fn _slotPreview{};
    component_entry_point_fn _writeCurrentToSlot{};
    component_entry_point_fn _deleteSlot{};
    component_entry_point_fn _swapSlots{};
    component_entry_point_fn _dropOnSlot{};
    component_entry_point_fn _needsClosePrompt{};
    component_entry_point_fn _needsOpenPrompt{};
    component_entry_point_fn _needsExportPrompt{};
    component_entry_point_fn _pathIsSave{};
    component_entry_point_fn _saveUserConfig{};
    component_entry_point_fn _exportBackup{};
    component_entry_point_fn _saveEntityPath{};
    component_entry_point_fn _prepareBackupName{};
    component_entry_point_fn _applyStartup{};
    component_entry_point_fn _takeBackupPrompt{};
    component_entry_point_fn _createBackupFolder{};
    component_entry_point_fn _isExportable{};
    component_entry_point_fn _prepareBackupPath{};
};
