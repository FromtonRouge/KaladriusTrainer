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

#pragma once

#include <QtWidgets/QGraphicsScene>
#include <QtCore/QHash>

class KeycapGraphicsItem;
class KeyboardPropertiesModel;
class KeyboardPropertiesTreeView;
class QSvgRenderer;
class QItemSelectionModel;
class QItemSelection;
class KeyboardGraphicsScene : public QGraphicsScene
{
    Q_OBJECT

public:
    typedef QHash<QString, KeycapGraphicsItem*> DictKeycaps;

public:
    KeyboardGraphicsScene(QObject* pParent);
    ~KeyboardGraphicsScene();

    void setKeyboardProperties(KeyboardPropertiesTreeView* pTreeView);
    KeycapGraphicsItem* getKeycapItem(const QString& sKeycapId) const;
    KeycapGraphicsItem* getKeycapItem(const QModelIndex& indexKeycap) const;

private slots:
    void onModelReset();
    void onRowsInserted(const QModelIndex& parent, int iFirst, int iLast);
    void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);
    void onKeyboardPropertiesSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:
    QSvgRenderer* _pSvgRenderer;
    KeyboardPropertiesModel* _pKeyboardPropertiesModel;
    DictKeycaps _dictKeycaps;
};