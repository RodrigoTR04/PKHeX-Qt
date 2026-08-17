#pragma once

#include "EditorBridge.h"

class DotNetEditorBridge : public EditorBridge
{
public:
    DotNetEditorBridge();
    ~DotNetEditorBridge() override;

    bool openPath(const QString &path) override;
    bool savePath(const QString &path) override;
    bool hasSession() const override;

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
};
