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

#include "ArrayTreeItem.h"
#include "AttributeValueTreeItem.h"

ArrayTreeItem::ArrayTreeItem(const QIcon& icon, const QString& sName)
{
    setIcon(icon);
    setText(sName);
    setEditable(false);
}

ArrayTreeItem::~ArrayTreeItem()
{
}

ArrayElementTreeItem* ArrayTreeItem::addAttribute(const QString& ignored, const QVariant& value)
{
    Q_UNUSED(ignored);
    auto pAttribute = new ArrayElementTreeItem();
    appendRow({pAttribute, new AttributeValueTreeItem(value)});
    return pAttribute;
}

ArrayElementTreeItem* ArrayTreeItem::addElement(const QVariant& value)
{
    return addAttribute(QString(), value);
}
