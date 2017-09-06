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

#include "DictionariesModel.h"
#include "TheoryModel.h"

DictionariesModel::DictionariesModel(QObject* pParent)
    : QIdentityProxyModel(pParent)
{
}

DictionariesModel::~DictionariesModel()
{

}

void DictionariesModel::setSourceModel(QAbstractItemModel* pSourceModel)
{
    // Keep the dictionaries index
    _sourceIndexDictionaries = QModelIndex();
    auto pTheoryModel = qobject_cast<TheoryModel*>(pSourceModel);
    if (pTheoryModel)
    {
        _sourceIndexDictionaries = pTheoryModel->getDictionariesIndex();
    }

    QIdentityProxyModel::setSourceModel(pSourceModel);
}

bool DictionariesModel::hasChildren(const QModelIndex& parent) const
{
    auto pSourceModel = sourceModel();
    if (pSourceModel)
    {
        if (parent.isValid())
        {
            if (parent.data(TreeItemTypeRole).toInt() == TreeItem::Dictionary)
            {
                const QModelIndex& sourceIndexDictionary = mapToSource(parent);
                const QModelIndex& sourceIndexEntries = sourceIndexDictionary.child(0,0);
                return pSourceModel->hasChildren(sourceIndexEntries);
            }
        }
        else
        {
            return pSourceModel->hasChildren(_sourceIndexDictionaries);
        }
    }
    return false;
}

int DictionariesModel::rowCount(const QModelIndex& parent) const
{
    auto pSourceModel = sourceModel();
    if (pSourceModel)
    {
        if (parent.isValid())
        {
            if (parent.data(TreeItemTypeRole).toInt() == TreeItem::Dictionary)
            {
                const QModelIndex& sourceIndexDictionary = mapToSource(parent);
                const QModelIndex& sourceIndexEntries = sourceIndexDictionary.child(0,0);
                return pSourceModel->rowCount(sourceIndexEntries);
            }
        }
        else
        {
            return pSourceModel->rowCount(_sourceIndexDictionaries);
        }
    }
    return 0;
}

QModelIndex DictionariesModel::index(int iRow, int iColumn, const QModelIndex& parent) const
{
    auto pSourceModel = sourceModel();
    if (pSourceModel)
    {
        if (parent.isValid())
        {
            if (parent.data(TreeItemTypeRole).toInt() == TreeItem::Dictionary)
            {
                const QModelIndex& sourceIndexDictionary = mapToSource(parent);
                const QModelIndex& sourceIndexEntries = sourceIndexDictionary.child(0,0);
                return pSourceModel->index(iRow, iColumn, sourceIndexEntries);
            }
        }
        else
        {
            return pSourceModel->index(iRow, iColumn, _sourceIndexDictionaries);
        }
    }
    return QModelIndex();
}
