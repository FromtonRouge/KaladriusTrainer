// ======================================================================
// This file is a part of the KaladriusTrainer project
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

#include "LevelsTreeView.h"
#include "Models/LevelsModel.h"
#include "TreeItems/LevelTreeItem.h"
#include "../Tree/Models/ItemDataRole.h"
#include "../Tree/TreeItems/TreeItem.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtCore/QSettings>
#include <QtCore/QUuid>
#include <iostream>

LevelsTreeView::LevelsTreeView(QWidget* pParent)
    : QTreeView(pParent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

LevelsTreeView::~LevelsTreeView()
{

}

void LevelsTreeView::setModel(QAbstractItemModel* pModel)
{
    QTreeView::setModel(pModel);
    expandAll();
    resizeColumnToContents(0);

    QSettings settings;
    const int iLastSelectedLevel = settings.value("lastSelectedLevel", 0).toInt();
    const QModelIndex& indexLevels = pModel->index(0, 0);
    setCurrentIndex(pModel->index(iLastSelectedLevel, 0, indexLevels));
}

void LevelsTreeView::restart()
{
    const QModelIndex& current = currentIndex();
    if (current.isValid())
    {
        auto pLevelsModel = qobject_cast<LevelsModel*>(model());
        const int iTreeItemType = current.data(TreeItemTypeRole).toInt();
        switch (iTreeItemType)
        {
        case TreeItem::Level:
            {
                auto pLevelTreeItem = static_cast<LevelTreeItem*>(pLevelsModel->itemFromIndex(current));
                const QUuid& uuidLevel = pLevelTreeItem->getUuid();
                const QStringList& randomWords = pLevelTreeItem->getRandomWords();
                const QString& sText = randomWords.join(" ");
                emit sendText(sText);

                QSettings settings;
                settings.setValue("lastSelectedLevel", current.row());

                const QString& sTableName = QString("Level %1").arg(uuidLevel.toString(QUuid::WithoutBraces));
                const QSqlDatabase& db = QSqlDatabase::database();
                if (!db.tables().contains(sTableName))
                {
                    QString sCreateDatabase = "CREATE TABLE IF NOT EXISTS \"%1\" ("
                                              "\"Date\"	TEXT,"
                                              "\"Wpm\"	REAL,"
                                              "\"Spm\"	REAL,"
                                              "\"Accuracy\"	REAL"
                                              ");";

                    sCreateDatabase = sCreateDatabase.arg(sTableName);

                    QSqlQuery query(db);
                    if (!query.exec(sCreateDatabase))
                    {
                        QString sError = QString("Can't create table: %1").arg(query.lastError().text());
                        std::cerr << sError.toStdString() << std::endl;
                    }
                }

                emit restarted(sTableName);
                break;
            }
        default:
            {
                break;
            }
        }
    }
}

void LevelsTreeView::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
    QTreeView::currentChanged(current, previous);
    restart();
}
