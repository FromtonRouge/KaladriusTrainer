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

#include "SetDataCommand.h"
#include "UndoableProxyModel.h"

SetDataCommand::SetDataCommand( const QModelIndex& index,
                                const QVariant& value,
                                int iRole,
                                QUndoCommand* pParent)
    : QUndoCommand(pParent)
    , _index(index)
    , _oldValue(index.data(iRole))
    , _newValue(value)
    , _iRole(iRole)
{
    const QModelIndex& indexName = index.sibling(index.row(), 0);
    const QString& sName = indexName.data().toString();
    const QString& sValue = value.toString();
    if (sValue.isEmpty())
    {
        setText(QObject::tr("\"%1\" changed").arg(sName));
    }
    else
    {
        setText(QObject::tr("\"%1\" changed to %2").arg(sName).arg(sValue));
    }
}

SetDataCommand::~SetDataCommand()
{
}

void SetDataCommand::redo()
{
    if (_index.isValid())
    {
        auto pUndoableProxyModel = qobject_cast<UndoableProxyModel*>(const_cast<QAbstractItemModel*>(_index.model()));
        pUndoableProxyModel->QIdentityProxyModel::setData(_index, _newValue, _iRole);
    }
}

void SetDataCommand::undo()
{
    if (_index.isValid())
    {
        auto pUndoableProxyModel = qobject_cast<UndoableProxyModel*>(const_cast<QAbstractItemModel*>(_index.model()));
        pUndoableProxyModel->QIdentityProxyModel::setData(_index, _oldValue, _iRole);
    }
}
