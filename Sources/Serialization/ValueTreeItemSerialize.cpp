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

#include "ValueTreeItemSerialize.h"
#include "TreeItemSerialize.h"
#include "Qt/QVariant.h"
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/base_object.hpp>
#include "ExplicitInstanciation.h"

EXPLICIT_INSTANCIATION(ValueTreeItem)
BOOST_CLASS_EXPORT_IMPLEMENT(ValueTreeItem)

namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, ValueTreeItem& obj,  const unsigned int fileVersion)
        {
            ar & make_nvp("base", base_object<TreeItem>(obj));
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const ValueTreeItem& obj,  const unsigned int)
        {
            const QVariant& value = obj.data(Qt::EditRole);
            ar << make_nvp("value", value);
        }

        template<class Archive> void load(Archive& ar, ValueTreeItem& obj,  const unsigned int)
        {
            QVariant value;
            ar >> make_nvp("value", value);
            obj.setData(value, Qt::EditRole);
        }
    }
}
