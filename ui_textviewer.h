/********************************************************************************
** Form generated from reading UI file 'textviewer.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEXTVIEWER_H
#define UI_TEXTVIEWER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_TextViewer
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *comboBox;
    QSpacerItem *horizontalSpacer;
    QTextEdit *textEdit;

    void setupUi(QDialog *TextViewer)
    {
        if (TextViewer->objectName().isEmpty())
            TextViewer->setObjectName(QStringLiteral("TextViewer"));
        TextViewer->setWindowModality(Qt::NonModal);
        TextViewer->resize(640, 480);
        verticalLayout = new QVBoxLayout(TextViewer);
        verticalLayout->setSpacing(3);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(TextViewer);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        comboBox = new QComboBox(TextViewer);
        comboBox->setObjectName(QStringLiteral("comboBox"));

        horizontalLayout->addWidget(comboBox);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        textEdit = new QTextEdit(TextViewer);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setUndoRedoEnabled(false);
        textEdit->setReadOnly(true);

        verticalLayout->addWidget(textEdit);


        retranslateUi(TextViewer);

        QMetaObject::connectSlotsByName(TextViewer);
    } // setupUi

    void retranslateUi(QDialog *TextViewer)
    {
        TextViewer->setWindowTitle(QApplication::translate("TextViewer", "Dialog", Q_NULLPTR));
        label->setText(QApplication::translate("TextViewer", "Encoding", Q_NULLPTR));
        comboBox->setCurrentText(QString());
    } // retranslateUi

};

namespace Ui {
    class TextViewer: public Ui_TextViewer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEXTVIEWER_H
