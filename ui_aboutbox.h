/********************************************************************************
** Form generated from reading UI file 'aboutbox.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUTBOX_H
#define UI_ABOUTBOX_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AboutBox
{
public:
    QHBoxLayout *horizontalLayout;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;

    void setupUi(QDialog *AboutBox)
    {
        if (AboutBox->objectName().isEmpty())
            AboutBox->setObjectName(QStringLiteral("AboutBox"));
        AboutBox->setWindowModality(Qt::ApplicationModal);
        AboutBox->resize(564, 500);
        AboutBox->setModal(true);
        horizontalLayout = new QHBoxLayout(AboutBox);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        scrollArea = new QScrollArea(AboutBox);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 562, 498));
        horizontalLayout_2 = new QHBoxLayout(scrollAreaWidgetContents);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(scrollAreaWidgetContents);
        label->setObjectName(QStringLiteral("label"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);
        label->setLineWidth(0);
        label->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        label->setWordWrap(true);
        label->setMargin(15);
        label->setTextInteractionFlags(Qt::LinksAccessibleByMouse);

        horizontalLayout_2->addWidget(label);

        scrollArea->setWidget(scrollAreaWidgetContents);

        horizontalLayout->addWidget(scrollArea);


        retranslateUi(AboutBox);

        QMetaObject::connectSlotsByName(AboutBox);
    } // setupUi

    void retranslateUi(QDialog *AboutBox)
    {
        AboutBox->setWindowTitle(QApplication::translate("AboutBox", "About WombatForensics", Q_NULLPTR));
        label->setText(QApplication::translate("AboutBox", "Loading...", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class AboutBox: public Ui_AboutBox {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUTBOX_H
