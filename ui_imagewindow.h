/********************************************************************************
** Form generated from reading UI file 'imagewindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IMAGEWINDOW_H
#define UI_IMAGEWINDOW_H

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

class Ui_ImageWindow
{
public:
    QHBoxLayout *horizontalLayout;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;

    void setupUi(QDialog *ImageWindow)
    {
        if (ImageWindow->objectName().isEmpty())
            ImageWindow->setObjectName(QStringLiteral("ImageWindow"));
        ImageWindow->resize(800, 600);
        horizontalLayout = new QHBoxLayout(ImageWindow);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        scrollArea = new QScrollArea(ImageWindow);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 798, 598));
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
        label->setAlignment(Qt::AlignCenter);
        label->setTextInteractionFlags(Qt::NoTextInteraction);

        horizontalLayout_2->addWidget(label);

        scrollArea->setWidget(scrollAreaWidgetContents);

        horizontalLayout->addWidget(scrollArea);


        retranslateUi(ImageWindow);

        QMetaObject::connectSlotsByName(ImageWindow);
    } // setupUi

    void retranslateUi(QDialog *ImageWindow)
    {
        ImageWindow->setWindowTitle(QApplication::translate("ImageWindow", "View Image", Q_NULLPTR));
        label->setText(QApplication::translate("ImageWindow", "Loading...", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ImageWindow: public Ui_ImageWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IMAGEWINDOW_H
