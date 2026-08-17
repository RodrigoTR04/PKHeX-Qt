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

private:
    using component_entry_point_fn = int (*)(void *arg, int arg_size_in_bytes);
    using close_fn = int (*)(void *context);

    bool call(component_entry_point_fn fn, const QString &path) const;
    QString readText(component_entry_point_fn prepare, const QString &key) const;

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
};
