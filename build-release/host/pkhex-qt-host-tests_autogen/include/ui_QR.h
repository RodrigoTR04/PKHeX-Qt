/********************************************************************************
** Form generated from reading UI file 'QR.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QR_H
#define UI_QR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QR
{
public:
    QVBoxLayout *QRLayout;
    QSplitter *splitContainer1;
    QWidget *QRPanel1;
    QHBoxLayout *flowLayoutPanel1;
    QLabel *L_Box;
    QSpinBox *NUD_Box;
    QLabel *L_Slot;
    QSpinBox *NUD_Slot;
    QLabel *L_Copies;
    QSpinBox *NUD_Copies;
    QPushButton *B_Refresh;
    QSpacerItem *QRPanel1Spacer;
    QLabel *PB_QR;

    void setupUi(QDialog *QR)
    {
        if (QR->objectName().isEmpty())
            QR->setObjectName("QR");
        QR->setModal(true);
        QRLayout = new QVBoxLayout(QR);
        QRLayout->setObjectName("QRLayout");
        QRLayout->setContentsMargins(0, 0, 0, 0);
        splitContainer1 = new QSplitter(QR);
        splitContainer1->setObjectName("splitContainer1");
        splitContainer1->setOrientation(Qt::Vertical);
        QRPanel1 = new QWidget(splitContainer1);
        QRPanel1->setObjectName("QRPanel1");
        flowLayoutPanel1 = new QHBoxLayout(QRPanel1);
        flowLayoutPanel1->setObjectName("flowLayoutPanel1");
        flowLayoutPanel1->setContentsMargins(8, 6, 8, 6);
        L_Box = new QLabel(QRPanel1);
        L_Box->setObjectName("L_Box");

        flowLayoutPanel1->addWidget(L_Box);

        NUD_Box = new QSpinBox(QRPanel1);
        NUD_Box->setObjectName("NUD_Box");
        NUD_Box->setMinimum(1);
        NUD_Box->setMaximum(32);
        NUD_Box->setValue(1);

        flowLayoutPanel1->addWidget(NUD_Box);

        L_Slot = new QLabel(QRPanel1);
        L_Slot->setObjectName("L_Slot");

        flowLayoutPanel1->addWidget(L_Slot);

        NUD_Slot = new QSpinBox(QRPanel1);
        NUD_Slot->setObjectName("NUD_Slot");
        NUD_Slot->setMinimum(1);
        NUD_Slot->setMaximum(30);
        NUD_Slot->setValue(1);

        flowLayoutPanel1->addWidget(NUD_Slot);

        L_Copies = new QLabel(QRPanel1);
        L_Copies->setObjectName("L_Copies");

        flowLayoutPanel1->addWidget(L_Copies);

        NUD_Copies = new QSpinBox(QRPanel1);
        NUD_Copies->setObjectName("NUD_Copies");
        NUD_Copies->setMinimum(1);
        NUD_Copies->setMaximum(960);
        NUD_Copies->setValue(1);

        flowLayoutPanel1->addWidget(NUD_Copies);

        B_Refresh = new QPushButton(QRPanel1);
        B_Refresh->setObjectName("B_Refresh");

        flowLayoutPanel1->addWidget(B_Refresh);

        QRPanel1Spacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        flowLayoutPanel1->addItem(QRPanel1Spacer);

        splitContainer1->addWidget(QRPanel1);
        PB_QR = new QLabel(splitContainer1);
        PB_QR->setObjectName("PB_QR");
        PB_QR->setAlignment(Qt::AlignCenter);
        splitContainer1->addWidget(PB_QR);

        QRLayout->addWidget(splitContainer1);


        retranslateUi(QR);

        QMetaObject::connectSlotsByName(QR);
    } // setupUi

    void retranslateUi(QDialog *QR)
    {
        QR->setWindowTitle(QCoreApplication::translate("QR", "PKHeX QR Code (Click QR to Copy Image)", nullptr));
        L_Box->setText(QCoreApplication::translate("QR", "Box:", nullptr));
        L_Slot->setText(QCoreApplication::translate("QR", "Slot:", nullptr));
        L_Copies->setText(QCoreApplication::translate("QR", "Copies:", nullptr));
        B_Refresh->setText(QCoreApplication::translate("QR", "Refresh", nullptr));
        PB_QR->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class QR: public Ui_QR {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QR_H
