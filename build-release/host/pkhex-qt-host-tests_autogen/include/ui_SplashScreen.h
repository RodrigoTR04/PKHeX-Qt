/********************************************************************************
** Form generated from reading UI file 'SplashScreen.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SPLASHSCREEN_H
#define UI_SPLASHSCREEN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SplashScreen
{
public:
    QHBoxLayout *SplashLayout;
    QLabel *PB_Icon;
    QVBoxLayout *SplashTextLayout;
    QLabel *L_Status;
    QLabel *L_Site;

    void setupUi(QDialog *SplashScreen)
    {
        if (SplashScreen->objectName().isEmpty())
            SplashScreen->setObjectName("SplashScreen");
        SplashLayout = new QHBoxLayout(SplashScreen);
        SplashLayout->setObjectName("SplashLayout");
        SplashLayout->setContentsMargins(8, 8, 8, 8);
        PB_Icon = new QLabel(SplashScreen);
        PB_Icon->setObjectName("PB_Icon");
        PB_Icon->setMinimumSize(QSize(32, 32));
        PB_Icon->setMaximumSize(QSize(32, 32));

        SplashLayout->addWidget(PB_Icon);

        SplashTextLayout = new QVBoxLayout();
        SplashTextLayout->setObjectName("SplashTextLayout");
        L_Status = new QLabel(SplashScreen);
        L_Status->setObjectName("L_Status");

        SplashTextLayout->addWidget(L_Status);

        L_Site = new QLabel(SplashScreen);
        L_Site->setObjectName("L_Site");

        SplashTextLayout->addWidget(L_Site);


        SplashLayout->addLayout(SplashTextLayout);


        retranslateUi(SplashScreen);

        QMetaObject::connectSlotsByName(SplashScreen);
    } // setupUi

    void retranslateUi(QDialog *SplashScreen)
    {
        SplashScreen->setWindowTitle(QCoreApplication::translate("SplashScreen", "PKHeX Qt", nullptr));
        PB_Icon->setText(QString());
        L_Status->setText(QCoreApplication::translate("SplashScreen", "Starting up PKHeX...", nullptr));
        L_Site->setText(QCoreApplication::translate("SplashScreen", "ProjectPokemon.org", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SplashScreen: public Ui_SplashScreen {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SPLASHSCREEN_H
