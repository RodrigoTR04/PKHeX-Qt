/********************************************************************************
** Form generated from reading UI file 'SAV_Pokedex.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SAV_POKEDEX_H
#define UI_SAV_POKEDEX_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SAV_Pokedex5
{
public:
    QHBoxLayout *PokedexRoot;
    QVBoxLayout *PokedexListCol;
    QHBoxLayout *gotoRow;
    QLabel *L_goto;
    QComboBox *CB_Species;
    QListWidget *LB_Species;
    QVBoxLayout *PokedexFlagsCol;
    QGroupBox *GB_Owned;
    QVBoxLayout *ownedLay;
    QCheckBox *CHK_P1;
    QGroupBox *GB_Encountered;
    QGridLayout *seenLay;
    QCheckBox *CHK_P2;
    QCheckBox *CHK_P3;
    QCheckBox *CHK_P4;
    QCheckBox *CHK_P5;
    QGroupBox *GB_Displayed;
    QGridLayout *dispLay;
    QCheckBox *CHK_P6;
    QCheckBox *CHK_P7;
    QCheckBox *CHK_P8;
    QCheckBox *CHK_P9;
    QGroupBox *GB_Language;
    QGridLayout *langLay;
    QCheckBox *CHK_L1;
    QCheckBox *CHK_L2;
    QCheckBox *CHK_L3;
    QCheckBox *CHK_L4;
    QCheckBox *CHK_L5;
    QCheckBox *CHK_L6;
    QCheckBox *CHK_L7;
    QCheckBox *CHK_L8;
    QCheckBox *CHK_L9;
    QCheckBox *CHK_NationalDexUnlocked;
    QCheckBox *CHK_NationalDexActive;
    QHBoxLayout *spindaRow;
    QLabel *L_Spinda;
    QLineEdit *TB_PID;
    QHBoxLayout *pokedexButtons;
    QPushButton *B_GiveAll;
    QPushButton *B_Modify;
    QSpacerItem *pokedexBtnSpacer;
    QPushButton *B_Save;
    QPushButton *B_Cancel;

    void setupUi(QDialog *SAV_Pokedex5)
    {
        if (SAV_Pokedex5->objectName().isEmpty())
            SAV_Pokedex5->setObjectName("SAV_Pokedex5");
        SAV_Pokedex5->setModal(true);
        SAV_Pokedex5->setMinimumSize(QSize(480, 360));
        PokedexRoot = new QHBoxLayout(SAV_Pokedex5);
        PokedexRoot->setObjectName("PokedexRoot");
        PokedexListCol = new QVBoxLayout();
        PokedexListCol->setObjectName("PokedexListCol");
        gotoRow = new QHBoxLayout();
        gotoRow->setObjectName("gotoRow");
        L_goto = new QLabel(SAV_Pokedex5);
        L_goto->setObjectName("L_goto");

        gotoRow->addWidget(L_goto);

        CB_Species = new QComboBox(SAV_Pokedex5);
        CB_Species->setObjectName("CB_Species");

        gotoRow->addWidget(CB_Species);


        PokedexListCol->addLayout(gotoRow);

        LB_Species = new QListWidget(SAV_Pokedex5);
        LB_Species->setObjectName("LB_Species");

        PokedexListCol->addWidget(LB_Species);


        PokedexRoot->addLayout(PokedexListCol);

        PokedexFlagsCol = new QVBoxLayout();
        PokedexFlagsCol->setObjectName("PokedexFlagsCol");
        GB_Owned = new QGroupBox(SAV_Pokedex5);
        GB_Owned->setObjectName("GB_Owned");
        ownedLay = new QVBoxLayout(GB_Owned);
        ownedLay->setObjectName("ownedLay");
        CHK_P1 = new QCheckBox(GB_Owned);
        CHK_P1->setObjectName("CHK_P1");

        ownedLay->addWidget(CHK_P1);


        PokedexFlagsCol->addWidget(GB_Owned);

        GB_Encountered = new QGroupBox(SAV_Pokedex5);
        GB_Encountered->setObjectName("GB_Encountered");
        seenLay = new QGridLayout(GB_Encountered);
        seenLay->setObjectName("seenLay");
        CHK_P2 = new QCheckBox(GB_Encountered);
        CHK_P2->setObjectName("CHK_P2");

        seenLay->addWidget(CHK_P2, 0, 0, 1, 1);

        CHK_P3 = new QCheckBox(GB_Encountered);
        CHK_P3->setObjectName("CHK_P3");

        seenLay->addWidget(CHK_P3, 0, 1, 1, 1);

        CHK_P4 = new QCheckBox(GB_Encountered);
        CHK_P4->setObjectName("CHK_P4");

        seenLay->addWidget(CHK_P4, 1, 0, 1, 1);

        CHK_P5 = new QCheckBox(GB_Encountered);
        CHK_P5->setObjectName("CHK_P5");

        seenLay->addWidget(CHK_P5, 1, 1, 1, 1);


        PokedexFlagsCol->addWidget(GB_Encountered);

        GB_Displayed = new QGroupBox(SAV_Pokedex5);
        GB_Displayed->setObjectName("GB_Displayed");
        dispLay = new QGridLayout(GB_Displayed);
        dispLay->setObjectName("dispLay");
        CHK_P6 = new QCheckBox(GB_Displayed);
        CHK_P6->setObjectName("CHK_P6");

        dispLay->addWidget(CHK_P6, 0, 0, 1, 1);

        CHK_P7 = new QCheckBox(GB_Displayed);
        CHK_P7->setObjectName("CHK_P7");

        dispLay->addWidget(CHK_P7, 0, 1, 1, 1);

        CHK_P8 = new QCheckBox(GB_Displayed);
        CHK_P8->setObjectName("CHK_P8");

        dispLay->addWidget(CHK_P8, 1, 0, 1, 1);

        CHK_P9 = new QCheckBox(GB_Displayed);
        CHK_P9->setObjectName("CHK_P9");

        dispLay->addWidget(CHK_P9, 1, 1, 1, 1);


        PokedexFlagsCol->addWidget(GB_Displayed);

        GB_Language = new QGroupBox(SAV_Pokedex5);
        GB_Language->setObjectName("GB_Language");
        langLay = new QGridLayout(GB_Language);
        langLay->setObjectName("langLay");
        CHK_L1 = new QCheckBox(GB_Language);
        CHK_L1->setObjectName("CHK_L1");

        langLay->addWidget(CHK_L1, 0, 0, 1, 1);

        CHK_L2 = new QCheckBox(GB_Language);
        CHK_L2->setObjectName("CHK_L2");

        langLay->addWidget(CHK_L2, 0, 1, 1, 1);

        CHK_L3 = new QCheckBox(GB_Language);
        CHK_L3->setObjectName("CHK_L3");

        langLay->addWidget(CHK_L3, 1, 0, 1, 1);

        CHK_L4 = new QCheckBox(GB_Language);
        CHK_L4->setObjectName("CHK_L4");

        langLay->addWidget(CHK_L4, 1, 1, 1, 1);

        CHK_L5 = new QCheckBox(GB_Language);
        CHK_L5->setObjectName("CHK_L5");

        langLay->addWidget(CHK_L5, 2, 0, 1, 1);

        CHK_L6 = new QCheckBox(GB_Language);
        CHK_L6->setObjectName("CHK_L6");

        langLay->addWidget(CHK_L6, 2, 1, 1, 1);

        CHK_L7 = new QCheckBox(GB_Language);
        CHK_L7->setObjectName("CHK_L7");

        langLay->addWidget(CHK_L7, 3, 0, 1, 1);

        CHK_L8 = new QCheckBox(GB_Language);
        CHK_L8->setObjectName("CHK_L8");

        langLay->addWidget(CHK_L8, 3, 1, 1, 1);

        CHK_L9 = new QCheckBox(GB_Language);
        CHK_L9->setObjectName("CHK_L9");

        langLay->addWidget(CHK_L9, 4, 0, 1, 1);


        PokedexFlagsCol->addWidget(GB_Language);

        CHK_NationalDexUnlocked = new QCheckBox(SAV_Pokedex5);
        CHK_NationalDexUnlocked->setObjectName("CHK_NationalDexUnlocked");

        PokedexFlagsCol->addWidget(CHK_NationalDexUnlocked);

        CHK_NationalDexActive = new QCheckBox(SAV_Pokedex5);
        CHK_NationalDexActive->setObjectName("CHK_NationalDexActive");

        PokedexFlagsCol->addWidget(CHK_NationalDexActive);

        spindaRow = new QHBoxLayout();
        spindaRow->setObjectName("spindaRow");
        L_Spinda = new QLabel(SAV_Pokedex5);
        L_Spinda->setObjectName("L_Spinda");

        spindaRow->addWidget(L_Spinda);

        TB_PID = new QLineEdit(SAV_Pokedex5);
        TB_PID->setObjectName("TB_PID");

        spindaRow->addWidget(TB_PID);


        PokedexFlagsCol->addLayout(spindaRow);

        pokedexButtons = new QHBoxLayout();
        pokedexButtons->setObjectName("pokedexButtons");
        B_GiveAll = new QPushButton(SAV_Pokedex5);
        B_GiveAll->setObjectName("B_GiveAll");

        pokedexButtons->addWidget(B_GiveAll);

        B_Modify = new QPushButton(SAV_Pokedex5);
        B_Modify->setObjectName("B_Modify");

        pokedexButtons->addWidget(B_Modify);

        pokedexBtnSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        pokedexButtons->addItem(pokedexBtnSpacer);

        B_Save = new QPushButton(SAV_Pokedex5);
        B_Save->setObjectName("B_Save");

        pokedexButtons->addWidget(B_Save);

        B_Cancel = new QPushButton(SAV_Pokedex5);
        B_Cancel->setObjectName("B_Cancel");

        pokedexButtons->addWidget(B_Cancel);


        PokedexFlagsCol->addLayout(pokedexButtons);


        PokedexRoot->addLayout(PokedexFlagsCol);


        retranslateUi(SAV_Pokedex5);

        QMetaObject::connectSlotsByName(SAV_Pokedex5);
    } // setupUi

    void retranslateUi(QDialog *SAV_Pokedex5)
    {
        SAV_Pokedex5->setWindowTitle(QCoreApplication::translate("SAV_Pokedex5", "Pok\303\251dex Editor", nullptr));
        L_goto->setText(QCoreApplication::translate("SAV_Pokedex5", "goto:", nullptr));
        GB_Owned->setTitle(QCoreApplication::translate("SAV_Pokedex5", "Owned", nullptr));
        CHK_P1->setText(QCoreApplication::translate("SAV_Pokedex5", "Owned", nullptr));
        GB_Encountered->setTitle(QCoreApplication::translate("SAV_Pokedex5", "Seen", nullptr));
        CHK_P2->setText(QCoreApplication::translate("SAV_Pokedex5", "Male", nullptr));
        CHK_P3->setText(QCoreApplication::translate("SAV_Pokedex5", "Female", nullptr));
        CHK_P4->setText(QCoreApplication::translate("SAV_Pokedex5", "Shiny Male", nullptr));
        CHK_P5->setText(QCoreApplication::translate("SAV_Pokedex5", "Shiny Female", nullptr));
        GB_Displayed->setTitle(QCoreApplication::translate("SAV_Pokedex5", "Displayed", nullptr));
        CHK_P6->setText(QCoreApplication::translate("SAV_Pokedex5", "Male", nullptr));
        CHK_P7->setText(QCoreApplication::translate("SAV_Pokedex5", "Female", nullptr));
        CHK_P8->setText(QCoreApplication::translate("SAV_Pokedex5", "Shiny Male", nullptr));
        CHK_P9->setText(QCoreApplication::translate("SAV_Pokedex5", "Shiny Female", nullptr));
        GB_Language->setTitle(QCoreApplication::translate("SAV_Pokedex5", "Languages", nullptr));
        CHK_L1->setText(QCoreApplication::translate("SAV_Pokedex5", "Japanese", nullptr));
        CHK_L2->setText(QCoreApplication::translate("SAV_Pokedex5", "English", nullptr));
        CHK_L3->setText(QCoreApplication::translate("SAV_Pokedex5", "French", nullptr));
        CHK_L4->setText(QCoreApplication::translate("SAV_Pokedex5", "Italian", nullptr));
        CHK_L5->setText(QCoreApplication::translate("SAV_Pokedex5", "German", nullptr));
        CHK_L6->setText(QCoreApplication::translate("SAV_Pokedex5", "Spanish", nullptr));
        CHK_L7->setText(QCoreApplication::translate("SAV_Pokedex5", "Korean", nullptr));
        CHK_L8->setText(QCoreApplication::translate("SAV_Pokedex5", "ChineseS", nullptr));
        CHK_L9->setText(QCoreApplication::translate("SAV_Pokedex5", "ChineseT", nullptr));
        CHK_NationalDexUnlocked->setText(QCoreApplication::translate("SAV_Pokedex5", "National Mode Unlocked", nullptr));
        CHK_NationalDexActive->setText(QCoreApplication::translate("SAV_Pokedex5", "National Mode Active", nullptr));
        L_Spinda->setText(QCoreApplication::translate("SAV_Pokedex5", "Spinda:", nullptr));
        B_GiveAll->setText(QCoreApplication::translate("SAV_Pokedex5", "Check All", nullptr));
        B_Modify->setText(QCoreApplication::translate("SAV_Pokedex5", "Modify...", nullptr));
        B_Save->setText(QCoreApplication::translate("SAV_Pokedex5", "Save", nullptr));
        B_Cancel->setText(QCoreApplication::translate("SAV_Pokedex5", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SAV_Pokedex5: public Ui_SAV_Pokedex5 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SAV_POKEDEX_H
