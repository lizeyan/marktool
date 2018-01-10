/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "dataedit.h"
#include "datascroll.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_3;
    DataEdit *dataEdit;
    QSlider *valueSlider;
    QHBoxLayout *horizontalLayout;
    QLabel *lblMonth;
    DataScroll *dataScroll;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QComboBox *selFeature;
    QLabel *support_label;
    QComboBox *selSupportFeature;
    QCheckBox *chkLabel2;
    QPushButton *btnOpen;
    QPushButton *btnSave;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1003, 400);
        MainWindow->setStyleSheet(QStringLiteral("s"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        dataEdit = new DataEdit(centralWidget);
        dataEdit->setObjectName(QStringLiteral("dataEdit"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(dataEdit->sizePolicy().hasHeightForWidth());
        dataEdit->setSizePolicy(sizePolicy);

        horizontalLayout_3->addWidget(dataEdit);

        valueSlider = new QSlider(centralWidget);
        valueSlider->setObjectName(QStringLiteral("valueSlider"));
        valueSlider->setMinimum(1);
        valueSlider->setMaximum(100);
        valueSlider->setSliderPosition(100);
        valueSlider->setOrientation(Qt::Vertical);

        horizontalLayout_3->addWidget(valueSlider);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        lblMonth = new QLabel(centralWidget);
        lblMonth->setObjectName(QStringLiteral("lblMonth"));
        QFont font;
        font.setPointSize(18);
        lblMonth->setFont(font);

        horizontalLayout->addWidget(lblMonth);

        dataScroll = new DataScroll(centralWidget);
        dataScroll->setObjectName(QStringLiteral("dataScroll"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(dataScroll->sizePolicy().hasHeightForWidth());
        dataScroll->setSizePolicy(sizePolicy1);
        dataScroll->setMinimumSize(QSize(0, 90));
        dataScroll->setMaximumSize(QSize(16777215, 90));

        horizontalLayout->addWidget(dataScroll);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        QSizePolicy sizePolicy2(QSizePolicy::Maximum, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy2);
        QFont font1;
        font1.setPointSize(14);
        label->setFont(font1);

        horizontalLayout_2->addWidget(label);

        selFeature = new QComboBox(centralWidget);
        selFeature->setObjectName(QStringLiteral("selFeature"));
        QSizePolicy sizePolicy3(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(selFeature->sizePolicy().hasHeightForWidth());
        selFeature->setSizePolicy(sizePolicy3);
        selFeature->setFont(font1);

        horizontalLayout_2->addWidget(selFeature);

        support_label = new QLabel(centralWidget);
        support_label->setObjectName(QStringLiteral("support_label"));
        sizePolicy2.setHeightForWidth(support_label->sizePolicy().hasHeightForWidth());
        support_label->setSizePolicy(sizePolicy2);

        horizontalLayout_2->addWidget(support_label);

        selSupportFeature = new QComboBox(centralWidget);
        selSupportFeature->setObjectName(QStringLiteral("selSupportFeature"));
        sizePolicy3.setHeightForWidth(selSupportFeature->sizePolicy().hasHeightForWidth());
        selSupportFeature->setSizePolicy(sizePolicy3);

        horizontalLayout_2->addWidget(selSupportFeature);

        chkLabel2 = new QCheckBox(centralWidget);
        chkLabel2->setObjectName(QStringLiteral("chkLabel2"));
        chkLabel2->setEnabled(false);
        QSizePolicy sizePolicy4(QSizePolicy::Maximum, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(chkLabel2->sizePolicy().hasHeightForWidth());
        chkLabel2->setSizePolicy(sizePolicy4);
        chkLabel2->setFont(font1);
        chkLabel2->setCheckable(true);

        horizontalLayout_2->addWidget(chkLabel2);

        btnOpen = new QPushButton(centralWidget);
        btnOpen->setObjectName(QStringLiteral("btnOpen"));
        sizePolicy4.setHeightForWidth(btnOpen->sizePolicy().hasHeightForWidth());
        btnOpen->setSizePolicy(sizePolicy4);
        btnOpen->setFont(font1);

        horizontalLayout_2->addWidget(btnOpen);

        btnSave = new QPushButton(centralWidget);
        btnSave->setObjectName(QStringLiteral("btnSave"));
        sizePolicy4.setHeightForWidth(btnSave->sizePolicy().hasHeightForWidth());
        btnSave->setSizePolicy(sizePolicy4);
        btnSave->setFont(font1);

        horizontalLayout_2->addWidget(btnSave);


        verticalLayout->addLayout(horizontalLayout_2);

        MainWindow->setCentralWidget(centralWidget);
#ifndef QT_NO_SHORTCUT
        label->setBuddy(selFeature);
        support_label->setBuddy(selSupportFeature);
#endif // QT_NO_SHORTCUT

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Please Open Your Data File ...", nullptr));
        lblMonth->setText(QApplication::translate("MainWindow", "Month", nullptr));
        label->setText(QApplication::translate("MainWindow", "Display", nullptr));
        support_label->setText(QApplication::translate("MainWindow", "Support", nullptr));
        chkLabel2->setText(QApplication::translate("MainWindow", "Diff Mode", nullptr));
        btnOpen->setText(QApplication::translate("MainWindow", "Open", nullptr));
        btnSave->setText(QApplication::translate("MainWindow", "Save", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
