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

#include "UndoableKeyboardModel.h"
#include "ValueTypes/KeycapRef.h"
#include "KeyboardModel.h"
#include "Theories/TheoryModel.h"
#include "Main/Application.h"
#include "Models/ItemDataRole.h"
#include "Models/Utils.h"
#include "TreeItems/TreeItem.h"
#include <QtWidgets/QUndoStack>
#include <QtCore/QMimeData>
#include <QtCore/QByteArray>
#include <QtCore/QDataStream>

UndoableKeyboardModel::UndoableKeyboardModel(QObject* pParent)
    : UndoableProxyModel(pParent)
{

}

bool UndoableKeyboardModel::setData(const QModelIndex& index, const QVariant& value, int iRole)
{
    switch (iRole)
    {
    case Qt::EditRole:
        {
            if (value.isValid() && value.userType() == qMetaTypeId<KeycapRef>())
            {
                // Get the keycap id from the keycap reference
                const auto& keycapRef = qvariant_cast<KeycapRef>(value);
                const QString& sKeycapId = keycapRef.keycapId;
                auto pSourceModel = qobject_cast<KeyboardModel*>(sourceModel());
                const QModelIndex& proxyIndexKeycapsIndex = mapFromSource(pSourceModel->getKeycapsIndex());
                const auto* pTheoryModel = qApp->getTheoryModel();

                if (proxyIndexKeycapsIndex.isValid() && hasChildren(proxyIndexKeycapsIndex))
                {
                    // Check if the first parent is the linked theory item
                    const QModelIndex& parent = index.parent();
                    if (parent.data(TreeItemTypeRole).toInt() == TreeItem::ArrayElement)
                    {
                        const auto& matches = match(proxyIndexKeycapsIndex.child(0,0), Qt::DisplayRole, sKeycapId, 1, Qt::MatchExactly);
                        if (!matches.isEmpty())
                        {
                            const QModelIndex& proxyIndexKeycap = matches.front();
                            const QModelIndex& proxyIndexLabelValue = Utils::index(this, "Label", 1, proxyIndexKeycap);
                            if (proxyIndexLabelValue.isValid())
                            {
                                getUndoStack()->beginMacro(tr("1 keycap linked to theory"));

                                // Now get the key label in the theory model
                                const QModelIndex& indexSpecialKeys = Utils::index(pTheoryModel, "Special Keys", 0, pTheoryModel->getTheoryIndex());
                                if (indexSpecialKeys.isValid())
                                {
                                    const QModelIndex& parentArray = index.parent();
                                    const QModelIndex& indexName = parentArray.sibling(parentArray.row(), 0);
                                    const QString& sPath = QString("%1").arg(indexName.data().toString());
                                    const QModelIndex& indexKeyLabelInTheoryModel = Utils::index(pTheoryModel, sPath, 1, indexSpecialKeys);
                                    if (indexKeyLabelInTheoryModel.isValid())
                                    {
                                        setData(proxyIndexLabelValue, indexKeyLabelInTheoryModel.data().toString());
                                    }
                                }

                                UndoableProxyModel::setData(index, value, iRole);
                                getUndoStack()->endMacro();
                                return true;
                            }
                        }
                    }
                    else
                    {
                        // Check if we are under a linked dictionary branch
                        const QModelIndex& indexLinkedDictionary = Utils::findParent(index, TreeItem::LinkedDictionary);
                        if (indexLinkedDictionary.isValid())
                        {
                            const auto& matches = match(proxyIndexKeycapsIndex.child(0,0), Qt::DisplayRole, sKeycapId, 1, Qt::MatchExactly);
                            if (!matches.isEmpty())
                            {
                                const QModelIndex& proxyIndexKeycap = matches.front();
                                const QModelIndex& proxyIndexLabelValue = Utils::index(this, "Label", 1, proxyIndexKeycap);
                                if (proxyIndexLabelValue.isValid())
                                {
                                    getUndoStack()->beginMacro(tr("1 keycap linked to theory"));

                                    // Now get the key label in the theory model
                                    const QModelIndex& indexDictionariesInTheoryModel = pTheoryModel->getDictionariesIndex();
                                    if (indexDictionariesInTheoryModel.isValid())
                                    {
                                        const QString& sLinkedDictionaryName = indexLinkedDictionary.data().toString();
                                        const QModelIndex& indexName = index.sibling(index.row(), 0);
                                        const QString& sPath = QString("%1/Keys/%2").arg(sLinkedDictionaryName).arg(indexName.data().toString());
                                        const QModelIndex& indexKeyLabelInTheoryModel = Utils::index(pTheoryModel, sPath, 1, indexDictionariesInTheoryModel);
                                        if (indexKeyLabelInTheoryModel.isValid())
                                        {
                                            setData(proxyIndexLabelValue, indexKeyLabelInTheoryModel.data().toString());
                                        }
                                    }

                                    UndoableProxyModel::setData(index, value, iRole);
                                    getUndoStack()->endMacro();
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
            break;
        }
    default:
        {
            break;
        }
    }

    return UndoableProxyModel::setData(index, value, iRole);
}

bool UndoableKeyboardModel::dropMimeData(const QMimeData* pMimeData, Qt::DropAction action, int iRow, int iColumn, const QModelIndex& parent)
{
    if (pMimeData->hasFormat("application/prs.stenotutor.keycapid") && parent.isValid())
    {
        QByteArray data = pMimeData->data("application/prs.stenotutor.keycapid");
        QDataStream stream(&data, QIODevice::ReadOnly);
        QString sKeycapId;
        stream >> sKeycapId;
        setData(parent, qVariantFromValue(KeycapRef(sKeycapId)));
    }
    return UndoableProxyModel::dropMimeData(pMimeData, action, iRow, iColumn, parent);
}
