/********************************************************************************
** Form generated from reading UI file 'SAV_Database.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SAV_DATABASE_H
#define UI_SAV_DATABASE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SAV_Database
{
public:
    QAction *Menu_Exit;
    QAction *Menu_SearchBoxes;
    QAction *Menu_SearchDatabase;
    QAction *Menu_SearchBackups;
    QAction *Menu_SearchLegal;
    QAction *Menu_SearchIllegal;
    QVBoxLayout *DatabaseRoot;
    QMenuBar *menuStrip1;
    QMenu *Menu_Close;
    QMenu *Menu_Tools;
    QMenu *Menu_SearchSettings;
    QTabWidget *TC_SearchSettings;
    QWidget *Tab_General;
    QFormLayout *GeneralForm;
    QLabel *Label_Species;
    QComboBox *CB_Species;
    QLabel *Label_Nickname;
    QLineEdit *TB_Nickname;
    QLabel *CHK_ShinyLabel;
    QCheckBox *CHK_Shiny;
    QLabel *CHK_IsEggLabel;
    QCheckBox *CHK_IsEgg;
    QWidget *Tab_Advanced;
    QVBoxLayout *AdvancedLayout;
    QPlainTextEdit *RTB_Instructions;
    QHBoxLayout *SearchRow;
    QPushButton *B_Search;
    QPushButton *B_Reset;
    QLabel *L_Count;
    QLabel *L_Viewed;
    QWidget *P_Results;
    QVBoxLayout *ResultsLayout;
    QTableWidget *DatabasePokeGrid;

    void setupUi(QDialog *SAV_Database)
    {
        if (SAV_Database->objectName().isEmpty())
            SAV_Database->setObjectName("SAV_Database");
        SAV_Database->setMinimumSize(QSize(640, 420));
        Menu_Exit = new QAction(SAV_Database);
        Menu_Exit->setObjectName("Menu_Exit");
        Menu_SearchBoxes = new QAction(SAV_Database);
        Menu_SearchBoxes->setObjectName("Menu_SearchBoxes");
        Menu_SearchBoxes->setCheckable(true);
        Menu_SearchBoxes->setChecked(true);
        Menu_SearchDatabase = new QAction(SAV_Database);
        Menu_SearchDatabase->setObjectName("Menu_SearchDatabase");
        Menu_SearchDatabase->setCheckable(true);
        Menu_SearchDatabase->setChecked(true);
        Menu_SearchBackups = new QAction(SAV_Database);
        Menu_SearchBackups->setObjectName("Menu_SearchBackups");
        Menu_SearchBackups->setCheckable(true);
        Menu_SearchBackups->setChecked(true);
        Menu_SearchLegal = new QAction(SAV_Database);
        Menu_SearchLegal->setObjectName("Menu_SearchLegal");
        Menu_SearchLegal->setCheckable(true);
        Menu_SearchIllegal = new QAction(SAV_Database);
        Menu_SearchIllegal->setObjectName("Menu_SearchIllegal");
        Menu_SearchIllegal->setCheckable(true);
        DatabaseRoot = new QVBoxLayout(SAV_Database);
        DatabaseRoot->setObjectName("DatabaseRoot");
        menuStrip1 = new QMenuBar(SAV_Database);
        menuStrip1->setObjectName("menuStrip1");
        menuStrip1->setNativeMenuBar(false);
        Menu_Close = new QMenu(menuStrip1);
        Menu_Close->setObjectName("Menu_Close");
        Menu_Tools = new QMenu(menuStrip1);
        Menu_Tools->setObjectName("Menu_Tools");
        Menu_SearchSettings = new QMenu(Menu_Tools);
        Menu_SearchSettings->setObjectName("Menu_SearchSettings");

        DatabaseRoot->addWidget(menuStrip1);

        TC_SearchSettings = new QTabWidget(SAV_Database);
        TC_SearchSettings->setObjectName("TC_SearchSettings");
        Tab_General = new QWidget();
        Tab_General->setObjectName("Tab_General");
        GeneralForm = new QFormLayout(Tab_General);
        GeneralForm->setObjectName("GeneralForm");
        Label_Species = new QLabel(Tab_General);
        Label_Species->setObjectName("Label_Species");

        GeneralForm->setWidget(0, QFormLayout::ItemRole::LabelRole, Label_Species);

        CB_Species = new QComboBox(Tab_General);
        CB_Species->setObjectName("CB_Species");

        GeneralForm->setWidget(0, QFormLayout::ItemRole::FieldRole, CB_Species);

        Label_Nickname = new QLabel(Tab_General);
        Label_Nickname->setObjectName("Label_Nickname");

        GeneralForm->setWidget(1, QFormLayout::ItemRole::LabelRole, Label_Nickname);

        TB_Nickname = new QLineEdit(Tab_General);
        TB_Nickname->setObjectName("TB_Nickname");

        GeneralForm->setWidget(1, QFormLayout::ItemRole::FieldRole, TB_Nickname);

        CHK_ShinyLabel = new QLabel(Tab_General);
        CHK_ShinyLabel->setObjectName("CHK_ShinyLabel");

        GeneralForm->setWidget(2, QFormLayout::ItemRole::LabelRole, CHK_ShinyLabel);

        CHK_Shiny = new QCheckBox(Tab_General);
        CHK_Shiny->setObjectName("CHK_Shiny");

        GeneralForm->setWidget(2, QFormLayout::ItemRole::FieldRole, CHK_Shiny);

        CHK_IsEggLabel = new QLabel(Tab_General);
        CHK_IsEggLabel->setObjectName("CHK_IsEggLabel");

        GeneralForm->setWidget(3, QFormLayout::ItemRole::LabelRole, CHK_IsEggLabel);

        CHK_IsEgg = new QCheckBox(Tab_General);
        CHK_IsEgg->setObjectName("CHK_IsEgg");

        GeneralForm->setWidget(3, QFormLayout::ItemRole::FieldRole, CHK_IsEgg);

        TC_SearchSettings->addTab(Tab_General, QString());
        Tab_Advanced = new QWidget();
        Tab_Advanced->setObjectName("Tab_Advanced");
        AdvancedLayout = new QVBoxLayout(Tab_Advanced);
        AdvancedLayout->setObjectName("AdvancedLayout");
        RTB_Instructions = new QPlainTextEdit(Tab_Advanced);
        RTB_Instructions->setObjectName("RTB_Instructions");

        AdvancedLayout->addWidget(RTB_Instructions);

        TC_SearchSettings->addTab(Tab_Advanced, QString());

        DatabaseRoot->addWidget(TC_SearchSettings);

        SearchRow = new QHBoxLayout();
        SearchRow->setObjectName("SearchRow");
        B_Search = new QPushButton(SAV_Database);
        B_Search->setObjectName("B_Search");

        SearchRow->addWidget(B_Search);

        B_Reset = new QPushButton(SAV_Database);
        B_Reset->setObjectName("B_Reset");

        SearchRow->addWidget(B_Reset);

        L_Count = new QLabel(SAV_Database);
        L_Count->setObjectName("L_Count");

        SearchRow->addWidget(L_Count);

        L_Viewed = new QLabel(SAV_Database);
        L_Viewed->setObjectName("L_Viewed");

        SearchRow->addWidget(L_Viewed);


        DatabaseRoot->addLayout(SearchRow);

        P_Results = new QWidget(SAV_Database);
        P_Results->setObjectName("P_Results");
        ResultsLayout = new QVBoxLayout(P_Results);
        ResultsLayout->setObjectName("ResultsLayout");
        ResultsLayout->setContentsMargins(0, 0, 0, 0);
        DatabasePokeGrid = new QTableWidget(P_Results);
        if (DatabasePokeGrid->columnCount() < 1)
            DatabasePokeGrid->setColumnCount(1);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        DatabasePokeGrid->setHorizontalHeaderItem(0, __qtablewidgetitem);
        DatabasePokeGrid->setObjectName("DatabasePokeGrid");
        DatabasePokeGrid->setColumnCount(1);

        ResultsLayout->addWidget(DatabasePokeGrid);


        DatabaseRoot->addWidget(P_Results);


        menuStrip1->addAction(Menu_Close->menuAction());
        menuStrip1->addAction(Menu_Tools->menuAction());
        Menu_Close->addAction(Menu_Exit);
        Menu_Tools->addAction(Menu_SearchSettings->menuAction());
        Menu_SearchSettings->addAction(Menu_SearchBoxes);
        Menu_SearchSettings->addAction(Menu_SearchDatabase);
        Menu_SearchSettings->addAction(Menu_SearchBackups);
        Menu_SearchSettings->addAction(Menu_SearchLegal);
        Menu_SearchSettings->addAction(Menu_SearchIllegal);

        retranslateUi(SAV_Database);

        TC_SearchSettings->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(SAV_Database);
    } // setupUi

    void retranslateUi(QDialog *SAV_Database)
    {
        SAV_Database->setWindowTitle(QCoreApplication::translate("SAV_Database", "Database", nullptr));
        Menu_Exit->setText(QCoreApplication::translate("SAV_Database", "&Close", nullptr));
        Menu_SearchBoxes->setText(QCoreApplication::translate("SAV_Database", "Search Within Boxes", nullptr));
        Menu_SearchDatabase->setText(QCoreApplication::translate("SAV_Database", "Search Within Database", nullptr));
        Menu_SearchBackups->setText(QCoreApplication::translate("SAV_Database", "Search Within Backups", nullptr));
        Menu_SearchLegal->setText(QCoreApplication::translate("SAV_Database", "Show Legal", nullptr));
        Menu_SearchIllegal->setText(QCoreApplication::translate("SAV_Database", "Show Illegal", nullptr));
        Menu_Close->setTitle(QCoreApplication::translate("SAV_Database", "File", nullptr));
        Menu_Tools->setTitle(QCoreApplication::translate("SAV_Database", "Tools", nullptr));
        Menu_SearchSettings->setTitle(QCoreApplication::translate("SAV_Database", "Search Settings", nullptr));
        Label_Species->setText(QCoreApplication::translate("SAV_Database", "Species:", nullptr));
        Label_Nickname->setText(QCoreApplication::translate("SAV_Database", "Nickname:", nullptr));
        CHK_ShinyLabel->setText(QCoreApplication::translate("SAV_Database", "Shiny", nullptr));
        CHK_IsEggLabel->setText(QCoreApplication::translate("SAV_Database", "Egg", nullptr));
        TC_SearchSettings->setTabText(TC_SearchSettings->indexOf(Tab_General), QCoreApplication::translate("SAV_Database", "General", nullptr));
        TC_SearchSettings->setTabText(TC_SearchSettings->indexOf(Tab_Advanced), QCoreApplication::translate("SAV_Database", "Advanced", nullptr));
        B_Search->setText(QCoreApplication::translate("SAV_Database", "Search!", nullptr));
        B_Reset->setText(QCoreApplication::translate("SAV_Database", "Reset Filters", nullptr));
        L_Count->setText(QCoreApplication::translate("SAV_Database", "Count: 0", nullptr));
        L_Viewed->setText(QString());
        QTableWidgetItem *___qtablewidgetitem = DatabasePokeGrid->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("SAV_Database", "Identify", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SAV_Database: public Ui_SAV_Database {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SAV_DATABASE_H
