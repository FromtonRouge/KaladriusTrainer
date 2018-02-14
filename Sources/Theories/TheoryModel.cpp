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
#include "Models/Utils.h"
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
                auto pItem = static_cast<DictionaryTreeItem*>(itemFromIndex(index));
                result = pItem->getEntries()->index();
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

    // Clear existing dictionaries
    auto pDictionaries = pTheoryTreeItem->getDictionaries();
    if (pDictionaries->hasChildren())
    {
        pDictionaries->removeRows(0, pDictionaries->rowCount());
    }

    // Insert new dictionaries
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
                const int iSize = int(entry.bits.size());
                QBitArray bits(iSize);
                for (int i = 0; i < iSize; ++i)
                {
                    bits[i] = entry.bits.test(size_t(i));
                }
                auto pInputKeysItem = new InputKeysTreeItem(dictionary.getKeysLabelsInReadingOrder(entry), bits);
                pEntriesItem->appendRow({pTextItem, pInputKeysItem});
            }
        }
    }

    buildCache();
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

    buildCache();
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

QModelIndex TheoryModel::getSpecialKeysIndex() const
{
    return indexFromItem(getTheoryTreeItem()->getSpecialKeys());
}

QModelIndex TheoryModel::getDictionariesIndex() const
{
    return indexFromItem(getTheoryTreeItem()->getDictionaries());
}

QString TheoryModel::getTheoryName() const
{
    const QModelIndex& indexTheoryName = Utils::index(this, "Theory/Name", 1);
    if (indexTheoryName.isValid())
    {
        return indexTheoryName.data(Qt::DisplayRole).toString();
    }
    return QString();
}

void TheoryModel::buildCache()
{
    _cache.clear();
    const QModelIndex& indexDictionaries = getDictionariesIndex();
    if (indexDictionaries.isValid())
    {
        const int iDictionaries = rowCount(indexDictionaries);
        for (int iDictionary = 0; iDictionary < iDictionaries; ++iDictionary)
        {
            const QModelIndex& indexDictionary = index(iDictionary, 0, indexDictionaries);
            const QString& sDictionaryName = indexDictionary.data(Qt::DisplayRole).toString();
            if (!sDictionaryName.isEmpty())
            {
                const QModelIndex& indexEntries = Utils::index(this, "Entries", 0, indexDictionary);
                if (indexEntries.isValid())
                {
                    CacheDictionaryEntries entries;
                    const int iEntries = rowCount(indexEntries);
                    for (int iEntry = 0; iEntry < iEntries; ++iEntry)
                    {
                        const QModelIndex& indexEntry = index(iEntry, 0, indexEntries);
                        const QModelIndex& indexEntryBits = index(iEntry, 1, indexEntries);
                        const QString& sText = indexEntry.data(Qt::DisplayRole).toString();
                        const QBitArray& bits = indexEntryBits.data(InputKeyBitsRole).toBitArray();
                        if (sText != "<no_entry>")
                        {
                            entries.insert(sText, bits);
                        }
                    }

                    if (!entries.isEmpty())
                    {
                        QString sMandatorySpecialKey;
                        const QModelIndex& indexMandatorySpecialKey = Utils::index(this, "Mandatory Key", 1, indexDictionary);
                        if (indexMandatorySpecialKey.isValid())
                        {
                            sMandatorySpecialKey = indexMandatorySpecialKey.data().toString();
                        }
                        _cache.insert(sDictionaryName, {sMandatorySpecialKey, entries});
                    }
                }
            }
        }
    }
}
