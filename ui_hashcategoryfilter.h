/********************************************************************************
** Form generated from reading UI file 'hashcategoryfilter.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HASHCATEGORYFILTER_H
#define UI_HASHCATEGORYFILTER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_hashcategoryfilter
{
public:
    QGridLayout *gridLayout;
    QCheckBox *checkBox;
    QComboBox *comboBox;

    void setupUi(QWidget *hashcategoryfilter)
    {
        if (hashcategoryfilter->objectName().isEmpty())
            hashcategoryfilter->setObjectName(QStringLiteral("hashcategoryfilter"));
        hashcategoryfilter->setWindowModality(Qt::ApplicationModal);
        hashcategoryfilter->resize(383, 42);
        QFont font;
        font.setPointSize(8);
        hashcategoryfilter->setFont(font);
        gridLayout = new QGridLayout(hashcategoryfilter);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setHorizontalSpacing(0);
        checkBox = new QCheckBox(hashcategoryfilter);
        checkBox->setObjectName(QStringLiteral("checkBox"));

        gridLayout->addWidget(checkBox, 0, 0, 1, 1);

        comboBox = new QComboBox(hashcategoryfilter);
        comboBox->setObjectName(QStringLiteral("comboBox"));
        comboBox->setEditable(false);

        gridLayout->addWidget(comboBox, 0, 1, 1, 1);


        retranslateUi(hashcategoryfilter);
        QObject::connect(checkBox, SIGNAL(toggled(bool)), comboBox, SLOT(setEnabled(bool)));

        QMetaObject::connectSlotsByName(hashcategoryfilter);
    } // setupUi

    void retranslateUi(QWidget *hashcategoryfilter)
    {
        hashcategoryfilter->setWindowTitle(QApplication::translate("hashcategoryfilter", "Filter", Q_NULLPTR));
        checkBox->setText(QApplication::translate("hashcategoryfilter", "Show Items where hash matches", Q_NULLPTR));
        comboBox->setCurrentText(QString());
    } // retranslateUi

};

namespace Ui {
    class hashcategoryfilter: public Ui_hashcategoryfilter {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HASHCATEGORYFILTER_H
