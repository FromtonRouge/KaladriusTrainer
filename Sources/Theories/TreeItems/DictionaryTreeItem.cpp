// ======================================================================
// This file is a part of the KaladriusTrainer project
//
// Copyright (C) 2020  Vissale NEANG <fromtonrouge at gmail dot com>
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
#include "../../Tree/TreeItems/ListTreeItem.h"
#include "../../Tree/TreeItems/ValueTreeItem.h"
#include "../../Tree/TreeItems/AttributeTreeItem.h"
#include "../../Values/Types/ListValue.h"
#include <QtGui/QIcon>

DictionaryTreeItem::DictionaryTreeItem(const QString& sText, const QVector<QString>& keysLabels)
{
    setIcon(QIcon(":/Icons/book-brown.png"));
    setText(sText);
    setEditable(false);

    appendRow({new AttributeTreeItem(QObject::tr("Mandatory Key")), new ValueTreeItem(QString())});

    auto pKeysTreeItem = new ListTreeItem(QIcon(":/Icons/keyboard-full.png"), QObject::tr("Keys"));
    for (const QString& sKeyLabel : keysLabels)
    {
        pKeysTreeItem->appendRow({new ListTreeItem(QIcon(":/Icons/keyboard.png")), new ValueTreeItem(sKeyLabel)});
    }
    appendRow({pKeysTreeItem, new ValueTreeItem(QVariant::fromValue(ListValue(QObject::tr("Labels"), QString())))});

    auto pEntriesTreeItem = new ListTreeItem(QIcon(":/Icons/book-brown.png"), QObject::tr("Entries"));
    appendRow({pEntriesTreeItem, new EmptyTreeItem()});
}

DictionaryTreeItem::~DictionaryTreeItem()
{
}

AttributeTreeItem*DictionaryTreeItem::getMandatoryKey() const
{
    return static_cast<AttributeTreeItem*>(child(0, 0));
}

ListTreeItem* DictionaryTreeItem::getKeys() const
{
    return static_cast<ListTreeItem*>(child(1, 0));
}

ListTreeItem* DictionaryTreeItem::getEntries() const
{
    return static_cast<ListTreeItem*>(child(2, 0));
}
