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
#include <QtCore/QSet>
#include <QtCore/QHash>

class DiffModel : public QIdentityProxyModel
{
    Q_OBJECT

public:
    DiffModel(QObject* pParent = nullptr);
    ~DiffModel();

    virtual void setSourceModel(QAbstractItemModel* pSourceModel) override;
    virtual bool setData(const QModelIndex& index, const QVariant& value, int iRole) override;

    void clearMapping();
    bool hasMapping() const {return !_sourceValuesByPath.isEmpty();}

    const QModelIndexList& getSourceIndexes() const {return _sourceIndexes;}
    void addSourceIndex(const QModelIndex& sourceIndex);
    void removeSourceIndex(const QModelIndex& sourceIndex);

    bool hasDifferentValues(const QModelIndex& proxyIndex) const;

private slots:
    void onSourceModelAboutToBeReset();

private:
    typedef QStringList Path;
    typedef QHash<Path, QSet<QModelIndex>> SourcesValueByPath;
    typedef QHash<QModelIndex, Path> PathBySourceValue;
    SourcesValueByPath _sourceValuesByPath;
    PathBySourceValue _pathBySourceValue;
    QModelIndexList _sourceIndexes;
};
