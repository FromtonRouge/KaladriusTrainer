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

#pragma once

#include <QtGui/QStandardItemModel>

class TextsModel : public QStandardItemModel
{
public:
    TextsModel(QObject* pParent = nullptr);

    void reset();

    bool hasChildren(const QModelIndex &parent) const override;
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;
    QVariant headerData(int iSection, Qt::Orientation orientation, int iRole) const override;
    QVariant data(const QModelIndex &index, int iRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int iRole) override;
    bool removeRows(int iRow, int iCount, const QModelIndex &parent) override;
};

