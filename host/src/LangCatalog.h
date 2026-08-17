#pragma once

#include <QHash>
#include <QString>

class QWidget;

class LangCatalog
{
public:
    bool loadFromFile(const QString &path);
    QString text(const QString &form, const QString &name, const QString &fallback) const;
    void apply(QWidget *root, const QString &formName) const;

private:
    QHash<QString, QString> _lines;
};
