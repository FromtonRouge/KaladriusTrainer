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

#include "TheoryTreeView.h"
#include "UndoableTheoryModel.h"
#include "TreeItems/TreeItem.h"
#include "TreeItems/ArrayTreeItem.h"
#include "TreeItems/AttributeValueTreeItem.h"
#include "Models/ItemDataRole.h"
#include "Serialization/Serialize.h"
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>
#include <QtGui/QContextMenuEvent>

TheoryTreeView::TheoryTreeView(QWidget* pParent)
    : QTreeView(pParent)
{
    setAlternatingRowColors(true);
    setSelectionMode(ExtendedSelection);
    setEditTriggers(AllEditTriggers);
    header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    _pActionRemove = new QAction(QIcon(":/Icons/cross.png"), tr("Remove"), this);
    _pActionRemove->setShortcut(QKeySequence::Delete);
    connect(_pActionRemove, SIGNAL(triggered()), this, SLOT(onRemove()));
    addAction(_pActionRemove);
    _pActionRemove->setDisabled(true);

    _pActionAdd = new QAction(QIcon(":/Icons/plus.png"), tr("Add"), this);
    connect(_pActionAdd, SIGNAL(triggered()), this, SLOT(onAdd()));
    addAction(_pActionAdd);
    _pActionAdd->setDisabled(true);
}

TheoryTreeView::~TheoryTreeView()
{

}

void TheoryTreeView::setModel(QAbstractItemModel* pModel)
{
    if (model())
    {
        model()->disconnect(this);
    }

    QTreeView::setModel(pModel);

    if (pModel)
    {
        connect(pModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(onRowsInserted(QModelIndex,int,int)));
        expandToDepth(1);
    }
}

void TheoryTreeView::onRowsInserted(const QModelIndex& parent, int iFirst, int iLast)
{
}

void TheoryTreeView::onRemove()
{
    const QModelIndex& current = currentIndex();
    if (current.isValid())
    {
        const QModelIndex& currentName = current.sibling(current.row(), 0);
        auto pUndoableTheoryModel = qobject_cast<UndoableTheoryModel*>(model());
        pUndoableTheoryModel->removeRow(currentName.row(), currentName.parent());
    }
}

void TheoryTreeView::onAdd()
{
    const QModelIndex& current = currentIndex();
    if (current.isValid())
    {
        auto pUndoableTheoryModel = qobject_cast<UndoableTheoryModel*>(model());
        auto pNewElement = new ArrayElementTreeItem();
        auto pNewElementValue = new AttributeValueTreeItem(QString());
        const QByteArray& branch = Serialization::Save({pNewElement, pNewElementValue});
        Q_ASSERT(!branch.isEmpty());
        const int iInsertRow = pUndoableTheoryModel->rowCount(current);
        const QModelIndex& currentName = current.sibling(current.row(), 0);
        const QModelIndex& indexBranch = pUndoableTheoryModel->insertBranch(iInsertRow, currentName, branch);
        Q_ASSERT(indexBranch.isValid());
        setCurrentIndex(indexBranch);
    }
}

void TheoryTreeView::contextMenuEvent(QContextMenuEvent* pEvent)
{
    QMenu menu;

    for (QAction* pAction : actions())
    {
        if (pAction->isEnabled())
        {
            menu.addAction(pAction);
        }
    }

    if (!menu.isEmpty())
    {
        menu.exec(pEvent->globalPos());
    }
}

void TheoryTreeView::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
    QTreeView::currentChanged(current, previous);

    if (current.isValid())
    {
        const int iType = current.data(TreeItemTypeRole).toInt();
        switch (iType)
        {
        case TreeItem::Array:
            {
                _pActionRemove->setEnabled(false);
                _pActionAdd->setEnabled(true);
                break;
            }
        case TreeItem::ArrayElement:
            {
                _pActionRemove->setEnabled(true);
                _pActionAdd->setEnabled(false);
                break;
            }
        default:
            {
                _pActionRemove->setEnabled(false);
                _pActionAdd->setEnabled(false);
                break;
            }
        }
    }
}
