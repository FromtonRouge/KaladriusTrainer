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

#include "MainTabWindow.h"
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QUndoStack>
#include <QtWidgets/QAction>

MainTabWindow::MainTabWindow(QWidget* pParent)
    : QMainWindow(pParent)
{
    setDockOptions(dockOptions() | DockOption::GroupedDragging | DockOption::AllowNestedDocks);
    setWindowFlags(Qt::Widget);
}

QTabWidget* MainTabWindow::getParentTabWidget() const
{
    QObject* pParent = parent();
    while (pParent && pParent->metaObject()->className() != QTabWidget::staticMetaObject.className())
    {
        pParent = pParent->parent();
    }
    return qobject_cast<QTabWidget*>(pParent);
}

QAction* MainTabWindow::createUndoAction(QUndoStack* pUndoStack)
{
    QAction* pUndoAction = pUndoStack->createUndoAction(this);
    pUndoAction->setIcon(QIcon(":/Icons/arrow-curve-180-left.png"));
    pUndoAction->setShortcut(QKeySequence("Ctrl+Z"));
    return pUndoAction;
}

QAction* MainTabWindow::createRedoAction(QUndoStack* pUndoStack)
{
    QAction* pRedoAction = pUndoStack->createRedoAction(this);
    pRedoAction->setIcon(QIcon(":/Icons/arrow-curve.png"));
    pRedoAction->setShortcut(QKeySequence("Ctrl+Y"));
    return pRedoAction;
}

void MainTabWindow::onUndoCleanChanged(bool bClean)
{
    auto pTabWidget = getParentTabWidget();
    if (pTabWidget)
    {
        const int iTabIndex = pTabWidget->indexOf(qobject_cast<QWidget*>(parent()));
        if (iTabIndex != -1)
        {
            QString sWindowTitle = windowTitle();
            pTabWidget->setTabText(iTabIndex, sWindowTitle.replace("[*]", bClean ? "":"*"));
        }
    }
}
