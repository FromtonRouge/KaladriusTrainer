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

#include <QtWidgets/QTreeView>

class TextsTreeView : public QTreeView
{
    Q_OBJECT

signals:
    void sendText(int iTextId) const;
    void importRequest(const QStringList& files) const;

public:
    TextsTreeView(QWidget* pParent = nullptr);
    ~TextsTreeView();

protected:
    void mouseDoubleClickEvent(QMouseEvent* pEvent) override;
    void contextMenuEvent(QContextMenuEvent*) override;
    void dragEnterEvent(QDragEnterEvent* pEvent) override;
    void dragMoveEvent(QDragMoveEvent* pEvent) override;
    void dropEvent(QDropEvent* pEvent) override;

private slots:
    void enableSelection();
    void disableSelection();
    void sendToLearnWindow();
    void removeSelection();
    void removeAll();

private:
    QStringList getFilesFromMimeData(const QMimeData* pMimeData) const;

private:
    QAction* _pActionEnable = nullptr;
    QAction* _pActionDisable = nullptr;
    QAction* _pActionSendToLearnTab = nullptr;
    QAction* _pActionRemove = nullptr;
    QAction* _pActionRemoveAll = nullptr;
};
