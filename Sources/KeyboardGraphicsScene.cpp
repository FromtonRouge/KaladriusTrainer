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

#include "KeyboardGraphicsScene.h"
#include "KeyboardPropertiesModel.h"
#include "UndoableProxyModel.h"
#include "KeyboardPropertiesTreeView.h"
#include "KeycapGraphicsItem.h"
#include <QtSvg/QSvgRenderer>
#include <QtGui/QFont>
#include <QtCore/QItemSelectionModel>
#include <QtCore/QSignalBlocker>
#include <functional>
#include <iostream>

KeyboardGraphicsScene::KeyboardGraphicsScene(QObject* pParent)
    : QGraphicsScene(pParent)
    , _pSvgRenderer(nullptr)
    , _pUndoableKeyboardModel(nullptr)
{
}

KeyboardGraphicsScene::~KeyboardGraphicsScene()
{
}

void KeyboardGraphicsScene::setKeyboardProperties(KeyboardPropertiesTreeView* pTreeView)
{
    _pUndoableKeyboardModel = qobject_cast<UndoableProxyModel*>(pTreeView->model());
    connect(_pUndoableKeyboardModel, SIGNAL(modelReset()), this, SLOT(onModelReset()));
    connect(_pUndoableKeyboardModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(onRowsInserted(QModelIndex, int, int)));
    connect(_pUndoableKeyboardModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(onDataChanged(QModelIndex,QModelIndex,QVector<int>)));
    connect(pTreeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onKeyboardPropertiesSelectionChanged(QItemSelection, QItemSelection)));
    connect(this, SIGNAL(selectionChanged()), pTreeView, SLOT(onGraphicsSceneSelectionChanged()));
}

KeycapGraphicsItem*KeyboardGraphicsScene::getKeycapItem(const QString& sKeycapId) const
{
    auto it = _dictKeycaps.find(sKeycapId);
    if (it != _dictKeycaps.end())
    {
        return it.value();
    }
    return nullptr;
}

KeycapGraphicsItem*KeyboardGraphicsScene::getKeycapItem(const QModelIndex& indexKeycap) const
{
    return getKeycapItem(indexKeycap.data().toString());
}

void KeyboardGraphicsScene::onModelReset()
{
   _dictKeycaps.clear();
   clear();
   delete _pSvgRenderer;
   auto pKeyboardPropertiesModel = qobject_cast<KeyboardPropertiesModel*>(_pUndoableKeyboardModel->sourceModel());
   _pSvgRenderer = new QSvgRenderer(pKeyboardPropertiesModel->getSvgContent(), this);
}

void KeyboardGraphicsScene::onRowsInserted(const QModelIndex& parent, int iFirst, int iLast)
{
    for (int iRow = iFirst; iRow <= iLast; ++iRow)
    {
        const QModelIndex& indexInserted = _pUndoableKeyboardModel->index(iRow, 0, parent);
        if (!parent.isValid())
        {
            // Inserting a top level item
            if (indexInserted.data().toString() == tr("Keys"))
            {
                // Parsing all keycaps under "Keys"
                const int iKeycaps = _pUndoableKeyboardModel->rowCount(indexInserted);
                for (int iKeycap = 0; iKeycap < iKeycaps; ++iKeycap)
                {
                    const QModelIndex& indexKeycap = indexInserted.child(iKeycap, 0);
                    const QString& sKeycapId = indexKeycap.data().toString();
                    const qreal& dRotationAngle = indexKeycap.data(KeyboardPropertiesModel::RotationAngleRole).toReal();
                    const QPointF& rotationOrigin = indexKeycap.data(KeyboardPropertiesModel::RotationOriginRole).toPointF();
                    const QRectF& rectOuterBorder = indexKeycap.data(KeyboardPropertiesModel::OuterBorderRole).toRectF();
                    auto pKeycapGraphicsItem = new KeycapGraphicsItem(  sKeycapId,
                                                                        dRotationAngle,
                                                                        rotationOrigin,
                                                                        rectOuterBorder,
                                                                        _pSvgRenderer);

                    addItem(pKeycapGraphicsItem);
                    _dictKeycaps.insert(sKeycapId, pKeycapGraphicsItem);

                    std::function<void (const QModelIndex&)> traverse = [&](const QModelIndex& index)
                    {
                        const int iColumns = _pUndoableKeyboardModel->columnCount();
                        onDataChanged(index, index.sibling(index.row(), iColumns-1), {});
                        const int iRows = _pUndoableKeyboardModel->rowCount(index);
                        for (int iRow = 0; iRow < iRows; ++iRow)
                        {
                            traverse(index.child(iRow, 0));
                        }
                    };
                    traverse(indexKeycap);
                }
            }
        }
    }
}

void KeyboardGraphicsScene::onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    if (!topLeft.isValid() || !bottomRight.isValid())
    {
        return;
    }

    if (roles.isEmpty() || roles.contains(Qt::EditRole))
    {
        const QModelIndex& parent = topLeft.parent();
        const QModelIndex& indexKeycap = KeyboardPropertiesModel::getParentKeycap(parent);
        if (!indexKeycap.isValid())
        {
            return;
        }

        auto pKeycapItem = getKeycapItem(indexKeycap);
        if (!pKeycapItem)
        {
            return;
        }

        const int iTopRow = topLeft.row();
        const int iBottomRow = bottomRight.row();
        const int iLeftColumn = topLeft.column();
        const int iRightColumn = bottomRight.column();
        for (int iRow = iTopRow; iRow <= iBottomRow; ++iRow)
        {
            const QModelIndex& indexName = _pUndoableKeyboardModel->index(iRow, 0, parent);
            const QString& sName = indexName.data().toString();
            for (int iColumn = iLeftColumn; iColumn <= iRightColumn; ++iColumn)
            {
                const QModelIndex& indexChanged = indexName.sibling(iRow, iColumn);
                const QVariant& value = indexChanged.data(Qt::EditRole);
                if (sName == tr("Label"))
                {
                    pKeycapItem->setText(value.toString());
                }
                else if (sName == tr("Font"))
                {
                    pKeycapItem->setTextFont(value.value<QFont>());
                }
                else if (sName == tr("X"))
                {
                    pKeycapItem->setTextOffsetX(value.toReal());
                }
                else if (sName == tr("Y"))
                {
                    pKeycapItem->setTextOffsetY(value.toReal());
                }
                else if (sName == tr("Color"))
                {
                    pKeycapItem->setColor(value.value<QColor>());
                }
            }
        }
    }
}

void KeyboardGraphicsScene::onKeyboardPropertiesSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    QSignalBlocker blocker(this);

    // Handle deselected indexes
    const auto& deselectedIndexes = deselected.indexes();
    for (const auto& deselectedIndex : deselectedIndexes)
    {
        if (deselectedIndex.column() == 0)
        {
            auto pKeycapItem = getKeycapItem(KeyboardPropertiesModel::getParentKeycap(deselectedIndex));
            if (pKeycapItem)
            {
                pKeycapItem->setSelected(false);
            }
        }
    }

    // Handle selected indexes
    const auto& selectedIndexes = selected.indexes();
    for (const auto& selectedIndex : selectedIndexes)
    {
        if (selectedIndex.column() == 0)
        {
            auto pKeycapItem = getKeycapItem(KeyboardPropertiesModel::getParentKeycap(selectedIndex));
            if (pKeycapItem)
            {
                pKeycapItem->setSelected(true);
            }
        }
    }
}
