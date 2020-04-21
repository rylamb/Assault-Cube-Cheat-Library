/********************************************************************************
** Form generated from reading UI file 'acinjectorui.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ACINJECTORUI_H
#define UI_ACINJECTORUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ACInjectorUI
{
public:
    QWidget *centralwidget;
    QLabel *ACLogo;
    QLabel *ProcessFoundIcon;
    QLabel *ProcessFoundText;
    QLabel *InjectableFoundIcon;
    QLabel *InjectableFoundText;
    QLabel *LocationLabel;
    QLineEdit *LocationTextField;
    QPushButton *LocationBrowseButton;
    QPushButton *InjectEjectButton;
    QPushButton *CheckProcessButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ACInjectorUI)
    {
        if (ACInjectorUI->objectName().isEmpty())
            ACInjectorUI->setObjectName(QString::fromUtf8("ACInjectorUI"));
        ACInjectorUI->resize(805, 398);
        centralwidget = new QWidget(ACInjectorUI);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        centralwidget->setAutoFillBackground(true);
        ACLogo = new QLabel(centralwidget);
        ACLogo->setObjectName(QString::fromUtf8("ACLogo"));
        ACLogo->setGeometry(QRect(0, 0, 801, 111));
        ACLogo->setPixmap(QPixmap(QString::fromUtf8("../Images/ACEZModeLogo.png")));
        ACLogo->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
        ProcessFoundIcon = new QLabel(centralwidget);
        ProcessFoundIcon->setObjectName(QString::fromUtf8("ProcessFoundIcon"));
        ProcessFoundIcon->setGeometry(QRect(80, 140, 31, 31));
        ProcessFoundIcon->setPixmap(QPixmap(QString::fromUtf8("../Images/checkmark-512.png")));
        ProcessFoundIcon->setScaledContents(true);
        ProcessFoundText = new QLabel(centralwidget);
        ProcessFoundText->setObjectName(QString::fromUtf8("ProcessFoundText"));
        ProcessFoundText->setGeometry(QRect(130, 140, 591, 31));
        QFont font;
        font.setPointSize(13);
        ProcessFoundText->setFont(font);
        InjectableFoundIcon = new QLabel(centralwidget);
        InjectableFoundIcon->setObjectName(QString::fromUtf8("InjectableFoundIcon"));
        InjectableFoundIcon->setGeometry(QRect(80, 180, 31, 31));
        InjectableFoundIcon->setPixmap(QPixmap(QString::fromUtf8("../Images/checkmark-512.png")));
        InjectableFoundIcon->setScaledContents(true);
        InjectableFoundText = new QLabel(centralwidget);
        InjectableFoundText->setObjectName(QString::fromUtf8("InjectableFoundText"));
        InjectableFoundText->setGeometry(QRect(130, 180, 601, 31));
        InjectableFoundText->setFont(font);
        LocationLabel = new QLabel(centralwidget);
        LocationLabel->setObjectName(QString::fromUtf8("LocationLabel"));
        LocationLabel->setGeometry(QRect(80, 220, 111, 41));
        LocationLabel->setFont(font);
        LocationTextField = new QLineEdit(centralwidget);
        LocationTextField->setObjectName(QString::fromUtf8("LocationTextField"));
        LocationTextField->setGeometry(QRect(182, 220, 411, 41));
        LocationTextField->setFont(font);
        LocationTextField->setReadOnly(true);
        LocationBrowseButton = new QPushButton(centralwidget);
        LocationBrowseButton->setObjectName(QString::fromUtf8("LocationBrowseButton"));
        LocationBrowseButton->setGeometry(QRect(599, 214, 131, 51));
        LocationBrowseButton->setFont(font);
        InjectEjectButton = new QPushButton(centralwidget);
        InjectEjectButton->setObjectName(QString::fromUtf8("InjectEjectButton"));
        InjectEjectButton->setEnabled(false);
        InjectEjectButton->setGeometry(QRect(320, 280, 171, 51));
        QFont font1;
        font1.setPointSize(18);
        InjectEjectButton->setFont(font1);
        CheckProcessButton = new QPushButton(centralwidget);
        CheckProcessButton->setObjectName(QString::fromUtf8("CheckProcessButton"));
        CheckProcessButton->setGeometry(QRect(600, 140, 131, 51));
        CheckProcessButton->setFont(font);
        ACInjectorUI->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ACInjectorUI);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 805, 20));
        ACInjectorUI->setMenuBar(menubar);
        statusbar = new QStatusBar(ACInjectorUI);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        ACInjectorUI->setStatusBar(statusbar);

        retranslateUi(ACInjectorUI);

        QMetaObject::connectSlotsByName(ACInjectorUI);
    } // setupUi

    void retranslateUi(QMainWindow *ACInjectorUI)
    {
        ACInjectorUI->setWindowTitle(QCoreApplication::translate("ACInjectorUI", "ACInjectorUI", nullptr));
        ACLogo->setText(QString());
        ProcessFoundIcon->setText(QString());
        ProcessFoundText->setText(QCoreApplication::translate("ACInjectorUI", "Assault Cube Process Detected", nullptr));
        InjectableFoundIcon->setText(QString());
        InjectableFoundText->setText(QCoreApplication::translate("ACInjectorUI", "Injectable DLL Found", nullptr));
        LocationLabel->setText(QCoreApplication::translate("ACInjectorUI", "Location: ", nullptr));
        LocationBrowseButton->setText(QCoreApplication::translate("ACInjectorUI", "Browse", nullptr));
        InjectEjectButton->setText(QCoreApplication::translate("ACInjectorUI", "Inject", nullptr));
        CheckProcessButton->setText(QCoreApplication::translate("ACInjectorUI", "Check Again", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ACInjectorUI: public Ui_ACInjectorUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ACINJECTORUI_H
