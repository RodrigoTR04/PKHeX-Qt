/********************************************************************************
** Form generated from reading UI file 'SAV_Wondercard.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SAV_WONDERCARD_H
#define UI_SAV_WONDERCARD_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SAV_Wondercard
{
public:
    QHBoxLayout *WondercardRoot;
    QVBoxLayout *AlbumCol;
    QTableWidget *FLP_Gifts;
    QHBoxLayout *UsedRow;
    QPushButton *B_UnusedAll;
    QPushButton *B_UsedAll;
    QVBoxLayout *DetailCol;
    QLabel *L_Details;
    QPlainTextEdit *RTB;
    QLabel *L_Received;
    QListWidget *LB_Received;
    QHBoxLayout *IoRow;
    QPushButton *B_Import;
    QPushButton *B_Output;
    QLabel *L_QR;
    QHBoxLayout *SaveRow;
    QPushButton *B_Cancel;
    QPushButton *B_Save;

    void setupUi(QDialog *SAV_Wondercard)
    {
        if (SAV_Wondercard->objectName().isEmpty())
            SAV_Wondercard->setObjectName("SAV_Wondercard");
        SAV_Wondercard->setMinimumSize(QSize(640, 420));
        WondercardRoot = new QHBoxLayout(SAV_Wondercard);
        WondercardRoot->setObjectName("WondercardRoot");
        AlbumCol = new QVBoxLayout();
        AlbumCol->setObjectName("AlbumCol");
        FLP_Gifts = new QTableWidget(SAV_Wondercard);
        if (FLP_Gifts->columnCount() < 3)
            FLP_Gifts->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        FLP_Gifts->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        FLP_Gifts->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        FLP_Gifts->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        FLP_Gifts->setObjectName("FLP_Gifts");
        FLP_Gifts->setColumnCount(3);

        AlbumCol->addWidget(FLP_Gifts);

        UsedRow = new QHBoxLayout();
        UsedRow->setObjectName("UsedRow");
        B_UnusedAll = new QPushButton(SAV_Wondercard);
        B_UnusedAll->setObjectName("B_UnusedAll");

        UsedRow->addWidget(B_UnusedAll);

        B_UsedAll = new QPushButton(SAV_Wondercard);
        B_UsedAll->setObjectName("B_UsedAll");

        UsedRow->addWidget(B_UsedAll);


        AlbumCol->addLayout(UsedRow);


        WondercardRoot->addLayout(AlbumCol);

        DetailCol = new QVBoxLayout();
        DetailCol->setObjectName("DetailCol");
        L_Details = new QLabel(SAV_Wondercard);
        L_Details->setObjectName("L_Details");

        DetailCol->addWidget(L_Details);

        RTB = new QPlainTextEdit(SAV_Wondercard);
        RTB->setObjectName("RTB");

        DetailCol->addWidget(RTB);

        L_Received = new QLabel(SAV_Wondercard);
        L_Received->setObjectName("L_Received");

        DetailCol->addWidget(L_Received);

        LB_Received = new QListWidget(SAV_Wondercard);
        LB_Received->setObjectName("LB_Received");

        DetailCol->addWidget(LB_Received);

        IoRow = new QHBoxLayout();
        IoRow->setObjectName("IoRow");
        B_Import = new QPushButton(SAV_Wondercard);
        B_Import->setObjectName("B_Import");

        IoRow->addWidget(B_Import);

        B_Output = new QPushButton(SAV_Wondercard);
        B_Output->setObjectName("B_Output");

        IoRow->addWidget(B_Output);

        L_QR = new QLabel(SAV_Wondercard);
        L_QR->setObjectName("L_QR");

        IoRow->addWidget(L_QR);


        DetailCol->addLayout(IoRow);

        SaveRow = new QHBoxLayout();
        SaveRow->setObjectName("SaveRow");
        B_Cancel = new QPushButton(SAV_Wondercard);
        B_Cancel->setObjectName("B_Cancel");

        SaveRow->addWidget(B_Cancel);

        B_Save = new QPushButton(SAV_Wondercard);
        B_Save->setObjectName("B_Save");

        SaveRow->addWidget(B_Save);


        DetailCol->addLayout(SaveRow);


        WondercardRoot->addLayout(DetailCol);


        retranslateUi(SAV_Wondercard);

        QMetaObject::connectSlotsByName(SAV_Wondercard);
    } // setupUi

    void retranslateUi(QDialog *SAV_Wondercard)
    {
        SAV_Wondercard->setWindowTitle(QCoreApplication::translate("SAV_Wondercard", "Mystery Gift I/O", nullptr));
        QTableWidgetItem *___qtablewidgetitem = FLP_Gifts->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("SAV_Wondercard", "#", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = FLP_Gifts->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("SAV_Wondercard", "Type", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = FLP_Gifts->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("SAV_Wondercard", "Card", nullptr));
        B_UnusedAll->setText(QCoreApplication::translate("SAV_Wondercard", "All Unused", nullptr));
        B_UsedAll->setText(QCoreApplication::translate("SAV_Wondercard", "All Used", nullptr));
        L_Details->setText(QCoreApplication::translate("SAV_Wondercard", "Details:", nullptr));
        L_Received->setText(QCoreApplication::translate("SAV_Wondercard", "Received List:", nullptr));
        B_Import->setText(QCoreApplication::translate("SAV_Wondercard", "Import", nullptr));
        B_Output->setText(QCoreApplication::translate("SAV_Wondercard", "Export", nullptr));
        L_QR->setText(QCoreApplication::translate("SAV_Wondercard", "QR!", nullptr));
        B_Cancel->setText(QCoreApplication::translate("SAV_Wondercard", "Cancel", nullptr));
        B_Save->setText(QCoreApplication::translate("SAV_Wondercard", "Save", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SAV_Wondercard: public Ui_SAV_Wondercard {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SAV_WONDERCARD_H
