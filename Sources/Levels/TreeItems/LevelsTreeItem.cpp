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

#include "LevelsTreeItem.h"
#include "LevelTreeItem.h"
#include <QtGui/QIcon>

LevelsTreeItem::LevelsTreeItem()
{
    setIcon(QIcon(":/Icons/controller.png"));
    setText(QObject::tr("Levels"));
    setEditable(false);

    appendRow({new LevelTreeItem(LevelTreeItem::TimedRandomWords, QObject::tr("Short words"), ":/Words/first200ShortWords.txt")});
    appendRow({new LevelTreeItem(LevelTreeItem::TimedRandomWords, QObject::tr("Medium words"), ":/Words/first200MediumWords.txt")});
    appendRow({new LevelTreeItem(LevelTreeItem::TimedRandomWords, QObject::tr("Long words"), ":/Words/first200LongWords.txt")});
}

LevelsTreeItem::~LevelsTreeItem()
{

}
