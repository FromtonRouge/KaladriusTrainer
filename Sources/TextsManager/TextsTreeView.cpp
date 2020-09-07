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

#include "TextsTreeView.h"
#include "../Database/Database.h"
#include "../Main/Application.h"
#include <QtWidgets/QAction>
#include <QtGui/QKeySequence>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QMenu>
#include <QtCore/QVector>
#include <QtCore/QMimeData>
#include <QtCore/QPersistentModelIndex>
#include <QtCore/QItemSelectionModel>
#include <QtCore/QFileInfo>
#include <QtCore/QUrl>
#include <QtCore/QDebug>

TextsTreeView::TextsTreeView(QWidget* pParent)
    : QTreeView(pParent)
{
    setAcceptDrops(true);
    setSelectionMode(QTreeView::ContiguousSelection);

    _pActionEnable = new QAction(tr("Enable"));
    addAction(_pActionEnable);
    connect(_pActionEnable, &QAction::triggered, this, &TextsTreeView::enableSelection);

    _pActionDisable = new QAction(tr("Disable"));
    addAction(_pActionDisable);
    connect(_pActionDisable, &QAction::triggered, this, &TextsTreeView::disableSelection);

    _pActionSendToLearnTab = new QAction(QIcon(":/Icons/ui-tab--arrow.png"), tr("Send to Learn Tab"));
    addAction(_pActionSendToLearnTab);
    connect(_pActionSendToLearnTab, &QAction::triggered, this, &TextsTreeView::sendToLearnWindow);

    _pActionRemove = new QAction(QIcon(":/Icons/cross.png"), tr("Remove"));
    _pActionRemove->setShortcut(Qt::Key_Delete);
    addAction(_pActionRemove);
    connect(_pActionRemove, &QAction::triggered, this, &TextsTreeView::removeSelection);

    _pActionRemoveAll = new QAction(QIcon(":/Icons/cross.png"), tr("Remove All"));
    addAction(_pActionRemoveAll);
    connect(_pActionRemoveAll, &QAction::triggered, this, &TextsTreeView::removeAll);
}

TextsTreeView::~TextsTreeView()
{

}

void TextsTreeView::mouseDoubleClickEvent(QMouseEvent* pEvent)
{
    QTreeView::mouseDoubleClickEvent(pEvent);

    const QModelIndex& index = indexAt(pEvent->pos());
    if (index.isValid() && index.parent().isValid())
    {
        emit sendText(index.data(Qt::UserRole + 1).toInt());
    }
}

void TextsTreeView::contextMenuEvent(QContextMenuEvent*)
{
    QMenu menu;

    if (selectedIndexes().isEmpty())
    {
        if (model()->rowCount())
        {
            menu.addAction(_pActionRemoveAll);
        }
    }
    else
    {
        menu.addAction(_pActionEnable);
        menu.addAction(_pActionDisable);

        if (currentIndex().parent().isValid())
        {
            menu.addSeparator();
            menu.addAction(_pActionSendToLearnTab);
        }

        menu.addSeparator();
        menu.addAction(_pActionRemove);
    }

    if (!menu.isEmpty())
    {
        menu.exec(QCursor::pos());
    }
}

void TextsTreeView::dragEnterEvent(QDragEnterEvent* pEvent)
{
    const QStringList& files = getFilesFromMimeData(pEvent->mimeData());
    if (files.isEmpty() == false)
    {
        pEvent->acceptProposedAction();
        return;
    }

    QTreeView::dragEnterEvent(pEvent);
}

void TextsTreeView::dragMoveEvent(QDragMoveEvent* pEvent)
{
    const QStringList& files = getFilesFromMimeData(pEvent->mimeData());
    if (files.isEmpty() == false)
    {
        pEvent->acceptProposedAction();
        return;
    }

    QTreeView::dragMoveEvent(pEvent);
}

void TextsTreeView::dropEvent(QDropEvent* pEvent)
{
    const QStringList& files = getFilesFromMimeData(pEvent->mimeData());
    if (files.isEmpty() == false)
    {
        emit importRequest(files);
        return;
    }

    QTreeView::dropEvent(pEvent);
}

void TextsTreeView::enableSelection()
{
    const QModelIndexList& indexes = selectedIndexes();
    for (const QModelIndex& index : indexes)
    {
        model()->setData(index, Qt::Checked, Qt::CheckStateRole);
    }
}

void TextsTreeView::disableSelection()
{
    const QModelIndexList& indexes = selectedIndexes();
    for (const QModelIndex& index : indexes)
    {
        model()->setData(index, Qt::Unchecked, Qt::CheckStateRole);
    }
}

void TextsTreeView::sendToLearnWindow()
{
    const QModelIndex& current = currentIndex();
    if (current.isValid() && current.parent().isValid())
    {
        emit sendText(current.data(Qt::UserRole + 1).toInt());
    }
}

void TextsTreeView::removeSelection()
{
    QItemSelectionModel* pSelectionModel = selectionModel();
    for (const QItemSelectionRange& selectionRange : pSelectionModel->selection())
    {
        model()->removeRows(selectionRange.top(), selectionRange.height(), selectionRange.parent());
    }
}

void TextsTreeView::removeAll()
{
    auto pModel = model();
    const int iRows = pModel->rowCount();
    if (iRows)
    {
        pModel->removeRows(0, iRows);
    }
}

QStringList TextsTreeView::getFilesFromMimeData(const QMimeData* pMimeData) const
{
    QStringList files;
    if (!pMimeData || !pMimeData->hasUrls())
    {
        return files;
    }

    const QList<QUrl>& urls = pMimeData->urls();
    for (const QUrl& url : urls)
    {
        const QString& sUrl = url.toLocalFile();
        QFileInfo file(sUrl);
        if (file.exists())
        {
            if (file.isDir())
            {
                files << sUrl;
            }
            else if (file.suffix() == "txt")
            {
                files << sUrl;
            }
        }
    }
    return files;
}
