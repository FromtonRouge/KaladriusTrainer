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

#include "LinkedDictionaryTreeItemSerialize.h"
#include "TreeItemSerialize.h"
#include "ListTreeItemSerialize.h"
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/base_object.hpp>
#include "ExplicitInstanciation.h"

EXPLICIT_INSTANCIATION(LinkedDictionaryTreeItem)
BOOST_CLASS_EXPORT_IMPLEMENT(LinkedDictionaryTreeItem)

namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, LinkedDictionaryTreeItem& obj,  const unsigned int fileVersion)
        {
            ar & make_nvp("base", base_object<TreeItem>(obj));
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const LinkedDictionaryTreeItem& obj,  const unsigned int)
        {
            std::string sText = obj.text().toStdString();
            ar << make_nvp("name", sText);

            auto pLinkedKeysTreeItem = obj.getLinkedKeys();
            ar << make_nvp("linked_keys", *pLinkedKeysTreeItem);
        }

        template<class Archive> void load(Archive& ar, LinkedDictionaryTreeItem& obj,  const unsigned int)
        {
            std::string sText;
            ar >> make_nvp("name", sText);
            obj.setText(QString::fromStdString(sText));

            auto pLinkedKeysTreeItem = obj.getLinkedKeys();
            ar >> make_nvp("linked_keys", *pLinkedKeysTreeItem);
        }
    }
}
