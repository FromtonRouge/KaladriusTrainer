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

#include "LevelsTreeView.h"
#include "Models/LevelsModel.h"
#include "TreeItems/LevelTreeItem.h"
#include "../Tree/Models/ItemDataRole.h"
#include "../Tree/TreeItems/TreeItem.h"
#include "../Main/Application.h"
#include "../Database/Database.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtCore/QSettings>
#include <QtCore/QUuid>
#include <QtCore/QRandomGenerator>
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

                auto pDatabase = qApp->getDatabase();
                pDatabase->createLevelWordsTable(current.data(LevelWordsTableNameRole).toString());

                QString sText;
                const int iLevelType = pLevelTreeItem->getLevelType();
                switch (iLevelType)
                {
                case LevelTreeItem::Text:
                    {
                        emit sendWordsToPractice({}, {});
                        sText = "It can also be argued that DNA is nothing more than a program designed to preserve itself. Life has become more complex in the overwhelming sea of information. And life, when organized into species, relies upon genes to be its memory system. So, man is an individual only because of his intangible memory... and memory cannot be defined, but it defines mankind. The advent of computers, and the subsequent accumulation of incalculable data has given rise to a new system of memory and thought parallel to your own. Humanity has underestimated the consequences of computerization.";
                        break;
                    }
                case LevelTreeItem::TimedRandomWords:
                case LevelTreeItem::PunctuationMarks:
                    {
                        pLevelTreeItem->loadWords();
                        QVector<int> progressValues;
                        const QStringList& wordsToPractice = pLevelTreeItem->get5WordsToPractice(&progressValues);
                        emit sendWordsToPractice(wordsToPractice, progressValues);

                        const QStringList& randomWords = pLevelTreeItem->getRandomWords();

                        if (iLevelType == LevelTreeItem::PunctuationMarks)
                        {
                            const QStringList punctuationMarks = {".", "...", "?", "!", ",", ":", ";"};

                            QStringList finalRandomWords;
                            int iPreviousRandomIndex = 0;
                            for (QString sWord : randomWords)
                            {
                                QString sPunctuation;
                                const int iFortune = QRandomGenerator::global()->bounded(1, 101);
                                int iRandomIndex = -1;

                                if (iFortune >= 1 && iFortune <= 25)
                                {
                                    iRandomIndex = QRandomGenerator::global()->bounded(punctuationMarks.size());
                                    sPunctuation = punctuationMarks[iRandomIndex];
                                }

                                if (iPreviousRandomIndex >= 0 && iPreviousRandomIndex <= 3)
                                {
                                    sWord.replace(0, 1, sWord[0].toUpper());
                                }

                                finalRandomWords << (sWord + sPunctuation);
                                iPreviousRandomIndex = iRandomIndex;
                            }

                            sText = finalRandomWords.join(" ");
                        }
                        else
                        {
                            sText = randomWords.join(" ");
                        }
                        break;
                    }
                default:
                    {
                        break;
                    }
                }

                emit sendText(sText);

                QSettings settings;
                settings.setValue("lastSelectedLevel", current.row());

                const QString& sLevelTableName = current.data(LevelTableNameRole).toString();
                pDatabase->createLevelTable(sLevelTableName);
                emit restarted(sLevelTableName);
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
