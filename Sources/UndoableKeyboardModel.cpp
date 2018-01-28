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

#include "UndoableKeyboardModel.h"
#include "ValueTypes/KeycapRef.h"
#include <QtCore/QMimeData>
#include <QtCore/QByteArray>
#include <QtCore/QDataStream>

UndoableKeyboardModel::UndoableKeyboardModel(QObject* pParent)
    : UndoableProxyModel(pParent)
{

}

bool UndoableKeyboardModel::dropMimeData(const QMimeData* pMimeData, Qt::DropAction action, int iRow, int iColumn, const QModelIndex& parent)
{
    if (pMimeData->hasFormat("application/prs.stenotutor.keycapid") && parent.isValid())
    {
        QByteArray data = pMimeData->data("application/prs.stenotutor.keycapid");
        QDataStream stream(&data, QIODevice::ReadOnly);
        QString sKeycapId;
        stream >> sKeycapId;
        setData(parent, qVariantFromValue(KeycapRef(sKeycapId)));
    }
    return UndoableProxyModel::dropMimeData(pMimeData, action, iRow, iColumn, parent);
}
