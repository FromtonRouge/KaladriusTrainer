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

#include <QtCore/QIdentityProxyModel>

class DictionariesModel : public QIdentityProxyModel
{
    Q_OBJECT

public:
    DictionariesModel(QObject* pParent = nullptr);
    ~DictionariesModel();

    virtual void setSourceModel(QAbstractItemModel* pSourceModel) override;
    virtual bool hasChildren(const QModelIndex& parent) const override;
    virtual int rowCount(const QModelIndex& parent) const override;
    virtual QModelIndex index(int iRow, int iColumn, const QModelIndex& parent) const override;
    virtual QVariant headerData(int iSection, Qt::Orientation orientation, int iRole = Qt::DisplayRole) const override;

private:
    QPersistentModelIndex _sourceIndexDictionaries;
};

