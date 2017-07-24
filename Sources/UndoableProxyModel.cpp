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

#include "UndoableProxyModel.h"
#include "SetDataCommand.h"

UndoableProxyModel::UndoableProxyModel(QObject* pParent)
    : QIdentityProxyModel(pParent)
    , _pUndoStack(nullptr)
{

}

UndoableProxyModel::~UndoableProxyModel()
{

}

bool UndoableProxyModel::setData(const QModelIndex& index, const QVariant& value, int iRole)
{
    if (_pUndoStack)
    {
        _pUndoStack->push(new SetDataCommand(index, value, iRole));
        return true;
    }

    return QIdentityProxyModel::setData(index, value, iRole);
}
