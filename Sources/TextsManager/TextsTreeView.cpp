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

#include "TextsTreeView.h"
#include <QtWidgets/QAction>
#include <QtGui/QKeySequence>
#include <QtCore/QVector>
#include <QtCore/QPersistentModelIndex>
#include <QtCore/QItemSelectionModel>
#include <QtCore/QDebug>

TextsTreeView::TextsTreeView(QWidget* pParent)
    : QTreeView(pParent)
{
    setSelectionMode(QTreeView::ContiguousSelection);

    _pActionEnable = new QAction(tr("Enable"));
    addAction(_pActionEnable);
    connect(_pActionEnable, &QAction::triggered, this, &TextsTreeView::enableSelection);

    _pActionDisable = new QAction(tr("Disable"));
    addAction(_pActionDisable);
    connect(_pActionDisable, &QAction::triggered, this, &TextsTreeView::disableSelection);

    _pActionRemove = new QAction(QIcon(":/Icons/cross.png"), tr("Remove"));
    _pActionRemove->setShortcut(Qt::Key_Delete);
    addAction(_pActionRemove);
    connect(_pActionRemove, &QAction::triggered, this, &TextsTreeView::removeSelection);

    setContextMenuPolicy(Qt::ActionsContextMenu);
}

TextsTreeView::~TextsTreeView()
{

}

void TextsTreeView::enableSelection()
{
    const QModelIndexList& indexes = selectedIndexes();
    for (const QModelIndex& index : indexes)
    {
        model()->setData(index, Qt::Checked, Qt::CheckStateRole);
    }
}

void TextsTreeView::disableSelection()
{
    const QModelIndexList& indexes = selectedIndexes();
    for (const QModelIndex& index : indexes)
    {
        model()->setData(index, Qt::Unchecked, Qt::CheckStateRole);
    }
}

void TextsTreeView::removeSelection()
{
    QItemSelectionModel* pSelectionModel = selectionModel();
    for (const QItemSelectionRange& selectionRange : pSelectionModel->selection())
    {
        model()->removeRows(selectionRange.top(), selectionRange.height(), selectionRange.parent());
    }
}
