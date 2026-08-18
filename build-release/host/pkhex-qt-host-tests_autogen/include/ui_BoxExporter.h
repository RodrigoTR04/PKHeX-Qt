/********************************************************************************
** Form generated from reading UI file 'BoxExporter.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BOXEXPORTER_H
#define UI_BOXEXPORTER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BoxExporter
{
public:
    QVBoxLayout *BoxExportRoot;
    QWidget *FLP_NamerSelect;
    QHBoxLayout *NamerRow;
    QLabel *L_Namer;
    QComboBox *CB_Namer;
    QWidget *PG_Settings;
    QFormLayout *SettingsForm;
    QLabel *L_Scope;
    QComboBox *CB_Scope;
    QLabel *L_FolderCreation;
    QComboBox *CB_FolderCreation;
    QLabel *L_FolderPrefix;
    QComboBox *CB_FolderPrefix;
    QLabel *L_EmptySlots;
    QComboBox *CB_EmptySlots;
    QLabel *L_FileIndexPrefix;
    QComboBox *CB_FileIndexPrefix;
    QLabel *L_Notify;
    QComboBox *CB_Notify;
    QPushButton *B_Export;

    void setupUi(QDialog *BoxExporter)
    {
        if (BoxExporter->objectName().isEmpty())
            BoxExporter->setObjectName("BoxExporter");
        BoxExporter->setMinimumSize(QSize(304, 281));
        BoxExportRoot = new QVBoxLayout(BoxExporter);
        BoxExportRoot->setObjectName("BoxExportRoot");
        BoxExportRoot->setContentsMargins(0, 0, 0, 0);
        FLP_NamerSelect = new QWidget(BoxExporter);
        FLP_NamerSelect->setObjectName("FLP_NamerSelect");
        NamerRow = new QHBoxLayout(FLP_NamerSelect);
        NamerRow->setObjectName("NamerRow");
        NamerRow->setContentsMargins(0, 0, 0, 0);
        L_Namer = new QLabel(FLP_NamerSelect);
        L_Namer->setObjectName("L_Namer");

        NamerRow->addWidget(L_Namer);

        CB_Namer = new QComboBox(FLP_NamerSelect);
        CB_Namer->setObjectName("CB_Namer");

        NamerRow->addWidget(CB_Namer);


        BoxExportRoot->addWidget(FLP_NamerSelect);

        PG_Settings = new QWidget(BoxExporter);
        PG_Settings->setObjectName("PG_Settings");
        SettingsForm = new QFormLayout(PG_Settings);
        SettingsForm->setObjectName("SettingsForm");
        SettingsForm->setContentsMargins(0, 0, 0, 0);
        L_Scope = new QLabel(PG_Settings);
        L_Scope->setObjectName("L_Scope");

        SettingsForm->setWidget(0, QFormLayout::ItemRole::LabelRole, L_Scope);

        CB_Scope = new QComboBox(PG_Settings);
        CB_Scope->setObjectName("CB_Scope");

        SettingsForm->setWidget(0, QFormLayout::ItemRole::FieldRole, CB_Scope);

        L_FolderCreation = new QLabel(PG_Settings);
        L_FolderCreation->setObjectName("L_FolderCreation");

        SettingsForm->setWidget(1, QFormLayout::ItemRole::LabelRole, L_FolderCreation);

        CB_FolderCreation = new QComboBox(PG_Settings);
        CB_FolderCreation->setObjectName("CB_FolderCreation");

        SettingsForm->setWidget(1, QFormLayout::ItemRole::FieldRole, CB_FolderCreation);

        L_FolderPrefix = new QLabel(PG_Settings);
        L_FolderPrefix->setObjectName("L_FolderPrefix");

        SettingsForm->setWidget(2, QFormLayout::ItemRole::LabelRole, L_FolderPrefix);

        CB_FolderPrefix = new QComboBox(PG_Settings);
        CB_FolderPrefix->setObjectName("CB_FolderPrefix");

        SettingsForm->setWidget(2, QFormLayout::ItemRole::FieldRole, CB_FolderPrefix);

        L_EmptySlots = new QLabel(PG_Settings);
        L_EmptySlots->setObjectName("L_EmptySlots");

        SettingsForm->setWidget(3, QFormLayout::ItemRole::LabelRole, L_EmptySlots);

        CB_EmptySlots = new QComboBox(PG_Settings);
        CB_EmptySlots->setObjectName("CB_EmptySlots");

        SettingsForm->setWidget(3, QFormLayout::ItemRole::FieldRole, CB_EmptySlots);

        L_FileIndexPrefix = new QLabel(PG_Settings);
        L_FileIndexPrefix->setObjectName("L_FileIndexPrefix");

        SettingsForm->setWidget(4, QFormLayout::ItemRole::LabelRole, L_FileIndexPrefix);

        CB_FileIndexPrefix = new QComboBox(PG_Settings);
        CB_FileIndexPrefix->setObjectName("CB_FileIndexPrefix");

        SettingsForm->setWidget(4, QFormLayout::ItemRole::FieldRole, CB_FileIndexPrefix);

        L_Notify = new QLabel(PG_Settings);
        L_Notify->setObjectName("L_Notify");

        SettingsForm->setWidget(5, QFormLayout::ItemRole::LabelRole, L_Notify);

        CB_Notify = new QComboBox(PG_Settings);
        CB_Notify->setObjectName("CB_Notify");

        SettingsForm->setWidget(5, QFormLayout::ItemRole::FieldRole, CB_Notify);


        BoxExportRoot->addWidget(PG_Settings);

        B_Export = new QPushButton(BoxExporter);
        B_Export->setObjectName("B_Export");

        BoxExportRoot->addWidget(B_Export);


        retranslateUi(BoxExporter);

        QMetaObject::connectSlotsByName(BoxExporter);
    } // setupUi

    void retranslateUi(QDialog *BoxExporter)
    {
        BoxExporter->setWindowTitle(QCoreApplication::translate("BoxExporter", "Box Export", nullptr));
        L_Namer->setText(QCoreApplication::translate("BoxExporter", "Namer:", nullptr));
        L_Scope->setText(QCoreApplication::translate("BoxExporter", "Scope", nullptr));
        L_FolderCreation->setText(QCoreApplication::translate("BoxExporter", "FolderCreation", nullptr));
        L_FolderPrefix->setText(QCoreApplication::translate("BoxExporter", "FolderPrefix", nullptr));
        L_EmptySlots->setText(QCoreApplication::translate("BoxExporter", "EmptySlots", nullptr));
        L_FileIndexPrefix->setText(QCoreApplication::translate("BoxExporter", "FileIndexPrefix", nullptr));
        L_Notify->setText(QCoreApplication::translate("BoxExporter", "Notify", nullptr));
        B_Export->setText(QCoreApplication::translate("BoxExporter", "Export", nullptr));
    } // retranslateUi

};

namespace Ui {
    class BoxExporter: public Ui_BoxExporter {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BOXEXPORTER_H
