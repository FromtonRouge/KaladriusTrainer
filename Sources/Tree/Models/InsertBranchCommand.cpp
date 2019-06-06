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

#include "InsertBranchCommand.h"
#include <QtCore/QMimeData>
#include <QtCore/QAbstractItemModel>

InsertBranchCommand::InsertBranchCommand(int iRow,
                                         const QModelIndex& parent,
                                         const QByteArray& branchData,
                                         QAbstractItemModel* pModel,
                                         QUndoCommand* pParent)
    : QUndoCommand(pParent)
    , _iRow(iRow)
    , _parent(parent)
    , _branchData(branchData)
    , _pModel(pModel)
{
    setText(QObject::tr("1 branch inserted"));
}

InsertBranchCommand::~InsertBranchCommand()
{

}

void InsertBranchCommand::redo()
{
    auto pMimeData = new QMimeData();
    pMimeData->setData("application/x.treeitemmodel.serialized-branch", _branchData);
    _pModel->dropMimeData(pMimeData, Qt::CopyAction, _iRow, 0, _parent);
}

void InsertBranchCommand::undo()
{
    _pModel->removeRow(_iRow, _parent);
}
