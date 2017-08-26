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

#include "DictionariesModel.h"
#include "TreeItems/ListTreeItem.h"
#include "TreeItems/OutputTextTreeItem.h"
#include "TreeItems/InputKeysTreeItem.h"

DictionariesModel::DictionariesModel(QObject* pParent)
    : QStandardItemModel(pParent)
{

}

DictionariesModel::~DictionariesModel()
{

}

void DictionariesModel::setDictionaries(const Dictionaries &dictionaries)
{
    clear();
    setHorizontalHeaderLabels(QStringList() << tr("Text") << tr("Keys"));
    for (const auto& dictionary : dictionaries)
    {
        auto pDictionaryItem = new ListTreeItem(QIcon(":/Icons/book-brown.png"), dictionary.getName());
        appendRow({pDictionaryItem, new EmptyTreeItem()});
        const auto& entries = dictionary.getKeyBitsToEntry();
        for (const Dictionary::Entry& entry : entries)
        {
            if (entry.bits.any())
            {
                auto pTextItem = new OutputTextTreeItem(entry.keycodesAsUserString);
                auto pInputKeysItem = new InputKeysTreeItem(dictionary.getKeysLabels(entry));
                pDictionaryItem->appendRow({pTextItem, pInputKeysItem});
            }
        }
    }
    emit dictionariesLoaded();
}
