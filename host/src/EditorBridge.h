#pragma once

class QString;

class EditorBridge
{
public:
    virtual ~EditorBridge() = default;
    virtual bool openPath(const QString &path) = 0;
    virtual bool savePath(const QString &path) = 0;
    virtual bool hasSession() const = 0;
};
