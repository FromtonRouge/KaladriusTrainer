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
#include "KeyboardPropertiesModel.h"
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

// QStandardItem
BOOST_CLASS_VERSION(QStandardItem, 0)
namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, QStandardItem& obj,  const unsigned int fileVersion)
        {
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const QStandardItem& obj,  const unsigned int fileVersion)
        {
            QByteArray bytes;
            QDataStream ds(&bytes, QIODevice::WriteOnly);
            obj.write(ds);
            int i = bytes.size();
            ar << make_nvp("data_size", i);
            ar << make_nvp("data", make_binary_object(bytes.data(), bytes.size()));

            const int iRows = obj.rowCount();
            const int iColumns = obj.columnCount();
            ar << make_nvp("rows", iRows);
            ar << make_nvp("columns", iColumns);
            for (int iRow = 0; iRow < iRows; ++iRow)
            {
                for (int iColumn = 0; iColumn < iColumns; ++iColumn)
                {
                    QStandardItem* pCell = obj.child(iRow, iColumn);
                    ar << make_nvp("cell", pCell);
                }
            }
        }

        template<class Archive> void load(Archive& ar, QStandardItem& obj,  const unsigned int fileVersion)
        {
            int i;
            ar >> make_nvp("data_size", i);

            QByteArray byteArray(i, 0);
            char* bytes = byteArray.data();
            ar >> make_nvp("data", make_binary_object(bytes, i));
            QDataStream ds(&byteArray, QIODevice::ReadOnly);
            obj.read(ds);

            int iRows = 0;
            int iColumns = 0;
            ar >> make_nvp("rows", iRows);
            ar >> make_nvp("columns", iColumns);
            for (int iRow = 0; iRow < iRows; ++iRow)
            {
                QList<QStandardItem*> cells;
                for (int iColumn = 0; iColumn < iColumns; ++iColumn)
                {
                    QStandardItem* pCell = nullptr;
                    ar >> make_nvp("cell", pCell);
                    cells << pCell;
                }

                if (!cells.isEmpty())
                {
                    obj.appendRow(cells);
                }
            }
        }
    }
}

namespace Serialization
{
    bool Save(KeyboardPropertiesModel* pModel, const QString& sFilePath)
    {
        auto pRootItem = pModel->invisibleRootItem();
        std::ofstream ofs(sFilePath.toStdString());
        boost::archive::xml_oarchive oa(ofs);
        try
        {
            oa << BOOST_SERIALIZATION_NVP(pRootItem);
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            return false;
        }
        return true;
    }

    bool Load(KeyboardPropertiesModel* pModel, const QString& sFilePath)
    {
        std::ifstream ifs(sFilePath.toStdString());
        boost::archive::xml_iarchive ia(ifs);
        try
        {
            pModel->clear();
            auto pRootItem = pModel->invisibleRootItem();
            ia >> BOOST_SERIALIZATION_NVP(*pRootItem);
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            return false;
        }
        return true;
    }
}
