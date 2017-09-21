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
#include "TreeItems/DictionaryTreeItem.h"

TheoryModel::TheoryModel(QObject* pParent)
    : TreeItemModel(pParent)
{
    setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Value"));
    appendRow({new TheoryTreeItem(), new EmptyTreeItem()});
}

TheoryModel::~TheoryModel()
{

}

QVariant TheoryModel::data(const QModelIndex& index, int iRole) const
{
    QVariant result;
    switch (iRole)
    {
    case EntriesIndexRole:
        {
            if (index.data(TreeItemTypeRole).toInt() == TreeItem::Dictionary)
            {
                result = index.child(1, 0);
            }
            break;
        }
    default:
        {
            result = TreeItemModel::data(index, iRole);
            break;
        }
    }
    return result;
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
        auto pDictionaryItem = new DictionaryTreeItem(dictionary.getName(), dictionary.getKeysLabels());
        pDictionaries->appendRow({pDictionaryItem, new EmptyTreeItem()});
        auto pEntriesItem = pDictionaryItem->getEntries();
        const auto& entries = dictionary.getKeyBitsToEntry();
        for (const Dictionary::Entry& entry : entries)
        {
            if (entry.bits.any())
            {
                auto pTextItem = new OutputTextTreeItem(entry.keycodesAsUserString);
                auto pInputKeysItem = new InputKeysTreeItem(dictionary.getKeysLabelsInReadingOrder(entry), uint(entry.bits.to_ulong()));
                pEntriesItem->appendRow({pTextItem, pInputKeysItem});
            }
        }
    }
    emit dictionariesLoaded();
}

TheoryTreeItem*TheoryModel::getTheoryTreeItem() const
{
    const QModelIndex& indexTheory = getTheoryIndex();
    return indexTheory.isValid() ? static_cast<TheoryTreeItem*>(itemFromIndex(indexTheory)) : nullptr;
}

void TheoryModel::setTheoryTreeItem(TheoryTreeItem* pTheoryTreeItem)
{
    clear();
    appendRow({pTheoryTreeItem, new EmptyTreeItem()});
    emit dictionariesLoaded();
    setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Value"));
}

QModelIndex TheoryModel::getTheoryIndex() const
{
    if (hasChildren())
    {
        return index(0, 0);
    }
    return QModelIndex();
}

QModelIndex TheoryModel::getDictionariesIndex() const
{
    const QModelIndex& indexTheory = getTheoryIndex();
    if (indexTheory.isValid())
    {
        return indexTheory.child(2, 0);
    }
    return QModelIndex();
}
