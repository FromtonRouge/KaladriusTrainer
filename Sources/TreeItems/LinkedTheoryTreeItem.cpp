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

#include "LinkedTheoryTreeItem.h"
#include "ValueTreeItem.h"
#include "ValueTypes/ListValue.h"
#include "ValueTypes/KeycapRef.h"
#include "ListTreeItem.h"

LinkedTheoryTreeItem::LinkedTheoryTreeItem(const QString& sTheoryName)
{
    setIcon(QIcon(":/Icons/graduation-hat.png"));
    setText(sTheoryName);
    setEditable(false);

    _pLinkedSpecialKeys = new ListTreeItem(QIcon(":/Icons/keyboard-full.png"), QObject::tr("Linked Special Keys"));
    const QVariant& defaultSubList = qVariantFromValue(ListValue(QObject::tr("Keycaps"), qVariantFromValue(KeycapRef())));
    auto pValueTreeItem = new ValueTreeItem(qVariantFromValue(ListValue(QObject::tr("Keycaps List"), defaultSubList, ListValue::NameIsEditable)));
    appendRow({_pLinkedSpecialKeys, pValueTreeItem});

    _pLinkedDictionaries = new ListTreeItem(QIcon(":/Icons/books-brown.png"), QObject::tr("Linked Dictionaries"));
    appendRow({_pLinkedDictionaries, new EmptyTreeItem()});
}
