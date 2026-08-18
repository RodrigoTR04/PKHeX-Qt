/********************************************************************************
** Form generated from reading UI file 'SAV_Inventory.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SAV_INVENTORY_H
#define UI_SAV_INVENTORY_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SAV_Inventory
{
public:
    QVBoxLayout *InventoryRoot;
    QTabWidget *tabControl1;
    QWidget *tableLayoutPanel1;
    QGridLayout *tableLayoutPanel1Grid;
    QPushButton *B_Sort;
    QLabel *L_Count;
    QPushButton *B_Save;
    QPushButton *B_GiveAll;
    QSpinBox *NUD_Count;
    QPushButton *B_Cancel;

    void setupUi(QDialog *SAV_Inventory)
    {
        if (SAV_Inventory->objectName().isEmpty())
            SAV_Inventory->setObjectName("SAV_Inventory");
        SAV_Inventory->setModal(true);
        SAV_Inventory->setMinimumSize(QSize(320, 400));
        InventoryRoot = new QVBoxLayout(SAV_Inventory);
        InventoryRoot->setObjectName("InventoryRoot");
        InventoryRoot->setContentsMargins(0, 0, 0, 0);
        tabControl1 = new QTabWidget(SAV_Inventory);
        tabControl1->setObjectName("tabControl1");

        InventoryRoot->addWidget(tabControl1);

        tableLayoutPanel1 = new QWidget(SAV_Inventory);
        tableLayoutPanel1->setObjectName("tableLayoutPanel1");
        tableLayoutPanel1Grid = new QGridLayout(tableLayoutPanel1);
        tableLayoutPanel1Grid->setObjectName("tableLayoutPanel1Grid");
        tableLayoutPanel1Grid->setContentsMargins(4, 4, 4, 4);
        B_Sort = new QPushButton(tableLayoutPanel1);
        B_Sort->setObjectName("B_Sort");

        tableLayoutPanel1Grid->addWidget(B_Sort, 0, 0, 1, 1);

        L_Count = new QLabel(tableLayoutPanel1);
        L_Count->setObjectName("L_Count");

        tableLayoutPanel1Grid->addWidget(L_Count, 0, 1, 1, 1);

        B_Save = new QPushButton(tableLayoutPanel1);
        B_Save->setObjectName("B_Save");

        tableLayoutPanel1Grid->addWidget(B_Save, 0, 2, 1, 1);

        B_GiveAll = new QPushButton(tableLayoutPanel1);
        B_GiveAll->setObjectName("B_GiveAll");

        tableLayoutPanel1Grid->addWidget(B_GiveAll, 1, 0, 1, 1);

        NUD_Count = new QSpinBox(tableLayoutPanel1);
        NUD_Count->setObjectName("NUD_Count");
        NUD_Count->setMinimum(1);
        NUD_Count->setMaximum(999);
        NUD_Count->setValue(1);

        tableLayoutPanel1Grid->addWidget(NUD_Count, 1, 1, 1, 1);

        B_Cancel = new QPushButton(tableLayoutPanel1);
        B_Cancel->setObjectName("B_Cancel");

        tableLayoutPanel1Grid->addWidget(B_Cancel, 1, 2, 1, 1);


        InventoryRoot->addWidget(tableLayoutPanel1);


        retranslateUi(SAV_Inventory);

        QMetaObject::connectSlotsByName(SAV_Inventory);
    } // setupUi

    void retranslateUi(QDialog *SAV_Inventory)
    {
        SAV_Inventory->setWindowTitle(QCoreApplication::translate("SAV_Inventory", "Inventory Editor", nullptr));
        B_Sort->setText(QCoreApplication::translate("SAV_Inventory", "Sort", nullptr));
        L_Count->setText(QCoreApplication::translate("SAV_Inventory", "Count:", nullptr));
        B_Save->setText(QCoreApplication::translate("SAV_Inventory", "Save", nullptr));
        B_GiveAll->setText(QCoreApplication::translate("SAV_Inventory", "Give All", nullptr));
        B_Cancel->setText(QCoreApplication::translate("SAV_Inventory", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SAV_Inventory: public Ui_SAV_Inventory {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SAV_INVENTORY_H
