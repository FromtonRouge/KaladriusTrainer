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

#include <QtWidgets/QTreeView>

class QAction;
class TheoryTreeView : public QTreeView
{
    Q_OBJECT

public:
    TheoryTreeView(QWidget* pParent = nullptr);
    ~TheoryTreeView();

    virtual void setModel(QAbstractItemModel* pModel) override;

private slots:
    void onRowsInserted(const QModelIndex& parent, int iFirst, int iLast);
    void onRemove();
    void onAdd();

protected:
    virtual void contextMenuEvent(QContextMenuEvent* pEvent) override;
    virtual void currentChanged(const QModelIndex& current, const QModelIndex& previous) override;

private:
    QAction* _pActionRemove;
    QAction* _pActionAdd;
};
