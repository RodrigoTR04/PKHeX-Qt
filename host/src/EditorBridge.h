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
};
