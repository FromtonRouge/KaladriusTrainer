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

#pragma once

#include <QtCore/QIdentityProxyModel>

class QUndoStack;
class UndoableProxyModel : public QIdentityProxyModel
{
    Q_OBJECT

public:
    UndoableProxyModel(QObject* pParent = nullptr);
    ~UndoableProxyModel();

    void setUndoStack(QUndoStack* pUndoStack) {_pUndoStack = pUndoStack;}
    QUndoStack* getUndoStack() const {return _pUndoStack;}
    virtual bool setData(const QModelIndex& index, const QVariant& value, int iRole) override;
    virtual bool removeRows(int iRow, int iCount, const QModelIndex& parent) override;
    QModelIndex insertBranch(int iRow, const QModelIndex& parent, const QByteArray& branch);

private:
    QUndoStack* _pUndoStack;
};
