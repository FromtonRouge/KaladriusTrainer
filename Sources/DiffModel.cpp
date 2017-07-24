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

#include "DiffModel.h"
#include <functional>

DiffModel::DiffModel(QObject* pParent)
    : QIdentityProxyModel(pParent)
{

}

DiffModel::~DiffModel()
{

}

bool DiffModel::setData(const QModelIndex& index, const QVariant& value, int iRole)
{
    const QModelIndex& sourceIndex = mapToSource(index);
    if (hasMapping() && sourceIndex.isValid())
    {
        auto itPath = _pathBySourceValue.find(sourceIndex);
        if (itPath != _pathBySourceValue.end())
        {
            auto itSourceValues = _sourceValuesByPath.find(itPath.value());
            if (itSourceValues != _sourceValuesByPath.end())
            {
                auto pSourceModel = sourceModel();
                const auto& sourceValues = itSourceValues.value();
                for (const auto& sourceIndex : sourceValues)
                {
                    pSourceModel->setData(sourceIndex, value, iRole);
                }
            }
        }
        return true;
    }

    return QIdentityProxyModel::setData(index, value, iRole);
}

void DiffModel::clearMapping()
{
    _sourceIndexes.clear();
    _sourceValuesByPath.clear();
    _pathBySourceValue.clear();
}

void DiffModel::addSourceIndex(const QModelIndex& sourceIndex)
{
    _sourceIndexes << sourceIndex;
    Path path;
    auto pSourceModel = sourceModel();
    std::function<void (const QModelIndex&)> visitor = [&](const QModelIndex& parent)
    {
        path.push_back(parent.data().toString());

        // Visit children
        const int iRows = pSourceModel->rowCount(parent);
        for (int iRow = 0; iRow < iRows; ++iRow)
        {
            visitor(pSourceModel->index(iRow, 0, parent));
        }

        // Get the editable index on the 2nd column (hard coded at the moment)
        const QModelIndex& parentValue = parent.sibling(parent.row(), 1);
        if (parentValue.flags().testFlag(Qt::ItemIsEditable))
        {
            _sourceValuesByPath[path].insert(parentValue);
            _pathBySourceValue.insert(parentValue, path);
        }

        path.pop_back();
    };

    const int iRows = pSourceModel->rowCount(sourceIndex);
    for (int iRow = 0; iRow < iRows; ++iRow)
    {
        visitor(pSourceModel->index(iRow, 0, sourceIndex));
    }
}

void DiffModel::removeSourceIndex(const QModelIndex& sourceIndex)
{
    _sourceIndexes.removeOne(sourceIndex);

    Path path;
    auto pSourceModel = sourceModel();
    std::function<void (const QModelIndex&)> visitor = [&](const QModelIndex& parent)
    {
        path.push_back(parent.data().toString());

        // Visit children
        const int iRows = pSourceModel->rowCount(parent);
        for (int iRow = 0; iRow < iRows; ++iRow)
        {
            visitor(pSourceModel->index(iRow, 0, parent));
        }

        // Get the editable index on the 2nd column (hard coded at the moment)
        const QModelIndex& parentValue = parent.sibling(parent.row(), 1);
        if (parentValue.flags().testFlag(Qt::ItemIsEditable))
        {
            auto itSourceValues = _sourceValuesByPath.find(path);
            if (itSourceValues != _sourceValuesByPath.end())
            {
                itSourceValues.value().remove(parentValue);
                _pathBySourceValue.remove(parentValue);
                if (itSourceValues.value().isEmpty())
                {
                    _sourceValuesByPath.remove(path);
                }
            }
        }

        path.pop_back();
    };

    const int iRows = pSourceModel->rowCount(sourceIndex);
    for (int iRow = 0; iRow < iRows; ++iRow)
    {
        visitor(pSourceModel->index(iRow, 0, sourceIndex));
    }
}

bool DiffModel::hasDifferentValues(const QModelIndex& proxyIndex) const
{
    // Optimization needed: we should do the diff in addSourceIndex() and removeSourceIndex()
    // and we should just return the result of the diff here
    const QModelIndex& sourceIndex = mapToSource(proxyIndex);
    auto itPath = _pathBySourceValue.find(sourceIndex);
    if (itPath != _pathBySourceValue.end())
    {
        auto itSourceValues = _sourceValuesByPath.find(itPath.value());
        if (itSourceValues != _sourceValuesByPath.end())
        {
            const auto& sourceIndexes = itSourceValues.value();
            for (const auto& otherIndex : sourceIndexes)
            {
                if (otherIndex == sourceIndex)
                {
                    continue;
                }

                const QVariant& v1 = sourceIndex.data(Qt::EditRole);
                const QVariant& v2 = otherIndex.data(Qt::EditRole);

                // Note: for custom types don't forget to register comparators with
                // QMetaType::registerComparators<CustomType>()
                if (v1.type() != v2.type() || v1 != v2)
                {
                    return true;
                }
            }
        }
    }

    return false;
}
