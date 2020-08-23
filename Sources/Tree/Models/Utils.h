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

#pragma once

#include <QtCore/QModelIndex>
#include <QtCore/QString>

class QAbstractItemModel;
namespace Utils
{
    /**
     * @brief Get index from path
     * @param sPath Relative path in a tree model eg: "Root/Child1/Child2"
     */
    QModelIndex index(const QAbstractItemModel* pModel,
                      const QString& sPath,
                      int iColumn = 0,
                      const QModelIndex& parent = QModelIndex());

    /**
     * @brief Find the first parent with type iTreeItemType
     * @param iTreeItemType Type as defined in TreeItem.h
     * @return valid index of the parent if found
     */
    QModelIndex findParent(const QModelIndex& index, int iTreeItemType);
}
