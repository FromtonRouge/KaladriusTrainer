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
#include <QtCore/QScopedPointer>

namespace Ui
{
    class StrokesSolverWidget;
}

class StrokesSolverWidget : public QWidget
{
    Q_OBJECT

public:
    StrokesSolverWidget(QWidget* pParent = nullptr);

private:
    QScopedPointer<Ui::StrokesSolverWidget> _pUi;
};
