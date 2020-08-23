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

#include "../../Tree/Models/Utils.h"
#include "../../Tree/Models/ItemDataRole.h"

namespace Utils
{
    QModelIndex index(const QAbstractItemModel* pModel,
                      const QString& sPath,
                      int iColumn,
                      const QModelIndex& parent)
    {
        // Split the string path
        QStringList path = sPath.split('/');
        if (path.isEmpty())
        {
            return QModelIndex();
        }

        // Parse the path and find indexes
        QModelIndex currentParent = parent;
        while (!path.isEmpty())
        {
            // Check for at least one child
            if (!pModel->hasChildren(currentParent))
            {
                return QModelIndex(); // No full match for the given path
            }

            const QString& sItem = path.front();
            const QModelIndex& start = pModel->index(0, 0, currentParent);
            const auto& matches = pModel->match(start, Qt::DisplayRole, sItem, 1, Qt::MatchExactly);
            if (matches.isEmpty())
            {
                return QModelIndex(); // No full match for the given path
            }
            currentParent = matches.front();
            path.pop_front();
        }

        return currentParent.sibling(currentParent.row(), iColumn);
    }

    QModelIndex findParent(const QModelIndex& index, int iTreeItemType)
    {
        if (index.isValid())
        {
            QModelIndex currentParent = index.parent();
            while (currentParent.isValid())
            {
                if (currentParent.data(TreeItemTypeRole).toInt() == iTreeItemType)
                {
                    return currentParent;
                }
                currentParent = currentParent.parent();
            }
        }
        return QModelIndex();
    }
}
