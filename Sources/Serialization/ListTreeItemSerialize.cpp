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

#include "ListTreeItemSerialize.h"
#include "../Values/Types/ListValue.h"
#include "TreeItemSerialize.h"
#include "Qt/QStandardItem.h"
#include "../Tree/Models/ItemDataRole.h"
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/base_object.hpp>
#include "ExplicitInstanciation.h"

EXPLICIT_INSTANCIATION(ListTreeItem)
BOOST_CLASS_EXPORT_IMPLEMENT(ListTreeItem)

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
