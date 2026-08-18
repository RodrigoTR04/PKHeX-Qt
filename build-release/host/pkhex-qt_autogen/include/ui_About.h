/********************************************************************************
** Form generated from reading UI file 'About.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUT_H
#define UI_ABOUT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_About
{
public:
    QVBoxLayout *AboutLayout;
    QLabel *L_Disclaimer;
    QLabel *L_Version;
    QLabel *L_Thanks;
    QTabWidget *TC_About;
    QWidget *Tab_Shortcuts;
    QVBoxLayout *Tab_ShortcutsLayout;
    QPlainTextEdit *RTB_Shortcuts;
    QWidget *Tab_Changelog;
    QVBoxLayout *Tab_ChangelogLayout;
    QPlainTextEdit *RTB_Changelog;
    QWidget *Tab_Notices;
    QVBoxLayout *Tab_NoticesLayout;
    QPlainTextEdit *RTB_Notices;

    void setupUi(QDialog *About)
    {
        if (About->objectName().isEmpty())
            About->setObjectName("About");
        About->setModal(true);
        AboutLayout = new QVBoxLayout(About);
        AboutLayout->setObjectName("AboutLayout");
        L_Disclaimer = new QLabel(About);
        L_Disclaimer->setObjectName("L_Disclaimer");
        L_Disclaimer->setWordWrap(true);

        AboutLayout->addWidget(L_Disclaimer);

        L_Version = new QLabel(About);
        L_Version->setObjectName("L_Version");

        AboutLayout->addWidget(L_Version);

        L_Thanks = new QLabel(About);
        L_Thanks->setObjectName("L_Thanks");
        L_Thanks->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

        AboutLayout->addWidget(L_Thanks);

        TC_About = new QTabWidget(About);
        TC_About->setObjectName("TC_About");
        Tab_Shortcuts = new QWidget();
        Tab_Shortcuts->setObjectName("Tab_Shortcuts");
        Tab_ShortcutsLayout = new QVBoxLayout(Tab_Shortcuts);
        Tab_ShortcutsLayout->setObjectName("Tab_ShortcutsLayout");
        Tab_ShortcutsLayout->setContentsMargins(0, 0, 0, 0);
        RTB_Shortcuts = new QPlainTextEdit(Tab_Shortcuts);
        RTB_Shortcuts->setObjectName("RTB_Shortcuts");
        RTB_Shortcuts->setReadOnly(true);

        Tab_ShortcutsLayout->addWidget(RTB_Shortcuts);

        TC_About->addTab(Tab_Shortcuts, QString());
        Tab_Changelog = new QWidget();
        Tab_Changelog->setObjectName("Tab_Changelog");
        Tab_ChangelogLayout = new QVBoxLayout(Tab_Changelog);
        Tab_ChangelogLayout->setObjectName("Tab_ChangelogLayout");
        Tab_ChangelogLayout->setContentsMargins(0, 0, 0, 0);
        RTB_Changelog = new QPlainTextEdit(Tab_Changelog);
        RTB_Changelog->setObjectName("RTB_Changelog");
        RTB_Changelog->setReadOnly(true);

        Tab_ChangelogLayout->addWidget(RTB_Changelog);

        TC_About->addTab(Tab_Changelog, QString());
        Tab_Notices = new QWidget();
        Tab_Notices->setObjectName("Tab_Notices");
        Tab_NoticesLayout = new QVBoxLayout(Tab_Notices);
        Tab_NoticesLayout->setObjectName("Tab_NoticesLayout");
        Tab_NoticesLayout->setContentsMargins(0, 0, 0, 0);
        RTB_Notices = new QPlainTextEdit(Tab_Notices);
        RTB_Notices->setObjectName("RTB_Notices");
        RTB_Notices->setReadOnly(true);

        Tab_NoticesLayout->addWidget(RTB_Notices);

        TC_About->addTab(Tab_Notices, QString());

        AboutLayout->addWidget(TC_About);


        retranslateUi(About);

        TC_About->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(About);
    } // setupUi

    void retranslateUi(QDialog *About)
    {
        About->setWindowTitle(QCoreApplication::translate("About", "About", nullptr));
        L_Disclaimer->setText(QCoreApplication::translate("About", "PKHeX Qt is an unofficial independent port of PKHeX. It is not affiliated with or endorsed by the Original PKHeX authors.", nullptr));
        L_Version->setText(QString());
        L_Thanks->setText(QCoreApplication::translate("About", "Thanks to all the researchers!", nullptr));
        TC_About->setTabText(TC_About->indexOf(Tab_Shortcuts), QCoreApplication::translate("About", "Shortcuts", nullptr));
        TC_About->setTabText(TC_About->indexOf(Tab_Changelog), QCoreApplication::translate("About", "Changelog", nullptr));
        TC_About->setTabText(TC_About->indexOf(Tab_Notices), QCoreApplication::translate("About", "Licenses", nullptr));
    } // retranslateUi

};

namespace Ui {
    class About: public Ui_About {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUT_H
