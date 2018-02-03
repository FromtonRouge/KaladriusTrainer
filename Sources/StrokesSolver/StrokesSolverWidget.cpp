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

#include "StrokesSolverWidget.h"
#include "ui_StrokesSolverWidget.h"
#include <QtCore/QSettings>

StrokesSolverWidget::StrokesSolverWidget(QWidget* pParent)
    : QWidget(pParent)
    , _pUi(new Ui::StrokesSolverWidget())
{
    _pUi->setupUi(this);
    _pUi->checkBoxTrainingMode->setChecked(QSettings().value("trainingMode", false).toBool());
}

void StrokesSolverWidget::on_checkBoxTrainingMode_toggled(bool bChecked)
{
    QSettings().setValue("trainingMode", bChecked);
    _pUi->textEdit->setTrainingMode(bChecked);
}
