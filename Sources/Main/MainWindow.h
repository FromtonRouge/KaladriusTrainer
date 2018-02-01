// ======================================================================
// This file is a part of the ProgrammerStenoTutor project
//
// Copyright (C) 2017  Vissale NEANG <fromtonrouge at gmail dot com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ======================================================================

#pragma once

#include "Dictionaries/Dictionary.h"
#include <QtWidgets/QMainWindow>
#include <QtCore/QScopedPointer>

namespace Ui
{
    class MainWindow;
}

class QUndoStack;
class KeyboardGraphicsScene;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected slots:
    void on_actionQuit_triggered();
    void on_actionImport_Keyboard_Svg_triggered();
    void on_actionImport_Default_Keyboard_Svg_triggered();
    void on_actionLoad_Theory_triggered();
    void on_actionLoad_Default_Theory_triggered();
    void on_actionSave_Theory_as_triggered();
    void on_actionImport_Dictionaries_triggered();
    void on_actionWrite_Markdown_Files_To_triggered();
    void on_actionWrite_Markdown_Files_triggered();
    void on_actionAbout_triggered();
    void on_actionKeyboard_Window_triggered();
    void on_actionLoad_Keyboard_triggered();
    void on_actionLoad_Default_Keyboard_triggered();
    void on_actionSave_Keyboard_as_triggered();
    void on_actionSave_Keyboard_triggered();
    void delayedRestoreState();
    void logs(const QString& sText, int iWarningLevel = 0);

private:
    enum SettingsOperation
    {
        SaveSettings = 0,
        ClearSettings,
    };
    void loadKeyboard(const QString& sKeyboardFileName, SettingsOperation settingsOperation = SaveSettings);
    void loadTheory(const QString& sTheoryFileName, SettingsOperation settingsOperation = SaveSettings);

protected:
    virtual void closeEvent(QCloseEvent* pEvent) override;
    virtual bool event(QEvent* pEvent) override;

private:
    QScopedPointer<Ui::MainWindow> _pUi;
    Dictionaries _dictionaries;
    KeyboardGraphicsScene* _pKeyboardGraphicsScene; ///< Only 1 scene and potentialy N views on this scene.
    QUndoStack* _pUndoStack;
};
