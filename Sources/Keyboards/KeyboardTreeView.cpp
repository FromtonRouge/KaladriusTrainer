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

#include "KeyboardTreeView.h"
#include "Main/Application.h"
#include "UndoableKeyboardModel.h"
#include "Theories/TheoryModel.h"
#include "KeyboardModel.h"
#include "Keycaps/KeycapDelegate.h"
#include "KeyboardGraphicsScene.h"
#include "Keycaps/KeycapGraphicsItem.h"
#include "Serialization/Serialize.h"
#include "TreeItems/TreeItem.h"
#include "TreeItems/DictionaryTreeItem.h"
#include "TreeItems/LinkedTheoryTreeItem.h"
#include "TreeItems/LinkedDictionaryTreeItem.h"
#include "TreeItems/ListTreeItem.h"
#include "TreeItems/ArrayTreeItem.h"
#include "TreeItems/AttributeValueTreeItem.h"
#include "ValueTypes/KeycapRef.h"
#include "ValueTypes/ArrayValue.h"
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>
#include <QtWidgets/QUndoStack>
#include <QtGui/QContextMenuEvent>
#include <QtCore/QItemSelectionModel>
#include <functional>

KeyboardTreeView::KeyboardTreeView(QWidget* pParent)
    : QTreeView(pParent)
{
    setAcceptDrops(true);
    setAlternatingRowColors(true);
    setSelectionMode(ExtendedSelection);
    setEditTriggers(AllEditTriggers);
    header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    delete itemDelegate();
    setItemDelegate(new KeycapDelegate(this));

    _pActionLinkTheory = new QAction(QIcon(":/Icons/chain.png"), tr("Link Theory"), this);
    connect(_pActionLinkTheory, SIGNAL(triggered()), this, SLOT(onLinkTheory()));
    addAction(_pActionLinkTheory);

    _pActionRelabelLinkedKeys = new QAction(QIcon(":/Icons/keyboard.png"), tr("Relabel Linked Keys"), this);
    connect(_pActionRelabelLinkedKeys, SIGNAL(triggered()), this, SLOT(onRelabelLinkedKeys()));
    addAction(_pActionRelabelLinkedKeys);

    _pActionAdd = new QAction(QIcon(":/Icons/plus.png"), tr("Add"), this);
    connect(_pActionAdd, SIGNAL(triggered()), this, SLOT(onAdd()));
    _pActionAdd->setShortcut(QKeySequence::New);
    addAction(_pActionAdd);
    _pActionAdd->setDisabled(true);

    _pActionRemove = new QAction(QIcon(":/Icons/cross.png"), tr("Remove"), this);
    _pActionRemove->setShortcut(QKeySequence::Delete);
    connect(_pActionRemove, SIGNAL(triggered()), this, SLOT(onRemove()));
    addAction(_pActionRemove);
    _pActionRemove->setDisabled(true);
}

KeyboardTreeView::~KeyboardTreeView()
{
}

void KeyboardTreeView::setModel(QAbstractItemModel* pModel)
{
    if (model())
    {
        model()->disconnect(this);
    }

    QTreeView::setModel(pModel);

    if (pModel)
    {
        connect(pModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(onRowsInserted(QModelIndex,int,int)));
    }
}

void KeyboardTreeView::onGraphicsSceneSelectionChanged()
{
    auto pGraphicsScene = qobject_cast<KeyboardGraphicsScene*>(sender());
    if (pGraphicsScene)
    {
        QModelIndexList matches;
        const auto& selected = pGraphicsScene->selectedItems();
        if (selected.isEmpty())
        {
            selectionModel()->clearSelection();
            return;
        }

        for (auto pSelected : selected)
        {
            auto pKeycapItem = dynamic_cast<KeycapGraphicsItem*>(pSelected);
            if (pKeycapItem)
            {
                const QString& sKeycapId = pKeycapItem->elementId();
                const QModelIndex& start = model()->index(0, 0);
                matches << model()->match(start, Qt::DisplayRole, sKeycapId, 1, Qt::MatchExactly|Qt::MatchRecursive);
            }
        }

        if (!matches.isEmpty())
        {
            QItemSelection finalSelection;
            for (const auto& index : matches)
            {
                QItemSelection selection;
                selection.select(index, index);
                finalSelection.merge(selection, QItemSelectionModel::Select);
            }
            selectionModel()->select(finalSelection, QItemSelectionModel::ClearAndSelect);
        }
    }
}

void KeyboardTreeView::onRowsInserted(const QModelIndex& parent, int iFirst, int iLast)
{
    auto pUndoableKeyboardModel = qobject_cast<UndoableKeyboardModel*>(model());
    auto pKeyboardModel = qobject_cast<KeyboardModel*>(pUndoableKeyboardModel->sourceModel());
    bool bResizeColumn = false;
    for (int iRow = iFirst; iRow <= iLast; ++iRow)
    {
        if (!parent.isValid())
        {
            // Search for the Keycaps index
            const auto& sourceIndexKeycaps = pKeyboardModel->getKeycapsIndex();
            if (!sourceIndexKeycaps.isValid())
            {
                return;
            }

            bResizeColumn = true;

            expand(pUndoableKeyboardModel->index(iRow, 0, parent));
        }
    }

    if (bResizeColumn)
    {
        resizeColumnToContents(0);
    }
}

void KeyboardTreeView::onLinkTheory()
{
    const QModelIndex& current = currentIndex();
    if (current.isValid())
    {
        const QModelIndex& currentName = current.sibling(current.row(), 0);
        auto pUndoableKeyboardModel = qobject_cast<UndoableKeyboardModel*>(model());
        const int iInsertRow = pUndoableKeyboardModel->rowCount(currentName);

        auto pTheoryModel = qApp->getTheoryModel();
        auto pLinkedTheory = new LinkedTheoryTreeItem(pTheoryModel->getTheoryName());

        // Add key mappings for each dictionary
        auto pLinkedDictionaries = pLinkedTheory->getLinkedDictionaries();
        const QModelIndex& indexDictionaries = pTheoryModel->getDictionariesIndex();
        const int iDictionaries = pTheoryModel->rowCount(indexDictionaries);
        for (int iDictionary = 0; iDictionary < iDictionaries; ++iDictionary)
        {
            const QModelIndex& indexDictionary = pTheoryModel->index(iDictionary, 0, indexDictionaries);
            const QString& sDictionaryName = indexDictionary.data(Qt::DisplayRole).toString();
            auto pLinkedDictionary = new LinkedDictionaryTreeItem(sDictionaryName);

            // Add empty keys bindings
            auto pLinkedKeys = pLinkedDictionary->getLinkedKeys();
            auto pDictionary = static_cast<DictionaryTreeItem*>(pTheoryModel->itemFromIndex(indexDictionary));
            auto pKeys = pDictionary->getKeys();
            const int iKeys = pKeys->rowCount();
            for (int iKey = 0; iKey < iKeys; ++iKey)
            {
                pLinkedKeys->addElement(qVariantFromValue(KeycapRef()));
            }

            pLinkedDictionaries->appendRow({pLinkedDictionary, new EmptyTreeItem()});
        }

        const QByteArray& branch = Serialization::Save({pLinkedTheory});
        const QModelIndex& indexBranch = pUndoableKeyboardModel->insertBranch(iInsertRow, currentName, branch);
        Q_ASSERT(indexBranch.isValid());
        setCurrentIndex(indexBranch);
        expand(indexBranch);
    }
}

void KeyboardTreeView::onRemove()
{
    const QModelIndex& current = currentIndex();
    if (current.isValid())
    {
        const QModelIndex& currentName = current.sibling(current.row(), 0);
        auto pUndoableKeyboardModel = qobject_cast<UndoableKeyboardModel*>(model());
        pUndoableKeyboardModel->removeRow(currentName.row(), currentName.parent());
    }
}

void KeyboardTreeView::onAdd()
{
    const QModelIndex& current = currentIndex();
    if (current.isValid())
    {
        const QModelIndex& currentName = current.sibling(current.row(), 0);
        const QModelIndex& currentValue = current.sibling(current.row(), 1);
        auto pUndoableKeyboardModel = qobject_cast<UndoableKeyboardModel*>(model());

        QVariant variant;
        switch (currentName.data(TreeItemTypeRole).toInt())
        {
        case TreeItem::ArrayElement:
            {
                const QVariant& currentVariant = currentValue.data(Qt::EditRole);
                if (currentVariant.isValid())
                {
                    const int iType = currentVariant.userType();
                    if (iType == qMetaTypeId<ArrayValue>())
                    {
                        const auto& arrayValue = qvariant_cast<ArrayValue>(currentVariant);
                        variant = arrayValue.defaultValue;
                    }
                }
                break;
            }
        case TreeItem::Array:
            {
                variant = qVariantFromValue(ArrayValue("Special Keys", qVariantFromValue(KeycapRef())));
                break;
            }
        default:
            {
                return;
            }
        }

        if (variant.isValid())
        {
            auto pNewElement = new ArrayElementTreeItem();
            auto pNewElementValue = new AttributeValueTreeItem(variant);
            const QByteArray& branch = Serialization::Save({pNewElement, pNewElementValue});
            Q_ASSERT(!branch.isEmpty());
            const int iInsertRow = pUndoableKeyboardModel->rowCount(currentName);
            const QModelIndex& indexBranch = pUndoableKeyboardModel->insertBranch(iInsertRow, currentName, branch);
            Q_ASSERT(indexBranch.isValid());
            setCurrentIndex(indexBranch);
        }
    }
}

void KeyboardTreeView::onRelabelLinkedKeys()
{
    const QModelIndex& current = currentIndex();
    if (current.isValid())
    {
        auto pUndoableKeyboardModel = qobject_cast<UndoableKeyboardModel*>(model());
        std::function<void (const QModelIndex&)> visitor = [&visitor, pUndoableKeyboardModel](const QModelIndex& index)
        {
            if (index.isValid())
            {
                const QModelIndex& indexValue = index.sibling(index.row(), 1);
                const QVariant& variant = indexValue.data(Qt::EditRole);
                if (variant.isValid() && variant.userType() == qMetaTypeId<KeycapRef>())
                {
                    pUndoableKeyboardModel->setData(indexValue, variant);
                }

                const int iRows = pUndoableKeyboardModel->rowCount(index);
                for (int iRow = 0; iRow < iRows; ++iRow)
                {
                    visitor(pUndoableKeyboardModel->index(iRow, 0, index));
                }
            }
        };

        pUndoableKeyboardModel->getUndoStack()->beginMacro(tr("keycaps relinked"));
        visitor(current);
        pUndoableKeyboardModel->getUndoStack()->endMacro();
    }
}

void KeyboardTreeView::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
    QTreeView::currentChanged(current, previous);

    if (current.isValid())
    {
        const int iType = current.data(TreeItemTypeRole).toInt();
        switch (iType)
        {
        case TreeItem::Array:
            {
                _pActionRemove->setEnabled(false);
                _pActionAdd->setEnabled(true);
                _pActionLinkTheory->setEnabled(false);
                _pActionRelabelLinkedKeys->setEnabled(true);
                break;
            }
        case TreeItem::ArrayElement:
            {
                _pActionRemove->setEnabled(true);
                _pActionAdd->setEnabled(false);
                _pActionLinkTheory->setEnabled(false);
                _pActionRelabelLinkedKeys->setEnabled(false);

                // Check the value type of the element
                const QModelIndex& currentValue = current.sibling(current.row(), 1);
                const QVariant& value = currentValue.data(Qt::EditRole);
                if (value.isValid())
                {
                    const int iUserType = value.userType();
                    if (iUserType == qMetaTypeId<ArrayValue>())
                    {
                        _pActionAdd->setEnabled(true);
                        _pActionRelabelLinkedKeys->setEnabled(true);
                    }
                }
                break;
            }
        case TreeItem::LinkedTheories:
            {
                _pActionRemove->setEnabled(false);
                _pActionAdd->setEnabled(false);
                _pActionLinkTheory->setEnabled(true);
                _pActionRelabelLinkedKeys->setEnabled(false);
                break;
            }
        case TreeItem::LinkedTheory:
            {
                _pActionRemove->setEnabled(true);
                _pActionAdd->setEnabled(false);
                _pActionLinkTheory->setEnabled(false);
                _pActionRelabelLinkedKeys->setEnabled(false);
                break;
            }
        default:
            {
                _pActionRemove->setEnabled(false);
                _pActionAdd->setEnabled(false);
                _pActionLinkTheory->setEnabled(false);
                _pActionRelabelLinkedKeys->setEnabled(false);
                break;
            }
        }
    }
}

void KeyboardTreeView::contextMenuEvent(QContextMenuEvent* pEvent)
{
    QMenu menu;

    for (QAction* pAction : actions())
    {
        if (pAction->isEnabled())
        {
            menu.addAction(pAction);
        }
    }

    if (!menu.isEmpty())
    {
        menu.exec(pEvent->globalPos());
    }
}
