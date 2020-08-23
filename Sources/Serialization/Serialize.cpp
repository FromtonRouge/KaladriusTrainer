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

#include "Serialize.h"
#include "KeyboardTreeItemSerialize.h"
#include "TheoryTreeItemSerialize.h"
#include "Qt/QByteArray.h"
#include "Qt/QStandardItem.h"
#include "../Keyboards/Models/KeyboardModel.h"
#include "../Theories/Models/TheoryModel.h"
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/stream.hpp>
#include <fstream>
#include <iostream>

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

    bool Save(TheoryModel* pModel, const QString& sFilePath)
    {
        std::ofstream ofs(sFilePath.toStdString());
        boost::archive::xml_oarchive oa(ofs);
        try
        {
            auto pTheoryTreeItem = pModel->getTheoryTreeItem();
            oa << BOOST_SERIALIZATION_NVP(pTheoryTreeItem);
            return true;
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
        return false;
    }

    bool Load(TheoryModel* pModel, const QString& sFilePath)
    {
        std::ifstream ifs(sFilePath.toStdString());
        boost::archive::xml_iarchive ia(ifs);
        try
        {
            TheoryTreeItem* pTheoryTreeItem;
            ia >> BOOST_SERIALIZATION_NVP(pTheoryTreeItem);
            pModel->setTheoryTreeItem(pTheoryTreeItem);
            return true;
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
        return false;
    }

    QByteArray Save(const QList<QStandardItem*>& itemsRow)
    {
        typedef std::vector<char> CharBuffer;
        typedef boost::iostreams::back_insert_device<CharBuffer> BackInsertCharBuffer;
        CharBuffer data;
        BackInsertCharBuffer backInsertBuffer(data);
        boost::iostreams::stream<BackInsertCharBuffer> os(backInsertBuffer);
        boost::archive::binary_oarchive oa(os);
        try
        {
            const int iItemsCount = itemsRow.count();
            oa << boost::serialization::make_nvp("count", iItemsCount);
            for (int i = 0; i < iItemsCount; ++i)
            {
                oa << boost::serialization::make_nvp("item", itemsRow[i]);
            }
            os.flush();
            return QByteArray(&data[0], int(data.size()));
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
        return QByteArray();
    }

    QList<QStandardItem*> Load(const QByteArray& branchData)
    {
        using namespace boost::iostreams;
        QList<QStandardItem*> itemsRow;
        array_source source(branchData.data(), branchData.size());
        stream<array_source> is(source);
        boost::archive::binary_iarchive ia(is);
        try
        {
            int iItemsCount = 0;
            ia >> boost::serialization::make_nvp("count", iItemsCount);
            for (int i = 0; i < iItemsCount; ++i)
            {
                QStandardItem* pItem = nullptr;
                ia >> boost::serialization::make_nvp("item", pItem);
                itemsRow << pItem;
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
        return itemsRow;
    }
}
