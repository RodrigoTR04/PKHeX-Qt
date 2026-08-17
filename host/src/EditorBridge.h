#pragma once

class QString;
class QByteArray;

struct StorageLayout
{
    int boxCount = 24;
    int boxSlotCount = 30;
    int partySlotCount = 6;
    int currentBox = 0;
};

class EditorBridge
{
public:
    virtual ~EditorBridge() = default;
    virtual bool openPath(const QString &path) = 0;
    virtual bool savePath(const QString &path) = 0;
    virtual bool hasSession() const = 0;
    virtual bool selectSlot(const QString &key) = 0;
    virtual bool setCurrentBox(int box) = 0;
    virtual bool storageLayout(StorageLayout &out) const = 0;
    virtual QByteArray slotPng(const QString &key) = 0;
    virtual QString getField(const QString &name) = 0;
    virtual bool setField(const QString &name, const QString &value) = 0;
    virtual bool commitCurrent() = 0;
    virtual bool legalityValid() const = 0;
    virtual QString legalityReport(bool verbose) = 0;
    virtual QString fieldChoices(const QString &name) = 0;
    virtual QString previewShowdown(const QString &text) = 0;
    virtual bool importShowdown(const QString &text) = 0;
    virtual QString exportShowdown(const QString &scope) = 0;
    virtual QByteArray exportEntity() = 0;
    virtual QString entityFileName() = 0;
    virtual bool importEntity(const QByteArray &data) = 0;
    virtual QString slotPreview(const QString &key) = 0;
    virtual bool writeCurrentToSlot(const QString &key) = 0;
    virtual bool deleteSlot(const QString &key) = 0;
    virtual bool swapSlots(const QString &source, const QString &destination) = 0;
    virtual bool dropOnSlot(const QString &key, const QByteArray &data) = 0;
    virtual bool needsClosePrompt() const = 0;
    virtual bool needsOpenPrompt() const = 0;
    virtual bool needsExportPrompt() const = 0;
    virtual bool pathIsSave(const QString &path) = 0;
    virtual bool saveUserConfig() = 0;
    virtual bool exportBackup(const QString &path) = 0;
    virtual bool saveEntityPath(const QString &path) = 0;
    virtual QString suggestedBackupName() = 0;
};
