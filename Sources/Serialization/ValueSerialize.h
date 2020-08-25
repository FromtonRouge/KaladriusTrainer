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

#pragma once

#include "KeycapRefSerialize.h"
#include "Qt/QBitArray.h"
#include "Qt/QByteArray.h"
#include "Qt/QColor.h"
#include "Qt/QFont.h"
#include "Qt/QPointF.h"
#include "Qt/QRectF.h"
#include "Qt/QString.h"
#include <QtCore/QVariant>
#include <QtCore/QMetaType>
#include <QtCore/QDebug>
#include <boost/serialization/string.hpp>
#include <string>

namespace boost
{
    namespace serialization
    {
        template<class Archive, typename T> void save(Archive& ar, const QVariant& variant) { const T& value = variant.value<T>(); ar << make_nvp("value", value); }
        template<class Archive, typename T> void load(Archive& ar, QVariant& variant) { T value; ar >> make_nvp("value", value); variant.setValue(value); }

        template<class Archive> void saveValue(Archive& ar, const QVariant& value)
        {
            // Save the typename
            std::string sTypeName;
            const int iUserType = value.userType();
            if (value.isValid())
            {
                const char* szTypeName = QMetaType::typeName(iUserType);
                Q_ASSERT(szTypeName != nullptr);
                sTypeName = szTypeName;
            }
            ar << make_nvp("type_name", sTypeName);

            // Save the value
            if (!sTypeName.empty())
            {
                switch (iUserType)
                {
                case QVariant::BitArray: { save<Archive, QBitArray>(ar, value); break; }
                case QVariant::ByteArray: { save<Archive, QByteArray>(ar, value); break; }
                case QVariant::Color: { save<Archive, QColor>(ar, value); break; }
                case QVariant::Font: { save<Archive, QFont>(ar, value); break; }
                case QVariant::PointF: { save<Archive, QPointF>(ar, value); break; }
                case QVariant::RectF: { save<Archive, QRectF>(ar, value); break; }
                case QVariant::String: { save<Archive, QString>(ar, value); break; }
                default:
                    {
                        if (qMetaTypeId<KeycapRef>() == iUserType)
                        {
                            save<Archive, KeycapRef>(ar, value);
                        }
                        else
                        {
                            qDebug() << "saveValue: Unhandled type" << QString::fromStdString(sTypeName);
                            Q_ASSERT_X(false, "saveValue", "TODO: Unhandled type");
                        }
                        break;
                    }
                }
            }
        }

        template<class Archive> void loadValue(Archive& ar, QVariant& value)
        {
            std::string sTypeName;
            ar >> make_nvp("type_name", sTypeName);

            if (!sTypeName.empty())
            {
                const char* szTypeName = sTypeName.c_str();
                const int iUserType = QMetaType::type(szTypeName);
                switch (iUserType)
                {
                case QVariant::BitArray: { load<Archive, QBitArray>(ar, value); break; }
                case QVariant::ByteArray: { load<Archive, QByteArray>(ar, value); break; }
                case QVariant::Color: { load<Archive, QColor>(ar, value); break; }
                case QVariant::Font: { load<Archive, QFont>(ar, value); break; }
                case QVariant::PointF: { load<Archive, QPointF>(ar, value); break; }
                case QVariant::RectF: { load<Archive, QRectF>(ar, value); break; }
                case QVariant::String: { load<Archive, QString>(ar, value); break; }
                default:
                    {
                        if (qMetaTypeId<KeycapRef>() == iUserType)
                        {
                            load<Archive, KeycapRef>(ar, value);
                        }
                        else
                        {
                            qDebug() << "loadValue: Unhandled type" << QString::fromStdString(sTypeName);
                            Q_ASSERT_X(false, "loadValue", "TODO: Unhandled type");
                        }
                        break;
                    }
                }
            }
        }
    }
}
