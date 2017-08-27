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

#include "TheoryModel.h"
#include "TreeItems/TreeItem.h"
#include "TreeItems/TheoryTreeItem.h"
#include "TreeItems/ListTreeItem.h"
#include "TreeItems/OutputTextTreeItem.h"
#include "TreeItems/InputKeysTreeItem.h"

TheoryModel::TheoryModel(QObject* pParent)
    : QStandardItemModel(pParent)
{
    setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Value"));
    appendRow({new TheoryTreeItem(), new EmptyTreeItem()});
}

TheoryModel::~TheoryModel()
{

}

void TheoryModel::setDictionaries(const Dictionaries& dictionaries)
{
    auto pTheoryTreeItem = getTheoryTreeItem();
    auto pDictionaries = pTheoryTreeItem->getDictionaries();
    if (pDictionaries->hasChildren())
    {
        pDictionaries->removeRows(0, pDictionaries->rowCount());
    }

    for (const auto& dictionary : dictionaries)
    {
        auto pDictionaryItem = new ListTreeItem(QIcon(":/Icons/book-brown.png"), dictionary.getName());
        pDictionaries->appendRow({pDictionaryItem, new EmptyTreeItem()});
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
}

TheoryTreeItem*TheoryModel::getTheoryTreeItem() const
{
    const QModelIndex& indexTheory = getTheoryIndex();
    return indexTheory.isValid() ? static_cast<TheoryTreeItem*>(itemFromIndex(indexTheory)) : nullptr;
}

QModelIndex TheoryModel::getTheoryIndex() const
{
    if (hasChildren())
    {
        return index(0, 0);
    }
    return QModelIndex();
}
