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

#include "DictionaryTreeItemSerialize.h"
#include "TreeItemSerialize.h"
#include "ListTreeItemSerialize.h"
#include "Qt/QString.h"
#include "../Tree/TreeItems/AttributeTreeItem.h"
#include "../Tree/TreeItems/ValueTreeItem.h"
#include "../Tree/Models/ItemDataRole.h"
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/base_object.hpp>
#include "ExplicitInstanciation.h"

EXPLICIT_INSTANCIATION(DictionaryTreeItem)
BOOST_CLASS_EXPORT_IMPLEMENT(DictionaryTreeItem) // For serializing from a base pointer

namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, DictionaryTreeItem& obj,  const unsigned int fileVersion)
        {
            ar & make_nvp("base", base_object<TreeItem>(obj));
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const DictionaryTreeItem& obj,  const unsigned int)
        {
            std::string sText = obj.text().toStdString();
            ar << make_nvp("name", sText);

            QString sValue = obj.getMandatoryKey()->getValue()->data(Qt::EditRole).toString();
            ar << make_nvp("mandatory_key", sValue);

            auto pKeysTreeItem = obj.getKeys();
            ar << make_nvp("keys", *pKeysTreeItem);

            auto pEntriesTreeItem = obj.getEntries();
            ar << make_nvp("entries", *pEntriesTreeItem);
        }

        template<class Archive> void load(Archive& ar, DictionaryTreeItem& obj,  const unsigned int)
        {
            std::string sText;
            ar >> make_nvp("name", sText);
            obj.setText(QString::fromStdString(sText));

            QString sValue;
            ar >> make_nvp("mandatory_key", sValue);
            obj.getMandatoryKey()->getValue()->setData(sValue, Qt::EditRole);

            auto pKeysTreeItem = obj.getKeys();
            ar >> make_nvp("keys", *pKeysTreeItem);

            auto pEntriesTreeItem = obj.getEntries();
            ar >> make_nvp("entries", *pEntriesTreeItem);
        }
    }
}
