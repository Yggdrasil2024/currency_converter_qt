/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QTextEdit *source_amount;
    QComboBox *source_currency;
    QComboBox *target_currency;
    QLabel *label;
    QPushButton *convert;
    QPushButton *refresh;
    QTextEdit *response_text;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(261, 350);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        source_amount = new QTextEdit(centralwidget);
        source_amount->setObjectName("source_amount");
        source_amount->setGeometry(QRect(20, 70, 221, 41));
        source_currency = new QComboBox(centralwidget);
        source_currency->setObjectName("source_currency");
        source_currency->setGeometry(QRect(20, 190, 71, 31));
        target_currency = new QComboBox(centralwidget);
        target_currency->setObjectName("target_currency");
        target_currency->setGeometry(QRect(170, 190, 71, 31));
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        label->setGeometry(QRect(20, 10, 151, 41));
        convert = new QPushButton(centralwidget);
        convert->setObjectName("convert");
        convert->setGeometry(QRect(20, 230, 221, 41));
        refresh = new QPushButton(centralwidget);
        refresh->setObjectName("refresh");
        refresh->setGeometry(QRect(20, 280, 221, 41));
        response_text = new QTextEdit(centralwidget);
        response_text->setObjectName("response_text");
        response_text->setGeometry(QRect(20, 130, 221, 41));
        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        source_amount->setPlaceholderText(QCoreApplication::translate("MainWindow", "Montant", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "currency converter", nullptr));
        convert->setText(QCoreApplication::translate("MainWindow", "convert", nullptr));
        refresh->setText(QCoreApplication::translate("MainWindow", "refresh", nullptr));
        response_text->setPlaceholderText(QCoreApplication::translate("MainWindow", "Reponse", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
