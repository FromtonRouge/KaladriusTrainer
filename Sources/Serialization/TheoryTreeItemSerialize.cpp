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

#include "TheoryTreeItemSerialize.h"
#include "TreeItemSerialize.h"
#include "ListTreeItemSerialize.h"
#include "../Tree/TreeItems/AttributeTreeItem.h"
#include "../Tree/TreeItems/ValueTreeItem.h"
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/base_object.hpp>
#include "ExplicitInstanciation.h"
#include "Qt/QString.h"

EXPLICIT_INSTANCIATION(TheoryTreeItem)

namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, TheoryTreeItem& obj,  const unsigned int fileVersion)
        {
            ar & make_nvp("base", base_object<TreeItem>(obj));
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const TheoryTreeItem& obj,  const unsigned int)
        {
            QString sValue;
            sValue = obj.getName()->getValue()->data(Qt::EditRole).toString();
            ar << make_nvp("name", sValue);

            sValue = obj.getDescription()->getValue()->data(Qt::EditRole).toString();
            ar << make_nvp("description", sValue);

            auto pSpecialKeys = obj.getSpecialKeys();
            ar << make_nvp("special_keys", *pSpecialKeys);

            auto pDictionaries = obj.getDictionaries();
            ar << make_nvp("dictionaries", *pDictionaries);
        }

        template<class Archive> void load(Archive& ar, TheoryTreeItem& obj,  const unsigned int)
        {
            QString sValue;
            auto pValue = obj.getName()->getValue();
            ar >> make_nvp("name", sValue);
            pValue->setData(sValue, Qt::EditRole);

            pValue = obj.getDescription()->getValue();
            ar >> make_nvp("description", sValue);
            pValue->setData(sValue, Qt::EditRole);

            auto pSpecialKeys = obj.getSpecialKeys();
            ar >> make_nvp("special_keys", *pSpecialKeys);

            auto pDictionaries = obj.getDictionaries();
            ar >> make_nvp("dictionaries", *pDictionaries);
        }
    }
}
