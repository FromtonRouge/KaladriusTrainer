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
#include "KeyboardPropertiesTreeView.h"
#include "KeycapGraphicsItem.h"
#include <QtSvg/QSvgRenderer>
#include <QtCore/QItemSelectionModel>
#include <iostream>

KeyboardGraphicsScene::KeyboardGraphicsScene(QObject* pParent)
    : QGraphicsScene(pParent)
    , _pSvgRenderer(nullptr)
    , _pKeyboardPropertiesModel(nullptr)
{
}

KeyboardGraphicsScene::~KeyboardGraphicsScene()
{
}

void KeyboardGraphicsScene::setKeyboardProperties(KeyboardPropertiesTreeView* pTreeView)
{
    _pKeyboardPropertiesModel = qobject_cast<KeyboardPropertiesModel*>(pTreeView->model());
    connect(_pKeyboardPropertiesModel, SIGNAL(modelReset()), this, SLOT(onModelReset()));
    connect(_pKeyboardPropertiesModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(onRowsInserted(QModelIndex, int, int)));
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

void KeyboardGraphicsScene::onModelReset()
{
   _dictKeycaps.clear();
   clear();
    delete _pSvgRenderer;
    _pSvgRenderer = new QSvgRenderer(_pKeyboardPropertiesModel->getKeyboardSvgPath(), this);
}

void KeyboardGraphicsScene::onRowsInserted(const QModelIndex& parent, int iFirst, int iLast)
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
                    const QModelIndex& indexKeycap = indexInserted.child(iKeycap, 0);
                    const QString& sKeycapId = indexKeycap.data().toString();
                    const float fAngle = indexKeycap.data(int(KeyboardPropertiesModel::UserRole::AngleRole)).toFloat();
                    auto pKeycapGraphicsItem = new KeycapGraphicsItem(sKeycapId, fAngle, _pSvgRenderer);
                    pKeycapGraphicsItem->setText(sKeycapId);
                    pKeycapGraphicsItem->setTextSize(12);
                    addItem(pKeycapGraphicsItem);
                    _dictKeycaps.insert(sKeycapId, pKeycapGraphicsItem);
                }
            }
        }
    }
}

void KeyboardGraphicsScene::onKeyboardPropertiesSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    // Handle deselected indexes
    const auto& deselectedIndexes = deselected.indexes();
    for (const auto& deselectedIndex : deselectedIndexes)
    {
        if (deselectedIndex.column() == 0 && deselectedIndex.data(int(KeyboardPropertiesModel::UserRole::PropertyTypeRole)).toInt() == int(KeyboardPropertiesModel::PropertyType::Keycap))
        {
            const QString& sKeycapId = deselectedIndex.data().toString();
            auto pKeycapItem = getKeycapItem(sKeycapId);
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
        if (selectedIndex.column() == 0 && selectedIndex.data(int(KeyboardPropertiesModel::UserRole::PropertyTypeRole)).toInt() == int(KeyboardPropertiesModel::PropertyType::Keycap))
        {
            const QString& sKeycapId = selectedIndex.data().toString();
            auto pKeycapItem = getKeycapItem(sKeycapId);
            if (pKeycapItem)
            {
                pKeycapItem->setSelected(true);
            }
        }
    }
}
