/********************************************************************************
** Form generated from reading UI file 'SAV_Encounters.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SAV_ENCOUNTERS_H
#define UI_SAV_ENCOUNTERS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SAV_Encounters
{
public:
    QHBoxLayout *Workspace;
    QWidget *P_Results;
    QVBoxLayout *ResultsCol;
    QLabel *L_Count;
    QTableWidget *EncounterPokeGrid;
    QLabel *L_Viewed;
    QVBoxLayout *FilterRail;
    QTabWidget *TC_SearchOptions;
    QWidget *Tab_General;
    QFormLayout *TLP_Filters;
    QLabel *Label_Species;
    QComboBox *CB_Species;
    QLabel *L_Move1;
    QComboBox *CB_Move1;
    QLabel *L_Move2;
    QComboBox *CB_Move2;
    QLabel *L_Move3;
    QComboBox *CB_Move3;
    QLabel *L_Move4;
    QComboBox *CB_Move4;
    QLabel *L_Version;
    QComboBox *CB_GameOrigin;
    QCheckBox *CHK_Shiny;
    QCheckBox *CHK_IsEgg;
    QWidget *TypeFilters;
    QGridLayout *TypeGrid;
    QCheckBox *Egg;
    QCheckBox *Mystery;
    QCheckBox *Static;
    QCheckBox *Trade;
    QCheckBox *Slot;
    QWidget *Tab_Advanced;
    QVBoxLayout *AdvancedLayout;
    QPlainTextEdit *RTB_Instructions;
    QHBoxLayout *SearchRow;
    QPushButton *B_Search;
    QPushButton *B_Reset;

    void setupUi(QDialog *SAV_Encounters)
    {
        if (SAV_Encounters->objectName().isEmpty())
            SAV_Encounters->setObjectName("SAV_Encounters");
        SAV_Encounters->setMinimumSize(QSize(720, 460));
        Workspace = new QHBoxLayout(SAV_Encounters);
        Workspace->setObjectName("Workspace");
        P_Results = new QWidget(SAV_Encounters);
        P_Results->setObjectName("P_Results");
        ResultsCol = new QVBoxLayout(P_Results);
        ResultsCol->setObjectName("ResultsCol");
        ResultsCol->setContentsMargins(0, 0, 0, 0);
        L_Count = new QLabel(P_Results);
        L_Count->setObjectName("L_Count");

        ResultsCol->addWidget(L_Count);

        EncounterPokeGrid = new QTableWidget(P_Results);
        if (EncounterPokeGrid->columnCount() < 1)
            EncounterPokeGrid->setColumnCount(1);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        EncounterPokeGrid->setHorizontalHeaderItem(0, __qtablewidgetitem);
        EncounterPokeGrid->setObjectName("EncounterPokeGrid");
        EncounterPokeGrid->setColumnCount(1);

        ResultsCol->addWidget(EncounterPokeGrid);

        L_Viewed = new QLabel(P_Results);
        L_Viewed->setObjectName("L_Viewed");

        ResultsCol->addWidget(L_Viewed);


        Workspace->addWidget(P_Results);

        FilterRail = new QVBoxLayout();
        FilterRail->setObjectName("FilterRail");
        TC_SearchOptions = new QTabWidget(SAV_Encounters);
        TC_SearchOptions->setObjectName("TC_SearchOptions");
        Tab_General = new QWidget();
        Tab_General->setObjectName("Tab_General");
        TLP_Filters = new QFormLayout(Tab_General);
        TLP_Filters->setObjectName("TLP_Filters");
        Label_Species = new QLabel(Tab_General);
        Label_Species->setObjectName("Label_Species");

        TLP_Filters->setWidget(0, QFormLayout::ItemRole::LabelRole, Label_Species);

        CB_Species = new QComboBox(Tab_General);
        CB_Species->setObjectName("CB_Species");

        TLP_Filters->setWidget(0, QFormLayout::ItemRole::FieldRole, CB_Species);

        L_Move1 = new QLabel(Tab_General);
        L_Move1->setObjectName("L_Move1");

        TLP_Filters->setWidget(1, QFormLayout::ItemRole::LabelRole, L_Move1);

        CB_Move1 = new QComboBox(Tab_General);
        CB_Move1->setObjectName("CB_Move1");

        TLP_Filters->setWidget(1, QFormLayout::ItemRole::FieldRole, CB_Move1);

        L_Move2 = new QLabel(Tab_General);
        L_Move2->setObjectName("L_Move2");

        TLP_Filters->setWidget(2, QFormLayout::ItemRole::LabelRole, L_Move2);

        CB_Move2 = new QComboBox(Tab_General);
        CB_Move2->setObjectName("CB_Move2");

        TLP_Filters->setWidget(2, QFormLayout::ItemRole::FieldRole, CB_Move2);

        L_Move3 = new QLabel(Tab_General);
        L_Move3->setObjectName("L_Move3");

        TLP_Filters->setWidget(3, QFormLayout::ItemRole::LabelRole, L_Move3);

        CB_Move3 = new QComboBox(Tab_General);
        CB_Move3->setObjectName("CB_Move3");

        TLP_Filters->setWidget(3, QFormLayout::ItemRole::FieldRole, CB_Move3);

        L_Move4 = new QLabel(Tab_General);
        L_Move4->setObjectName("L_Move4");

        TLP_Filters->setWidget(4, QFormLayout::ItemRole::LabelRole, L_Move4);

        CB_Move4 = new QComboBox(Tab_General);
        CB_Move4->setObjectName("CB_Move4");

        TLP_Filters->setWidget(4, QFormLayout::ItemRole::FieldRole, CB_Move4);

        L_Version = new QLabel(Tab_General);
        L_Version->setObjectName("L_Version");

        TLP_Filters->setWidget(5, QFormLayout::ItemRole::LabelRole, L_Version);

        CB_GameOrigin = new QComboBox(Tab_General);
        CB_GameOrigin->setObjectName("CB_GameOrigin");

        TLP_Filters->setWidget(5, QFormLayout::ItemRole::FieldRole, CB_GameOrigin);

        CHK_Shiny = new QCheckBox(Tab_General);
        CHK_Shiny->setObjectName("CHK_Shiny");

        TLP_Filters->setWidget(6, QFormLayout::ItemRole::LabelRole, CHK_Shiny);

        CHK_IsEgg = new QCheckBox(Tab_General);
        CHK_IsEgg->setObjectName("CHK_IsEgg");

        TLP_Filters->setWidget(6, QFormLayout::ItemRole::FieldRole, CHK_IsEgg);

        TypeFilters = new QWidget(Tab_General);
        TypeFilters->setObjectName("TypeFilters");
        TypeGrid = new QGridLayout(TypeFilters);
        TypeGrid->setObjectName("TypeGrid");
        TypeGrid->setContentsMargins(0, 0, 0, 0);
        Egg = new QCheckBox(TypeFilters);
        Egg->setObjectName("Egg");
        Egg->setChecked(true);

        TypeGrid->addWidget(Egg, 0, 0, 1, 1);

        Mystery = new QCheckBox(TypeFilters);
        Mystery->setObjectName("Mystery");
        Mystery->setChecked(true);

        TypeGrid->addWidget(Mystery, 0, 1, 1, 1);

        Static = new QCheckBox(TypeFilters);
        Static->setObjectName("Static");
        Static->setChecked(true);

        TypeGrid->addWidget(Static, 1, 0, 1, 1);

        Trade = new QCheckBox(TypeFilters);
        Trade->setObjectName("Trade");
        Trade->setChecked(true);

        TypeGrid->addWidget(Trade, 1, 1, 1, 1);

        Slot = new QCheckBox(TypeFilters);
        Slot->setObjectName("Slot");
        Slot->setChecked(true);

        TypeGrid->addWidget(Slot, 2, 0, 1, 1);


        TLP_Filters->setWidget(7, QFormLayout::ItemRole::SpanningRole, TypeFilters);

        TC_SearchOptions->addTab(Tab_General, QString());
        Tab_Advanced = new QWidget();
        Tab_Advanced->setObjectName("Tab_Advanced");
        AdvancedLayout = new QVBoxLayout(Tab_Advanced);
        AdvancedLayout->setObjectName("AdvancedLayout");
        RTB_Instructions = new QPlainTextEdit(Tab_Advanced);
        RTB_Instructions->setObjectName("RTB_Instructions");

        AdvancedLayout->addWidget(RTB_Instructions);

        TC_SearchOptions->addTab(Tab_Advanced, QString());

        FilterRail->addWidget(TC_SearchOptions);

        SearchRow = new QHBoxLayout();
        SearchRow->setObjectName("SearchRow");
        B_Search = new QPushButton(SAV_Encounters);
        B_Search->setObjectName("B_Search");

        SearchRow->addWidget(B_Search);

        B_Reset = new QPushButton(SAV_Encounters);
        B_Reset->setObjectName("B_Reset");

        SearchRow->addWidget(B_Reset);


        FilterRail->addLayout(SearchRow);


        Workspace->addLayout(FilterRail);


        retranslateUi(SAV_Encounters);

        QMetaObject::connectSlotsByName(SAV_Encounters);
    } // setupUi

    void retranslateUi(QDialog *SAV_Encounters)
    {
        SAV_Encounters->setWindowTitle(QCoreApplication::translate("SAV_Encounters", "Database", nullptr));
        L_Count->setText(QCoreApplication::translate("SAV_Encounters", "Ready...", nullptr));
        QTableWidgetItem *___qtablewidgetitem = EncounterPokeGrid->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("SAV_Encounters", "Encounter", nullptr));
        L_Viewed->setText(QString());
        Label_Species->setText(QCoreApplication::translate("SAV_Encounters", "Species:", nullptr));
        L_Move1->setText(QCoreApplication::translate("SAV_Encounters", "Move 1:", nullptr));
        L_Move2->setText(QCoreApplication::translate("SAV_Encounters", "Move 2:", nullptr));
        L_Move3->setText(QCoreApplication::translate("SAV_Encounters", "Move 3:", nullptr));
        L_Move4->setText(QCoreApplication::translate("SAV_Encounters", "Move 4:", nullptr));
        L_Version->setText(QCoreApplication::translate("SAV_Encounters", "OT Version:", nullptr));
        CHK_Shiny->setText(QCoreApplication::translate("SAV_Encounters", "Shiny", nullptr));
        CHK_IsEgg->setText(QCoreApplication::translate("SAV_Encounters", "Egg", nullptr));
        Egg->setText(QCoreApplication::translate("SAV_Encounters", "Egg", nullptr));
        Mystery->setText(QCoreApplication::translate("SAV_Encounters", "Mystery", nullptr));
        Static->setText(QCoreApplication::translate("SAV_Encounters", "Static", nullptr));
        Trade->setText(QCoreApplication::translate("SAV_Encounters", "Trade", nullptr));
        Slot->setText(QCoreApplication::translate("SAV_Encounters", "Slot", nullptr));
        TC_SearchOptions->setTabText(TC_SearchOptions->indexOf(Tab_General), QCoreApplication::translate("SAV_Encounters", "General", nullptr));
        TC_SearchOptions->setTabText(TC_SearchOptions->indexOf(Tab_Advanced), QCoreApplication::translate("SAV_Encounters", "Advanced", nullptr));
        B_Search->setText(QCoreApplication::translate("SAV_Encounters", "Search!", nullptr));
        B_Reset->setText(QCoreApplication::translate("SAV_Encounters", "Reset Filters", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SAV_Encounters: public Ui_SAV_Encounters {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SAV_ENCOUNTERS_H
