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

#include "StatisticsMainWindow.h"
#include "../Main/Application.h"
#include "../Levels/Models/LevelsModel.h"
#include "ui_StatisticsMainWindow.h"
#include <QtCore/QItemSelection>

StatisticsMainWindow::StatisticsMainWindow(QWidget *parent)
    : MainTabWindow(parent)
    , _pUi(new Ui::StatisticsMainWindow)
{
    _pUi->setupUi(this);

    auto pLevelsModel = qApp->getLevelsModel();
    _pUi->treeViewLevels->setModel(pLevelsModel);
    _pUi->treeViewLevels->expandAll();
    _pUi->treeViewLevels->resizeColumnToContents(0);
    _pUi->treeViewLevels->hideColumn(1);
    _pUi->treeViewLevels->setSelectionMode(QTreeView::ExtendedSelection);

    QItemSelectionModel* pItemSelectionModel = _pUi->treeViewLevels->selectionModel();
    connect(pItemSelectionModel, &QItemSelectionModel::selectionChanged, this, &StatisticsMainWindow::onLevelSelectionChanged);
}

StatisticsMainWindow::~StatisticsMainWindow()
{

}

void StatisticsMainWindow::Init()
{
    auto pLevelsModel = qApp->getLevelsModel();
    const QModelIndex& indexLevels = pLevelsModel->index(0, 0);
    if (indexLevels.isValid())
    {
        QItemSelectionModel* pItemSelectionModel = _pUi->treeViewLevels->selectionModel();
        const QModelIndex& indexFirstLevel = pLevelsModel->index(0, 0, indexLevels);
        pItemSelectionModel->select(indexFirstLevel, QItemSelectionModel::ClearAndSelect);
    }
}

void StatisticsMainWindow::onLevelSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
    if (!selected.isEmpty())
    {
        _pUi->widgetStatistics->addSelectedLevels(selected);
    }

    if (!deselected.isEmpty())
    {
        _pUi->widgetStatistics->removeDeselectedLevels(deselected);
    }
}

void StatisticsMainWindow::on_actionAbout_triggered()
{
    qApp->showAboutDialog();
}
