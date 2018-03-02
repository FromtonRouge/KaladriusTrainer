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
    class TheoryEditorMainWindow;
}

class TheoryEditorMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    TheoryEditorMainWindow(QWidget *parent = nullptr);
    ~TheoryEditorMainWindow();

protected slots:
    void on_actionLoad_triggered();
    void on_actionLoad_Default_triggered();
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();
    void on_actionImport_Dictionaries_triggered();
    void on_actionWrite_Markdown_Files_triggered();
    void on_actionWrite_Markdown_Files_To_triggered();
    void onUndoCleanChanged(bool bClean);

private:
    enum SettingsOperation
    {
        SaveSettings = 0,
        ClearSettings,
    };
    void loadTheory(const QString& sTheoryFileName, SettingsOperation settingsOperation = SaveSettings);

private:
    QScopedPointer<Ui::TheoryEditorMainWindow> _pUi;
    Dictionaries _dictionaries;
};
