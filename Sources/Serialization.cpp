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
#include <QtGui/QFont>
#include <QtGui/QColor>
#include <QtCore/QPointF>
#include <QtCore/QRectF>
#include <QtCore/QVariant>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/string.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <fstream>
#include <iostream>
#include <string>

// QPointF
BOOST_CLASS_VERSION(QPointF, 0)
namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, QPointF& obj,  const unsigned int fileVersion)
        {
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const QPointF& obj,  const unsigned int fileVersion)
        {
            qreal d = obj.x();
            ar << make_nvp("x", d);

            d = obj.y();
            ar << make_nvp("y", d);
        }

        template<class Archive> void load(Archive& ar, QPointF& obj,  const unsigned int fileVersion)
        {
            qreal d;
            ar >> make_nvp("x", d);
            obj.setX(d);

            ar >> make_nvp("y", d);
            obj.setY(d);
        }
    }
}

// QRectF
BOOST_CLASS_VERSION(QRectF, 0)
namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, QRectF& obj,  const unsigned int fileVersion)
        {
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const QRectF& obj,  const unsigned int fileVersion)
        {
            qreal d = obj.x();
            ar << make_nvp("x", d);

            d = obj.y();
            ar << make_nvp("y", d);

            d = obj.width();
            ar << make_nvp("w", d);

            d = obj.height();
            ar << make_nvp("h", d);
        }

        template<class Archive> void load(Archive& ar, QRectF& obj,  const unsigned int fileVersion)
        {
            qreal d;
            ar >> make_nvp("x", d);
            obj.setX(d);

            ar >> make_nvp("y", d);
            obj.setY(d);

            ar >> make_nvp("w", d);
            obj.setWidth(d);

            ar >> make_nvp("h", d);
            obj.setHeight(d);
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
            int i = obj.userType();
            ar << make_nvp("user_type", i);
            switch (i)
            {
            case QVariant::Invalid:
                {
                    break;
                }
            case QVariant::Int:
                {
                    const auto& value = obj.toInt();
                    ar << make_nvp("value", value);
                    break;
                }
            case QVariant::Double:
                {
                    const auto& value = obj.toDouble();
                    ar << make_nvp("value", value);
                    break;
                }
            case QVariant::String:
                {
                    const auto& value = obj.toString().toStdString();
                    ar << make_nvp("value", value);
                    break;
                }
            case QVariant::PointF:
                {
                    const auto& value = obj.toPointF();
                    ar << make_nvp("value", value);
                    break;
                }
            case QVariant::RectF:
                {
                    const auto& value = obj.toRectF();
                    ar << make_nvp("value", value);
                    break;
                }
            case QVariant::Font:
            case QVariant::Color:
                {
                    const auto& value = obj.toString().toStdString();
                    ar << make_nvp("value", value);
                    break;
                }
            default:
                {
                    const QString sMsg = QString("Can't save QVariant type %1").arg(QVariant::typeToName(i));
                    std::cerr << sMsg.toStdString() << std::endl;
                    break;
                }
            }
        }

        template<class Archive> void load(Archive& ar, QVariant& obj,  const unsigned int fileVersion)
        {
            int i = 0;
            ar >> make_nvp("user_type", i);
            obj = QVariant(QVariant::Type(i));

            switch (i)
            {
            case QVariant::Invalid:
                {
                    break;
                }
            case QVariant::Int:
                {
                    int value;
                    ar >> make_nvp("value", value);
                    obj.setValue(value);
                    break;
                }
            case QVariant::Double:
                {
                    double value;
                    ar >> make_nvp("value", value);
                    obj.setValue(value);
                    break;
                }
            case QVariant::String:
                {
                    std::string value;
                    ar >> make_nvp("value", value);
                    obj.setValue(QString::fromStdString(value));
                    break;
                }
            case QVariant::PointF:
                {
                    QPointF value;
                    ar >> make_nvp("value", value);
                    obj.setValue(value);
                    break;
                }
            case QVariant::RectF:
                {
                    QRectF value;
                    ar >> make_nvp("value", value);
                    obj.setValue(value);
                    break;
                }
            case QVariant::Font:
                {
                    std::string value;
                    ar >> make_nvp("value", value);
                    QFont font;
                    font.fromString(QString::fromStdString(value));
                    obj.setValue(font);
                    break;
                }
            case QVariant::Color:
                {
                    std::string value;
                    ar >> make_nvp("value", value);
                    QColor color(QString::fromStdString(value));
                    obj.setValue(color);
                    break;
                }
            default:
                {
                    const QString sMsg = QString("Can't load QVariant type %1").arg(QVariant::typeToName(i));
                    std::cerr << sMsg.toStdString() << std::endl;
                    break;
                }
            }
        }
    }
}

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
            std::string s = obj.text().toStdString();
            ar << make_nvp("text", s);

            s = obj.toolTip().toStdString();
            ar << make_nvp("tooltip", s);

            bool b = obj.isSelectable();
            ar << make_nvp("selectable", b);

            b = obj.isEditable();
            ar << make_nvp("editable", b);

            QVariant v;
            if (b)
            {
                v = obj.data(Qt::EditRole);
                ar << make_nvp("role_edit", v);
            }

            v = obj.data(KeyboardPropertiesModel::PropertyTypeRole);
            ar << make_nvp("role_property_type", v);

            v = obj.data(KeyboardPropertiesModel::RotationAngleRole);
            ar << make_nvp("role_rotation_angle", v);

            v = obj.data(KeyboardPropertiesModel::RotationOriginRole);
            ar << make_nvp("role_rotation_origin", v);

            v = obj.data(KeyboardPropertiesModel::OuterBorderRole);
            ar << make_nvp("role_outer_border", v);

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
            std::string sText;
            ar >> make_nvp("text", sText);
            obj.setText(QString::fromStdString(sText));

            ar >> make_nvp("tooltip", sText);
            obj.setToolTip(QString::fromStdString(sText));

            bool b = false;
            ar >> make_nvp("selectable", b);
            obj.setSelectable(b);

            ar >> make_nvp("editable", b);
            obj.setEditable(b);

            QVariant v;
            if (b)
            {
                ar >> make_nvp("role_edit", v);
                obj.setData(v, Qt::EditRole);
            }

            ar >> make_nvp("role_property_type", v);
            obj.setData(v, KeyboardPropertiesModel::PropertyTypeRole);

            ar >> make_nvp("role_rotation_angle", v);
            obj.setData(v, KeyboardPropertiesModel::RotationAngleRole);

            ar >> make_nvp("role_rotation_origin", v);
            obj.setData(v, KeyboardPropertiesModel::RotationOriginRole);

            ar >> make_nvp("role_outer_border", v);
            obj.setData(v, KeyboardPropertiesModel::OuterBorderRole);

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
