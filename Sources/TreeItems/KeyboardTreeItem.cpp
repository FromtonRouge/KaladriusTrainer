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

#include "KeyboardTreeItem.h"
#include "ListTreeItem.h"
#include <QtGui/QIcon>

KeyboardTreeItem::KeyboardTreeItem()
{
    setIcon(QIcon(":/Icons/keyboard-full.png"));
    setText(QObject::tr("Keyboard"));
    setEditable(false);

    auto pKeycapsListItem = new ListTreeItem(QIcon(":/Icons/keyboard-full.png"), QObject::tr("Keycaps"));
    appendRow({pKeycapsListItem, new EmptyTreeItem()});

    auto pLinkedTheories = new ListTreeItem(QIcon(":/Icons/book-open.png"), QObject::tr("Linked Theories"));
    appendRow({pLinkedTheories, new EmptyTreeItem()});
}

KeyboardTreeItem::~KeyboardTreeItem()
{

}

ListTreeItem* KeyboardTreeItem::getKeycaps() const
{
    return static_cast<ListTreeItem*>(child(0));
}

ListTreeItem*KeyboardTreeItem::getLinkedTheories() const
{
    return static_cast<ListTreeItem*>(child(1));
}
