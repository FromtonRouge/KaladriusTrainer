// ======================================================================
// This file is a part of the KaladriusTrainer project
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
#include "Models/KeyboardModel.h"
#include "../Tree/Models/UndoableProxyModel.h"
#include "KeyboardTreeView.h"
#include "../Keycaps/KeycapGraphicsItem.h"
#include "../Tree/TreeItems/TreeItem.h"
#include "../Tree/Models/Utils.h"
#include "../Values/Types/Finger.h"
#include <QtSvg/QSvgRenderer>
#include <QtGui/QFont>
#include <QtCore/QItemSelectionModel>
#include <QtCore/QSignalBlocker>
#include <QtCore/QDebug>
#include <QtCore/QMultiMap>
#include <functional>
#include <iostream>
#include <set>

KeyboardGraphicsScene::KeyboardGraphicsScene(QObject* pParent)
    : QGraphicsScene(pParent)
    , _pSvgRenderer(nullptr)
    , _pUndoableKeyboardModel(nullptr)
{
}

KeyboardGraphicsScene::~KeyboardGraphicsScene()
{
}

void KeyboardGraphicsScene::setKeyboardProperties(KeyboardTreeView* pTreeView)
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

void KeyboardGraphicsScene::setKeycapsStates(const QVector<KeycapsStates>& possibleKeycapsEntries)
{
    // Get keyboard model data for fingers information
    auto pKeyboardModel = qobject_cast<KeyboardModel*>(_pUndoableKeyboardModel->sourceModel());
    const QModelIndex& indexKeycapsInKeyboardModel = pKeyboardModel->getKeycapsIndex();

    // Classify possible entries from the easiest to stroke to the hardest
    QMultiMap<uint, KeycapsStates> possibleEntriesByFingersEffort;
    for (const auto& keycapsForOneEntry : possibleKeycapsEntries)
    {
        // Compute the fingers effort value, at the moment it's just the sum of fingers ids
        uint uiFingersEffort = 0;

        // Parse keycaps for the current entry
        for (const auto& keycapState : keycapsForOneEntry)
        {
            const QString& sKeycapId = keycapState.first;
            if (indexKeycapsInKeyboardModel.isValid())
            {
                const QModelIndex& indexFingerValue = Utils::index(pKeyboardModel, sKeycapId + "/Finger", 1, indexKeycapsInKeyboardModel);
                if (indexFingerValue.isValid())
                {
                    const Finger& finger = qvariant_cast<Finger>(indexFingerValue.data(Qt::EditRole));
                    if (finger.id != Finger::None)
                    {
                        uiFingersEffort += uint(finger.id);
                    }
                }
            }
        }

        possibleEntriesByFingersEffort.insert(uiFingersEffort, keycapsForOneEntry);
    }

    if (!possibleEntriesByFingersEffort.isEmpty())
    {
        // Get the easiest entry to stroke for the user, it's the first one in the map
        const KeycapsStates& easiestEntry = possibleEntriesByFingersEffort.first();
        for (const auto& keycapState : easiestEntry)
        {
            const QString& sKeycapId = keycapState.first;
            const bool bPressed = keycapState.second;
            auto pKeycapItem = getKeycapItem(sKeycapId);
            if (pKeycapItem)
            {
                pKeycapItem->setSelected(bPressed);
            }
        }
    }
}

void KeyboardGraphicsScene::setKeycapsStates(const KeycapsStates& keycapsStates)
{
    for (const KeycapState& keycapState : keycapsStates)
    {
        auto pKeycapItem = getKeycapItem(keycapState.first);
        if (pKeycapItem)
        {
            pKeycapItem->setSelected(keycapState.second);
        }
    }
}

void KeyboardGraphicsScene::onModelReset()
{
   _dictKeycaps.clear();
   clear();
   delete _pSvgRenderer;
   auto pKeyboardModel = qobject_cast<KeyboardModel*>(_pUndoableKeyboardModel->sourceModel());
   _pSvgRenderer = new QSvgRenderer(pKeyboardModel->getSvgContent(), this);
}

void KeyboardGraphicsScene::onRowsInserted(const QModelIndex& parent, int iFirst, int iLast)
{
    for (int iRow = iFirst; iRow <= iLast; ++iRow)
    {
        const QModelIndex& indexInserted = _pUndoableKeyboardModel->index(iRow, 0, parent);
        if (!parent.isValid())
        {
            // Search for the Keyboard index first
            auto matches = _pUndoableKeyboardModel->match(indexInserted, TreeItemTypeRole, TreeItem::Keyboard, 1, Qt::MatchExactly);
            if (matches.isEmpty())
            {
                return;
            }

            // Search for the first list element under the keyboard
            const QModelIndex& indexKeyboard = matches.front();
            matches = _pUndoableKeyboardModel->match(_pUndoableKeyboardModel->index(0, 0, indexKeyboard), TreeItemTypeRole, TreeItem::List, 1, Qt::MatchExactly);
            if (matches.isEmpty())
            {
                return;
            }

            // Parsing all keycaps
            const QModelIndex& indexKeycaps = matches.front();
            const int iKeycaps = _pUndoableKeyboardModel->rowCount(indexKeycaps);
            for (int iKeycap = 0; iKeycap < iKeycaps; ++iKeycap)
            {
                const QModelIndex& indexKeycap = _pUndoableKeyboardModel->index(iKeycap, 0, indexKeycaps);
                const QString& sKeycapId = indexKeycap.data().toString();
                const qreal& dRotationAngle = indexKeycap.data(RotationAngleRole).toReal();
                const QPointF& rotationOrigin = indexKeycap.data(RotationOriginRole).toPointF();
                const QRectF& rectOuterBorder = indexKeycap.data(OuterBorderRole).toRectF();
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
                        traverse(_pUndoableKeyboardModel->index(iRow, 0, index));
                    }
                };
                traverse(indexKeycap);
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
        const QModelIndex& indexKeycap = KeyboardModel::getParentKeycap(parent);
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
            auto pKeycapItem = getKeycapItem(KeyboardModel::getParentKeycap(deselectedIndex));
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
            auto pKeycapItem = getKeycapItem(KeyboardModel::getParentKeycap(selectedIndex));
            if (pKeycapItem)
            {
                pKeycapItem->setSelected(true);
            }
        }
    }
}
