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
#include "TreeItems/KeyboardTreeItem.h"
#include "TreeItems/KeycapsTreeItem.h"
#include "TreeItems/KeycapTreeItem.h"
#include "TreeItems/AttributeTreeItem.h"
#include "TreeItems/AttributeValueTreeItem.h"
#include <QtCore/QDataStream>
#include <QtCore/QByteArray>
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
            // TODO
        }

        template<class Archive> void load(Archive& ar, KeycapTreeItem& obj,  const unsigned int fileVersion)
        {
            // TODO
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
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            return false;
        }
        return true;
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
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            return false;
        }
        return true;
    }
}
