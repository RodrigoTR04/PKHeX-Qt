/********************************************************************************
** Form generated from reading UI file 'RibbonEditor.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RIBBONEDITOR_H
#define UI_RIBBONEDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RibbonEditor
{
public:
    QVBoxLayout *RibbonRoot;
    QSplitter *SPLIT_Ribbons;
    QWidget *FLP_Ribbons;
    QTableWidget *TLP_Ribbons;
    QHBoxLayout *RibbonButtons;
    QPushButton *B_All;
    QPushButton *B_None;
    QComboBox *CB_Affixed;
    QSpacerItem *RibbonButtonSpacer;
    QPushButton *B_Cancel;
    QPushButton *B_Save;

    void setupUi(QDialog *RibbonEditor)
    {
        if (RibbonEditor->objectName().isEmpty())
            RibbonEditor->setObjectName("RibbonEditor");
        RibbonEditor->setModal(true);
        RibbonEditor->setMinimumSize(QSize(627, 363));
        RibbonRoot = new QVBoxLayout(RibbonEditor);
        RibbonRoot->setObjectName("RibbonRoot");
        SPLIT_Ribbons = new QSplitter(RibbonEditor);
        SPLIT_Ribbons->setObjectName("SPLIT_Ribbons");
        SPLIT_Ribbons->setOrientation(Qt::Horizontal);
        FLP_Ribbons = new QWidget(SPLIT_Ribbons);
        FLP_Ribbons->setObjectName("FLP_Ribbons");
        SPLIT_Ribbons->addWidget(FLP_Ribbons);
        TLP_Ribbons = new QTableWidget(SPLIT_Ribbons);
        if (TLP_Ribbons->columnCount() < 2)
            TLP_Ribbons->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        TLP_Ribbons->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        TLP_Ribbons->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        TLP_Ribbons->setObjectName("TLP_Ribbons");
        TLP_Ribbons->setColumnCount(2);
        SPLIT_Ribbons->addWidget(TLP_Ribbons);

        RibbonRoot->addWidget(SPLIT_Ribbons);

        RibbonButtons = new QHBoxLayout();
        RibbonButtons->setObjectName("RibbonButtons");
        B_All = new QPushButton(RibbonEditor);
        B_All->setObjectName("B_All");

        RibbonButtons->addWidget(B_All);

        B_None = new QPushButton(RibbonEditor);
        B_None->setObjectName("B_None");

        RibbonButtons->addWidget(B_None);

        CB_Affixed = new QComboBox(RibbonEditor);
        CB_Affixed->setObjectName("CB_Affixed");

        RibbonButtons->addWidget(CB_Affixed);

        RibbonButtonSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        RibbonButtons->addItem(RibbonButtonSpacer);

        B_Cancel = new QPushButton(RibbonEditor);
        B_Cancel->setObjectName("B_Cancel");

        RibbonButtons->addWidget(B_Cancel);

        B_Save = new QPushButton(RibbonEditor);
        B_Save->setObjectName("B_Save");

        RibbonButtons->addWidget(B_Save);


        RibbonRoot->addLayout(RibbonButtons);


        retranslateUi(RibbonEditor);

        QMetaObject::connectSlotsByName(RibbonEditor);
    } // setupUi

    void retranslateUi(QDialog *RibbonEditor)
    {
        RibbonEditor->setWindowTitle(QCoreApplication::translate("RibbonEditor", "Ribbon Editor", nullptr));
        B_All->setText(QCoreApplication::translate("RibbonEditor", "Give All", nullptr));
        B_None->setText(QCoreApplication::translate("RibbonEditor", "Remove All", nullptr));
        B_Cancel->setText(QCoreApplication::translate("RibbonEditor", "Cancel", nullptr));
        B_Save->setText(QCoreApplication::translate("RibbonEditor", "Save", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RibbonEditor: public Ui_RibbonEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RIBBONEDITOR_H
