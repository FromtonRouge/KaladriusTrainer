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

#include "Serialize.h"
#include "Serialization/Qt/Serialize.h"
#include "Keyboards/Models/KeyboardModel.h"
#include "Theories/Models/TheoryModel.h"
#include "Keyboards/TreeItems/KeyboardTreeItem.h"
#include "Keyboards/TreeItems/KeycapTreeItem.h"
#include "Tree/TreeItems/ListTreeItem.h"
#include "Theories/TreeItems/TheoryTreeItem.h"
#include "Theories/TreeItems/DictionaryTreeItem.h"
#include "Theories/TreeItems/OutputTextTreeItem.h"
#include "Theories/TreeItems/InputKeysTreeItem.h"
#include "Tree/TreeItems/AttributeTreeItem.h"
#include "Tree/TreeItems/ValueTreeItem.h"
#include "Theories/TreeItems/LinkedTheoryTreeItem.h"
#include "Theories/TreeItems/LinkedDictionaryTreeItem.h"
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/stream.hpp>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

BOOST_CLASS_VERSION(TreeItem, 0)
BOOST_CLASS_EXPORT(TreeItem) // For serializing from a base pointer
namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, TreeItem& obj,  const unsigned int)
        {
            ar & make_nvp("base", base_object<QStandardItem>(obj));
        }
    }
}

BOOST_CLASS_VERSION(EmptyTreeItem, 0)
BOOST_CLASS_EXPORT(EmptyTreeItem) // For serializing from a base pointer
namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, EmptyTreeItem& obj,  const unsigned int)
        {
            ar & make_nvp("base", base_object<TreeItem>(obj));
        }
    }
}

BOOST_CLASS_VERSION(ValueTreeItem, 0)
BOOST_CLASS_EXPORT(ValueTreeItem) // For serializing from a base pointer
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

BOOST_CLASS_VERSION(KeycapTreeItem, 1)
BOOST_CLASS_EXPORT(KeycapTreeItem) // For serializing from a base pointer
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

BOOST_CLASS_VERSION(ListTreeItem, 0)
BOOST_CLASS_EXPORT(ListTreeItem) // For serializing from a base pointer
namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, ListTreeItem& obj,  const unsigned int fileVersion)
        {
            ar & make_nvp("base", base_object<TreeItem>(obj));
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const ListTreeItem& obj,  const unsigned int)
        {
            // Check if the name is editable
            bool bNameIsEditable = false;
            auto pParent = obj.parent();
            if (pParent && pParent->type() == TreeItem::List)
            {
                auto pGrandParent = pParent->parent();
                if (pGrandParent)
                {
                    auto pParentValue = pGrandParent->child(pParent->row(), 1);
                    if (pParentValue)
                    {
                        const QVariant& variant = pParentValue->data(Qt::EditRole);
                        if (variant.isValid() && variant.userType() == qMetaTypeId<ListValue>())
                        {
                            bNameIsEditable = qvariant_cast<ListValue>(variant).namingPolicy == ListValue::NameIsEditable;
                        }
                    }
                }
            }

            ar << make_nvp("name_is_editable", bNameIsEditable);

            if (bNameIsEditable)
            {
                std::string sText = obj.text().toStdString();
                ar << make_nvp("name", sText);
            }

            const int iRows = obj.rowCount();
            const int iColumns = obj.columnCount();
            ar << make_nvp("rows", iRows);
            ar << make_nvp("columns", iColumns);
            for (int iRow = 0; iRow < iRows; ++iRow)
            {
                for (int iColumn = 0; iColumn < iColumns; ++iColumn)
                {
                    auto pTreeItem = obj.child(iRow, iColumn);
                    ar << make_nvp("item", pTreeItem);
                }
            }
        }

        template<class Archive> void load(Archive& ar, ListTreeItem& obj,  const unsigned int)
        {
            bool bNameIsEditable = false;
            ar >> make_nvp("name_is_editable", bNameIsEditable);

            if (bNameIsEditable)
            {
                std::string sText;
                ar >> make_nvp("name", sText);
                obj.setText(QString::fromStdString(sText));
            }

            int iRows = 0;
            int iColumns = 0;
            ar >> make_nvp("rows", iRows);
            ar >> make_nvp("columns", iColumns);
            for (int iRow = 0; iRow < iRows; ++iRow)
            {
                QList<QStandardItem*> items;
                for (int iColumn = 0; iColumn < iColumns; ++iColumn)
                {
                    QStandardItem* pTreeItem = nullptr;
                    ar >> make_nvp("item", pTreeItem);
                    if (pTreeItem)
                    {
                        items << pTreeItem;
                    }
                }
                obj.appendRow(items);
            }
        }
    }
}

BOOST_CLASS_VERSION(LinkedDictionaryTreeItem, 0)
BOOST_CLASS_EXPORT(LinkedDictionaryTreeItem) // For serializing from a base pointer
namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, LinkedDictionaryTreeItem& obj,  const unsigned int fileVersion)
        {
            ar & make_nvp("base", base_object<TreeItem>(obj));
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const LinkedDictionaryTreeItem& obj,  const unsigned int)
        {
            std::string sText = obj.text().toStdString();
            ar << make_nvp("name", sText);

            auto pLinkedKeysTreeItem = obj.getLinkedKeys();
            ar << make_nvp("linked_keys", *pLinkedKeysTreeItem);
        }

        template<class Archive> void load(Archive& ar, LinkedDictionaryTreeItem& obj,  const unsigned int)
        {
            std::string sText;
            ar >> make_nvp("name", sText);
            obj.setText(QString::fromStdString(sText));

            auto pLinkedKeysTreeItem = obj.getLinkedKeys();
            ar >> make_nvp("linked_keys", *pLinkedKeysTreeItem);
        }
    }
}

BOOST_CLASS_VERSION(LinkedTheoryTreeItem, 0)
BOOST_CLASS_EXPORT(LinkedTheoryTreeItem) // For serializing from a base pointer
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

BOOST_CLASS_VERSION(KeyboardTreeItem, 0)
namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, KeyboardTreeItem& obj,  const unsigned int fileVersion)
        {
            ar & make_nvp("base", base_object<TreeItem>(obj));
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const KeyboardTreeItem& obj,  const unsigned int)
        {
            ListTreeItem* pKeycapsTreeItem = obj.getKeycaps();
            ar << make_nvp("keycaps", *pKeycapsTreeItem);

            ListTreeItem* pLinkedTheories = obj.getLinkedTheories();
            ar << make_nvp("linked_theories", *pLinkedTheories);
        }

        template<class Archive> void load(Archive& ar, KeyboardTreeItem& obj,  const unsigned int)
        {
            ListTreeItem* pKeycapsTreeItem = obj.getKeycaps();
            ar >> make_nvp("keycaps", *pKeycapsTreeItem);

            ListTreeItem* pLinkedTheories = obj.getLinkedTheories();
            ar >> make_nvp("linked_theories", *pLinkedTheories);
        }
    }
}

BOOST_CLASS_VERSION(OutputTextTreeItem, 0)
BOOST_CLASS_EXPORT(OutputTextTreeItem) // For serializing from a base pointer
namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, OutputTextTreeItem& obj,  const unsigned int fileVersion)
        {
            ar & make_nvp("base", base_object<TreeItem>(obj));
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const OutputTextTreeItem& obj,  const unsigned int)
        {
            std::string sText = obj.getOutputText().toStdString();
            ar << make_nvp("output", sText);
        }

        template<class Archive> void load(Archive& ar, OutputTextTreeItem& obj,  const unsigned int)
        {
            std::string sText;
            ar >> make_nvp("output", sText);
            obj.setOutputText(QString::fromStdString(sText));
        }
    }
}

BOOST_CLASS_VERSION(InputKeysTreeItem, 0)
BOOST_CLASS_EXPORT(InputKeysTreeItem) // For serializing from a base pointer
namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, InputKeysTreeItem& obj,  const unsigned int fileVersion)
        {
            ar & make_nvp("base", base_object<TreeItem>(obj));
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const InputKeysTreeItem& obj,  const unsigned int)
        {
            std::string sText = obj.text().toStdString();
            ar << make_nvp("keys", sText);

            const auto& bits = obj.data(InputKeyBitsRole);
            ar << make_nvp("bits", bits);
        }

        template<class Archive> void load(Archive& ar, InputKeysTreeItem& obj,  const unsigned int)
        {
            std::string sText;
            ar >> make_nvp("keys", sText);
            obj.setText(QString::fromStdString(sText));

            QVariant variant;
            ar >> make_nvp("bits", variant);
            obj.setKeyBits(variant.toBitArray());
        }
    }
}

BOOST_CLASS_VERSION(DictionaryTreeItem, 0)
BOOST_CLASS_EXPORT(DictionaryTreeItem) // For serializing from a base pointer
namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, DictionaryTreeItem& obj,  const unsigned int fileVersion)
        {
            ar & make_nvp("base", base_object<TreeItem>(obj));
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const DictionaryTreeItem& obj,  const unsigned int)
        {
            std::string sText = obj.text().toStdString();
            ar << make_nvp("name", sText);

            QVariant value = obj.getMandatoryKey()->getValue()->data(Qt::EditRole);
            ar << make_nvp("mandatory_key", value);

            auto pKeysTreeItem = obj.getKeys();
            ar << make_nvp("keys", *pKeysTreeItem);

            auto pEntriesTreeItem = obj.getEntries();
            ar << make_nvp("entries", *pEntriesTreeItem);
        }

        template<class Archive> void load(Archive& ar, DictionaryTreeItem& obj,  const unsigned int iVersion)
        {
            std::string sText;
            ar >> make_nvp("name", sText);
            obj.setText(QString::fromStdString(sText));

            QVariant value;
            ar >> make_nvp("mandatory_key", value);
            obj.getMandatoryKey()->getValue()->setData(value, Qt::EditRole);

            auto pKeysTreeItem = obj.getKeys();
            ar >> make_nvp("keys", *pKeysTreeItem);

            auto pEntriesTreeItem = obj.getEntries();
            ar >> make_nvp("entries", *pEntriesTreeItem);
        }
    }
}

BOOST_CLASS_VERSION(TheoryTreeItem, 0)
namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, TheoryTreeItem& obj,  const unsigned int fileVersion)
        {
            ar & make_nvp("base", base_object<TreeItem>(obj));
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const TheoryTreeItem& obj,  const unsigned int)
        {
            QVariant value;
            value = obj.getName()->getValue()->data(Qt::EditRole);
            ar << make_nvp("name", value);

            value = obj.getDescription()->getValue()->data(Qt::EditRole);
            ar << make_nvp("description", value);

            auto pSpecialKeys = obj.getSpecialKeys();
            ar << make_nvp("special_keys", *pSpecialKeys);

            auto pDictionaries = obj.getDictionaries();
            ar << make_nvp("dictionaries", *pDictionaries);
        }

        template<class Archive> void load(Archive& ar, TheoryTreeItem& obj,  const unsigned int)
        {
            QVariant value;
            auto pValue = obj.getName()->getValue();
            ar >> make_nvp("name", value);
            pValue->setData(value, Qt::EditRole);

            pValue = obj.getDescription()->getValue();
            ar >> make_nvp("description", value);
            pValue->setData(value, Qt::EditRole);

            auto pSpecialKeys = obj.getSpecialKeys();
            ar >> make_nvp("special_keys", *pSpecialKeys);

            auto pDictionaries = obj.getDictionaries();
            ar >> make_nvp("dictionaries", *pDictionaries);
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
