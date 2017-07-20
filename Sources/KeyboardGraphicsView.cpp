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

#include "KeyboardGraphicsView.h"
#include "KeyboardGraphicsScene.h"
#include "KeyboardPropertiesModel.h"
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>
#include <QtGui/QContextMenuEvent>
#include <QtSvg/QGraphicsSvgItem>
#include <QtSvg/QSvgRenderer>
#include <QtCore/QItemSelectionModel>
#include <QtCore/QTimer>
#include <iostream>

KeyboardGraphicsView::KeyboardGraphicsView(QWidget* pParent)
    : QGraphicsView(pParent)
    , _pSvgRenderer(nullptr)
    , _pKeyboardPropertiesModel(nullptr)
{
    setScene(new KeyboardGraphicsScene(this));
}

KeyboardGraphicsView::~KeyboardGraphicsView()
{
}

void KeyboardGraphicsView::setKeyboardProperties(KeyboardPropertiesModel* pKeyboardPropertiesModel, QItemSelectionModel* pSelectionModel)
{
    _pKeyboardPropertiesModel = pKeyboardPropertiesModel;
    connect(_pKeyboardPropertiesModel, SIGNAL(modelReset()), this, SLOT(onModelReset()));
    connect(_pKeyboardPropertiesModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(onRowsInserted(QModelIndex, int, int)));
    connect(pSelectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onKeyboardPropertiesSelectionChanged(QItemSelection, QItemSelection)));
    reloadKeyboard();
}

void KeyboardGraphicsView::contextMenuEvent(QContextMenuEvent* pEvent)
{
    QMenu menu;
    if (!menu.isEmpty())
    {
        menu.exec(pEvent->globalPos());
    }
}

void KeyboardGraphicsView::resizeEvent(QResizeEvent* pEvent)
{
    fitKeyboardInView();
    QGraphicsView::resizeEvent(pEvent);
}

void KeyboardGraphicsView::fitKeyboardInView()
{
    fitInView(scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void KeyboardGraphicsView::reloadKeyboard()
{
    onModelReset();

    // Search for the "Keys" index and load all keycaps svg elements
    const auto& start = _pKeyboardPropertiesModel->index(0, 0);
    const auto& matches = _pKeyboardPropertiesModel->match(start, Qt::DisplayRole, tr("Keys"), 1, Qt::MatchExactly);
    if (!matches.isEmpty())
    {
        const int iRows = _pKeyboardPropertiesModel->rowCount();
        if (iRows)
        {
            onRowsInserted(QModelIndex(), 0, iRows-1);
        }
    }
}

void KeyboardGraphicsView::onModelReset()
{
   scene()->clear();
    delete _pSvgRenderer;
    _pSvgRenderer = new QSvgRenderer(_pKeyboardPropertiesModel->getKeyboardSvgPath(), scene());
}

void KeyboardGraphicsView::onRowsInserted(const QModelIndex& parent, int iFirst, int iLast)
{
    for (int iRow = iFirst; iRow <= iLast; ++iRow)
    {
        const QModelIndex& indexInserted = _pKeyboardPropertiesModel->index(iRow, 0, parent);
        if (!parent.isValid())
        {
            // Inserting a top level item
            if (indexInserted.data().toString() == tr("Keys"))
            {
                // Parsing all keycaps under "Keys"
                const int iKeycaps = _pKeyboardPropertiesModel->rowCount(indexInserted);
                for (int iKeycap = 0; iKeycap < iKeycaps; ++iKeycap)
                {
                    addKeyIndex(indexInserted.child(iKeycap, 0));
                }

                // All keycaps added, we can fit the keyboard to graphics view
                fitKeyboardInView();
            }
        }
    }
}

void KeyboardGraphicsView::onKeyboardPropertiesSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    // Handle deselected indexes
    const auto& deselectedIndexes = deselected.indexes();
    for (const auto& deselectedIndex : deselectedIndexes)
    {
        if (deselectedIndex.column() == 0 && deselectedIndex.data(int(KeyboardPropertiesModel::UserRole::PropertyType)).toInt() == int(KeyboardPropertiesModel::PropertyType::Keycap))
        {
            const QString& sKeycapId = deselectedIndex.data().toString();
            std::cerr << sKeycapId.toStdString() << std::endl;
        }
    }

    // Handle selected indexes
    const auto& selectedIndexes = selected.indexes();
    for (const auto& selectedIndex : selectedIndexes)
    {
        if (selectedIndex.column() == 0 && selectedIndex.data(int(KeyboardPropertiesModel::UserRole::PropertyType)).toInt() == int(KeyboardPropertiesModel::PropertyType::Keycap))
        {
            const QString& sKeycapId = selectedIndex.data().toString();
            std::cout << sKeycapId.toStdString() << std::endl;
        }
    }
}

void KeyboardGraphicsView::addKeyIndex(const QModelIndex& indexKey)
{
    const QString& sKeyId = indexKey.data().toString();
    auto pSvgItem = new QGraphicsSvgItem();
    pSvgItem->setFlag(QGraphicsItem::ItemIsSelectable);
    pSvgItem->setSharedRenderer(_pSvgRenderer);
    pSvgItem->setElementId(sKeyId);
    pSvgItem->setPos(_pSvgRenderer->boundsOnElement(sKeyId).topLeft());
    scene()->addItem(pSvgItem);
}
