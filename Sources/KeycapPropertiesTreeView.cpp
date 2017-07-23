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

#include "KeycapPropertiesTreeView.h"
#include "KeycapPropertiesDelegate.h"
#include "KeyboardPropertiesTreeView.h"
#include "KeyboardPropertiesModel.h"
#include "DiffModel.h"
#include <QtWidgets/QHeaderView>
#include <QtCore/QItemSelectionModel>

KeycapPropertiesTreeView::KeycapPropertiesTreeView(QWidget* pParent)
    : QTreeView(pParent)
    , _pKeyboardPropertiesModel(nullptr)
    , _pItemSelectionModel(nullptr)
    , _pDiffModel(new DiffModel(this))
{
    setAlternatingRowColors(true);
    setSelectionMode(ExtendedSelection);
    setEditTriggers(SelectedClicked|DoubleClicked|AnyKeyPressed);
    header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    delete itemDelegate();
    setItemDelegate(new KeycapPropertiesDelegate(this));
    setModel(_pDiffModel);
}

KeycapPropertiesTreeView::~KeycapPropertiesTreeView()
{

}

void KeycapPropertiesTreeView::setKeyboardProperties(KeyboardPropertiesTreeView* pTreeView)
{
    _pKeyboardPropertiesModel = qobject_cast<KeyboardPropertiesModel*>(pTreeView->model());
    _pItemSelectionModel = pTreeView->selectionModel();
    connect(_pItemSelectionModel, SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(onKeyboardPropertiesSelectionChanged(QItemSelection,QItemSelection)));
    updateRootIndexFromSelection();
}

void KeycapPropertiesTreeView::updateRootIndexFromSelection(const QItemSelection& selected, const QItemSelection& deselected)
{
    if (selected.isEmpty() && deselected.isEmpty())
    {
        const auto& allSelectedIndexes = _pItemSelectionModel->selectedIndexes();
        for (const auto& index : allSelectedIndexes)
        {
            const int iPropertyType = index.data(KeyboardPropertiesModel::PropertyTypeRole).toInt();
            if (iPropertyType >= KeyboardPropertiesModel::Keycap)
            {
                const QModelIndex& indexKeycap = _pKeyboardPropertiesModel->getParentKeycap(index);
                _selectedIndexes << indexKeycap;
            }
        }
    }
    else
    {
        for (const auto& selectedIndex : selected.indexes())
        {
            _selectedIndexes.push_back(selectedIndex);
        }

        for (const auto& deselectedIndex : deselected.indexes())
        {
            _selectedIndexes.removeOne(deselectedIndex);
        }
    }

    if (!_selectedIndexes.isEmpty())
    {
        if (!_pDiffModel->sourceModel())
        {
            _pDiffModel->setSourceModel(_pKeyboardPropertiesModel);
        }
        setRootIndex(_pDiffModel->mapFromSource(_selectedIndexes.back()));
        expandAll();

        auto pItemDelegate = static_cast<KeycapPropertiesDelegate*>(itemDelegate());
        pItemDelegate->setMultipleEditions(_selectedIndexes.size() > 1);
    }
    else
    {
        _pDiffModel->setSourceModel(nullptr);
    }
}

void KeycapPropertiesTreeView::onKeyboardPropertiesSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
    updateRootIndexFromSelection(selected, deselected);
}
