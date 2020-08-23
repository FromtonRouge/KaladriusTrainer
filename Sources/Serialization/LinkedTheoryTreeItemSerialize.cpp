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

#include "LinkedTheoryTreeItemSerialize.h"
#include "TreeItemSerialize.h"
#include "ListTreeItemSerialize.h"
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/base_object.hpp>
#include "ExplicitInstanciation.h"

EXPLICIT_INSTANCIATION(LinkedTheoryTreeItem)
BOOST_CLASS_EXPORT_IMPLEMENT(LinkedTheoryTreeItem) // For serializing from a base pointer

namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, LinkedTheoryTreeItem& obj,  const unsigned int fileVersion)
        {
            ar & make_nvp("base", base_object<TreeItem>(obj));
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const LinkedTheoryTreeItem& obj,  const unsigned int)
        {
            std::string sText = obj.text().toStdString();
            ar << make_nvp("name", sText);

            auto pLinkedSpecialKeys = obj.getLinkedSpecialKeys();
            ar << make_nvp("linked_special_keys", *pLinkedSpecialKeys);

            auto pLinkedDictionaries = obj.getLinkedDictionaries();
            ar << make_nvp("linked_dictionaries", *pLinkedDictionaries);
        }

        template<class Archive> void load(Archive& ar, LinkedTheoryTreeItem& obj,  const unsigned int)
        {
            std::string sText;
            ar >> make_nvp("name", sText);
            obj.setText(QString::fromStdString(sText));

            auto pLinkedSpecialKeys = obj.getLinkedSpecialKeys();
            ar >> make_nvp("linked_special_keys", *pLinkedSpecialKeys);

            auto pLinkedDictionaries = obj.getLinkedDictionaries();
            ar >> make_nvp("linked_dictionaries", *pLinkedDictionaries);
        }
    }
}
