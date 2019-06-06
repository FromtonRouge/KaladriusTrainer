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

#include "TheoryTreeView.h"
#include "Theories/Models/UndoableTheoryModel.h"
#include "Tree/TreeItems/TreeItem.h"
#include "Tree/TreeItems/ListTreeItem.h"
#include "Tree/TreeItems/ValueTreeItem.h"
#include "Values/Types/ListValue.h"
#include "Keycaps/KeycapDelegate.h"
#include "Tree/Models/ItemDataRole.h"
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

    delete itemDelegate();
    setItemDelegate(new KeycapDelegate(this));

    _pActionAdd = new QAction(QIcon(":/Icons/plus.png"), tr("Add"), this);
    connect(_pActionAdd, SIGNAL(triggered()), this, SLOT(onAdd()));
    _pActionAdd->setShortcut(QKeySequence::New);
    addAction(_pActionAdd);
    _pActionAdd->setDisabled(true);

    _pActionRemove = new QAction(QIcon(":/Icons/cross.png"), tr("Remove"), this);
    _pActionRemove->setShortcut(QKeySequence::Delete);
    connect(_pActionRemove, SIGNAL(triggered()), this, SLOT(onRemove()));
    addAction(_pActionRemove);
    _pActionRemove->setDisabled(true);
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
        const QModelIndex& currentName = current.sibling(current.row(), 0);
        const QVariant& variant = currentName.data(ListValueRole);
        if (variant.isValid())
        {
            const auto& listValue = qvariant_cast<ListValue>(variant);
            auto pUndoableTheoryModel = qobject_cast<UndoableTheoryModel*>(model());
            auto pNewElement = new ListTreeItem();
            auto pNewElementValue = new ValueTreeItem(listValue.defaultValue);
            const QByteArray& branch = Serialization::Save({pNewElement, pNewElementValue});
            delete pNewElement;
            delete pNewElementValue;
            Q_ASSERT(!branch.isEmpty());
            const int iInsertRow = pUndoableTheoryModel->rowCount(current);
            const QModelIndex& indexBranch = pUndoableTheoryModel->insertBranch(iInsertRow, currentName, branch);
            Q_ASSERT(indexBranch.isValid());
            setCurrentIndex(indexBranch);
        }
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
        switch (current.data(TreeItemTypeRole).toInt())
        {
        case TreeItem::List:
            {
                _pActionRemove->setEnabled(current.parent().data(ListValueRole).isValid());
                _pActionAdd->setEnabled(current.data(ListValueRole).isValid());
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
