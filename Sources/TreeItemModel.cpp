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
#include "Serialization.h"
#include <QtCore/QMimeData>

TreeItemModel::TreeItemModel(QObject* pParent)
    : QStandardItemModel(pParent)
{

}

TreeItemModel::~TreeItemModel()
{

}

QVariant TreeItemModel::data(const QModelIndex& index, int iRole) const
{
    QVariant result;
    switch (iRole)
    {
    case Qt::DisplayRole:
        {
            const int iType = itemFromIndex(index)->type();
            if (iType == TreeItem::ArrayElement)
            {
                result = QString("[%1]").arg(index.row());
            }
            else
            {
                result = QStandardItemModel::data(index, iRole);
            }
            break;
        }
    case TreeItemTypeRole:
        {
            result = itemFromIndex(index)->type();
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
