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

#include "UserEditorFactory.h"
#include "ColorEditor.h"
#include "FontEditor.h"
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QDoubleSpinBox>
#include <cfloat>

UserEditorFactory::UserEditorFactory()
{
}

UserEditorFactory::~UserEditorFactory()
{

}

QWidget* UserEditorFactory::createEditor(int iUserType, QWidget* pParent) const
{
    switch (iUserType)
    {
    case QVariant::Font:
        {
            auto pFontEditor = new FontEditor(pParent);
            return pFontEditor;
        }
    case QVariant::Color:
        {
            auto pColorEditor = new ColorEditor(pParent);
            return pColorEditor;
        }
    case QVariant::String:
        {
            auto pLineEdit = new QLineEdit(pParent);
            pLineEdit->setFrame(false);
            return pLineEdit;
        }
    case QVariant::Double:
        {
            auto pDoubleSpinBox = new QDoubleSpinBox(pParent);
            pDoubleSpinBox->setFrame(false);
            pDoubleSpinBox->setMinimum(-DBL_MAX);
            pDoubleSpinBox->setMaximum(DBL_MAX);
            return pDoubleSpinBox;
        }
    }
    return nullptr;
}
