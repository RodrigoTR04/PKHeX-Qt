#include "LangCatalog.h"

#include <QAbstractButton>
#include <QAction>
#include <QFile>
#include <QLabel>
#include <QMenu>
#include <QStringConverter>
#include <QTabWidget>
#include <QTextStream>
#include <QWidget>

bool LangCatalog::loadFromFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    while (!in.atEnd())
    {
        const QString line = in.readLine();
        const int eq = line.indexOf(QLatin1Char('='));
        if (eq <= 0)
            continue;
        _lines.insert(line.left(eq), line.mid(eq + 1));
    }
    return true;
}

QString LangCatalog::text(const QString &form, const QString &name, const QString &fallback) const
{
    const QString key = form + QLatin1Char('.') + name;
    return _lines.value(key, fallback);
}

void LangCatalog::apply(QWidget *root, const QString &formName) const
{
    if (root == nullptr)
        return;

    const auto applyOne = [&](QObject *obj) {
        const QString name = obj->objectName();
        if (name.isEmpty())
            return;
        const QString translated = text(formName, name, QString());
        if (translated.isEmpty())
            return;
        if (auto *action = qobject_cast<QAction *>(obj))
            action->setText(translated);
        else if (auto *menu = qobject_cast<QMenu *>(obj))
            menu->setTitle(translated);
        else if (auto *button = qobject_cast<QAbstractButton *>(obj))
            button->setText(translated);
        else if (auto *label = qobject_cast<QLabel *>(obj))
            label->setText(translated);
    };

    applyOne(root);
    const auto objects = root->findChildren<QObject *>();
    for (auto *obj : objects)
        applyOne(obj);

    const auto tabs = root->findChildren<QTabWidget *>();
    for (auto *tabsWidget : tabs)
    {
        for (int i = 0; i < tabsWidget->count(); ++i)
        {
            QWidget *page = tabsWidget->widget(i);
            if (page == nullptr)
                continue;
            const QString translated = text(formName, page->objectName(), tabsWidget->tabText(i));
            tabsWidget->setTabText(i, translated);
        }
    }
}
