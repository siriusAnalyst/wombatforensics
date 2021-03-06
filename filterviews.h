#ifndef FILTERVIEWS_H
#define FILTERVIEWS_H

// Copyright 2015 Pasquale J. Rinaldi, Jr.
// Distrubted under the terms of the GNU General Public License version 2

#include "wombatinclude.h"
#include "globals.h"
#include "ui_accesseddatefilter.h"
#include "ui_analysistypefilter.h"
#include "ui_changeddatefilter.h"
#include "ui_createddatefilter.h"
#include "ui_filetypefilter.h"
#include "ui_hashcategoryfilter.h"
#include "ui_hashfilter.h"
#include "ui_idfilter.h"
#include "ui_modifieddatefilter.h"
#include "ui_namefilter.h"
#include "ui_pathfilter.h"
#include "ui_sizefilter.h"
#include "ui_jumphex.h"
#include "ui_filecategoryfilter.h"

namespace Ui {
    class IdFilter;
    class NameFilter;
    class PathFilter;
    class SizeFilter;
    class CreatedDateFilter;
    class AccessedDateFilter;
    class ModifiedDateFilter;
    class ChangedDateFilter;
    class FileTypeFilter;
    class FileCategoryFilter;
    class HashFilter;
    class JumpHex;
}

class IdFilter : public QFrame
{
    Q_OBJECT
public:
    IdFilter(QWidget* parent = 0);
    ~IdFilter();
    void DisplayFilter(void);

signals:
    void HeaderChanged();

private slots:
    void HideClicked();
    void BuildId(QString curstring);
    void Rebuild();

private:
    Ui::IdFilter* ui;
    QString builtstring;
    QString estring;
    QString vstring;
    QString pstring;
    QString fstring;
};

class JumpHex : public QDialog
{
    Q_OBJECT
public:
    JumpHex(QWidget* parent = 0);
    ~JumpHex();

signals:
    void SetOffset();
private slots:
    void HideClicked();
    void DisplayFilter(void);

private:
    Ui::JumpHex* ui;
};

class NameFilter : public QWidget
{
    Q_OBJECT

public:
    NameFilter(QWidget* parent = 0);
    ~NameFilter();
    void DisplayFilter(void);

signals:
    void HeaderChanged();

private slots:
    void HideClicked();

private:
    Ui::NameFilter* ui;
};

class PathFilter : public QWidget
{
    Q_OBJECT

public:
    PathFilter(QWidget* parent = 0);
    ~PathFilter();
    void DisplayFilter(void);

signals:
    void HeaderChanged();

private slots:
    void HideClicked();

private:
    Ui::PathFilter* ui;
};

class SizeFilter : public QWidget
{
    Q_OBJECT
public:
    SizeFilter(QWidget* parent = 0);
    ~SizeFilter();
    void DisplayFilter(void);

signals:
    void HeaderChanged();

private slots:
    void HideClicked();

private:
    Ui::SizeFilter* ui;
};

class CreatedDateFilter : public QWidget
{
    Q_OBJECT
public:
    CreatedDateFilter(QWidget* parent = 0);
    ~CreatedDateFilter();
    void DisplayFilter();

signals:
    void HeaderChanged();

private slots:
    void HideClicked();

private:
    Ui::CreatedDateFilter* ui;
};

class AccessedDateFilter : public QWidget
{
    Q_OBJECT
public:
    AccessedDateFilter(QWidget* parent = 0);
    ~AccessedDateFilter();
    void DisplayFilter();

signals:
    void HeaderChanged();

private slots:
    void HideClicked();
private:
    Ui::AccessedDateFilter* ui;
};

class ModifiedDateFilter : public QWidget
{
    Q_OBJECT
public:
    ModifiedDateFilter(QWidget* parent = 0);
    ~ModifiedDateFilter();
    void DisplayFilter();

signals:
    void HeaderChanged();

private slots:
    void HideClicked();
private:
    Ui::ModifiedDateFilter* ui;
};

class ChangedDateFilter : public QWidget
{
    Q_OBJECT
public:
    ChangedDateFilter(QWidget* parent = 0);
    ~ChangedDateFilter();
    void DisplayFilter();

signals:
    void HeaderChanged();

private slots:
    void HideClicked();
private:
    Ui::ChangedDateFilter* ui;
};

class FileTypeFilter : public QWidget
{
    Q_OBJECT
public:
    FileTypeFilter(QWidget* parent = 0);
    ~FileTypeFilter();
    void DisplayFilter();

signals:
    void HeaderChanged();

private slots:
    void HideClicked();
private:
    Ui::FileTypeFilter* ui;
};

class FileCategoryFilter : public QWidget
{
    Q_OBJECT
public:
    FileCategoryFilter(QWidget* parent = 0);
    ~FileCategoryFilter();
    void DisplayFilter();

signals:
    void HeaderChanged();

private slots:
    void HideClicked();
private:
    Ui::FileCategoryFilter* ui;
};

class HashFilter : public QWidget
{
    Q_OBJECT
public:
    HashFilter(QWidget* parent = 0);
    ~HashFilter();
    void DisplayFilter();

signals:
    void HeaderChanged();

private slots:
    void HideClicked();
private:
    Ui::HashFilter* ui;
};

Q_DECLARE_METATYPE(IdFilter*);
Q_DECLARE_METATYPE(NameFilter*);
Q_DECLARE_METATYPE(PathFilter*);
Q_DECLARE_METATYPE(SizeFilter*);
Q_DECLARE_METATYPE(CreatedDateFilter*);
Q_DECLARE_METATYPE(AccessedDateFilter*);
Q_DECLARE_METATYPE(ModifiedDateFilter*);
Q_DECLARE_METATYPE(ChangedDateFilter*);
Q_DECLARE_METATYPE(FileTypeFilter*);
Q_DECLARE_METATYPE(FileCategoryFilter*);
Q_DECLARE_METATYPE(HashFilter*);
Q_DECLARE_METATYPE(JumpHex*);
#endif
