#ifndef SETTINGSDIALOG_H 
#define SETTINGSDIALOG_H

// Copyright 2018 Pasquale J. Rinaldi, Jr.
// Distrubted under the terms of the GNU General Public License version 2

#include "wombatinclude.h"
#include "globals.h"
#include "wombatfunctions.h"
#include "ui_settingsdialog.h"

namespace Ui
{
    class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(QWidget* parent = 0);
    ~SettingsDialog();
    void LoadSettings();
private slots:
    void CancelChanges();
    void SaveChanges();

signals:
    void HideSettingsWindow();
private:
    Ui::SettingsDialog* ui;
protected:
};

Q_DECLARE_METATYPE(SettingsDialog*)
#endif // SETTINGSDIALOG_H
