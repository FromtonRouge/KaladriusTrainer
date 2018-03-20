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

#include "CourseTreeItem.h"
#include "Tree/TreeItems/ListTreeItem.h"
#include "Tree/TreeItems/ValueTreeItem.h"
#include "Values/Types/ListValue.h"

CourseTreeItem::CourseTreeItem()
{
    setEditable(false);
    setText(QObject::tr("Course"));
    setIcon(QIcon(":/Icons/books-stack.png"));
    auto pValueTreeItem = new ValueTreeItem(qVariantFromValue(ListValue(QObject::tr("Lessons"), QString(), ListValue::NameIsEditable)));
    appendRow({new ListTreeItem(QIcon(":/Icons/books.png"), QObject::tr("Lessons")), pValueTreeItem });
}

CourseTreeItem::~CourseTreeItem()
{

}
