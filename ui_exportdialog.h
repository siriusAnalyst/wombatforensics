/********************************************************************************
** Form generated from reading UI file 'exportdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EXPORTDIALOG_H
#define UI_EXPORTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ExportDialog
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_2;
    QRadioButton *selectedFileRadioButton;
    QRadioButton *checkedFileRadioButton;
    QRadioButton *listedFileRadioButton;
    QHBoxLayout *horizontalLayout_3;
    QLineEdit *exportDirectoryLineEdit;
    QPushButton *browseButton;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label;
    QRadioButton *includePathRadioButton;
    QRadioButton *excludePathRadioButton;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *cancelButton;
    QPushButton *exportButton;
    QButtonGroup *pathButtonGroup;
    QButtonGroup *exportButtonGroup;

    void setupUi(QDialog *ExportDialog)
    {
        if (ExportDialog->objectName().isEmpty())
            ExportDialog->setObjectName(QStringLiteral("ExportDialog"));
        ExportDialog->setWindowModality(Qt::ApplicationModal);
        ExportDialog->resize(298, 300);
        ExportDialog->setModal(true);
        verticalLayout_2 = new QVBoxLayout(ExportDialog);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(9, 9, 9, 9);
        groupBox_2 = new QGroupBox(ExportDialog);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        verticalLayout_3 = new QVBoxLayout(groupBox_2);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        selectedFileRadioButton = new QRadioButton(groupBox_2);
        exportButtonGroup = new QButtonGroup(ExportDialog);
        exportButtonGroup->setObjectName(QStringLiteral("exportButtonGroup"));
        exportButtonGroup->addButton(selectedFileRadioButton);
        selectedFileRadioButton->setObjectName(QStringLiteral("selectedFileRadioButton"));

        horizontalLayout_2->addWidget(selectedFileRadioButton);

        checkedFileRadioButton = new QRadioButton(groupBox_2);
        exportButtonGroup->addButton(checkedFileRadioButton);
        checkedFileRadioButton->setObjectName(QStringLiteral("checkedFileRadioButton"));

        horizontalLayout_2->addWidget(checkedFileRadioButton);

        listedFileRadioButton = new QRadioButton(groupBox_2);
        exportButtonGroup->addButton(listedFileRadioButton);
        listedFileRadioButton->setObjectName(QStringLiteral("listedFileRadioButton"));

        horizontalLayout_2->addWidget(listedFileRadioButton);


        verticalLayout_3->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        exportDirectoryLineEdit = new QLineEdit(groupBox_2);
        exportDirectoryLineEdit->setObjectName(QStringLiteral("exportDirectoryLineEdit"));
        exportDirectoryLineEdit->setReadOnly(true);

        horizontalLayout_3->addWidget(exportDirectoryLineEdit);

        browseButton = new QPushButton(groupBox_2);
        browseButton->setObjectName(QStringLiteral("browseButton"));

        horizontalLayout_3->addWidget(browseButton);


        verticalLayout_3->addLayout(horizontalLayout_3);


        verticalLayout->addWidget(groupBox_2);

        groupBox = new QGroupBox(ExportDialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        horizontalLayout_4 = new QHBoxLayout(groupBox);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_4->addWidget(label);

        includePathRadioButton = new QRadioButton(groupBox);
        pathButtonGroup = new QButtonGroup(ExportDialog);
        pathButtonGroup->setObjectName(QStringLiteral("pathButtonGroup"));
        pathButtonGroup->addButton(includePathRadioButton);
        includePathRadioButton->setObjectName(QStringLiteral("includePathRadioButton"));

        horizontalLayout_4->addWidget(includePathRadioButton);

        excludePathRadioButton = new QRadioButton(groupBox);
        pathButtonGroup->addButton(excludePathRadioButton);
        excludePathRadioButton->setObjectName(QStringLiteral("excludePathRadioButton"));

        horizontalLayout_4->addWidget(excludePathRadioButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);


        verticalLayout->addWidget(groupBox);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, -1, 9, -1);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        cancelButton = new QPushButton(ExportDialog);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));

        horizontalLayout->addWidget(cancelButton);

        exportButton = new QPushButton(ExportDialog);
        exportButton->setObjectName(QStringLiteral("exportButton"));

        horizontalLayout->addWidget(exportButton);


        verticalLayout->addLayout(horizontalLayout);


        verticalLayout_2->addLayout(verticalLayout);


        retranslateUi(ExportDialog);
        QObject::connect(cancelButton, SIGNAL(clicked()), ExportDialog, SLOT(close()));

        QMetaObject::connectSlotsByName(ExportDialog);
    } // setupUi

    void retranslateUi(QDialog *ExportDialog)
    {
        ExportDialog->setWindowTitle(QApplication::translate("ExportDialog", "Export Dialog", Q_NULLPTR));
        groupBox_2->setTitle(QApplication::translate("ExportDialog", "Export File(s)", Q_NULLPTR));
        selectedFileRadioButton->setText(QApplication::translate("ExportDialog", "Selected", Q_NULLPTR));
        checkedFileRadioButton->setText(QApplication::translate("ExportDialog", "Checked (#)", Q_NULLPTR));
        listedFileRadioButton->setText(QApplication::translate("ExportDialog", "All (#)", Q_NULLPTR));
        exportDirectoryLineEdit->setPlaceholderText(QApplication::translate("ExportDialog", "Export Directory", Q_NULLPTR));
        browseButton->setText(QApplication::translate("ExportDialog", "Browse", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("ExportDialog", "Path", Q_NULLPTR));
        label->setText(QApplication::translate("ExportDialog", "Include Original Path:", Q_NULLPTR));
        includePathRadioButton->setText(QApplication::translate("ExportDialog", "Yes", Q_NULLPTR));
        excludePathRadioButton->setText(QApplication::translate("ExportDialog", "No", Q_NULLPTR));
        cancelButton->setText(QApplication::translate("ExportDialog", "Cancel", Q_NULLPTR));
        exportButton->setText(QApplication::translate("ExportDialog", "Export", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ExportDialog: public Ui_ExportDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EXPORTDIALOG_H
