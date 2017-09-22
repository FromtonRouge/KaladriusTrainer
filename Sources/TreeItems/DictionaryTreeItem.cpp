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

#include "DictionaryTreeItem.h"
#include "ListTreeItem.h"
#include "ArrayTreeItem.h"
#include <QtGui/QIcon>

DictionaryTreeItem::DictionaryTreeItem(const QString& sText, const QVector<QString>& keysLabels)
{
    setIcon(QIcon(":/Icons/book-brown.png"));
    setText(sText);
    setEditable(false);

    auto pKeysTreeItem = new ArrayTreeItem(QIcon(":/Icons/keyboard-full.png"), QObject::tr("Keys"));
    for (const QString& sKeyLabel : keysLabels)
    {
        pKeysTreeItem->addElement(sKeyLabel);
    }
    appendRow({pKeysTreeItem, new EmptyTreeItem()});

    auto pEntriesTreeItem = new ListTreeItem(QIcon(":/Icons/book-brown.png"), QObject::tr("Entries"));
    appendRow({pEntriesTreeItem, new EmptyTreeItem()});
}

DictionaryTreeItem::~DictionaryTreeItem()
{
}

ArrayTreeItem* DictionaryTreeItem::getKeys() const
{
    return static_cast<ArrayTreeItem*>(child(0, 0));
}

ListTreeItem* DictionaryTreeItem::getEntries() const
{
    return static_cast<ListTreeItem*>(child(1, 0));
}
