// ======================================================================
// This file is a part of the KaladriusTrainer project
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

#include "Main/MainTabWindow.h"
#include <QtCore/QScopedPointer>

namespace Ui
{
    class KeyboardEditorMainWindow;
}

class KeyboardEditorMainWindow : public MainTabWindow
{
    Q_OBJECT

public:
    KeyboardEditorMainWindow(QWidget* pParent = nullptr);
    ~KeyboardEditorMainWindow();

protected slots:
    void on_actionLoad_Default_triggered();
    void on_actionLoad_triggered();
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();
    void on_actionImport_Svg_triggered();
    void on_actionImport_Default_Svg_triggered();

private:
    enum SettingsOperation
    {
        SaveSettings = 0,
        ClearSettings,
    };
    void loadKeyboard(const QString& sKeyboardFileName, SettingsOperation settingsOperation = SaveSettings);

private:
    QScopedPointer<Ui::KeyboardEditorMainWindow> _pUi;
};
