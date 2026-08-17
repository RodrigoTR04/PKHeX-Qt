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

private:
    using component_entry_point_fn = int (*)(void *arg, int arg_size_in_bytes);
    using close_fn = int (*)(void *context);

    bool call(component_entry_point_fn fn, const QString &path) const;

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
};
