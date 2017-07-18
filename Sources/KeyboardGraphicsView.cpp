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
#include "KeyboardPropertiesModel.h"
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>
#include <QtGui/QContextMenuEvent>
#include <QtSvg/QGraphicsSvgItem>
#include <QtSvg/QSvgRenderer>
#include <QtCore/QTimer>

KeyboardGraphicsView::KeyboardGraphicsView(QWidget* pParent)
    : QGraphicsView(pParent)
    , _pSvgRenderer(nullptr)
    , _pKeyboardPropertiesModel(nullptr)
{
    setScene(new QGraphicsScene(this));
}

KeyboardGraphicsView::~KeyboardGraphicsView()
{
}

void KeyboardGraphicsView::setKeyboardPropertiesModel(KeyboardPropertiesModel* pKeyboardPropertiesModel)
{
    _pKeyboardPropertiesModel = pKeyboardPropertiesModel;
    connect(_pKeyboardPropertiesModel, SIGNAL(modelReset()), this, SLOT(onModelReset()));
    connect(_pKeyboardPropertiesModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(onRowsInserted(QModelIndex, int, int)));
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
    delete _pSvgRenderer;
    _pSvgRenderer = new QSvgRenderer(_pKeyboardPropertiesModel->getKeyboardSvgPath(), scene());

    const int iRows = _pKeyboardPropertiesModel->rowCount();
    for (int iRow = 0; iRow < iRows; ++iRow)
    {
        const QModelIndex& indexKey = _pKeyboardPropertiesModel->index(iRow, 0);
        const QString& sKeyId = indexKey.data().toString();
        auto pSvgItem = new QGraphicsSvgItem();
        pSvgItem->setFlag(QGraphicsItem::ItemIsSelectable);
        pSvgItem->setSharedRenderer(_pSvgRenderer);
        pSvgItem->setElementId(sKeyId);
        const auto& bounds = _pSvgRenderer->boundsOnElement(sKeyId);
        pSvgItem->setPos(bounds.topLeft());
        scene()->addItem(pSvgItem);
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
    if (!parent.isValid())
    {
        for (int iRow = iFirst; iRow < iLast+1; ++iRow)
        {
            const QModelIndex& indexKey = _pKeyboardPropertiesModel->index(iRow, 0, parent);
            const QString& sKeyId = indexKey.data().toString();
            auto pSvgItem = new QGraphicsSvgItem();
            pSvgItem->setFlag(QGraphicsItem::ItemIsSelectable);
            pSvgItem->setSharedRenderer(_pSvgRenderer);
            pSvgItem->setElementId(sKeyId);
            const auto& bounds = _pSvgRenderer->boundsOnElement(sKeyId);
            pSvgItem->setPos(bounds.topLeft());
            scene()->addItem(pSvgItem);
        }
    }

    fitKeyboardInView();
}
