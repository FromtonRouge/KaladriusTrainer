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

#include "TheoryTreeItem.h"
#include "Tree/TreeItems/ListTreeItem.h"
#include "Tree/TreeItems/ValueTreeItem.h"
#include "Tree/TreeItems/AttributeTreeItem.h"
#include "Values/Types/ListValue.h"
#include <QtGui/QIcon>

TheoryTreeItem::TheoryTreeItem()
{
    setIcon(QIcon(":/Icons/graduation-hat.png"));
    setText(QObject::tr("Theory"));
    setEditable(false);

    addAttribute(QObject::tr("Name"), QString());
    addAttribute(QObject::tr("Description"), QString());
    auto pValueTreeItem = new ValueTreeItem(qVariantFromValue(ListValue(QObject::tr("Labels"), QString(), ListValue::NameIsEditable)));
    appendRow({new ListTreeItem(QIcon(":/Icons/keyboard-full.png"), QObject::tr("Special Keys")), pValueTreeItem });
    appendRow({new ListTreeItem(QIcon(":/Icons/books-brown.png"), QObject::tr("Dictionaries")), new EmptyTreeItem()});
}

TheoryTreeItem::~TheoryTreeItem()
{

}

AttributeTreeItem*TheoryTreeItem::getName() const
{
    return static_cast<AttributeTreeItem*>(child(0));
}

AttributeTreeItem*TheoryTreeItem::getDescription() const
{
    return static_cast<AttributeTreeItem*>(child(1));
}

ListTreeItem* TheoryTreeItem::getSpecialKeys() const
{
    return static_cast<ListTreeItem*>(child(2));
}

ListTreeItem* TheoryTreeItem::getDictionaries() const
{
    return static_cast<ListTreeItem*>(child(3));
}
