/********************************************************************************
** Form generated from reading UI file 'BatchEditor.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BATCHEDITOR_H
#define UI_BATCHEDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BatchEditor
{
public:
    QVBoxLayout *BatchRoot;
    QWidget *FLP_RB;
    QHBoxLayout *ScopeRow;
    QRadioButton *RB_Boxes;
    QRadioButton *RB_Party;
    QRadioButton *RB_Path;
    QLineEdit *TB_Folder;
    QHBoxLayout *AddRow;
    QComboBox *CB_Property;
    QPushButton *B_Add;
    QPlainTextEdit *RTB_Instructions;
    QHBoxLayout *RunRow;
    QProgressBar *PB_Show;
    QPushButton *B_Go;

    void setupUi(QDialog *BatchEditor)
    {
        if (BatchEditor->objectName().isEmpty())
            BatchEditor->setObjectName("BatchEditor");
        BatchEditor->setMinimumSize(QSize(500, 340));
        BatchRoot = new QVBoxLayout(BatchEditor);
        BatchRoot->setObjectName("BatchRoot");
        FLP_RB = new QWidget(BatchEditor);
        FLP_RB->setObjectName("FLP_RB");
        ScopeRow = new QHBoxLayout(FLP_RB);
        ScopeRow->setObjectName("ScopeRow");
        ScopeRow->setContentsMargins(0, 0, 0, 0);
        RB_Boxes = new QRadioButton(FLP_RB);
        RB_Boxes->setObjectName("RB_Boxes");
        RB_Boxes->setChecked(true);

        ScopeRow->addWidget(RB_Boxes);

        RB_Party = new QRadioButton(FLP_RB);
        RB_Party->setObjectName("RB_Party");

        ScopeRow->addWidget(RB_Party);

        RB_Path = new QRadioButton(FLP_RB);
        RB_Path->setObjectName("RB_Path");

        ScopeRow->addWidget(RB_Path);

        TB_Folder = new QLineEdit(FLP_RB);
        TB_Folder->setObjectName("TB_Folder");
        TB_Folder->setReadOnly(true);

        ScopeRow->addWidget(TB_Folder);


        BatchRoot->addWidget(FLP_RB);

        AddRow = new QHBoxLayout();
        AddRow->setObjectName("AddRow");
        CB_Property = new QComboBox(BatchEditor);
        CB_Property->setObjectName("CB_Property");

        AddRow->addWidget(CB_Property);

        B_Add = new QPushButton(BatchEditor);
        B_Add->setObjectName("B_Add");

        AddRow->addWidget(B_Add);


        BatchRoot->addLayout(AddRow);

        RTB_Instructions = new QPlainTextEdit(BatchEditor);
        RTB_Instructions->setObjectName("RTB_Instructions");

        BatchRoot->addWidget(RTB_Instructions);

        RunRow = new QHBoxLayout();
        RunRow->setObjectName("RunRow");
        PB_Show = new QProgressBar(BatchEditor);
        PB_Show->setObjectName("PB_Show");

        RunRow->addWidget(PB_Show);

        B_Go = new QPushButton(BatchEditor);
        B_Go->setObjectName("B_Go");

        RunRow->addWidget(B_Go);


        BatchRoot->addLayout(RunRow);


        retranslateUi(BatchEditor);

        QMetaObject::connectSlotsByName(BatchEditor);
    } // setupUi

    void retranslateUi(QDialog *BatchEditor)
    {
        BatchEditor->setWindowTitle(QCoreApplication::translate("BatchEditor", "Batch Editor", nullptr));
        RB_Boxes->setText(QCoreApplication::translate("BatchEditor", "Boxes", nullptr));
        RB_Party->setText(QCoreApplication::translate("BatchEditor", "Party", nullptr));
        RB_Path->setText(QCoreApplication::translate("BatchEditor", "Folder...", nullptr));
        B_Add->setText(QCoreApplication::translate("BatchEditor", "Add", nullptr));
        B_Go->setText(QCoreApplication::translate("BatchEditor", "Run", nullptr));
    } // retranslateUi

};

namespace Ui {
    class BatchEditor: public Ui_BatchEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BATCHEDITOR_H
