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
#include "Keycaps/KeycapGraphicsItem.h"
#include "Models/UndoableProxyModel.h"
#include "KeyboardModel.h"
#include <QtWidgets/QMenu>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QKeySequence>
#include <QtWidgets/QAction>
#include <QtWidgets/QUndoStack>
#include <QtSvg/QSvgRenderer>
#include <QtGui/QCursor>
#include <QtGui/QPainter>
#include <QtCore/QHash>
#include <iostream>

KeyboardGraphicsView::KeyboardGraphicsView(QWidget* pParent)
    : QGraphicsView(pParent)
    , _pSvgRenderer(new QSvgRenderer(QString(":/Svgs/rec-button.svg"), this))
    , _bRecordKeyboardMode(false)
{
    setDragMode(RubberBandDrag);

    _pActionSelectAll = new QAction(tr("Select all"), this);
    _pActionSelectAll->setShortcut(QKeySequence("Ctrl+a"));
    connect(_pActionSelectAll, SIGNAL(triggered(bool)), this, SLOT(selectAll()));
    addAction(_pActionSelectAll);
}

KeyboardGraphicsView::~KeyboardGraphicsView()
{
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

void KeyboardGraphicsView::keyReleaseEvent(QKeyEvent* pEvent)
{
    QGraphicsView::keyReleaseEvent(pEvent);

    if (_bRecordKeyboardMode)
    {
        // Get selected KeycapGraphicsItem
        const auto& selected = scene()->selectedItems();
        QStringList selectedKeycaps;
        for (auto pItem : selected)
        {
            auto pKeycapItem = qgraphicsitem_cast<KeycapGraphicsItem*>(pItem);
            if (pKeycapItem)
            {
                selectedKeycaps << pKeycapItem->elementId();
            }
        }

        if (!selectedKeycaps.isEmpty())
        {
            QString sKey = QKeySequence(pEvent->key()).toString();

            static QHash<int, QString> dictModifiers;
            dictModifiers[Qt::Key_Shift] = "Shift";
            dictModifiers[Qt::Key_Control] = "Ctrl";
            dictModifiers[Qt::Key_Meta] = "Meta";
            dictModifiers[Qt::Key_Alt] = "Alt";
            dictModifiers[Qt::Key_AltGr] = "AltGr";

            auto it = dictModifiers.find(pEvent->key());
            if (it != dictModifiers.end())
            {
                sKey = it.value();
            }

            bool bCanSetKey = false;
            const QPoint& posInViewport = mapFromGlobal(QCursor::pos());
            const auto& itemsUnderCursor = items(posInViewport);
            for (auto pGraphicsItem : itemsUnderCursor)
            {
                auto pKeycapItem = qgraphicsitem_cast<KeycapGraphicsItem*>(pGraphicsItem);
                if (pKeycapItem && pKeycapItem->isSelected())
                {
                    bCanSetKey = true;
                    break;
                }
            }

            if (bCanSetKey)
            {
                auto pKeyboardScene = qobject_cast<KeyboardGraphicsScene*>(scene());
                if (pKeyboardScene)
                {
                    auto pUndoableKeyboardModel = pKeyboardScene->getUndoableKeyboardModel();
                    auto pKeyboardModel = qobject_cast<KeyboardModel*>(pUndoableKeyboardModel->sourceModel());
                    pUndoableKeyboardModel->getUndoStack()->beginMacro(tr("%1 indexes changed to %2").arg(selectedKeycaps.size()).arg(sKey));
                    const QModelIndex& indexKeycaps = pUndoableKeyboardModel->mapFromSource(pKeyboardModel->getKeycapsIndex());
                    if (pUndoableKeyboardModel->hasChildren(indexKeycaps))
                    {
                        const QModelIndex& start = indexKeycaps.child(0,0);
                        for (const auto& sKeycap : selectedKeycaps)
                        {
                            const auto& matches = pUndoableKeyboardModel->match(start, Qt::DisplayRole, sKeycap, 1, Qt::MatchExactly);
                            if (!matches.isEmpty())
                            {
                                const QModelIndex& index = matches.front();
                                const QModelIndex& indexLabelValue = index.child(0,1);
                                pUndoableKeyboardModel->setData(indexLabelValue, sKey, Qt::EditRole);
                            }
                        }
                    }
                    pUndoableKeyboardModel->getUndoStack()->endMacro();
                }
            }
        }
    }
}

void KeyboardGraphicsView::paintEvent(QPaintEvent* pEvent)
{
    if (_bRecordKeyboardMode)
    {
        QPainter painter(viewport());
        QRectF bounds(QPointF(), QSize(80, 50));
        _pSvgRenderer->render(&painter, "g6", bounds);
    }

    QGraphicsView::paintEvent(pEvent);
}

void KeyboardGraphicsView::fitKeyboardInView()
{
    if (scene())
    {
        fitInView(scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
    }
}

void KeyboardGraphicsView::selectAll()
{
    QPainterPath path;
    path.addRect(scene()->sceneRect());
    scene()->setSelectionArea(path, transform());
}

void KeyboardGraphicsView::recordKeyboardInputs(bool bActivated)
{
    _bRecordKeyboardMode = bActivated;
    viewport()->update();
}
