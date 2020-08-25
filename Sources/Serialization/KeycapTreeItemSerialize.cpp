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
#include "FingerSerialize.h"
#include "Qt/QPointF.h"
#include "Qt/QFont.h"
#include "Qt/QRectF.h"
#include "Qt/QColor.h"
#include "Qt/QString.h"
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
            const qreal rotationAngle = obj.data(RotationAngleRole).toReal();
            ar << make_nvp("rotation_angle", rotationAngle);

            const QPointF& rotationOrigin = obj.data(RotationOriginRole).toPointF();
            ar << make_nvp("rotation_origin", rotationOrigin);

            const QRectF& outerBorder = obj.data(OuterBorderRole).toRectF();
            ar << make_nvp("outer_border", outerBorder);

            // Save keycap hierarchy
            auto pLabelTreeItem = obj.child(0, 0);
            auto pValueTreeItem = obj.child(0, 1);
            const QString& sLabel = pValueTreeItem->data(Qt::EditRole).toString();
            ar << make_nvp("label", sLabel);

            pValueTreeItem = pLabelTreeItem->child(0, 1);
            const QFont& font = pValueTreeItem->data(Qt::EditRole).value<QFont>();
            ar << make_nvp("label_font", font);

            pValueTreeItem = pLabelTreeItem->child(1, 1);
            const qreal x = pValueTreeItem->data(Qt::EditRole).toReal();
            ar << make_nvp("label_x", x);

            pValueTreeItem = pLabelTreeItem->child(2, 1);
            const qreal y = pValueTreeItem->data(Qt::EditRole).toReal();
            ar << make_nvp("label_y", y);

            pValueTreeItem = obj.child(1, 1);
            const QColor& color = pValueTreeItem->data(Qt::EditRole).value<QColor>();
            ar << make_nvp("color", color);

            pValueTreeItem = obj.child(2, 1);
            const Finger& finger = pValueTreeItem->data(Qt::EditRole).value<Finger>();
            ar << make_nvp("finger", finger);
        }

        template<class Archive> void load(Archive& ar, KeycapTreeItem& obj,  const unsigned int)
        {
            // Load keycap id
            std::string sId;
            ar >> make_nvp("id", sId);
            obj.setKeycapId(QString::fromStdString(sId));

            // Load keycap roles
            qreal rotationAngle;
            ar >> make_nvp("rotation_angle", rotationAngle);
            obj.setData(rotationAngle, RotationAngleRole);

            QPointF rotationOrigin;
            ar >> make_nvp("rotation_origin", rotationOrigin);
            obj.setData(rotationOrigin, RotationOriginRole);

            QRectF outerBorder;
            ar >> make_nvp("outer_border", outerBorder);
            obj.setData(outerBorder, OuterBorderRole);

            // Load keycap hierarchy data
            auto pLabelTreeItem = obj.child(0, 0);
            auto pValueTreeItem = obj.child(0, 1);

            QString sLabel;
            ar >> make_nvp("label", sLabel);
            pValueTreeItem->setData(sLabel, Qt::EditRole);

            pValueTreeItem = pLabelTreeItem->child(0, 1);

            QFont font;
            ar >> make_nvp("label_font", font);
            pValueTreeItem->setData(font, Qt::EditRole);

            pValueTreeItem = pLabelTreeItem->child(1, 1);

            qreal x;
            ar >> make_nvp("label_x", x);
            pValueTreeItem->setData(x, Qt::EditRole);

            pValueTreeItem = pLabelTreeItem->child(2, 1);

            qreal y;
            ar >> make_nvp("label_y", y);
            pValueTreeItem->setData(y, Qt::EditRole);

            pValueTreeItem = obj.child(1, 1);

            QColor color;
            ar >> make_nvp("color", color);
            pValueTreeItem->setData(color, Qt::EditRole);

            pValueTreeItem = obj.child(2, 1);

            Finger finger;
            ar >> make_nvp("finger", finger);
            pValueTreeItem->setData(QVariant::fromValue(finger), Qt::EditRole);
        }
    }
}
