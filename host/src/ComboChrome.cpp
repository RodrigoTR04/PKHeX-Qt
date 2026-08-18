#include "ComboChrome.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QWidget>

namespace
{
constexpr int kMaxVisibleItems = 12;
}

void configureComboBox(QComboBox *box)
{
    if (box == nullptr)
        return;
    box->setMaxVisibleItems(kMaxVisibleItems);
    if (auto *view = box->view())
        view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    if (box->property("comboLimited").toBool())
        return;
    box->setProperty("comboLimited", true);
    // Qt's item-view popup (not the native menu) respects maxVisibleItems and
    // opens downward unless the remaining screen space is too small.
    box->setStyleSheet(QStringLiteral("QComboBox { combobox-popup: 0; }"));
}

void configureComboBoxes(QWidget *root)
{
    if (root == nullptr)
        return;
    if (auto *self = qobject_cast<QComboBox *>(root))
        configureComboBox(self);
    const auto boxes = root->findChildren<QComboBox *>();
    for (auto *box : boxes)
        configureComboBox(box);
}
