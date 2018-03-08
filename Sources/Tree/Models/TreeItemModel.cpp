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

#include "TreeItemModel.h"
#include "Serialization/Serialize.h"
#include "Values/Types/ListValue.h"
#include <QtCore/QMimeData>

TreeItemModel::TreeItemModel(QObject* pParent)
    : QStandardItemModel(pParent)
{

}

TreeItemModel::~TreeItemModel()
{

}

Qt::ItemFlags TreeItemModel::flags(const QModelIndex& index) const
{
    auto result = QStandardItemModel::flags(index);
    if (index.isValid())
    {
        const int iType = index.data(TreeItemTypeRole).toInt();
        if (iType == TreeItem::List && index.column() == 0)
        {
            const QModelIndex& parent = index.parent();
            const QModelIndex& parentValue = parent.sibling(parent.row(), 1);
            if (parentValue.data(TreeItemTypeRole).toInt() == TreeItem::Value)
            {
                const QVariant& variant = parentValue.data(Qt::EditRole);
                if (variant.isValid() && variant.userType() == qMetaTypeId<ListValue>())
                {
                    const auto& listValue = qvariant_cast<ListValue>(variant);
                    switch (listValue.namingPolicy)
                    {
                    case ListValue::NameIsEditable:
                        {
                            result.setFlag(Qt::ItemIsEditable, true);
                            break;
                        }
                    case ListValue::NameIsNotEditable:
                    case ListValue::NameIsAutoIndexed:
                    default:
                        {
                            result.setFlag(Qt::ItemIsEditable, false);
                            break;
                        }
                    }
                }
            }
        }
    }

    return result;
}

QVariant TreeItemModel::data(const QModelIndex& index, int iRole) const
{
    QVariant result;
    switch (iRole)
    {
    case Qt::DisplayRole:
        {
            bool bAutoIndexing = false;
            const int iType = itemFromIndex(index)->type();
            if (iType == TreeItem::List)
            {
                const QModelIndex& parent = index.parent();
                const QModelIndex& parentValue = parent.sibling(parent.row(), 1);
                if (parentValue.data(TreeItemTypeRole).toInt() == TreeItem::Value)
                {
                    const QVariant& variant = parentValue.data(Qt::EditRole);
                    if (variant.isValid() && variant.userType() == qMetaTypeId<ListValue>())
                    {
                        bAutoIndexing = qvariant_cast<ListValue>(variant).namingPolicy == ListValue::NameIsAutoIndexed;
                    }
                }
            }

            if (bAutoIndexing)
            {
                result = QString("[%1]").arg(index.row());
            }
            else
            {
                result = QStandardItemModel::data(index, iRole);
            }
            break;
        }
    case Qt::DecorationRole:
        {
            QString sIconResource;
            const int iType = itemFromIndex(index)->type();
            if (iType == TreeItem::List && index.column() == 0)
            {
                const QModelIndex& parent = index.parent();
                const QModelIndex& parentValue = parent.sibling(parent.row(), 1);
                if (parentValue.data(TreeItemTypeRole).toInt() == TreeItem::Value)
                {
                    const QVariant& variant = parentValue.data(Qt::EditRole);
                    if (variant.isValid() && variant.userType() == qMetaTypeId<ListValue>())
                    {
                        sIconResource = qvariant_cast<ListValue>(variant).sDefaultIconResource;
                    }
                }
            }

            if (!sIconResource.isEmpty())
            {
                result = QIcon(sIconResource);
            }
            else
            {
                result = QStandardItemModel::data(index, iRole);
            }
            break;
        }
    case ListValueRole:
        {
            if (index.data(TreeItemTypeRole).toInt() == TreeItem::List)
            {
                const QModelIndex& indexValue = index.sibling(index.row(), 1);
                if (indexValue.isValid() && indexValue.data(TreeItemTypeRole).toInt() == TreeItem::Value)
                {
                    const QVariant& variant = indexValue.data(Qt::EditRole);
                    if (variant.isValid() && variant.userType() == qMetaTypeId<ListValue>())
                    {
                        result = variant;
                    }
                }
            }
            break;
        }
    case TreeItemTypeRole:
        {
            result = itemFromIndex(index)->type();
            break;
        }
    case TreeItemIndexRole:
        {
            result = index;
            break;
        }
    case BranchRole:
        {
            QList<QStandardItem*> items;
            const int iColumns = columnCount();
            for (int iColumn = 0; iColumn < iColumns; ++iColumn)
            {
                auto pItem = itemFromIndex(index.sibling(index.row(), iColumn));
                if (pItem)
                {
                    items << pItem;
                }
            }

            if (!items.isEmpty())
            {
                result = Serialization::Save(items);
            }
            break;
        }
    default:
        {
            result = QStandardItemModel::data(index, iRole);
            break;
        }
    }
    return result;
}

bool TreeItemModel::dropMimeData(const QMimeData* pMimeData, Qt::DropAction action, int iRow, int iColumn, const QModelIndex& parent)
{
    static const char* szMimeType = "application/x.treeitemmodel.serialized-branch";
    if (pMimeData->hasFormat(szMimeType))
    {
        auto pParentItem = itemFromIndex(parent);
        if (!pParentItem)
        {
            return false;
        }

        auto branchData = pMimeData->data(szMimeType);
        const auto& items = Serialization::Load(branchData);
        if (items.isEmpty())
        {
            return false;
        }

        pParentItem->insertRow(iRow, items);
        return true;
    }
    return QStandardItemModel::dropMimeData(pMimeData, action, iRow, iColumn, parent);
}
