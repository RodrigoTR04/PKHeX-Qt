#include "SavToolChrome.h"

#include <QGridLayout>
#include <QPushButton>
#include <QSizePolicy>
#include <QString>
#include <QStringList>
#include <QWidget>
#include <algorithm>
#include <vector>

void fillSavToolButtons(QWidget *flow)
{
    if (flow == nullptr)
        return;

    auto *layout = qobject_cast<QGridLayout *>(flow->layout());
    if (layout == nullptr)
    {
        layout = new QGridLayout(flow);
        layout->setContentsMargins(4, 4, 4, 4);
        layout->setSpacing(8);
        layout->setColumnStretch(0, 1);
        layout->setColumnStretch(1, 1);
        layout->setColumnStretch(2, 1);
    }

    static constexpr const char *kNames[] = {
        "B_OpenTrainerInfo",
        "B_OpenItemPouch",
        "B_OpenBoxLayout",
        "B_OpenWondercards",
        "B_OpenOPowers",
        "B_OpenEventFlags",
        "B_OpenPokedex",
        "B_OpenLinkInfo",
        "B_OpenBerryField",
        "B_OpenPokeblocks",
        "B_OpenSecretBase",
        "B_OpenPokepuffs",
        "B_OpenSuperTraining",
        "B_OpenHallofFame",
        "B_OUTPasserby",
        "B_DLC",
        "B_Donuts",
        "B_OpenPokeBeans",
        "B_CellsStickers",
        "B_OpenMiscEditor",
        "B_OpenHoneyTreeEditor",
        "B_OpenFriendSafari",
        "B_OpenRTCEditor",
        "B_OpenUGSEditor",
        "B_OpenGeonetEditor",
        "B_OpenUnityTowerEditor",
        "B_OpenJoinAvenueEditor",
        "B_OpenPokeathlon",
        "B_OpenMedalsEditor",
        "B_OpenChatterEditor",
        "B_Roamer",
        "B_FestivalPlaza",
        "B_MailBox",
        "B_OpenApricorn",
        "B_Raids",
        "B_RaidsDLC1",
        "B_RaidsDLC2",
        "B_Blocks",
        "B_OtherSlots",
        "B_OpenSealStickers",
        "B_Poffins",
        "B_RaidsSevenStar",
        "B_OpenBattlePass",
        "B_OpenGear",
        "B_OpenFashion",
        "B_OpenGlobalLink",
    };

    int index = 0;
    for (const char *name : kNames)
    {
        auto *button = new QPushButton(flow);
        button->setObjectName(QString::fromLatin1(name));
        button->setText(QString::fromLatin1(name));
        button->setMinimumSize(132, 44);
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        layout->addWidget(button, index / 3, index % 3);
        ++index;
    }
}

void applySavToolVisibility(QWidget *window, const QStringList &visible)
{
    if (window == nullptr)
        return;

    static constexpr const char *kUtilities[] = {
        "B_SaveBoxBin",
        "B_VerifyCHK",
        "B_VerifySaveEntities",
        "Menu_ExportBAK",
        "B_JPEG",
        "B_ConvertKorean",
    };
    for (const char *name : kUtilities)
    {
        if (auto *btn = window->findChild<QPushButton *>(QString::fromLatin1(name)))
            btn->setVisible(visible.contains(QString::fromLatin1(name)));
    }

    auto *flow = window->findChild<QWidget *>(QStringLiteral("FLP_SAVtools"));
    if (flow == nullptr)
        return;
    auto *layout = qobject_cast<QGridLayout *>(flow->layout());
    if (layout == nullptr)
        return;

    const auto buttons = flow->findChildren<QPushButton *>(QString(), Qt::FindDirectChildrenOnly);
    std::vector<QPushButton *> shown;
    for (auto *button : buttons)
    {
        const bool on = visible.contains(button->objectName());
        button->setVisible(on);
        if (on)
            shown.push_back(button);
    }
    std::sort(shown.begin(), shown.end(), [](QPushButton *a, QPushButton *b) {
        return QString::localeAwareCompare(a->text(), b->text()) < 0;
    });

    while (layout->count() > 0)
    {
        auto *item = layout->takeAt(0);
        delete item;
    }
    int index = 0;
    for (auto *button : shown)
    {
        layout->addWidget(button, index / 3, index % 3);
        ++index;
    }
    const int rows = (index + 2) / 3;
    for (int row = 0; row < layout->rowCount(); ++row)
        layout->setRowStretch(row, row < rows ? 1 : 0);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 1);
    layout->setColumnStretch(2, 1);
}
