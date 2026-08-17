#include "DotNetEditorBridge.h"

#include <coreclr_delegates.h>
#include <hostfxr.h>
#include <nethost.h>

#include <QByteArray>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <stdexcept>
#include <string>

#if defined(_WIN32)
#include <windows.h>
static void *load_library(const char_t *path)
{
    return static_cast<void *>(LoadLibraryW(path));
}
static void *get_export(void *library, const char *name)
{
    return reinterpret_cast<void *>(GetProcAddress(static_cast<HMODULE>(library), name));
}
#else
#include <dlfcn.h>
static void *load_library(const char_t *path)
{
    return dlopen(path, RTLD_LAZY | RTLD_LOCAL);
}
static void *get_export(void *library, const char *name)
{
    return dlsym(library, name);
}
#endif

namespace
{
QString editorRuntimeConfigPath()
{
    const QDir dir(QCoreApplication::applicationDirPath());
    return dir.filePath(QStringLiteral("dotnet/PKHeX.Editor.runtimeconfig.json"));
}

QString editorAssemblyPath()
{
    const QDir dir(QCoreApplication::applicationDirPath());
    return dir.filePath(QStringLiteral("dotnet/PKHeX.Editor.dll"));
}

std::string toFs(const QString &path)
{
    return QDir::toNativeSeparators(path).toStdString();
}
}

DotNetEditorBridge::DotNetEditorBridge()
{
    const std::string runtimeConfig = toFs(editorRuntimeConfigPath());
    const std::string assembly = toFs(editorAssemblyPath());
    if (!QFileInfo::exists(QString::fromStdString(runtimeConfig)))
        throw std::runtime_error("Editor runtimeconfig.json was not found next to the Host.");

    char_t hostfxrPath[4096];
    size_t hostfxrSize = sizeof(hostfxrPath) / sizeof(char_t);
    get_hostfxr_parameters params{};
    params.size = sizeof(params);
    const std::string assemblyPath = assembly;
    params.assembly_path = assemblyPath.c_str();
    if (get_hostfxr_path(hostfxrPath, &hostfxrSize, &params) != 0)
        throw std::runtime_error("Could not locate hostfxr.");

    _hostfxr = load_library(hostfxrPath);
    if (_hostfxr == nullptr)
        throw std::runtime_error("Could not load hostfxr.");

    auto init = reinterpret_cast<hostfxr_initialize_for_runtime_config_fn>(
        get_export(_hostfxr, "hostfxr_initialize_for_runtime_config"));
    auto getDelegate = reinterpret_cast<hostfxr_get_runtime_delegate_fn>(
        get_export(_hostfxr, "hostfxr_get_runtime_delegate"));
    _close = reinterpret_cast<close_fn>(get_export(_hostfxr, "hostfxr_close"));
    if (init == nullptr || getDelegate == nullptr || _close == nullptr)
        throw std::runtime_error("hostfxr is missing required exports.");

    hostfxr_handle context{};
    const int initRc = init(runtimeConfig.c_str(), nullptr, &context);
    if ((initRc != 0 && initRc != 1 && initRc != 2) || context == nullptr)
        throw std::runtime_error("Failed to initialize the .NET runtime.");
    _context = context;

    load_assembly_and_get_function_pointer_fn loadFn{};
    if (getDelegate(context, hdt_load_assembly_and_get_function_pointer, reinterpret_cast<void **>(&loadFn)) != 0
        || loadFn == nullptr)
        throw std::runtime_error("Failed to get load_assembly_and_get_function_pointer.");

    const auto typeName = "PKHeX.Editor.NativeExports, PKHeX.Editor";
    const auto load = [&](const char *method, component_entry_point_fn *out) {
        const int rc = loadFn(
            assembly.c_str(),
            typeName,
            method,
            nullptr,
            nullptr,
            reinterpret_cast<void **>(out));
        if (rc != 0 || *out == nullptr)
            throw std::runtime_error(std::string("Failed to bind ") + method);
    };

    load("OpenPath", &_openPath);
    load("SavePath", &_savePath);
    load("HasSession", &_hasSession);
    load("SelectSlot", &_selectSlot);
    load("SetCurrentBox", &_setCurrentBox);
    load("GetStorageLayout", &_getStorageLayout);
    load("PrepareSlotPng", &_prepareSlotPng);
    load("CopyPreparedPng", &_copyPreparedPng);
}

DotNetEditorBridge::~DotNetEditorBridge()
{
    if (_close != nullptr && _context != nullptr)
        _close(_context);
}

bool DotNetEditorBridge::openPath(const QString &path)
{
    return call(_openPath, path);
}

bool DotNetEditorBridge::savePath(const QString &path)
{
    return call(_savePath, path);
}

bool DotNetEditorBridge::hasSession() const
{
    if (_hasSession == nullptr)
        return false;
    return _hasSession(nullptr, 0) == 1;
}

bool DotNetEditorBridge::selectSlot(const QString &key)
{
    return call(_selectSlot, key);
}

bool DotNetEditorBridge::setCurrentBox(int box)
{
    return call(_setCurrentBox, QString::number(box));
}

bool DotNetEditorBridge::storageLayout(StorageLayout &out) const
{
    if (_getStorageLayout == nullptr)
        return false;
    int values[4]{};
    if (_getStorageLayout(values, static_cast<int>(sizeof(values))) != 0)
        return false;
    out.boxCount = values[0];
    out.boxSlotCount = values[1];
    out.partySlotCount = values[2];
    out.currentBox = values[3];
    return true;
}

QByteArray DotNetEditorBridge::slotPng(const QString &key)
{
    if (_prepareSlotPng == nullptr || _copyPreparedPng == nullptr)
        return {};
    QByteArray utf8 = key.toUtf8();
    const int length = _prepareSlotPng(utf8.data(), utf8.size());
    if (length <= 0)
        return {};
    QByteArray png(length, '\0');
    if (_copyPreparedPng(png.data(), png.size()) != 0)
        return {};
    return png;
}

bool DotNetEditorBridge::call(component_entry_point_fn fn, const QString &path) const
{
    if (fn == nullptr)
        return false;
    QByteArray utf8 = path.toUtf8();
    return fn(utf8.data(), utf8.size()) == 0;
}
