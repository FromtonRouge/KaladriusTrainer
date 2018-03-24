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

#include "Main/MainTabWindow.h"
#include <QtCore/QScopedPointer>

namespace Ui
{
    class LessonsEditorMainWindow;
}

class QTextCharFormat;
class QFontComboBox;
class QComboBox;
class LessonsEditorMainWindow : public MainTabWindow
{
    Q_OBJECT

public:
    LessonsEditorMainWindow(QWidget* pParent);
    ~LessonsEditorMainWindow();

private slots:
    void on_textEdit_currentCharFormatChanged(const QTextCharFormat& format);
    void on_textEdit_cursorPositionChanged();
    void on_actionBold_toggled(bool bChecked);
    void on_actionItalic_toggled(bool bChecked);
    void on_actionUnderline_toggled(bool bChecked);
    void on_actionForeground_Color_triggered(bool bChecked);
    void on_actionBackground_Color_triggered(bool bChecked);
    void on_actionInsert_List_triggered();
    void on_actionInsert_Ordered_List_triggered();
    void on_actionInsert_Table_triggered();
    void on_actionInsert_Frame_triggered();
    void on_actionInsert_Image_triggered();
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();
    void on_actionReload_triggered();
    void on_actionNew_triggered();
    void on_actionIncrease_Indent_triggered();
    void on_actionDecrease_Indent_triggered();

private:
    void fontChanged(const QFont &f);
    void foregroundColorChanged(const QColor &c);
    void backgroundColorChanged(const QColor &c);
    void alignmentChanged(Qt::Alignment a);
    void mergeFormatOnSelection(const QTextCharFormat& format);

private:
    QScopedPointer<Ui::LessonsEditorMainWindow> _pUi;
    QFontComboBox* _pFontComboBox;
    QComboBox* _pComboBoxSize;
};
