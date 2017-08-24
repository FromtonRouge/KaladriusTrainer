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

#include "Serialization.h"
#include "KeyboardModel.h"
#include "DictionariesModel.h"
#include "TreeItems/KeyboardTreeItem.h"
#include "TreeItems/KeycapsTreeItem.h"
#include "TreeItems/KeycapTreeItem.h"
#include <QtCore/QDataStream>
#include <QtCore/QByteArray>
#include <QtCore/QVariant>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/string.hpp>
#include <fstream>
#include <iostream>
#include <string>

// QByteArray
BOOST_CLASS_VERSION(QByteArray, 0)
namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, QByteArray& obj,  const unsigned int fileVersion)
        {
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const QByteArray& obj,  const unsigned int fileVersion)
        {
            int iSize = obj.size();
            ar << make_nvp("size", iSize);
            ar << make_nvp("bytes", make_binary_object((void*)obj.data(), iSize));
        }

        template<class Archive> void load(Archive& ar, QByteArray& obj,  const unsigned int fileVersion)
        {
            int iSize = 0;
            ar >> make_nvp("size", iSize);
            obj.resize(iSize);
            ar >> make_nvp("bytes", make_binary_object(obj.data(), iSize));
        }
    }
}

// QVariant
BOOST_CLASS_VERSION(QVariant, 0)
namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, QVariant& obj,  const unsigned int fileVersion)
        {
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const QVariant& obj,  const unsigned int fileVersion)
        {
            QByteArray data;
            QDataStream dataStream(&data, QIODevice::WriteOnly);
            dataStream << obj;
            ar << make_nvp("variant", data);
        }

        template<class Archive> void load(Archive& ar, QVariant& obj,  const unsigned int fileVersion)
        {
            QByteArray data;
            ar >> make_nvp("variant", data);
            QDataStream dataStream(data);
            dataStream >> obj;
        }
    }
}

// KeycapTreeItem
BOOST_CLASS_VERSION(KeycapTreeItem, 0)
namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, KeycapTreeItem& obj,  const unsigned int fileVersion)
        {
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const KeycapTreeItem& obj,  const unsigned int fileVersion)
        {
            // Save keycap id
            const std::string& sId = obj.getKeycapId().toStdString();
            ar << make_nvp("id", sId);

            // Save keycap roles
            QVariant value;
            value = obj.data(KeyboardModel::RotationAngleRole);
            ar << make_nvp("rotation_angle", value);
            value = obj.data(KeyboardModel::RotationOriginRole);
            ar << make_nvp("rotation_origin", value);
            value = obj.data(KeyboardModel::OuterBorderRole);
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
        }

        template<class Archive> void load(Archive& ar, KeycapTreeItem& obj,  const unsigned int fileVersion)
        {
            // Load keycap id
            std::string sId;
            ar >> make_nvp("id", sId);
            obj.setKeycapId(QString::fromStdString(sId));

            // Load keycap roles
            QVariant value;
            ar >> make_nvp("rotation_angle", value);
            obj.setData(value, KeyboardModel::RotationAngleRole);
            ar >> make_nvp("rotation_origin", value);
            obj.setData(value, KeyboardModel::RotationOriginRole);
            ar >> make_nvp("outer_border", value);
            obj.setData(value, KeyboardModel::OuterBorderRole);

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
        }
    }
}

// KeycapsTreeItem
BOOST_CLASS_VERSION(KeycapsTreeItem, 0)
namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, KeycapsTreeItem& obj,  const unsigned int fileVersion)
        {
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const KeycapsTreeItem& obj,  const unsigned int fileVersion)
        {
            const int iKeycaps = obj.rowCount();
            ar << make_nvp("keycaps_count", iKeycaps);
            for (int iKeycap = 0; iKeycap < iKeycaps; ++iKeycap)
            {
                auto pKeycapTreeItem = static_cast<KeycapTreeItem*>(obj.child(iKeycap));
                ar << make_nvp("keycap", pKeycapTreeItem);
            }
        }

        template<class Archive> void load(Archive& ar, KeycapsTreeItem& obj,  const unsigned int fileVersion)
        {
            int iKeycaps = 0;
            ar >> make_nvp("keycaps_count", iKeycaps);
            for (int iKeycap = 0; iKeycap < iKeycaps; ++iKeycap)
            {
                KeycapTreeItem* pKeycapTreeItem = nullptr;
                ar >> make_nvp("keycap", pKeycapTreeItem);
                if (pKeycapTreeItem)
                {
                    obj.appendRow({pKeycapTreeItem, new EmptyTreeItem()});
                }
            }
        }
    }
}

// KeyboardTreeItem
BOOST_CLASS_VERSION(KeyboardTreeItem, 0)
namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, KeyboardTreeItem& obj,  const unsigned int fileVersion)
        {
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const KeyboardTreeItem& obj,  const unsigned int fileVersion)
        {
            auto pKeycapsTreeItem = obj.getKeycaps();
            ar << make_nvp("keycaps", pKeycapsTreeItem);
        }

        template<class Archive> void load(Archive& ar, KeyboardTreeItem& obj,  const unsigned int fileVersion)
        {
            auto pKeycapsTreeItem = obj.getKeycaps();
            ar >> make_nvp("keycaps", *pKeycapsTreeItem);
        }
    }
}

namespace Serialization
{
    bool Save(KeyboardModel* pModel, const QString& sFilePath)
    {
        std::ofstream ofs(sFilePath.toStdString());
        boost::archive::xml_oarchive oa(ofs);
        try
        {
            const QByteArray& svg = pModel->getSvgContent();
            oa << BOOST_SERIALIZATION_NVP(svg);

            auto pKeyboardTreeItem = pModel->getKeyboardTreeItem();
            oa << BOOST_SERIALIZATION_NVP(pKeyboardTreeItem);
            return true;
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
        return false;
    }

    bool Load(KeyboardModel* pModel, const QString& sFilePath)
    {
        std::ifstream ifs(sFilePath.toStdString());
        boost::archive::xml_iarchive ia(ifs);
        try
        {
            QByteArray svg;
            ia >> BOOST_SERIALIZATION_NVP(svg);
            pModel->setSvgContent(svg);

            KeyboardTreeItem* pKeyboardTreeItem;
            ia >> BOOST_SERIALIZATION_NVP(pKeyboardTreeItem);
            pModel->setKeyboardTreeItem(pKeyboardTreeItem);
            return true;
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
        return false;
    }

    bool Save(DictionariesModel* pModel, const QString& sFilePath)
    {
        std::ofstream ofs(sFilePath.toStdString());
        boost::archive::xml_oarchive oa(ofs);
        try
        {
            // TODO
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
        return false;
    }

    bool Load(DictionariesModel* pModel, const QString& sFilePath)
    {
        std::ifstream ifs(sFilePath.toStdString());
        boost::archive::xml_iarchive ia(ifs);
        try
        {
            // TODO
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
        return false;
    }
}
