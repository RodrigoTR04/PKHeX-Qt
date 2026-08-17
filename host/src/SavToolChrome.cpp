#include "SavToolChrome.h"

#include <QGridLayout>
#include <QPushButton>
#include <QString>
#include <QWidget>

void fillSavToolButtons(QWidget *flow)
{
    if (flow == nullptr)
        return;

    auto *layout = qobject_cast<QGridLayout *>(flow->layout());
    if (layout == nullptr)
    {
        layout = new QGridLayout(flow);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(4);
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
        layout->addWidget(button, index / 3, index % 3);
        ++index;
    }
}
