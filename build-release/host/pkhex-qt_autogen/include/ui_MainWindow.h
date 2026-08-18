/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *Menu_Open;
    QAction *Menu_Save;
    QAction *Menu_ExportSAV;
    QAction *Menu_Exit;
    QAction *Menu_ShowdownImportPKM;
    QAction *Menu_ShowdownExportPKM;
    QAction *Menu_ShowdownExportParty;
    QAction *Menu_ShowdownExportCurrentBox;
    QAction *Menu_LoadBoxes;
    QAction *Menu_DumpBoxes;
    QAction *Menu_DumpBox;
    QAction *Menu_Report;
    QAction *Menu_Database;
    QAction *Menu_MGDatabase;
    QAction *Menu_EncDatabase;
    QAction *Menu_BatchEditor;
    QAction *Menu_Folder;
    QAction *Menu_Language;
    QAction *Menu_Undo;
    QAction *Menu_Redo;
    QAction *Menu_Settings;
    QAction *Menu_About;
    QSplitter *splitContainer1;
    QWidget *PKMPane;
    QVBoxLayout *PKMPaneLayout;
    QHBoxLayout *PKMHeaderLayout;
    QLabel *PB_Legal;
    QLabel *dragout;
    QSpacerItem *PKMHeaderSpacer;
    QTabWidget *PKME_Tabs;
    QWidget *Tab_Main;
    QWidget *Tab_Met;
    QWidget *Tab_Stats;
    QWidget *Tab_Moves;
    QWidget *Tab_Cosmetic;
    QWidget *Tab_OTMisc;
    QWidget *C_SAV;
    QVBoxLayout *C_SAVLayout;
    QTabWidget *tabBoxMulti;
    QWidget *Tab_Box;
    QWidget *Tab_PartyBattle;
    QWidget *Tab_Other;
    QWidget *Tab_SAV;
    QVBoxLayout *Tab_SAVLayout;
    QHBoxLayout *FLP_SAVToolsMisc;
    QPushButton *B_SaveBoxBin;
    QPushButton *B_VerifyCHK;
    QPushButton *B_VerifySaveEntities;
    QPushButton *Menu_ExportBAK;
    QPushButton *B_JPEG;
    QPushButton *B_ConvertKorean;
    QScrollArea *SAVToolsScroll;
    QWidget *FLP_SAVtools;
    QMenuBar *menuStrip1;
    QMenu *Menu_File;
    QMenu *Menu_Tools;
    QMenu *Menu_Showdown;
    QMenu *Menu_Data;
    QMenu *Menu_Options;

    void setupUi(QMainWindow *Main)
    {
        if (Main->objectName().isEmpty())
            Main->setObjectName("Main");
        Menu_Open = new QAction(Main);
        Menu_Open->setObjectName("Menu_Open");
        Menu_Save = new QAction(Main);
        Menu_Save->setObjectName("Menu_Save");
        Menu_ExportSAV = new QAction(Main);
        Menu_ExportSAV->setObjectName("Menu_ExportSAV");
        Menu_Exit = new QAction(Main);
        Menu_Exit->setObjectName("Menu_Exit");
        Menu_ShowdownImportPKM = new QAction(Main);
        Menu_ShowdownImportPKM->setObjectName("Menu_ShowdownImportPKM");
        Menu_ShowdownExportPKM = new QAction(Main);
        Menu_ShowdownExportPKM->setObjectName("Menu_ShowdownExportPKM");
        Menu_ShowdownExportParty = new QAction(Main);
        Menu_ShowdownExportParty->setObjectName("Menu_ShowdownExportParty");
        Menu_ShowdownExportCurrentBox = new QAction(Main);
        Menu_ShowdownExportCurrentBox->setObjectName("Menu_ShowdownExportCurrentBox");
        Menu_LoadBoxes = new QAction(Main);
        Menu_LoadBoxes->setObjectName("Menu_LoadBoxes");
        Menu_DumpBoxes = new QAction(Main);
        Menu_DumpBoxes->setObjectName("Menu_DumpBoxes");
        Menu_DumpBox = new QAction(Main);
        Menu_DumpBox->setObjectName("Menu_DumpBox");
        Menu_Report = new QAction(Main);
        Menu_Report->setObjectName("Menu_Report");
        Menu_Database = new QAction(Main);
        Menu_Database->setObjectName("Menu_Database");
        Menu_MGDatabase = new QAction(Main);
        Menu_MGDatabase->setObjectName("Menu_MGDatabase");
        Menu_EncDatabase = new QAction(Main);
        Menu_EncDatabase->setObjectName("Menu_EncDatabase");
        Menu_BatchEditor = new QAction(Main);
        Menu_BatchEditor->setObjectName("Menu_BatchEditor");
        Menu_Folder = new QAction(Main);
        Menu_Folder->setObjectName("Menu_Folder");
        Menu_Language = new QAction(Main);
        Menu_Language->setObjectName("Menu_Language");
        Menu_Undo = new QAction(Main);
        Menu_Undo->setObjectName("Menu_Undo");
        Menu_Undo->setEnabled(false);
        Menu_Redo = new QAction(Main);
        Menu_Redo->setObjectName("Menu_Redo");
        Menu_Redo->setEnabled(false);
        Menu_Settings = new QAction(Main);
        Menu_Settings->setObjectName("Menu_Settings");
        Menu_About = new QAction(Main);
        Menu_About->setObjectName("Menu_About");
        splitContainer1 = new QSplitter(Main);
        splitContainer1->setObjectName("splitContainer1");
        splitContainer1->setOrientation(Qt::Horizontal);
        PKMPane = new QWidget(splitContainer1);
        PKMPane->setObjectName("PKMPane");
        PKMPaneLayout = new QVBoxLayout(PKMPane);
        PKMPaneLayout->setSpacing(4);
        PKMPaneLayout->setObjectName("PKMPaneLayout");
        PKMPaneLayout->setContentsMargins(4, 4, 4, 4);
        PKMHeaderLayout = new QHBoxLayout();
        PKMHeaderLayout->setObjectName("PKMHeaderLayout");
        PB_Legal = new QLabel(PKMPane);
        PB_Legal->setObjectName("PB_Legal");
        PB_Legal->setMinimumSize(QSize(24, 24));

        PKMHeaderLayout->addWidget(PB_Legal);

        dragout = new QLabel(PKMPane);
        dragout->setObjectName("dragout");
        dragout->setMinimumSize(QSize(72, 56));

        PKMHeaderLayout->addWidget(dragout);

        PKMHeaderSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        PKMHeaderLayout->addItem(PKMHeaderSpacer);


        PKMPaneLayout->addLayout(PKMHeaderLayout);

        PKME_Tabs = new QTabWidget(PKMPane);
        PKME_Tabs->setObjectName("PKME_Tabs");
        PKME_Tabs->setTabPosition(QTabWidget::West);
        Tab_Main = new QWidget();
        Tab_Main->setObjectName("Tab_Main");
        PKME_Tabs->addTab(Tab_Main, QString());
        Tab_Met = new QWidget();
        Tab_Met->setObjectName("Tab_Met");
        PKME_Tabs->addTab(Tab_Met, QString());
        Tab_Stats = new QWidget();
        Tab_Stats->setObjectName("Tab_Stats");
        PKME_Tabs->addTab(Tab_Stats, QString());
        Tab_Moves = new QWidget();
        Tab_Moves->setObjectName("Tab_Moves");
        PKME_Tabs->addTab(Tab_Moves, QString());
        Tab_Cosmetic = new QWidget();
        Tab_Cosmetic->setObjectName("Tab_Cosmetic");
        PKME_Tabs->addTab(Tab_Cosmetic, QString());
        Tab_OTMisc = new QWidget();
        Tab_OTMisc->setObjectName("Tab_OTMisc");
        PKME_Tabs->addTab(Tab_OTMisc, QString());

        PKMPaneLayout->addWidget(PKME_Tabs);

        splitContainer1->addWidget(PKMPane);
        C_SAV = new QWidget(splitContainer1);
        C_SAV->setObjectName("C_SAV");
        C_SAVLayout = new QVBoxLayout(C_SAV);
        C_SAVLayout->setObjectName("C_SAVLayout");
        C_SAVLayout->setContentsMargins(4, 4, 4, 4);
        tabBoxMulti = new QTabWidget(C_SAV);
        tabBoxMulti->setObjectName("tabBoxMulti");
        Tab_Box = new QWidget();
        Tab_Box->setObjectName("Tab_Box");
        tabBoxMulti->addTab(Tab_Box, QString());
        Tab_PartyBattle = new QWidget();
        Tab_PartyBattle->setObjectName("Tab_PartyBattle");
        tabBoxMulti->addTab(Tab_PartyBattle, QString());
        Tab_Other = new QWidget();
        Tab_Other->setObjectName("Tab_Other");
        tabBoxMulti->addTab(Tab_Other, QString());
        Tab_SAV = new QWidget();
        Tab_SAV->setObjectName("Tab_SAV");
        Tab_SAVLayout = new QVBoxLayout(Tab_SAV);
        Tab_SAVLayout->setObjectName("Tab_SAVLayout");
        FLP_SAVToolsMisc = new QHBoxLayout();
        FLP_SAVToolsMisc->setObjectName("FLP_SAVToolsMisc");
        B_SaveBoxBin = new QPushButton(Tab_SAV);
        B_SaveBoxBin->setObjectName("B_SaveBoxBin");

        FLP_SAVToolsMisc->addWidget(B_SaveBoxBin);

        B_VerifyCHK = new QPushButton(Tab_SAV);
        B_VerifyCHK->setObjectName("B_VerifyCHK");

        FLP_SAVToolsMisc->addWidget(B_VerifyCHK);

        B_VerifySaveEntities = new QPushButton(Tab_SAV);
        B_VerifySaveEntities->setObjectName("B_VerifySaveEntities");

        FLP_SAVToolsMisc->addWidget(B_VerifySaveEntities);

        Menu_ExportBAK = new QPushButton(Tab_SAV);
        Menu_ExportBAK->setObjectName("Menu_ExportBAK");

        FLP_SAVToolsMisc->addWidget(Menu_ExportBAK);

        B_JPEG = new QPushButton(Tab_SAV);
        B_JPEG->setObjectName("B_JPEG");

        FLP_SAVToolsMisc->addWidget(B_JPEG);

        B_ConvertKorean = new QPushButton(Tab_SAV);
        B_ConvertKorean->setObjectName("B_ConvertKorean");

        FLP_SAVToolsMisc->addWidget(B_ConvertKorean);


        Tab_SAVLayout->addLayout(FLP_SAVToolsMisc);

        SAVToolsScroll = new QScrollArea(Tab_SAV);
        SAVToolsScroll->setObjectName("SAVToolsScroll");
        SAVToolsScroll->setWidgetResizable(true);
        FLP_SAVtools = new QWidget();
        FLP_SAVtools->setObjectName("FLP_SAVtools");
        SAVToolsScroll->setWidget(FLP_SAVtools);

        Tab_SAVLayout->addWidget(SAVToolsScroll);

        tabBoxMulti->addTab(Tab_SAV, QString());

        C_SAVLayout->addWidget(tabBoxMulti);

        splitContainer1->addWidget(C_SAV);
        Main->setCentralWidget(splitContainer1);
        menuStrip1 = new QMenuBar(Main);
        menuStrip1->setObjectName("menuStrip1");
        Menu_File = new QMenu(menuStrip1);
        Menu_File->setObjectName("Menu_File");
        Menu_Tools = new QMenu(menuStrip1);
        Menu_Tools->setObjectName("Menu_Tools");
        Menu_Showdown = new QMenu(Menu_Tools);
        Menu_Showdown->setObjectName("Menu_Showdown");
        Menu_Data = new QMenu(Menu_Tools);
        Menu_Data->setObjectName("Menu_Data");
        Menu_Options = new QMenu(menuStrip1);
        Menu_Options->setObjectName("Menu_Options");
        Main->setMenuBar(menuStrip1);

        menuStrip1->addAction(Menu_File->menuAction());
        menuStrip1->addAction(Menu_Tools->menuAction());
        menuStrip1->addAction(Menu_Options->menuAction());
        Menu_File->addAction(Menu_Open);
        Menu_File->addSeparator();
        Menu_File->addAction(Menu_Save);
        Menu_File->addAction(Menu_ExportSAV);
        Menu_File->addSeparator();
        Menu_File->addAction(Menu_Exit);
        Menu_Tools->addAction(Menu_Showdown->menuAction());
        Menu_Tools->addAction(Menu_Data->menuAction());
        Menu_Tools->addSeparator();
        Menu_Tools->addAction(Menu_Folder);
        Menu_Showdown->addAction(Menu_ShowdownImportPKM);
        Menu_Showdown->addSeparator();
        Menu_Showdown->addAction(Menu_ShowdownExportPKM);
        Menu_Showdown->addAction(Menu_ShowdownExportParty);
        Menu_Showdown->addAction(Menu_ShowdownExportCurrentBox);
        Menu_Data->addAction(Menu_LoadBoxes);
        Menu_Data->addAction(Menu_DumpBoxes);
        Menu_Data->addAction(Menu_DumpBox);
        Menu_Data->addSeparator();
        Menu_Data->addAction(Menu_Report);
        Menu_Data->addSeparator();
        Menu_Data->addAction(Menu_Database);
        Menu_Data->addAction(Menu_MGDatabase);
        Menu_Data->addAction(Menu_EncDatabase);
        Menu_Data->addSeparator();
        Menu_Data->addAction(Menu_BatchEditor);
        Menu_Options->addAction(Menu_Language);
        Menu_Options->addSeparator();
        Menu_Options->addAction(Menu_Undo);
        Menu_Options->addAction(Menu_Redo);
        Menu_Options->addSeparator();
        Menu_Options->addAction(Menu_Settings);
        Menu_Options->addAction(Menu_About);

        retranslateUi(Main);

        PKME_Tabs->setCurrentIndex(0);
        tabBoxMulti->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(Main);
    } // setupUi

    void retranslateUi(QMainWindow *Main)
    {
        Main->setWindowTitle(QCoreApplication::translate("MainWindow", "PKHeX Qt", nullptr));
        Menu_Open->setText(QCoreApplication::translate("MainWindow", "&Open...", nullptr));
#if QT_CONFIG(shortcut)
        Menu_Open->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
        Menu_Save->setText(QCoreApplication::translate("MainWindow", "&Save PKM...", nullptr));
#if QT_CONFIG(shortcut)
        Menu_Save->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+S", nullptr));
#endif // QT_CONFIG(shortcut)
        Menu_ExportSAV->setText(QCoreApplication::translate("MainWindow", "&Export SAV...", nullptr));
#if QT_CONFIG(shortcut)
        Menu_ExportSAV->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+E", nullptr));
#endif // QT_CONFIG(shortcut)
        Menu_Exit->setText(QCoreApplication::translate("MainWindow", "&Quit", nullptr));
#if QT_CONFIG(shortcut)
        Menu_Exit->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Q", nullptr));
#endif // QT_CONFIG(shortcut)
        Menu_ShowdownImportPKM->setText(QCoreApplication::translate("MainWindow", "&Import Set from Clipboard", nullptr));
#if QT_CONFIG(shortcut)
        Menu_ShowdownImportPKM->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+T", nullptr));
#endif // QT_CONFIG(shortcut)
        Menu_ShowdownExportPKM->setText(QCoreApplication::translate("MainWindow", "&Export Set to Clipboard", nullptr));
#if QT_CONFIG(shortcut)
        Menu_ShowdownExportPKM->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+T", nullptr));
#endif // QT_CONFIG(shortcut)
        Menu_ShowdownExportParty->setText(QCoreApplication::translate("MainWindow", "Export &Party to Clipboard", nullptr));
        Menu_ShowdownExportCurrentBox->setText(QCoreApplication::translate("MainWindow", "Export Current &Box to Clipboard", nullptr));
        Menu_LoadBoxes->setText(QCoreApplication::translate("MainWindow", "&Load Boxes", nullptr));
        Menu_DumpBoxes->setText(QCoreApplication::translate("MainWindow", "Dump Bo&xes", nullptr));
        Menu_DumpBox->setText(QCoreApplication::translate("MainWindow", "Dump Bo&x", nullptr));
        Menu_Report->setText(QCoreApplication::translate("MainWindow", "Box Data &Report", nullptr));
        Menu_Database->setText(QCoreApplication::translate("MainWindow", "PKM &Database", nullptr));
#if QT_CONFIG(shortcut)
        Menu_Database->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+D", nullptr));
#endif // QT_CONFIG(shortcut)
        Menu_MGDatabase->setText(QCoreApplication::translate("MainWindow", "Mystery &Gift Database", nullptr));
        Menu_EncDatabase->setText(QCoreApplication::translate("MainWindow", "E&ncounter Database", nullptr));
#if QT_CONFIG(shortcut)
        Menu_EncDatabase->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+N", nullptr));
#endif // QT_CONFIG(shortcut)
        Menu_BatchEditor->setText(QCoreApplication::translate("MainWindow", "&Batch Editor", nullptr));
#if QT_CONFIG(shortcut)
        Menu_BatchEditor->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+M", nullptr));
#endif // QT_CONFIG(shortcut)
        Menu_Folder->setText(QCoreApplication::translate("MainWindow", "Open &Folder", nullptr));
        Menu_Language->setText(QCoreApplication::translate("MainWindow", "&Language", nullptr));
        Menu_Undo->setText(QCoreApplication::translate("MainWindow", "&Undo Last Change", nullptr));
        Menu_Redo->setText(QCoreApplication::translate("MainWindow", "&Redo Last Change", nullptr));
        Menu_Settings->setText(QCoreApplication::translate("MainWindow", "&Settings", nullptr));
        Menu_About->setText(QCoreApplication::translate("MainWindow", "&About PKHeX", nullptr));
#if QT_CONFIG(shortcut)
        Menu_About->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+P", nullptr));
#endif // QT_CONFIG(shortcut)
        PB_Legal->setText(QString());
        dragout->setText(QString());
        PKME_Tabs->setTabText(PKME_Tabs->indexOf(Tab_Main), QCoreApplication::translate("MainWindow", "Main", nullptr));
        PKME_Tabs->setTabText(PKME_Tabs->indexOf(Tab_Met), QCoreApplication::translate("MainWindow", "Met", nullptr));
        PKME_Tabs->setTabText(PKME_Tabs->indexOf(Tab_Stats), QCoreApplication::translate("MainWindow", "Stats", nullptr));
        PKME_Tabs->setTabText(PKME_Tabs->indexOf(Tab_Moves), QCoreApplication::translate("MainWindow", "Moves", nullptr));
        PKME_Tabs->setTabText(PKME_Tabs->indexOf(Tab_Cosmetic), QCoreApplication::translate("MainWindow", "Cosmetic", nullptr));
        PKME_Tabs->setTabText(PKME_Tabs->indexOf(Tab_OTMisc), QCoreApplication::translate("MainWindow", "OT/Misc", nullptr));
        tabBoxMulti->setTabText(tabBoxMulti->indexOf(Tab_Box), QCoreApplication::translate("MainWindow", "Box", nullptr));
        tabBoxMulti->setTabText(tabBoxMulti->indexOf(Tab_PartyBattle), QCoreApplication::translate("MainWindow", "Party", nullptr));
        tabBoxMulti->setTabText(tabBoxMulti->indexOf(Tab_Other), QCoreApplication::translate("MainWindow", "Other", nullptr));
        B_SaveBoxBin->setText(QCoreApplication::translate("MainWindow", "Save Box Data++", nullptr));
        B_VerifyCHK->setText(QCoreApplication::translate("MainWindow", "Verify Checksums", nullptr));
        B_VerifySaveEntities->setText(QCoreApplication::translate("MainWindow", "Verify All PKMs", nullptr));
        Menu_ExportBAK->setText(QCoreApplication::translate("MainWindow", "Export Backup", nullptr));
        B_JPEG->setText(QCoreApplication::translate("MainWindow", "Save PGL .JPEG", nullptr));
        B_ConvertKorean->setText(QCoreApplication::translate("MainWindow", "Korean Save Conversion", nullptr));
        tabBoxMulti->setTabText(tabBoxMulti->indexOf(Tab_SAV), QCoreApplication::translate("MainWindow", "SAV", nullptr));
        Menu_File->setTitle(QCoreApplication::translate("MainWindow", "&File", nullptr));
        Menu_Tools->setTitle(QCoreApplication::translate("MainWindow", "&Tools", nullptr));
        Menu_Showdown->setTitle(QCoreApplication::translate("MainWindow", "&Showdown", nullptr));
        Menu_Data->setTitle(QCoreApplication::translate("MainWindow", "&Data", nullptr));
        Menu_Options->setTitle(QCoreApplication::translate("MainWindow", "&Options", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
