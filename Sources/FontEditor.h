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

#include <QtWidgets/QWidget>
#include <QtGui/QFont>

class QPushButton;
class QFontComboBox;
class FontEditor : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QFont font READ getFont WRITE setFont USER true)

public:
    FontEditor(QWidget* pParent = nullptr);
    ~FontEditor();

    const QFont& getFont() const {return _font;}
    void setFont(const QFont& font);

protected slots:
    void onPushButton();
    void onFontChanged(const QFont& font);

private:
    QFont _font;
    QFontComboBox* _pFontComboBox;
    QPushButton* _pPushButton;
};
