#include "DotNetEditorBridge.h"

#include <coreclr_delegates.h>
#include <hostfxr.h>
#include <nethost.h>

#include <QByteArray>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QStringList>
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

QString qrPlacement(int box, int slot, int copies)
{
    return QStringLiteral("%1,%2,%3").arg(box).arg(slot).arg(copies);
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

    auto getDelegate = reinterpret_cast<hostfxr_get_runtime_delegate_fn>(
        get_export(_hostfxr, "hostfxr_get_runtime_delegate"));
    _close = reinterpret_cast<close_fn>(get_export(_hostfxr, "hostfxr_close"));
    if (getDelegate == nullptr || _close == nullptr)
        throw std::runtime_error("hostfxr is missing required exports.");

    hostfxr_handle context{};
    int initRc = -1;
    const QString bundledRuntime = QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("dotnet/libcoreclr.so"));
    if (QFileInfo::exists(bundledRuntime))
    {
        auto initCmd = reinterpret_cast<hostfxr_initialize_for_dotnet_command_line_fn>(
            get_export(_hostfxr, "hostfxr_initialize_for_dotnet_command_line"));
        if (initCmd == nullptr)
            throw std::runtime_error("hostfxr is missing command-line initialization.");
        const char_t *argv[] = {assembly.c_str()};
        hostfxr_initialize_parameters initParams{};
        initParams.size = sizeof(initParams);
        const std::string hostPath = toFs(QCoreApplication::applicationFilePath());
        const std::string dotnetRoot = toFs(QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("dotnet")));
        initParams.host_path = hostPath.c_str();
        initParams.dotnet_root = dotnetRoot.c_str();
        initRc = initCmd(1, argv, &initParams, &context);
    }
    else
    {
        auto init = reinterpret_cast<hostfxr_initialize_for_runtime_config_fn>(
            get_export(_hostfxr, "hostfxr_initialize_for_runtime_config"));
        if (init == nullptr)
            throw std::runtime_error("hostfxr is missing runtimeconfig initialization.");
        initRc = init(runtimeConfig.c_str(), nullptr, &context);
    }
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
    load("GetField", &_getField);
    load("SetField", &_setField);
    load("CommitCurrent", &_commitCurrent);
    load("LegalityValid", &_legalityValid);
    load("PrepareLegalityReport", &_prepareLegalityReport);
    load("GetChoices", &_getChoices);
    load("CopyPreparedText", &_copyPreparedText);
    load("PreviewShowdown", &_previewShowdown);
    load("ImportShowdown", &_importShowdown);
    load("PrepareShowdownExport", &_prepareShowdownExport);
    load("PrepareEntityCopy", &_prepareEntityCopy);
    load("PrepareEntityFileName", &_prepareEntityFileName);
    load("ImportEntity", &_importEntity);
    load("SlotPreview", &_slotPreview);
    load("SlotCryPath", &_slotCryPath);
    load("QrHasBoxSlotCopies", &_qrHasBoxSlotCopies);
    load("PrepareQrMessage", &_prepareQrMessage);
    load("PrepareQrPng", &_prepareQrPng);
    load("ImportQrMessage", &_importQrMessage);
    load("ImportQrPng", &_importQrPng);
    load("WriteCurrentToSlot", &_writeCurrentToSlot);
    load("DeleteSlot", &_deleteSlot);
    load("SwapSlots", &_swapSlots);
    load("DropOnSlot", &_dropOnSlot);
    load("NeedsClosePrompt", &_needsClosePrompt);
    load("NeedsOpenPrompt", &_needsOpenPrompt);
    load("NeedsExportPrompt", &_needsExportPrompt);
    load("PathIsSave", &_pathIsSave);
    load("SaveUserConfig", &_saveUserConfig);
    load("ExportBackup", &_exportBackup);
    load("SaveEntityPath", &_saveEntityPath);
    load("PrepareBackupName", &_prepareBackupName);
    load("ApplyStartup", &_applyStartup);
    load("TakeBackupPrompt", &_takeBackupPrompt);
    load("CreateBackupFolder", &_createBackupFolder);
    load("IsExportable", &_isExportable);
    load("PrepareBackupPath", &_prepareBackupPath);
    load("PrepareInventory", &_prepareInventory);
    load("InventoryModify", &_inventoryModify);
    load("SaveInventory", &_saveInventory);
    load("HasPokedex", &_hasPokedex);
    load("PreparePokedex", &_preparePokedex);
    load("PokedexModify", &_pokedexModify);
    load("SavePokedex", &_savePokedex);
    load("CancelPokedex", &_cancelPokedex);
    load("HasAccessory", &_hasAccessory);
    load("PrepareAccessory", &_prepareAccessory);
    load("PrepareAccessoryPages", &_prepareAccessoryPages);
    load("AccessoryModify", &_accessoryModify);
    load("SaveAccessory", &_saveAccessory);
    load("CancelAccessory", &_cancelAccessory);
    load("HasSaveBlock", &_hasSaveBlock);
    load("PrepareSaveBlock", &_prepareSaveBlock);
    load("PrepareSaveBlockPages", &_prepareSaveBlockPages);
    load("SaveBlockModify", &_saveBlockModify);
    load("SaveSaveBlock", &_saveSaveBlock);
    load("CancelSaveBlock", &_cancelSaveBlock);
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

QString DotNetEditorBridge::readText(component_entry_point_fn prepare, const QString &key) const
{
    if (prepare == nullptr || _copyPreparedText == nullptr)
        return {};
    QByteArray utf8 = key.toUtf8();
    const int length = prepare(utf8.data(), utf8.size());
    if (length < 0)
        return {};
    if (length == 0)
        return {};
    QByteArray buf(length, '\0');
    if (_copyPreparedText(buf.data(), buf.size()) != 0)
        return {};
    return QString::fromUtf8(buf);
}

QString DotNetEditorBridge::getField(const QString &name)
{
    return readText(_getField, name);
}

bool DotNetEditorBridge::setField(const QString &name, const QString &value)
{
    return call(_setField, name + QLatin1Char('=') + value);
}

bool DotNetEditorBridge::commitCurrent()
{
    if (_commitCurrent == nullptr)
        return false;
    return _commitCurrent(nullptr, 0) == 0;
}

bool DotNetEditorBridge::legalityValid() const
{
    if (_legalityValid == nullptr)
        return false;
    return _legalityValid(nullptr, 0) == 1;
}

QString DotNetEditorBridge::legalityReport(bool verbose)
{
    return readText(_prepareLegalityReport, verbose ? QStringLiteral("1") : QStringLiteral("0"));
}

QString DotNetEditorBridge::fieldChoices(const QString &name)
{
    return readText(_getChoices, name);
}

QString DotNetEditorBridge::previewShowdown(const QString &text)
{
    return readText(_previewShowdown, text);
}

bool DotNetEditorBridge::importShowdown(const QString &text)
{
    return call(_importShowdown, text);
}

QString DotNetEditorBridge::exportShowdown(const QString &scope)
{
    return readText(_prepareShowdownExport, scope);
}

QByteArray DotNetEditorBridge::readBinary(component_entry_point_fn prepare) const
{
    return readBinary(prepare, QString());
}

QByteArray DotNetEditorBridge::readBinary(component_entry_point_fn prepare, const QString &key) const
{
    if (prepare == nullptr || _copyPreparedPng == nullptr)
        return {};
    QByteArray utf8 = key.toUtf8();
    const int length = prepare(utf8.isEmpty() ? nullptr : utf8.data(), utf8.size());
    if (length <= 0)
        return {};
    QByteArray data(length, '\0');
    if (_copyPreparedPng(data.data(), data.size()) != 0)
        return {};
    return data;
}

QByteArray DotNetEditorBridge::exportEntity()
{
    return readBinary(_prepareEntityCopy);
}

QString DotNetEditorBridge::entityFileName()
{
    return readText(_prepareEntityFileName, QStringLiteral("-"));
}

bool DotNetEditorBridge::importEntity(const QByteArray &data)
{
    if (_importEntity == nullptr || data.isEmpty())
        return false;
    return _importEntity(const_cast<void *>(static_cast<const void *>(data.constData())), data.size()) == 0;
}

QString DotNetEditorBridge::slotPreview(const QString &key)
{
    return readText(_slotPreview, key);
}

QString DotNetEditorBridge::slotCryPath(const QString &key)
{
    return readText(_slotCryPath, key);
}

bool DotNetEditorBridge::qrHasBoxSlotCopies() const
{
    if (_qrHasBoxSlotCopies == nullptr)
        return false;
    return _qrHasBoxSlotCopies(nullptr, 0) == 1;
}

QString DotNetEditorBridge::exportQrMessage(int box, int slot, int copies)
{
    return readText(_prepareQrMessage, qrPlacement(box, slot, copies));
}

QByteArray DotNetEditorBridge::exportQrPng(int box, int slot, int copies)
{
    return readBinary(_prepareQrPng, qrPlacement(box, slot, copies));
}

bool DotNetEditorBridge::importQrMessage(const QString &message)
{
    return call(_importQrMessage, message);
}

bool DotNetEditorBridge::importQrPng(const QByteArray &png)
{
    if (_importQrPng == nullptr || png.isEmpty())
        return false;
    return _importQrPng(const_cast<void *>(static_cast<const void *>(png.constData())), png.size()) == 0;
}

bool DotNetEditorBridge::writeCurrentToSlot(const QString &key)
{
    return call(_writeCurrentToSlot, key);
}

bool DotNetEditorBridge::deleteSlot(const QString &key)
{
    return call(_deleteSlot, key);
}

bool DotNetEditorBridge::swapSlots(const QString &source, const QString &destination)
{
    return call(_swapSlots, source + QLatin1Char('|') + destination);
}

bool DotNetEditorBridge::dropOnSlot(const QString &key, const QByteArray &data)
{
    if (_dropOnSlot == nullptr || data.isEmpty())
        return false;
    QByteArray payload = key.toUtf8();
    payload.append('\0');
    payload.append(data);
    return _dropOnSlot(payload.data(), payload.size()) == 0;
}

bool DotNetEditorBridge::needsClosePrompt() const
{
    if (_needsClosePrompt == nullptr)
        return false;
    return _needsClosePrompt(nullptr, 0) == 1;
}

bool DotNetEditorBridge::needsOpenPrompt() const
{
    if (_needsOpenPrompt == nullptr)
        return false;
    return _needsOpenPrompt(nullptr, 0) == 1;
}

bool DotNetEditorBridge::needsExportPrompt() const
{
    if (_needsExportPrompt == nullptr)
        return false;
    return _needsExportPrompt(nullptr, 0) == 1;
}

bool DotNetEditorBridge::pathIsSave(const QString &path)
{
    if (_pathIsSave == nullptr)
        return false;
    QByteArray utf8 = path.toUtf8();
    return _pathIsSave(utf8.data(), utf8.size()) == 1;
}

bool DotNetEditorBridge::saveUserConfig()
{
    if (_saveUserConfig == nullptr)
        return false;
    return _saveUserConfig(nullptr, 0) == 0;
}

bool DotNetEditorBridge::exportBackup(const QString &path)
{
    return call(_exportBackup, path);
}

bool DotNetEditorBridge::saveEntityPath(const QString &path)
{
    return call(_saveEntityPath, path);
}

QString DotNetEditorBridge::suggestedBackupName()
{
    return readText(_prepareBackupName, QStringLiteral("-"));
}

bool DotNetEditorBridge::applyStartup(const QStringList &args)
{
    return call(_applyStartup, args.join(QLatin1Char('\n')));
}

bool DotNetEditorBridge::takeBackupPrompt()
{
    if (_takeBackupPrompt == nullptr)
        return false;
    return _takeBackupPrompt(nullptr, 0) == 1;
}

bool DotNetEditorBridge::createBackupFolder()
{
    if (_createBackupFolder == nullptr)
        return false;
    return _createBackupFolder(nullptr, 0) == 0;
}

bool DotNetEditorBridge::isExportable() const
{
    if (_isExportable == nullptr)
        return false;
    return _isExportable(nullptr, 0) == 1;
}

QString DotNetEditorBridge::backupDirectory()
{
    return readText(_prepareBackupPath, QStringLiteral("-"));
}

QString DotNetEditorBridge::inventoryDocument()
{
    return readText(_prepareInventory, QStringLiteral("-"));
}

QString DotNetEditorBridge::inventoryModify(const QString &action, const QString &json)
{
    return readText(_inventoryModify, action + QLatin1Char('\n') + json);
}

bool DotNetEditorBridge::saveInventory(const QString &json)
{
    return call(_saveInventory, json);
}

bool DotNetEditorBridge::hasPokedex() const
{
    if (_hasPokedex == nullptr)
        return false;
    return _hasPokedex(nullptr, 0) == 1;
}

QString DotNetEditorBridge::pokedexDocument()
{
    return readText(_preparePokedex, QStringLiteral("-"));
}

QString DotNetEditorBridge::pokedexModify(const QString &action, const QString &json)
{
    return readText(_pokedexModify, action + QLatin1Char('\n') + json);
}

bool DotNetEditorBridge::savePokedex(const QString &json)
{
    return call(_savePokedex, json);
}

bool DotNetEditorBridge::cancelPokedex()
{
    if (_cancelPokedex == nullptr)
        return false;
    return _cancelPokedex(nullptr, 0) == 0;
}

bool DotNetEditorBridge::hasAccessory() const
{
    if (_hasAccessory == nullptr)
        return false;
    return _hasAccessory(nullptr, 0) == 1;
}

QString DotNetEditorBridge::accessoryDocument(const QString &page)
{
    return readText(_prepareAccessory, page.isEmpty() ? QStringLiteral("ribbons") : page);
}

QString DotNetEditorBridge::accessoryPages()
{
    return readText(_prepareAccessoryPages, QStringLiteral("-"));
}

QString DotNetEditorBridge::accessoryModify(const QString &action, const QString &json)
{
    return readText(_accessoryModify, action + QLatin1Char('\n') + json);
}

bool DotNetEditorBridge::saveAccessory(const QString &json)
{
    return call(_saveAccessory, json);
}

bool DotNetEditorBridge::cancelAccessory()
{
    if (_cancelAccessory == nullptr)
        return false;
    return _cancelAccessory(nullptr, 0) == 0;
}

bool DotNetEditorBridge::hasSaveBlock() const
{
    if (_hasSaveBlock == nullptr)
        return false;
    return _hasSaveBlock(nullptr, 0) == 1;
}

QString DotNetEditorBridge::saveBlockDocument(const QString &page)
{
    return readText(_prepareSaveBlock, page.isEmpty() ? QStringLiteral("trainer") : page);
}

QString DotNetEditorBridge::saveBlockPages()
{
    return readText(_prepareSaveBlockPages, QStringLiteral("-"));
}

QString DotNetEditorBridge::saveBlockModify(const QString &action, const QString &json)
{
    return readText(_saveBlockModify, action + QLatin1Char('\n') + json);
}

bool DotNetEditorBridge::saveSaveBlock(const QString &json)
{
    return call(_saveSaveBlock, json);
}

bool DotNetEditorBridge::cancelSaveBlock()
{
    if (_cancelSaveBlock == nullptr)
        return false;
    return _cancelSaveBlock(nullptr, 0) == 0;
}

bool DotNetEditorBridge::call(component_entry_point_fn fn, const QString &path) const
{
    if (fn == nullptr)
        return false;
    QByteArray utf8 = path.toUtf8();
    return fn(utf8.data(), utf8.size()) == 0;
}
