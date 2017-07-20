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

#include "KeyboardPropertiesTreeView.h"
#include "KeyboardGraphicsScene.h"
#include "KeycapGraphicsItem.h"

KeyboardPropertiesTreeView::KeyboardPropertiesTreeView(QWidget* pParent)
    : QTreeView(pParent)
{
    setAlternatingRowColors(true);
    setSelectionMode(ExtendedSelection);
}

KeyboardPropertiesTreeView::~KeyboardPropertiesTreeView()
{
}

void KeyboardPropertiesTreeView::setModel(QAbstractItemModel* pModel)
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

void KeyboardPropertiesTreeView::onGraphicsSceneSelectionChanged()
{
    auto pGraphicsScene = qobject_cast<KeyboardGraphicsScene*>(sender());
    if (pGraphicsScene)
    {
        QModelIndexList matches;
        const auto& selected = pGraphicsScene->selectedItems();
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

void KeyboardPropertiesTreeView::onRowsInserted(const QModelIndex& parent, int iFirst, int iLast)
{
    bool bResizeColumn = false;
    for (int iRow = iFirst; iRow <= iLast; ++iRow)
    {
        const QModelIndex& indexInserted = model()->index(iRow, 0, parent);
        if (!parent.isValid())
        {
            // Inserting a top level item
            if (indexInserted.data().toString() == tr("Keys"))
            {
                bResizeColumn = true;
                expand(indexInserted);
            }
        }
    }

    if (bResizeColumn)
    {
        resizeColumnToContents(0);
    }
}
