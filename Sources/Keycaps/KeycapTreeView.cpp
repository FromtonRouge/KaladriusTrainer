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

#include "KeycapTreeView.h"
#include "KeycapDelegate.h"
#include "Keyboards/KeyboardTreeView.h"
#include "Keyboards/Models/KeyboardModel.h"
#include "Tree/Models/UndoableProxyModel.h"
#include "Tree/Models/DiffModel.h"
#include "Tree/TreeItems/TreeItem.h"
#include <QtWidgets/QHeaderView>
#include <QtCore/QItemSelectionModel>

KeycapTreeView::KeycapTreeView(QWidget* pParent)
    : QTreeView(pParent)
    , _pUndoableKeyboardModel(nullptr)
    , _pItemSelectionModel(nullptr)
    , _pDiffModel(new DiffModel(this))
{
    setAlternatingRowColors(true);
    setSelectionMode(ExtendedSelection);
    setEditTriggers(AllEditTriggers);
    header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    delete itemDelegate();
    setItemDelegate(new KeycapDelegate(this));
    setModel(_pDiffModel);
}

KeycapTreeView::~KeycapTreeView()
{

}

void KeycapTreeView::setKeyboardProperties(KeyboardTreeView* pTreeView)
{
    _pUndoableKeyboardModel = qobject_cast<UndoableProxyModel*>(pTreeView->model());
    _pItemSelectionModel = pTreeView->selectionModel();
    connect(_pItemSelectionModel, SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(onKeyboardPropertiesSelectionChanged(QItemSelection,QItemSelection)));
    updateRootIndexFromSelection();
}

void KeycapTreeView::updateRootIndexFromSelection(const QItemSelection& selected, const QItemSelection& deselected)
{
    if (selected.isEmpty() && deselected.isEmpty())
    {
        const auto& allSelectedIndexes = _pItemSelectionModel->selectedIndexes();
        if (!allSelectedIndexes.isEmpty())
        {
            if (!_pDiffModel->sourceModel())
            {
                _pDiffModel->setSourceModel(_pUndoableKeyboardModel);
            }

            auto pKeyboardModel = qobject_cast<KeyboardModel*>(_pUndoableKeyboardModel->sourceModel());
            _pDiffModel->clearMapping();
            for (const auto& index : allSelectedIndexes)
            {
                const int iPropertyType = index.data(TreeItemTypeRole).toInt();
                if (iPropertyType >= TreeItem::Keycap)
                {
                    const QModelIndex& indexKeycapInKeyboardModel = pKeyboardModel->getParentKeycap(index);
                    _pDiffModel->addSourceIndex(_pUndoableKeyboardModel->mapFromSource(indexKeycapInKeyboardModel));
                }
            }
        }
    }
    else
    {
        if (!_pDiffModel->sourceModel())
        {
            _pDiffModel->setSourceModel(_pUndoableKeyboardModel);
        }

        for (const auto& selectedIndex : selected.indexes())
        {
            _pDiffModel->addSourceIndex(selectedIndex);
        }

        for (const auto& deselectedIndex : deselected.indexes())
        {
            _pDiffModel->removeSourceIndex(deselectedIndex);
        }
    }

    if (_pDiffModel->hasMapping())
    {
        const QModelIndex& proxyIndex = _pDiffModel->mapFromSource(_pDiffModel->getSourceIndexes().back());
        if (proxyIndex.isValid())
        {
            setRootIndex(proxyIndex);
            expandAll();
        }
        else
        {
            _pDiffModel->setSourceModel(nullptr);
        }
    }
    else
    {
        _pDiffModel->setSourceModel(nullptr);
    }
}

void KeycapTreeView::onKeyboardPropertiesSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
    updateRootIndexFromSelection(selected, deselected);
}
