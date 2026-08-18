/********************************************************************************
** Form generated from reading UI file 'ErrorWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ERRORWINDOW_H
#define UI_ERRORWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ErrorWindow
{
public:
    QVBoxLayout *ErrorLayout;
    QLabel *L_Message;
    QLabel *L_ProvideInfo;
    QPlainTextEdit *T_ExceptionDetails;
    QHBoxLayout *ErrorButtons;
    QPushButton *B_CopyToClipboard;
    QSpacerItem *ErrorButtonSpacer;
    QPushButton *B_Continue;
    QPushButton *B_Abort;

    void setupUi(QDialog *ErrorWindow)
    {
        if (ErrorWindow->objectName().isEmpty())
            ErrorWindow->setObjectName("ErrorWindow");
        ErrorWindow->setModal(true);
        ErrorLayout = new QVBoxLayout(ErrorWindow);
        ErrorLayout->setObjectName("ErrorLayout");
        L_Message = new QLabel(ErrorWindow);
        L_Message->setObjectName("L_Message");
        L_Message->setWordWrap(true);

        ErrorLayout->addWidget(L_Message);

        L_ProvideInfo = new QLabel(ErrorWindow);
        L_ProvideInfo->setObjectName("L_ProvideInfo");

        ErrorLayout->addWidget(L_ProvideInfo);

        T_ExceptionDetails = new QPlainTextEdit(ErrorWindow);
        T_ExceptionDetails->setObjectName("T_ExceptionDetails");
        T_ExceptionDetails->setReadOnly(true);

        ErrorLayout->addWidget(T_ExceptionDetails);

        ErrorButtons = new QHBoxLayout();
        ErrorButtons->setObjectName("ErrorButtons");
        B_CopyToClipboard = new QPushButton(ErrorWindow);
        B_CopyToClipboard->setObjectName("B_CopyToClipboard");

        ErrorButtons->addWidget(B_CopyToClipboard);

        ErrorButtonSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        ErrorButtons->addItem(ErrorButtonSpacer);

        B_Continue = new QPushButton(ErrorWindow);
        B_Continue->setObjectName("B_Continue");

        ErrorButtons->addWidget(B_Continue);

        B_Abort = new QPushButton(ErrorWindow);
        B_Abort->setObjectName("B_Abort");

        ErrorButtons->addWidget(B_Abort);


        ErrorLayout->addLayout(ErrorButtons);


        retranslateUi(ErrorWindow);

        QMetaObject::connectSlotsByName(ErrorWindow);
    } // setupUi

    void retranslateUi(QDialog *ErrorWindow)
    {
        ErrorWindow->setWindowTitle(QCoreApplication::translate("ErrorWindow", "Error", nullptr));
        L_Message->setText(QCoreApplication::translate("ErrorWindow", "An unknown error has occurred.", nullptr));
        L_ProvideInfo->setText(QCoreApplication::translate("ErrorWindow", "Please provide this information when reporting this error:", nullptr));
        B_CopyToClipboard->setText(QCoreApplication::translate("ErrorWindow", "Copy to Clipboard", nullptr));
        B_Continue->setText(QCoreApplication::translate("ErrorWindow", "Continue", nullptr));
        B_Abort->setText(QCoreApplication::translate("ErrorWindow", "Abort", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ErrorWindow: public Ui_ErrorWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ERRORWINDOW_H
