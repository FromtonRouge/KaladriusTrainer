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

#include <QtWidgets/QTreeView>

class KeyboardPropertiesTreeView;
class KeyboardPropertiesModel;
class KeycapPropertiesTreeView : public QTreeView
{
    Q_OBJECT

public:
    KeycapPropertiesTreeView(QWidget* pParent = nullptr);
    ~KeycapPropertiesTreeView();

    virtual void setModel(QAbstractItemModel* pModel) override;
    void setKeyboardProperties(KeyboardPropertiesTreeView* pTreeView);

private slots:
    void onKeyboardLoaded();
    void onKeyboardPropertiesSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:
    void updateRootIndexFromSelection(const QItemSelection& selected = QItemSelection(), const QItemSelection& deselected = QItemSelection());

private:
    KeyboardPropertiesModel* _pKeyboardPropertiesModel;
    QItemSelectionModel* _pItemSelectionModel;
};
