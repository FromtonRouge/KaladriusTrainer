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

#include "../../Values/Types/KeycapRef.h"
#include "../../Values/Types/ListValue.h"
#include "../../Values/Types/Finger.h"
#include "../../Streams/Iostream.h"
#include <QtGui/QTextDocument>
#include <QtGui/QTextFormat>
#include <QtGui/QTextCursor>
#include <QtGui/QTextFrame>
#include <QtGui/QTextTable>
#include <QtGui/QStandardItem>
#include <QtCore/QDataStream>
#include <QtCore/QByteArray>
#include <QtCore/QVariant>
#include <QtCore/QVector>
#include <QtCore/QDebug>
#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/split_free.hpp>

BOOST_CLASS_VERSION(QByteArray, 0)
namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, QByteArray& obj,  const unsigned int fileVersion)
        {
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const QByteArray& obj,  const unsigned int)
        {
            int iSize = obj.size();
            ar << make_nvp("size", iSize);
            void* pBinaryData = const_cast<void*>(static_cast<const void*>(obj.data()));
            ar << make_nvp("bytes", make_binary_object(pBinaryData, size_t(iSize)));
        }

        template<class Archive> void load(Archive& ar, QByteArray& obj,  const unsigned int)
        {
            int iSize = 0;
            ar >> make_nvp("size", iSize);
            obj.resize(iSize);
            ar >> make_nvp("bytes", make_binary_object(obj.data(), size_t(iSize)));
        }
    }
}

BOOST_CLASS_VERSION(QString, 0)
namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, QString& obj,  const unsigned int fileVersion)
        {
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const QString& obj,  const unsigned int)
        {
            const std::string& sString = obj.toStdString();
            ar << make_nvp("string", sString);
        }

        template<class Archive> void load(Archive& ar, QString& obj,  const unsigned int)
        {
            std::string sString;
            ar >> make_nvp("string", sString);
            obj = QString::fromStdString(sString);
        }
    }
}

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

BOOST_CLASS_VERSION(QStandardItem, 0)
namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive&, QStandardItem&,  const unsigned int)
        {
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

BOOST_CLASS_VERSION(QTextFormat, 0)
namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, QTextFormat& obj,  const unsigned int fileVersion)
        {
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const QTextFormat& obj,  const unsigned int fileVersion)
        {
            const int iFormatType = obj.type();
            ar << make_nvp("format_type", iFormatType);

            const auto& properties = obj.properties();
            const int iProperties = properties.size();
            ar << make_nvp("properties", iProperties);
            auto it = properties.begin();
            while (it != properties.end())
            {
                const int iPropertyId = it.key();
                const QVariant& propertyValue = it++.value();
                ar << make_nvp("id", iPropertyId);
                ar << make_nvp("value", propertyValue);
            }
        }

        template<class Archive> void load(Archive& ar, QTextFormat& obj,  const unsigned int fileVersion)
        {
            int iFormatType;
            ar >> make_nvp("format_type", iFormatType);
            obj = QTextFormat(iFormatType);

            int iProperties;
            ar >> make_nvp("properties", iProperties);
            for (int iProperty = 0; iProperty < iProperties; ++iProperty)
            {
                int iPropertyId;
                QVariant propertyValue;
                ar >> make_nvp("id", iPropertyId);
                ar >> make_nvp("value", propertyValue);
                obj.setProperty(iPropertyId, propertyValue);
            }
        }
    }
}

// Generic QVector<T> serialization
namespace boost
{
    namespace serialization
    {
        template<class T> struct version<QVector<T>>
        {
            typedef mpl::int_<0> type; // Increment the version number here, must be between 0 and 255
            typedef mpl::integral_c_tag tag;
            BOOST_STATIC_CONSTANT(int, value = version::type::value);
        };

        template<class Archive, class T> void serialize(Archive& ar, QVector<T>& obj, const unsigned int fileVersion)
        {
            split_free(ar, obj, fileVersion);
        }

        template<class Archive, class T> void save(Archive& ar, const QVector<T>& obj,  const unsigned int fileVersion)
        {
            const int iCount = obj.size();
            ar << make_nvp("count", iCount);
            for (const auto& element : obj)
            {
                ar << make_nvp("element", element);
            }
        }

        template<class Archive, class T> void load(Archive& ar, QVector<T>& obj,  const unsigned int fileVersion)
        {
            int iCount;
            ar >> make_nvp("count", iCount);
            for (int i = 0; i < iCount; ++i)
            {
                T element;
                ar >> make_nvp("element", element);
                obj << element;
            }
        }
    }
}

BOOST_CLASS_VERSION(QTextDocument, 0)
namespace boost
{
    namespace serialization
    {
        // Only used on load
        struct TextDocumentContext
        {
            TextDocumentContext(int version) : fileVersion(version) {}
            QVector<QTextFormat> allFormats;
            QTextCursor cursor;
            int fileVersion; // Version of QTextDocument
        };

        template<class Archive> void serialize(Archive& ar, QTextDocument& obj,  const unsigned int fileVersion)
        {
            split_free(ar, obj, fileVersion);
        }

        const bool HAS_NEXT = true;
        const bool NO_NEXT = false;
        template<class Archive> void SaveTextFragments(Archive& ar, QTextBlock::iterator itBegin, QTextBlock::iterator itEnd, const unsigned int fileVersion)
        {
            auto itFragment = itBegin;
            while (itFragment != itEnd)
            {
                const QTextFragment& fragment = itFragment.fragment();
                if (fragment.isValid())
                {
                    ar << make_nvp("fragment_iterator", HAS_NEXT);

                    int iTextFormatIndex = fragment.charFormatIndex();
                    ar << make_nvp("text_format_index", iTextFormatIndex);

                    const QString& sText = fragment.text();
                    ar << make_nvp("text", sText);
                }
                itFragment++;
            }
            ar << make_nvp("fragment_iterator", NO_NEXT);
        }

        template<class Archive> void LoadTextFragments(Archive& ar, TextDocumentContext& context)
        {
            int iTextFormatIndex = 0;
            bool bHasNext;
            ar >> make_nvp("fragment_iterator", bHasNext);
            while (bHasNext)
            {
                ar >> make_nvp("text_format_index", iTextFormatIndex);
                const QTextFormat& textCharFormat = context.allFormats[iTextFormatIndex];

                QString sText;
                ar >> make_nvp("text", sText);
                context.cursor.insertText(sText, textCharFormat.toCharFormat());

                ar >> make_nvp("fragment_iterator", bHasNext);
            }
        }

        template<class Archive> void SaveTextFrame(Archive& ar, QTextFrame::iterator itBegin, QTextFrame::iterator itEnd, const unsigned int fileVersion);
        template<class Archive> void SaveTextTable(Archive& ar, const QTextTable* pTextTable, const unsigned int fileVersion)
        {
            const int iRows = pTextTable->rows();
            ar << make_nvp("rows", iRows);

            const int iColumns = pTextTable->columns();
            ar << make_nvp("columns", iColumns);

            int iTextFormatIndex = 0;
            for (int iRow = 0; iRow < iRows; ++iRow)
            {
                for (int iColumn = 0; iColumn < iColumns; ++iColumn)
                {
                    const QTextTableCell& cell = pTextTable->cellAt(iRow, iColumn);
                    iTextFormatIndex = cell.tableCellFormatIndex();
                    ar << make_nvp("text_format_index", iTextFormatIndex);

                    SaveTextFrame(ar, cell.begin(), cell.end(), fileVersion);
                }
            }
        }

        template<class Archive> void LoadTextTable(Archive& ar, const QTextTableFormat& textTableFormat, TextDocumentContext& context)
        {
            int iRows;
            ar >> make_nvp("rows", iRows);

            int iColumns;
            ar >> make_nvp("columns", iColumns);

            int iTextFormatIndex = 0;
            QTextTable* pTextTable = context.cursor.insertTable(iRows, iColumns, textTableFormat);
            for (int iRow = 0; iRow < iRows; ++iRow)
            {
                for (int iColumn = 0; iColumn < iColumns; ++iColumn)
                {
                    QTextTableCell cell = pTextTable->cellAt(iRow, iColumn);

                    ar >> make_nvp("text_format_index", iTextFormatIndex);
                    cell.setFormat(context.allFormats[iTextFormatIndex].toCharFormat());

                    context.cursor = cell.firstCursorPosition();
                    LoadTextFrame(ar, context);
                }
            }
        }

        template<class Archive> void SaveTextFrame(Archive& ar, QTextFrame::iterator itBegin, QTextFrame::iterator itEnd, const unsigned int fileVersion)
        {
            int iTextFormatIndex = 0;
            auto itFrame = itBegin;
            while (itFrame != itEnd)
            {
                ar << make_nvp("block_iterator", HAS_NEXT);
                QTextFrame* pChildFrame = itFrame.currentFrame();
                if (pChildFrame)
                {
                    iTextFormatIndex = pChildFrame->formatIndex();
                    ar << make_nvp("text_format_index", iTextFormatIndex);
                    if (pChildFrame->format().isTableFormat())
                    {
                        SaveTextTable(ar, qobject_cast<QTextTable*>(pChildFrame), fileVersion);
                    }
                    else
                    {
                        SaveTextFrame(ar, pChildFrame->begin(), pChildFrame->end(), fileVersion);
                    }
                }
                else
                {
                    QTextBlock childBlock = itFrame.currentBlock();
                    iTextFormatIndex = childBlock.blockFormatIndex();
                    ar << make_nvp("text_format_index", iTextFormatIndex);
                    iTextFormatIndex = childBlock.charFormatIndex();
                    ar << make_nvp("char_format_index", iTextFormatIndex);
                    SaveTextFragments(ar, childBlock.begin(), childBlock.end(), fileVersion);

                }
                itFrame++;
            }
            ar << make_nvp("block_iterator", NO_NEXT);
        }

        template<class Archive> void LoadTextFrame(Archive& ar, TextDocumentContext& context)
        {
            int iTextFormatIndex = 0;
            bool bHasNext;
            bool bBlockInsertionAllowed = false;
            ar >> make_nvp("block_iterator", bHasNext);
            while (bHasNext)
            {
                QTextFormat textFormat;
                ar >> make_nvp("text_format_index", iTextFormatIndex);
                textFormat = context.allFormats[iTextFormatIndex];

                switch (textFormat.type())
                {
                case QTextFormat::FrameFormat:
                    {
                        if (textFormat.isTableFormat())
                        {
                            LoadTextTable(ar, textFormat.toTableFormat(), context);
                        }
                        else
                        {
                            context.cursor.insertFrame(textFormat.toFrameFormat());
                            LoadTextFrame(ar, context);
                        }

                        // Inserting a frame/table creates automatically a block in the frame and after the frame
                        context.cursor.movePosition(QTextCursor::NextBlock);
                        bBlockInsertionAllowed = false; // No need to create a new block after the frame
                        break;
                    }
                case QTextFormat::BlockFormat:
                    {
                        QTextFormat textCharFormat;
                        ar >> make_nvp("char_format_index", iTextFormatIndex);
                        textCharFormat = context.allFormats[iTextFormatIndex];

                        if (bBlockInsertionAllowed)
                        {
                            context.cursor.insertBlock(textFormat.toBlockFormat(), textCharFormat.toCharFormat());
                        }
                        else
                        {
                            if (!textFormat.isEmpty())
                            {
                                context.cursor.setBlockFormat(textFormat.toBlockFormat());
                            }

                            if (!textCharFormat.isEmpty())
                            {
                                context.cursor.setBlockCharFormat(textCharFormat.toCharFormat());
                            }
                            bBlockInsertionAllowed = true;
                        }

                        LoadTextFragments(ar, context);
                        break;
                    }
                }

                ar >> make_nvp("block_iterator", bHasNext);
            }
        }

        template<class Archive> void save(Archive& ar, const QTextDocument& obj,  const unsigned int fileVersion)
        {
            // Save all text formats first
            const auto& allFormats = obj.allFormats();
            ar << make_nvp("all_formats", allFormats);

            SaveTextFrame(ar, obj.rootFrame()->begin(), obj.rootFrame()->end(), fileVersion);
        }

        template<class Archive> void load(Archive& ar, QTextDocument& obj,  const unsigned int fileVersion)
        {
            // Load all text formats first
            TextDocumentContext context(fileVersion);
            ar >> make_nvp("all_formats", context.allFormats);

            // Load content
            context.cursor = QTextCursor(&obj);
            const bool bPreviousState = obj.isUndoRedoEnabled();
            obj.setUndoRedoEnabled(false);
            context.cursor.beginEditBlock();
            obj.clear();

            try
            {
                LoadTextFrame(ar, context);
            }
            catch (...)
            {
                context.cursor.endEditBlock();
                obj.setUndoRedoEnabled(bPreviousState);
                throw;
            }

            context.cursor.endEditBlock();
            obj.setUndoRedoEnabled(bPreviousState);
        }
    }
}
