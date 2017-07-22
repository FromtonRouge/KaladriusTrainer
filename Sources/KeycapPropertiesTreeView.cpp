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
#include "KeyboardPropertiesTreeView.h"
#include "KeyboardPropertiesModel.h"
#include <QtWidgets/QHeaderView>
#include <QtCore/QItemSelectionModel>

KeycapPropertiesTreeView::KeycapPropertiesTreeView(QWidget* pParent)
    : QTreeView(pParent)
    , _pKeyboardPropertiesModel(nullptr)
    , _pItemSelectionModel(nullptr)
{
    setAlternatingRowColors(true);
    setSelectionMode(ExtendedSelection);
    setEditTriggers(SelectedClicked|DoubleClicked|AnyKeyPressed);
    header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

KeycapPropertiesTreeView::~KeycapPropertiesTreeView()
{

}

void KeycapPropertiesTreeView::setModel(QAbstractItemModel* pModel)
{
    if (model())
    {
        model()->disconnect(this);
    }

    QTreeView::setModel(pModel);

    if (pModel)
    {
        connect(pModel, SIGNAL(keyboardLoaded()), this, SLOT(onKeyboardLoaded()));
    }
}

void KeycapPropertiesTreeView::setKeyboardProperties(KeyboardPropertiesTreeView* pTreeView)
{
    _pKeyboardPropertiesModel = qobject_cast<KeyboardPropertiesModel*>(pTreeView->model());
    _pItemSelectionModel = pTreeView->selectionModel();
    connect(_pItemSelectionModel, SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(onKeyboardPropertiesSelectionChanged(QItemSelection,QItemSelection)));
    updateRootIndexFromSelection();
}

void KeycapPropertiesTreeView::onKeyboardLoaded()
{
}

void KeycapPropertiesTreeView::updateRootIndexFromSelection(const QItemSelection& selected, const QItemSelection& deselected)
{
    QModelIndexList selectedKeycaps;

    if (selected.isEmpty() && deselected.isEmpty())
    {
        const auto& allSelectedIndexes = _pItemSelectionModel->selectedIndexes();
        for (const auto& index : allSelectedIndexes)
        {
            const int iPropertyType = index.data(KeyboardPropertiesModel::PropertyTypeRole).toInt();
            if (iPropertyType >= KeyboardPropertiesModel::Keycap)
            {
                const QModelIndex& indexKeycap = _pKeyboardPropertiesModel->getParentKeycap(index);
                selectedKeycaps << indexKeycap;
            }
        }
    }
    else
    {
        // TODO
    }

    if (!selectedKeycaps.isEmpty())
    {
        if (!model())
        {
            setModel(_pKeyboardPropertiesModel);
        }
        setRootIndex(selectedKeycaps.back());
        expandAll();
    }
    else
    {
        setModel(nullptr);
    }
}

void KeycapPropertiesTreeView::onKeyboardPropertiesSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
    updateRootIndexFromSelection();
}

