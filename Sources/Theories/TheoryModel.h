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

#pragma once

#include "Dictionaries/Dictionary.h"
#include "Models/TreeItemModel.h"
#include <QtCore/QHash>
#include <QtCore/QMultiHash>
#include <QtCore/QBitArray>

class TheoryTreeItem;
class TheoryModel : public TreeItemModel
{
    Q_OBJECT

signals:
    void dictionariesLoaded();

public:
    typedef QMultiHash<QString, QBitArray> CacheDictionaryEntries; ///< Key = text, value = keyboard keys bits.

    struct CacheDictionaryInfo
    {
        QString sMandatorySpecialKey;
        CacheDictionaryEntries entries;
    };

    typedef QHash<QString, CacheDictionaryInfo> CacheDictionaries; ///< Key = dictionary name, value = CacheDictionaryInfo.

public:
    TheoryModel(QObject* pParent = nullptr);
    ~TheoryModel();
    virtual QVariant data(const QModelIndex& index, int iRole) const override;
    void setDictionaries(const Dictionaries& dictionaries);
    TheoryTreeItem* getTheoryTreeItem() const;
    void setTheoryTreeItem(TheoryTreeItem* pTheoryTreeItem);
    QModelIndex getTheoryIndex() const;
    QModelIndex getDictionariesIndex() const;
    QString getTheoryName() const;
    const CacheDictionaries& getCachedDictionaries() const {return _cache;}

private:
    void buildCache();

private:
    CacheDictionaries _cache; ///<  Cache for fast lookup.
};
