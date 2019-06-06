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

#pragma once

#include <QtWidgets/QTreeView>

class DiffModel;
class UndoableProxyModel;
class KeyboardTreeView;
class KeycapTreeView : public QTreeView
{
    Q_OBJECT

public:
    KeycapTreeView(QWidget* pParent = nullptr);
    ~KeycapTreeView();

    void setKeyboardProperties(KeyboardTreeView* pTreeView);

private slots:
    void onKeyboardPropertiesSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:
    void updateRootIndexFromSelection(const QItemSelection& selected = QItemSelection(), const QItemSelection& deselected = QItemSelection());

private:
    DiffModel* _pDiffModel;
    UndoableProxyModel* _pUndoableKeyboardModel;
    QItemSelectionModel* _pItemSelectionModel;
};
