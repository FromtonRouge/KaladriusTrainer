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

#include "KeycapTreeItemSerialize.h"
#include "TreeItemSerialize.h"
#include "Qt/QVariant.h"
#include "../Tree/Models/ItemDataRole.h"
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/base_object.hpp>
#include "ExplicitInstanciation.h"

EXPLICIT_INSTANCIATION(KeycapTreeItem)
BOOST_CLASS_EXPORT_IMPLEMENT(KeycapTreeItem) // For serializing from a base pointer
namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, KeycapTreeItem& obj,  const unsigned int fileVersion)
        {
            ar & make_nvp("base", base_object<TreeItem>(obj));
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const KeycapTreeItem& obj,  const unsigned int)
        {
            // Save keycap id
            const std::string& sId = obj.getKeycapId().toStdString();
            ar << make_nvp("id", sId);

            // Save keycap roles
            QVariant value;
            value = obj.data(RotationAngleRole);
            ar << make_nvp("rotation_angle", value);
            value = obj.data(RotationOriginRole);
            ar << make_nvp("rotation_origin", value);
            value = obj.data(OuterBorderRole);
            ar << make_nvp("outer_border", value);

            // Save keycap hierarchy
            auto pLabelTreeItem = obj.child(0, 0);
            auto pValueTreeItem = obj.child(0, 1);
            value = pValueTreeItem->data(Qt::EditRole);
            ar << make_nvp("label", value);

            pValueTreeItem = pLabelTreeItem->child(0, 1);
            value = pValueTreeItem->data(Qt::EditRole);
            ar << make_nvp("label_font", value);

            pValueTreeItem = pLabelTreeItem->child(1, 1);
            value = pValueTreeItem->data(Qt::EditRole);
            ar << make_nvp("label_x", value);

            pValueTreeItem = pLabelTreeItem->child(2, 1);
            value = pValueTreeItem->data(Qt::EditRole);
            ar << make_nvp("label_y", value);

            pValueTreeItem = obj.child(1, 1);
            value = pValueTreeItem->data(Qt::EditRole);
            ar << make_nvp("color", value);

            pValueTreeItem = obj.child(2, 1);
            value = pValueTreeItem->data(Qt::EditRole);
            ar << make_nvp("finger", value);
        }

        template<class Archive> void load(Archive& ar, KeycapTreeItem& obj,  const unsigned int iVersion)
        {
            // Load keycap id
            std::string sId;
            ar >> make_nvp("id", sId);
            obj.setKeycapId(QString::fromStdString(sId));

            // Load keycap roles
            QVariant value;
            ar >> make_nvp("rotation_angle", value);
            obj.setData(value, RotationAngleRole);
            ar >> make_nvp("rotation_origin", value);
            obj.setData(value, RotationOriginRole);
            ar >> make_nvp("outer_border", value);
            obj.setData(value, OuterBorderRole);

            // Load keycap hierarchy data
            auto pLabelTreeItem = obj.child(0, 0);
            auto pValueTreeItem = obj.child(0, 1);
            ar >> make_nvp("label", value);
            pValueTreeItem->setData(value, Qt::EditRole);

            pValueTreeItem = pLabelTreeItem->child(0, 1);
            ar >> make_nvp("label_font", value);
            pValueTreeItem->setData(value, Qt::EditRole);

            pValueTreeItem = pLabelTreeItem->child(1, 1);
            ar >> make_nvp("label_x", value);
            pValueTreeItem->setData(value, Qt::EditRole);

            pValueTreeItem = pLabelTreeItem->child(2, 1);
            ar >> make_nvp("label_y", value);
            pValueTreeItem->setData(value, Qt::EditRole);

            pValueTreeItem = obj.child(1, 1);
            ar >> make_nvp("color", value);
            pValueTreeItem->setData(value, Qt::EditRole);

            if (iVersion >= 1)
            {
                pValueTreeItem = obj.child(2, 1);
                ar >> make_nvp("finger", value);
                pValueTreeItem->setData(value, Qt::EditRole);
            }
        }
    }
}
