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

#include "KeyboardTreeView.h"
#include "Main/Application.h"
#include "Models/UndoableProxyModel.h"
#include "Theories/TheoryModel.h"
#include "KeyboardModel.h"
#include "Keycaps/KeycapDelegate.h"
#include "KeyboardGraphicsScene.h"
#include "Keycaps/KeycapGraphicsItem.h"
#include "Serialization/Serialize.h"
#include "TreeItems/TreeItem.h"
#include "TreeItems/DictionaryTreeItem.h"
#include "TreeItems/LinkedTheoryTreeItem.h"
#include "TreeItems/LinkedDictionaryTreeItem.h"
#include "TreeItems/ListTreeItem.h"
#include "TreeItems/ArrayTreeItem.h"
#include "ValueTypes/KeycapRef.h"
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>
#include <QtGui/QContextMenuEvent>
#include <QtCore/QItemSelectionModel>

KeyboardTreeView::KeyboardTreeView(QWidget* pParent)
    : QTreeView(pParent)
{
    setAcceptDrops(true);
    setAlternatingRowColors(true);
    setSelectionMode(ExtendedSelection);
    setEditTriggers(AllEditTriggers);
    header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    delete itemDelegate();
    setItemDelegate(new KeycapDelegate(this));

    _pActionLinkTheory = new QAction(QIcon(":/Icons/chain.png"), tr("Link Theory"), this);
    connect(_pActionLinkTheory, SIGNAL(triggered()), this, SLOT(onLinkTheory()));
    addAction(_pActionLinkTheory);

    _pActionRemove = new QAction(QIcon(":/Icons/cross.png"), tr("Remove"), this);
    _pActionRemove->setShortcut(QKeySequence::Delete);
    connect(_pActionRemove, SIGNAL(triggered()), this, SLOT(onRemove()));
    addAction(_pActionRemove);
    _pActionRemove->setDisabled(true);
}

KeyboardTreeView::~KeyboardTreeView()
{
}

void KeyboardTreeView::setModel(QAbstractItemModel* pModel)
{
    if (model())
    {
        model()->disconnect(this);
    }

    QTreeView::setModel(pModel);

    if (pModel)
    {
        connect(pModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(onRowsInserted(QModelIndex,int,int)));
    }
}

void KeyboardTreeView::onGraphicsSceneSelectionChanged()
{
    auto pGraphicsScene = qobject_cast<KeyboardGraphicsScene*>(sender());
    if (pGraphicsScene)
    {
        QModelIndexList matches;
        const auto& selected = pGraphicsScene->selectedItems();
        if (selected.isEmpty())
        {
            selectionModel()->clearSelection();
            return;
        }

        for (auto pSelected : selected)
        {
            auto pKeycapItem = dynamic_cast<KeycapGraphicsItem*>(pSelected);
            if (pKeycapItem)
            {
                const QString& sKeycapId = pKeycapItem->elementId();
                const QModelIndex& start = model()->index(0, 0);
                matches << model()->match(start, Qt::DisplayRole, sKeycapId, 1, Qt::MatchExactly|Qt::MatchRecursive);
            }
        }

        if (!matches.isEmpty())
        {
            QItemSelection finalSelection;
            for (const auto& index : matches)
            {
                QItemSelection selection;
                selection.select(index, index);
                finalSelection.merge(selection, QItemSelectionModel::Select);
            }
            selectionModel()->select(finalSelection, QItemSelectionModel::ClearAndSelect);
        }
    }
}

void KeyboardTreeView::onRowsInserted(const QModelIndex& parent, int iFirst, int iLast)
{
    auto pUndoableProxyModel = qobject_cast<UndoableProxyModel*>(model());
    auto pKeyboardModel = qobject_cast<KeyboardModel*>(pUndoableProxyModel->sourceModel());
    bool bResizeColumn = false;
    for (int iRow = iFirst; iRow <= iLast; ++iRow)
    {
        if (!parent.isValid())
        {
            // Search for the Keycaps index
            const auto& sourceIndexKeycaps = pKeyboardModel->getKeycapsIndex();
            if (!sourceIndexKeycaps.isValid())
            {
                return;
            }

            bResizeColumn = true;

            expand(pUndoableProxyModel->index(iRow, 0, parent));
            expand(pUndoableProxyModel->mapFromSource(sourceIndexKeycaps));
        }
    }

    if (bResizeColumn)
    {
        resizeColumnToContents(0);
    }
}

void KeyboardTreeView::onLinkTheory()
{
    // TODO: Add a child node (LinkedTheoryTreeItem) with the name of the loaded theory
    // and all dictionaries + keys bindings
    const QModelIndex& current = currentIndex();
    if (current.isValid())
    {
        const QModelIndex& currentName = current.sibling(current.row(), 0);
        auto pUndoableProxyModel = qobject_cast<UndoableProxyModel*>(model());
        const int iInsertRow = pUndoableProxyModel->rowCount(currentName);

        auto pTheoryModel = qApp->getTheoryModel();
        auto pLinkedTheory = new LinkedTheoryTreeItem(pTheoryModel->getTheoryName());

        // Add key mappings for each dictionary
        auto pLinkedDictionaries = pLinkedTheory->getLinkedDictionaries();
        const QModelIndex& indexDictionaries = pTheoryModel->getDictionariesIndex();
        const int iDictionaries = pTheoryModel->rowCount(indexDictionaries);
        for (int iDictionary = 0; iDictionary < iDictionaries; ++iDictionary)
        {
            const QModelIndex& indexDictionary = pTheoryModel->index(iDictionary, 0, indexDictionaries);
            const QString& sDictionaryName = indexDictionary.data(Qt::DisplayRole).toString();
            auto pLinkedDictionary = new LinkedDictionaryTreeItem(sDictionaryName);

            // Add empty keys bindings
            auto pLinkedKeys = pLinkedDictionary->getLinkedKeys();
            auto pDictionary = static_cast<DictionaryTreeItem*>(pTheoryModel->itemFromIndex(indexDictionary));
            auto pKeys = pDictionary->getKeys();
            const int iKeys = pKeys->rowCount();
            for (int iKey = 0; iKey < iKeys; ++iKey)
            {
                pLinkedKeys->addElement(qVariantFromValue(KeycapRef()));
            }

            pLinkedDictionaries->appendRow({pLinkedDictionary, new EmptyTreeItem()});
        }

        const QByteArray& branch = Serialization::Save({pLinkedTheory});
        const QModelIndex& indexBranch = pUndoableProxyModel->insertBranch(iInsertRow, currentName, branch);
        Q_ASSERT(indexBranch.isValid());
        setCurrentIndex(indexBranch);
        expand(indexBranch);
    }
}

void KeyboardTreeView::onRemove()
{
    const QModelIndex& current = currentIndex();
    if (current.isValid())
    {
        const QModelIndex& currentName = current.sibling(current.row(), 0);
        auto pUndoableProxyModel = qobject_cast<UndoableProxyModel*>(model());
        pUndoableProxyModel->removeRow(currentName.row(), currentName.parent());
    }
}

void KeyboardTreeView::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
    QTreeView::currentChanged(current, previous);

    if (current.isValid())
    {
        const int iType = current.data(TreeItemTypeRole).toInt();
        switch (iType)
        {
        case TreeItem::LinkedTheories:
            {
                _pActionRemove->setEnabled(false);
                _pActionLinkTheory->setEnabled(true);
                break;
            }
        case TreeItem::LinkedTheory:
            {
                _pActionRemove->setEnabled(true);
                break;
            }
        default:
            {
                _pActionRemove->setEnabled(false);
                _pActionLinkTheory->setEnabled(false);
                break;
            }
        }
    }
}

void KeyboardTreeView::contextMenuEvent(QContextMenuEvent* pEvent)
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
