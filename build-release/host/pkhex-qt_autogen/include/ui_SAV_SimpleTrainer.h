/********************************************************************************
** Form generated from reading UI file 'SAV_SimpleTrainer.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SAV_SIMPLETRAINER_H
#define UI_SAV_SIMPLETRAINER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SAV_SimpleTrainer
{
public:
    QVBoxLayout *SimpleTrainerRoot;
    QGroupBox *GB_Trainer;
    QFormLayout *TrainerForm;
    QLabel *L_TrainerName;
    QHBoxLayout *OtRow;
    QLineEdit *TB_OTName;
    QComboBox *CB_Gender;
    QLabel *L_TID;
    QSpinBox *MT_TID;
    QLabel *L_SID;
    QSpinBox *MT_SID;
    QLabel *L_Money;
    QHBoxLayout *MoneyRow;
    QSpinBox *MT_Money;
    QPushButton *B_MaxCash;
    QLabel *L_Coins;
    QHBoxLayout *CoinRow;
    QSpinBox *MT_Coins;
    QPushButton *B_MaxCoins;
    QLabel *L_Country;
    QSpinBox *CB_Country;
    QLabel *L_Region;
    QSpinBox *CB_Region;
    QGroupBox *GB_Adventure;
    QFormLayout *AdventureForm;
    QLabel *L_Hours;
    QSpinBox *MT_Hours;
    QLabel *L_Minutes;
    QSpinBox *MT_Minutes;
    QLabel *L_Seconds;
    QSpinBox *MT_Seconds;
    QLabel *L_PikaFriend;
    QSpinBox *MT_PikaFriend;
    QLabel *L_PikaBeach;
    QSpinBox *MT_PikaBeach;
    QGroupBox *GB_Badges;
    QGridLayout *BadgeGrid;
    QCheckBox *CHK_1;
    QCheckBox *CHK_2;
    QCheckBox *CHK_3;
    QCheckBox *CHK_4;
    QCheckBox *CHK_5;
    QCheckBox *CHK_6;
    QCheckBox *CHK_7;
    QCheckBox *CHK_8;
    QCheckBox *CHK_H1;
    QCheckBox *CHK_H2;
    QCheckBox *CHK_H3;
    QCheckBox *CHK_H4;
    QCheckBox *CHK_H5;
    QCheckBox *CHK_H6;
    QCheckBox *CHK_H7;
    QCheckBox *CHK_H8;
    QGroupBox *GB_Options;
    QFormLayout *OptionsForm;
    QCheckBox *CHK_BattleEffects;
    QLabel *LBL_BattleStyle;
    QComboBox *CB_BattleStyle;
    QLabel *LBL_SoundType;
    QComboBox *CB_SoundType;
    QLabel *LBL_TextSpeed;
    QComboBox *CB_TextSpeed;
    QGroupBox *GB_Map;
    QFormLayout *MapForm;
    QLabel *L_CurrentMap;
    QSpinBox *NUD_M;
    QLabel *L_X;
    QSpinBox *NUD_X;
    QLabel *L_Y;
    QSpinBox *NUD_Y;
    QLabel *L_Z;
    QSpinBox *NUD_Z;
    QGroupBox *GB_Flags;
    QVBoxLayout *FlagsLayout;
    QLabel *L_EventFlagWarn;
    QTableWidget *TLP_Flags;
    QGroupBox *GB_FlagStatus;
    QHBoxLayout *FlagStatusRow;
    QCheckBox *c_CustomFlag;
    QSpinBox *NUD_Flag;
    QGroupBox *GB_Constants;
    QVBoxLayout *ConstLayout;
    QTableWidget *TLP_Const;
    QHBoxLayout *CustomWorkRow;
    QLabel *L_Stats;
    QSpinBox *CB_Stats;
    QSpinBox *MT_Stat;
    QGroupBox *GB_BlockTools;
    QVBoxLayout *BlockToolsLayout;
    QTableWidget *TLP_Fields;
    QWidget *P_BlockActions;
    QHBoxLayout *BlockActionsLayout;
    QHBoxLayout *ButtonRow;
    QSpacerItem *ButtonSpacer;
    QPushButton *B_Cancel;
    QPushButton *B_Save;

    void setupUi(QDialog *SAV_SimpleTrainer)
    {
        if (SAV_SimpleTrainer->objectName().isEmpty())
            SAV_SimpleTrainer->setObjectName("SAV_SimpleTrainer");
        SAV_SimpleTrainer->setModal(true);
        SimpleTrainerRoot = new QVBoxLayout(SAV_SimpleTrainer);
        SimpleTrainerRoot->setObjectName("SimpleTrainerRoot");
        GB_Trainer = new QGroupBox(SAV_SimpleTrainer);
        GB_Trainer->setObjectName("GB_Trainer");
        TrainerForm = new QFormLayout(GB_Trainer);
        TrainerForm->setObjectName("TrainerForm");
        L_TrainerName = new QLabel(GB_Trainer);
        L_TrainerName->setObjectName("L_TrainerName");

        TrainerForm->setWidget(0, QFormLayout::ItemRole::LabelRole, L_TrainerName);

        OtRow = new QHBoxLayout();
        OtRow->setObjectName("OtRow");
        TB_OTName = new QLineEdit(GB_Trainer);
        TB_OTName->setObjectName("TB_OTName");

        OtRow->addWidget(TB_OTName);

        CB_Gender = new QComboBox(GB_Trainer);
        CB_Gender->setObjectName("CB_Gender");

        OtRow->addWidget(CB_Gender);


        TrainerForm->setLayout(0, QFormLayout::ItemRole::FieldRole, OtRow);

        L_TID = new QLabel(GB_Trainer);
        L_TID->setObjectName("L_TID");

        TrainerForm->setWidget(1, QFormLayout::ItemRole::LabelRole, L_TID);

        MT_TID = new QSpinBox(GB_Trainer);
        MT_TID->setObjectName("MT_TID");
        MT_TID->setMaximum(65535);

        TrainerForm->setWidget(1, QFormLayout::ItemRole::FieldRole, MT_TID);

        L_SID = new QLabel(GB_Trainer);
        L_SID->setObjectName("L_SID");

        TrainerForm->setWidget(2, QFormLayout::ItemRole::LabelRole, L_SID);

        MT_SID = new QSpinBox(GB_Trainer);
        MT_SID->setObjectName("MT_SID");
        MT_SID->setMaximum(65535);

        TrainerForm->setWidget(2, QFormLayout::ItemRole::FieldRole, MT_SID);

        L_Money = new QLabel(GB_Trainer);
        L_Money->setObjectName("L_Money");

        TrainerForm->setWidget(3, QFormLayout::ItemRole::LabelRole, L_Money);

        MoneyRow = new QHBoxLayout();
        MoneyRow->setObjectName("MoneyRow");
        MT_Money = new QSpinBox(GB_Trainer);
        MT_Money->setObjectName("MT_Money");
        MT_Money->setMaximum(9999999);

        MoneyRow->addWidget(MT_Money);

        B_MaxCash = new QPushButton(GB_Trainer);
        B_MaxCash->setObjectName("B_MaxCash");

        MoneyRow->addWidget(B_MaxCash);


        TrainerForm->setLayout(3, QFormLayout::ItemRole::FieldRole, MoneyRow);

        L_Coins = new QLabel(GB_Trainer);
        L_Coins->setObjectName("L_Coins");

        TrainerForm->setWidget(4, QFormLayout::ItemRole::LabelRole, L_Coins);

        CoinRow = new QHBoxLayout();
        CoinRow->setObjectName("CoinRow");
        MT_Coins = new QSpinBox(GB_Trainer);
        MT_Coins->setObjectName("MT_Coins");
        MT_Coins->setMaximum(9999);

        CoinRow->addWidget(MT_Coins);

        B_MaxCoins = new QPushButton(GB_Trainer);
        B_MaxCoins->setObjectName("B_MaxCoins");

        CoinRow->addWidget(B_MaxCoins);


        TrainerForm->setLayout(4, QFormLayout::ItemRole::FieldRole, CoinRow);

        L_Country = new QLabel(GB_Trainer);
        L_Country->setObjectName("L_Country");

        TrainerForm->setWidget(5, QFormLayout::ItemRole::LabelRole, L_Country);

        CB_Country = new QSpinBox(GB_Trainer);
        CB_Country->setObjectName("CB_Country");
        CB_Country->setMaximum(255);

        TrainerForm->setWidget(5, QFormLayout::ItemRole::FieldRole, CB_Country);

        L_Region = new QLabel(GB_Trainer);
        L_Region->setObjectName("L_Region");

        TrainerForm->setWidget(6, QFormLayout::ItemRole::LabelRole, L_Region);

        CB_Region = new QSpinBox(GB_Trainer);
        CB_Region->setObjectName("CB_Region");
        CB_Region->setMaximum(255);

        TrainerForm->setWidget(6, QFormLayout::ItemRole::FieldRole, CB_Region);


        SimpleTrainerRoot->addWidget(GB_Trainer);

        GB_Adventure = new QGroupBox(SAV_SimpleTrainer);
        GB_Adventure->setObjectName("GB_Adventure");
        AdventureForm = new QFormLayout(GB_Adventure);
        AdventureForm->setObjectName("AdventureForm");
        L_Hours = new QLabel(GB_Adventure);
        L_Hours->setObjectName("L_Hours");

        AdventureForm->setWidget(0, QFormLayout::ItemRole::LabelRole, L_Hours);

        MT_Hours = new QSpinBox(GB_Adventure);
        MT_Hours->setObjectName("MT_Hours");
        MT_Hours->setMaximum(9999);

        AdventureForm->setWidget(0, QFormLayout::ItemRole::FieldRole, MT_Hours);

        L_Minutes = new QLabel(GB_Adventure);
        L_Minutes->setObjectName("L_Minutes");

        AdventureForm->setWidget(1, QFormLayout::ItemRole::LabelRole, L_Minutes);

        MT_Minutes = new QSpinBox(GB_Adventure);
        MT_Minutes->setObjectName("MT_Minutes");
        MT_Minutes->setMaximum(59);

        AdventureForm->setWidget(1, QFormLayout::ItemRole::FieldRole, MT_Minutes);

        L_Seconds = new QLabel(GB_Adventure);
        L_Seconds->setObjectName("L_Seconds");

        AdventureForm->setWidget(2, QFormLayout::ItemRole::LabelRole, L_Seconds);

        MT_Seconds = new QSpinBox(GB_Adventure);
        MT_Seconds->setObjectName("MT_Seconds");
        MT_Seconds->setMaximum(59);

        AdventureForm->setWidget(2, QFormLayout::ItemRole::FieldRole, MT_Seconds);

        L_PikaFriend = new QLabel(GB_Adventure);
        L_PikaFriend->setObjectName("L_PikaFriend");

        AdventureForm->setWidget(3, QFormLayout::ItemRole::LabelRole, L_PikaFriend);

        MT_PikaFriend = new QSpinBox(GB_Adventure);
        MT_PikaFriend->setObjectName("MT_PikaFriend");
        MT_PikaFriend->setMaximum(255);

        AdventureForm->setWidget(3, QFormLayout::ItemRole::FieldRole, MT_PikaFriend);

        L_PikaBeach = new QLabel(GB_Adventure);
        L_PikaBeach->setObjectName("L_PikaBeach");

        AdventureForm->setWidget(4, QFormLayout::ItemRole::LabelRole, L_PikaBeach);

        MT_PikaBeach = new QSpinBox(GB_Adventure);
        MT_PikaBeach->setObjectName("MT_PikaBeach");
        MT_PikaBeach->setMaximum(9999);

        AdventureForm->setWidget(4, QFormLayout::ItemRole::FieldRole, MT_PikaBeach);


        SimpleTrainerRoot->addWidget(GB_Adventure);

        GB_Badges = new QGroupBox(SAV_SimpleTrainer);
        GB_Badges->setObjectName("GB_Badges");
        BadgeGrid = new QGridLayout(GB_Badges);
        BadgeGrid->setObjectName("BadgeGrid");
        CHK_1 = new QCheckBox(GB_Badges);
        CHK_1->setObjectName("CHK_1");

        BadgeGrid->addWidget(CHK_1, 0, 0, 1, 1);

        CHK_2 = new QCheckBox(GB_Badges);
        CHK_2->setObjectName("CHK_2");

        BadgeGrid->addWidget(CHK_2, 0, 1, 1, 1);

        CHK_3 = new QCheckBox(GB_Badges);
        CHK_3->setObjectName("CHK_3");

        BadgeGrid->addWidget(CHK_3, 0, 2, 1, 1);

        CHK_4 = new QCheckBox(GB_Badges);
        CHK_4->setObjectName("CHK_4");

        BadgeGrid->addWidget(CHK_4, 0, 3, 1, 1);

        CHK_5 = new QCheckBox(GB_Badges);
        CHK_5->setObjectName("CHK_5");

        BadgeGrid->addWidget(CHK_5, 1, 0, 1, 1);

        CHK_6 = new QCheckBox(GB_Badges);
        CHK_6->setObjectName("CHK_6");

        BadgeGrid->addWidget(CHK_6, 1, 1, 1, 1);

        CHK_7 = new QCheckBox(GB_Badges);
        CHK_7->setObjectName("CHK_7");

        BadgeGrid->addWidget(CHK_7, 1, 2, 1, 1);

        CHK_8 = new QCheckBox(GB_Badges);
        CHK_8->setObjectName("CHK_8");

        BadgeGrid->addWidget(CHK_8, 1, 3, 1, 1);

        CHK_H1 = new QCheckBox(GB_Badges);
        CHK_H1->setObjectName("CHK_H1");

        BadgeGrid->addWidget(CHK_H1, 2, 0, 1, 1);

        CHK_H2 = new QCheckBox(GB_Badges);
        CHK_H2->setObjectName("CHK_H2");

        BadgeGrid->addWidget(CHK_H2, 2, 1, 1, 1);

        CHK_H3 = new QCheckBox(GB_Badges);
        CHK_H3->setObjectName("CHK_H3");

        BadgeGrid->addWidget(CHK_H3, 2, 2, 1, 1);

        CHK_H4 = new QCheckBox(GB_Badges);
        CHK_H4->setObjectName("CHK_H4");

        BadgeGrid->addWidget(CHK_H4, 2, 3, 1, 1);

        CHK_H5 = new QCheckBox(GB_Badges);
        CHK_H5->setObjectName("CHK_H5");

        BadgeGrid->addWidget(CHK_H5, 3, 0, 1, 1);

        CHK_H6 = new QCheckBox(GB_Badges);
        CHK_H6->setObjectName("CHK_H6");

        BadgeGrid->addWidget(CHK_H6, 3, 1, 1, 1);

        CHK_H7 = new QCheckBox(GB_Badges);
        CHK_H7->setObjectName("CHK_H7");

        BadgeGrid->addWidget(CHK_H7, 3, 2, 1, 1);

        CHK_H8 = new QCheckBox(GB_Badges);
        CHK_H8->setObjectName("CHK_H8");

        BadgeGrid->addWidget(CHK_H8, 3, 3, 1, 1);


        SimpleTrainerRoot->addWidget(GB_Badges);

        GB_Options = new QGroupBox(SAV_SimpleTrainer);
        GB_Options->setObjectName("GB_Options");
        OptionsForm = new QFormLayout(GB_Options);
        OptionsForm->setObjectName("OptionsForm");
        CHK_BattleEffects = new QCheckBox(GB_Options);
        CHK_BattleEffects->setObjectName("CHK_BattleEffects");

        OptionsForm->setWidget(0, QFormLayout::ItemRole::SpanningRole, CHK_BattleEffects);

        LBL_BattleStyle = new QLabel(GB_Options);
        LBL_BattleStyle->setObjectName("LBL_BattleStyle");

        OptionsForm->setWidget(1, QFormLayout::ItemRole::LabelRole, LBL_BattleStyle);

        CB_BattleStyle = new QComboBox(GB_Options);
        CB_BattleStyle->setObjectName("CB_BattleStyle");

        OptionsForm->setWidget(1, QFormLayout::ItemRole::FieldRole, CB_BattleStyle);

        LBL_SoundType = new QLabel(GB_Options);
        LBL_SoundType->setObjectName("LBL_SoundType");

        OptionsForm->setWidget(2, QFormLayout::ItemRole::LabelRole, LBL_SoundType);

        CB_SoundType = new QComboBox(GB_Options);
        CB_SoundType->setObjectName("CB_SoundType");

        OptionsForm->setWidget(2, QFormLayout::ItemRole::FieldRole, CB_SoundType);

        LBL_TextSpeed = new QLabel(GB_Options);
        LBL_TextSpeed->setObjectName("LBL_TextSpeed");

        OptionsForm->setWidget(3, QFormLayout::ItemRole::LabelRole, LBL_TextSpeed);

        CB_TextSpeed = new QComboBox(GB_Options);
        CB_TextSpeed->setObjectName("CB_TextSpeed");

        OptionsForm->setWidget(3, QFormLayout::ItemRole::FieldRole, CB_TextSpeed);


        SimpleTrainerRoot->addWidget(GB_Options);

        GB_Map = new QGroupBox(SAV_SimpleTrainer);
        GB_Map->setObjectName("GB_Map");
        MapForm = new QFormLayout(GB_Map);
        MapForm->setObjectName("MapForm");
        L_CurrentMap = new QLabel(GB_Map);
        L_CurrentMap->setObjectName("L_CurrentMap");

        MapForm->setWidget(0, QFormLayout::ItemRole::LabelRole, L_CurrentMap);

        NUD_M = new QSpinBox(GB_Map);
        NUD_M->setObjectName("NUD_M");
        NUD_M->setMaximum(9999);

        MapForm->setWidget(0, QFormLayout::ItemRole::FieldRole, NUD_M);

        L_X = new QLabel(GB_Map);
        L_X->setObjectName("L_X");

        MapForm->setWidget(1, QFormLayout::ItemRole::LabelRole, L_X);

        NUD_X = new QSpinBox(GB_Map);
        NUD_X->setObjectName("NUD_X");
        NUD_X->setMaximum(999999);

        MapForm->setWidget(1, QFormLayout::ItemRole::FieldRole, NUD_X);

        L_Y = new QLabel(GB_Map);
        L_Y->setObjectName("L_Y");

        MapForm->setWidget(2, QFormLayout::ItemRole::LabelRole, L_Y);

        NUD_Y = new QSpinBox(GB_Map);
        NUD_Y->setObjectName("NUD_Y");
        NUD_Y->setMaximum(999999);

        MapForm->setWidget(2, QFormLayout::ItemRole::FieldRole, NUD_Y);

        L_Z = new QLabel(GB_Map);
        L_Z->setObjectName("L_Z");

        MapForm->setWidget(3, QFormLayout::ItemRole::LabelRole, L_Z);

        NUD_Z = new QSpinBox(GB_Map);
        NUD_Z->setObjectName("NUD_Z");
        NUD_Z->setMaximum(999999);

        MapForm->setWidget(3, QFormLayout::ItemRole::FieldRole, NUD_Z);


        SimpleTrainerRoot->addWidget(GB_Map);

        GB_Flags = new QGroupBox(SAV_SimpleTrainer);
        GB_Flags->setObjectName("GB_Flags");
        FlagsLayout = new QVBoxLayout(GB_Flags);
        FlagsLayout->setObjectName("FlagsLayout");
        L_EventFlagWarn = new QLabel(GB_Flags);
        L_EventFlagWarn->setObjectName("L_EventFlagWarn");
        L_EventFlagWarn->setWordWrap(true);

        FlagsLayout->addWidget(L_EventFlagWarn);

        TLP_Flags = new QTableWidget(GB_Flags);
        if (TLP_Flags->columnCount() < 2)
            TLP_Flags->setColumnCount(2);
        TLP_Flags->setObjectName("TLP_Flags");
        TLP_Flags->setColumnCount(2);

        FlagsLayout->addWidget(TLP_Flags);


        SimpleTrainerRoot->addWidget(GB_Flags);

        GB_FlagStatus = new QGroupBox(SAV_SimpleTrainer);
        GB_FlagStatus->setObjectName("GB_FlagStatus");
        FlagStatusRow = new QHBoxLayout(GB_FlagStatus);
        FlagStatusRow->setObjectName("FlagStatusRow");
        c_CustomFlag = new QCheckBox(GB_FlagStatus);
        c_CustomFlag->setObjectName("c_CustomFlag");

        FlagStatusRow->addWidget(c_CustomFlag);

        NUD_Flag = new QSpinBox(GB_FlagStatus);
        NUD_Flag->setObjectName("NUD_Flag");
        NUD_Flag->setMaximum(65535);

        FlagStatusRow->addWidget(NUD_Flag);


        SimpleTrainerRoot->addWidget(GB_FlagStatus);

        GB_Constants = new QGroupBox(SAV_SimpleTrainer);
        GB_Constants->setObjectName("GB_Constants");
        ConstLayout = new QVBoxLayout(GB_Constants);
        ConstLayout->setObjectName("ConstLayout");
        TLP_Const = new QTableWidget(GB_Constants);
        if (TLP_Const->columnCount() < 2)
            TLP_Const->setColumnCount(2);
        TLP_Const->setObjectName("TLP_Const");
        TLP_Const->setColumnCount(2);

        ConstLayout->addWidget(TLP_Const);

        CustomWorkRow = new QHBoxLayout();
        CustomWorkRow->setObjectName("CustomWorkRow");
        L_Stats = new QLabel(GB_Constants);
        L_Stats->setObjectName("L_Stats");

        CustomWorkRow->addWidget(L_Stats);

        CB_Stats = new QSpinBox(GB_Constants);
        CB_Stats->setObjectName("CB_Stats");
        CB_Stats->setMaximum(65535);

        CustomWorkRow->addWidget(CB_Stats);

        MT_Stat = new QSpinBox(GB_Constants);
        MT_Stat->setObjectName("MT_Stat");
        MT_Stat->setMaximum(65535);

        CustomWorkRow->addWidget(MT_Stat);


        ConstLayout->addLayout(CustomWorkRow);


        SimpleTrainerRoot->addWidget(GB_Constants);

        GB_BlockTools = new QGroupBox(SAV_SimpleTrainer);
        GB_BlockTools->setObjectName("GB_BlockTools");
        BlockToolsLayout = new QVBoxLayout(GB_BlockTools);
        BlockToolsLayout->setObjectName("BlockToolsLayout");
        TLP_Fields = new QTableWidget(GB_BlockTools);
        if (TLP_Fields->columnCount() < 2)
            TLP_Fields->setColumnCount(2);
        TLP_Fields->setObjectName("TLP_Fields");
        TLP_Fields->setColumnCount(2);

        BlockToolsLayout->addWidget(TLP_Fields);

        P_BlockActions = new QWidget(GB_BlockTools);
        P_BlockActions->setObjectName("P_BlockActions");
        BlockActionsLayout = new QHBoxLayout(P_BlockActions);
        BlockActionsLayout->setObjectName("BlockActionsLayout");
        BlockActionsLayout->setContentsMargins(0, 0, 0, 0);

        BlockToolsLayout->addWidget(P_BlockActions);


        SimpleTrainerRoot->addWidget(GB_BlockTools);

        ButtonRow = new QHBoxLayout();
        ButtonRow->setObjectName("ButtonRow");
        ButtonSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        ButtonRow->addItem(ButtonSpacer);

        B_Cancel = new QPushButton(SAV_SimpleTrainer);
        B_Cancel->setObjectName("B_Cancel");

        ButtonRow->addWidget(B_Cancel);

        B_Save = new QPushButton(SAV_SimpleTrainer);
        B_Save->setObjectName("B_Save");

        ButtonRow->addWidget(B_Save);


        SimpleTrainerRoot->addLayout(ButtonRow);


        retranslateUi(SAV_SimpleTrainer);

        QMetaObject::connectSlotsByName(SAV_SimpleTrainer);
    } // setupUi

    void retranslateUi(QDialog *SAV_SimpleTrainer)
    {
        SAV_SimpleTrainer->setWindowTitle(QCoreApplication::translate("SAV_SimpleTrainer", "Trainer Data", nullptr));
        GB_Trainer->setTitle(QCoreApplication::translate("SAV_SimpleTrainer", "Trainer", nullptr));
        L_TrainerName->setText(QCoreApplication::translate("SAV_SimpleTrainer", "OT:", nullptr));
        L_TID->setText(QCoreApplication::translate("SAV_SimpleTrainer", "TID:", nullptr));
        L_SID->setText(QCoreApplication::translate("SAV_SimpleTrainer", "SID:", nullptr));
        L_Money->setText(QCoreApplication::translate("SAV_SimpleTrainer", "Money:", nullptr));
        B_MaxCash->setText(QCoreApplication::translate("SAV_SimpleTrainer", "Max", nullptr));
        L_Coins->setText(QCoreApplication::translate("SAV_SimpleTrainer", "Coins:", nullptr));
        B_MaxCoins->setText(QCoreApplication::translate("SAV_SimpleTrainer", "Max", nullptr));
        L_Country->setText(QCoreApplication::translate("SAV_SimpleTrainer", "Country:", nullptr));
        L_Region->setText(QCoreApplication::translate("SAV_SimpleTrainer", "Region:", nullptr));
        GB_Adventure->setTitle(QCoreApplication::translate("SAV_SimpleTrainer", "Adventure Info", nullptr));
        L_Hours->setText(QCoreApplication::translate("SAV_SimpleTrainer", "Hours:", nullptr));
        L_Minutes->setText(QCoreApplication::translate("SAV_SimpleTrainer", "Minutes:", nullptr));
        L_Seconds->setText(QCoreApplication::translate("SAV_SimpleTrainer", "Seconds:", nullptr));
        L_PikaFriend->setText(QCoreApplication::translate("SAV_SimpleTrainer", "Pikachu Friendship:", nullptr));
        L_PikaBeach->setText(QCoreApplication::translate("SAV_SimpleTrainer", "Pikachu Beach:", nullptr));
        GB_Badges->setTitle(QCoreApplication::translate("SAV_SimpleTrainer", "Badges", nullptr));
        CHK_1->setText(QCoreApplication::translate("SAV_SimpleTrainer", "1", nullptr));
        CHK_2->setText(QCoreApplication::translate("SAV_SimpleTrainer", "2", nullptr));
        CHK_3->setText(QCoreApplication::translate("SAV_SimpleTrainer", "3", nullptr));
        CHK_4->setText(QCoreApplication::translate("SAV_SimpleTrainer", "4", nullptr));
        CHK_5->setText(QCoreApplication::translate("SAV_SimpleTrainer", "5", nullptr));
        CHK_6->setText(QCoreApplication::translate("SAV_SimpleTrainer", "6", nullptr));
        CHK_7->setText(QCoreApplication::translate("SAV_SimpleTrainer", "7", nullptr));
        CHK_8->setText(QCoreApplication::translate("SAV_SimpleTrainer", "8", nullptr));
        CHK_H1->setText(QCoreApplication::translate("SAV_SimpleTrainer", "H1", nullptr));
        CHK_H2->setText(QCoreApplication::translate("SAV_SimpleTrainer", "H2", nullptr));
        CHK_H3->setText(QCoreApplication::translate("SAV_SimpleTrainer", "H3", nullptr));
        CHK_H4->setText(QCoreApplication::translate("SAV_SimpleTrainer", "H4", nullptr));
        CHK_H5->setText(QCoreApplication::translate("SAV_SimpleTrainer", "H5", nullptr));
        CHK_H6->setText(QCoreApplication::translate("SAV_SimpleTrainer", "H6", nullptr));
        CHK_H7->setText(QCoreApplication::translate("SAV_SimpleTrainer", "H7", nullptr));
        CHK_H8->setText(QCoreApplication::translate("SAV_SimpleTrainer", "H8", nullptr));
        GB_Options->setTitle(QCoreApplication::translate("SAV_SimpleTrainer", "Options", nullptr));
        CHK_BattleEffects->setText(QCoreApplication::translate("SAV_SimpleTrainer", "Battle Effects", nullptr));
        LBL_BattleStyle->setText(QCoreApplication::translate("SAV_SimpleTrainer", "Battle Style:", nullptr));
        LBL_SoundType->setText(QCoreApplication::translate("SAV_SimpleTrainer", "Sound:", nullptr));
        LBL_TextSpeed->setText(QCoreApplication::translate("SAV_SimpleTrainer", "Text Speed:", nullptr));
        GB_Map->setTitle(QCoreApplication::translate("SAV_SimpleTrainer", "Map", nullptr));
        L_CurrentMap->setText(QCoreApplication::translate("SAV_SimpleTrainer", "Map:", nullptr));
        L_X->setText(QCoreApplication::translate("SAV_SimpleTrainer", "X:", nullptr));
        L_Y->setText(QCoreApplication::translate("SAV_SimpleTrainer", "Y:", nullptr));
        L_Z->setText(QCoreApplication::translate("SAV_SimpleTrainer", "Z:", nullptr));
        GB_Flags->setTitle(QCoreApplication::translate("SAV_SimpleTrainer", "Event Flags", nullptr));
        L_EventFlagWarn->setText(QCoreApplication::translate("SAV_SimpleTrainer", "Altering Event Flags may impact other story events. Save file backups are recommended.", nullptr));
        GB_FlagStatus->setTitle(QCoreApplication::translate("SAV_SimpleTrainer", "Check Status", nullptr));
        c_CustomFlag->setText(QCoreApplication::translate("SAV_SimpleTrainer", "Flag:", nullptr));
        GB_Constants->setTitle(QCoreApplication::translate("SAV_SimpleTrainer", "Event Constants", nullptr));
        L_Stats->setText(QCoreApplication::translate("SAV_SimpleTrainer", "Constant:", nullptr));
        GB_BlockTools->setTitle(QCoreApplication::translate("SAV_SimpleTrainer", "Misc", nullptr));
        B_Cancel->setText(QCoreApplication::translate("SAV_SimpleTrainer", "Cancel", nullptr));
        B_Save->setText(QCoreApplication::translate("SAV_SimpleTrainer", "Save", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SAV_SimpleTrainer: public Ui_SAV_SimpleTrainer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SAV_SIMPLETRAINER_H
