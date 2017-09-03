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
#include "ListTreeItem.h"
#include <QtGui/QIcon>

TheoryTreeItem::TheoryTreeItem()
{
    setIcon(QIcon(":/Icons/graduation-hat.png"));
    setText(QObject::tr("Theory"));
    setEditable(false);

    addAttribute(QObject::tr("Name"), QString());
    addAttribute(QObject::tr("Description"), QString());
    appendRow({new ListTreeItem(QIcon(":/Icons/books-brown.png"), QObject::tr("Dictionaries")), new EmptyTreeItem()});
}

TheoryTreeItem::~TheoryTreeItem()
{

}

ListTreeItem* TheoryTreeItem::getDictionaries() const
{
    return static_cast<ListTreeItem*>(child(2));
}