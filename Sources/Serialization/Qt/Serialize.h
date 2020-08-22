// ======================================================================
// This file is a part of the KaladriusTrainer project
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

#include "QStandardItem.h"
#include "QVector.h"
#include "QByteArray.h"
#include "QString.h"
#include "../../Values/Types/KeycapRef.h"
#include "../../Values/Types/ListValue.h"
#include "../../Values/Types/Finger.h"
#include "../../Streams/Iostream.h"
#include <QtCore/QDataStream>
#include <QtCore/QVariant>
#include <QtCore/QDebug>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/split_free.hpp>

BOOST_CLASS_VERSION(QVariant, 0)
namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, QVariant& obj,  const unsigned int fileVersion)
        {
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const QVariant& obj,  const unsigned int)
        {
            const int iType = obj.type();
            ar << make_nvp("type", iType);

            if (iType != QVariant::UserType)
            {
                // For standard qt types we just serialize them in a QByteArray
                QByteArray data;
                QDataStream dataStream(&data, QIODevice::WriteOnly);
                dataStream << obj;
                ar << make_nvp("variant", data);
            }
            else
            {
                const QString sTypeName(QMetaType::typeName(obj.userType()));
                ar << make_nvp("user_type_name", sTypeName);

                if (sTypeName == QMetaType::typeName(qMetaTypeId<KeycapRef>()))
                {
                    const auto& data = qvariant_cast<KeycapRef>(obj);
                    ar << make_nvp("variant", data);
                }
                else if (sTypeName == QMetaType::typeName(qMetaTypeId<ListValue>()))
                {
                    const auto& data = qvariant_cast<ListValue>(obj);
                    ar << make_nvp("variant", data);
                }
                else if (sTypeName == QMetaType::typeName(qMetaTypeId<Finger>()))
                {
                    const auto& data = qvariant_cast<Finger>(obj);
                    ar << make_nvp("variant", data);
                }
                else
                {
                    Q_ASSERT_X(false, "Qt/Serialize.h", "Can't save: Unknown QVariant user type");
                }
            }
        }

        template<class Archive> void load(Archive& ar, QVariant& obj,  const unsigned int)
        {
            int iType = 0;
            ar >> make_nvp("type", iType);
            if (iType != QVariant::UserType)
            {
                QByteArray data;
                ar >> make_nvp("variant", data);
                QDataStream dataStream(data);
                dataStream >> obj;
            }
            else
            {
                QString sTypeName;
                ar >> make_nvp("user_type_name", sTypeName);

                if (sTypeName == QMetaType::typeName(qMetaTypeId<KeycapRef>()))
                {
                    KeycapRef data;
                    ar >> make_nvp("variant", data);
                    obj = QVariant::fromValue(data);
                }
                else if (sTypeName == QMetaType::typeName(qMetaTypeId<ListValue>()))
                {
                    ListValue data;
                    ar >> make_nvp("variant", data);
                    obj = QVariant::fromValue(data);
                }
                else if (sTypeName == QMetaType::typeName(qMetaTypeId<Finger>()))
                {
                    Finger data;
                    ar >> make_nvp("variant", data);
                    obj = QVariant::fromValue(data);
                }
                else
                {
                    Q_ASSERT_X(false, "Qt/Serialize.h", "Can't load: Unknown QVariant user type");
                }
            }
        }
    }
}

BOOST_CLASS_VERSION(KeycapRef, 0)
namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, KeycapRef& obj,  const unsigned int)
        {
            ar & make_nvp("keycap_id", obj.keycapId);
        }
    }
}

BOOST_CLASS_VERSION(Finger, 0)
namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, Finger& obj,  const unsigned int)
        {
            ar & make_nvp("finger_id", obj.id);
        }
    }
}

BOOST_CLASS_VERSION(ListValue, 0)
namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, ListValue& obj,  const unsigned int)
        {
            ar & make_nvp("label", obj.sLabel);
            ar & make_nvp("default_value", obj.defaultValue);
            ar & make_nvp("naming_policy", obj.namingPolicy);
        }
    }
}
