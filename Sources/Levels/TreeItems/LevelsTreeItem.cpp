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

#include "LevelsTreeItem.h"
#include "LevelTreeItem.h"
#include <QtGui/QIcon>

LevelsTreeItem::LevelsTreeItem()
{
    setIcon(QIcon(":/Icons/controller.png"));
    setText(QObject::tr("Levels"));
    setEditable(false);

    appendRow({new LevelTreeItem(LevelTreeItem::TimedRandomWords, "4d685a55-f8f9-4584-b8de-4cce1012fa14",  QObject::tr("Short words"), ":/Words/first200ShortWords.txt"), new EmptyTreeItem()});
    appendRow({new LevelTreeItem(LevelTreeItem::PunctuationMarks, "a22eb342-841e-4099-84b7-5c38f4ce0584",  QObject::tr("Punctuation Marks"), ":/Words/first200ShortWords.txt"), new EmptyTreeItem()});
    appendRow({new LevelTreeItem(LevelTreeItem::TimedRandomWords, "ed2f7d01-6fc3-4fc1-bdbe-147cd278948c", QObject::tr("Medium words"), ":/Words/first200MediumWords.txt"), new EmptyTreeItem()});
    appendRow({new LevelTreeItem(LevelTreeItem::PunctuationMarks, "4da0b177-0bed-4f15-af80-f44e8042a8a6",  QObject::tr("Punctuation Marks"), ":/Words/first200MediumWords.txt"), new EmptyTreeItem()});
    appendRow({new LevelTreeItem(LevelTreeItem::TimedRandomWords, "389f8144-00f9-48ea-8192-45ebd542a3a7", QObject::tr("Short/Medium words"), ":/Words/first400ShortMediumWords.txt"), new EmptyTreeItem()});
    appendRow({new LevelTreeItem(LevelTreeItem::TimedRandomWords, "aae7e2e5-a877-485f-84e5-b896c6a61cc5", QObject::tr("Long words"), ":/Words/first200LongWords.txt"), new EmptyTreeItem()});
    appendRow({new LevelTreeItem(LevelTreeItem::PunctuationMarks, "ac64e2bf-4bab-4e4b-9b9b-6f82f5c5fb3d",  QObject::tr("Punctuation Marks"), ":/Words/first200LongWords.txt"), new EmptyTreeItem()});
    appendRow({new LevelTreeItem(LevelTreeItem::TimedRandomWords, "13e31fa2-1122-49e9-89fa-f7aaa4bb1a2d", QObject::tr("Short/Medium/Long words"), ":/Words/first600ShortMediumLongWords.txt"), new EmptyTreeItem()});
    appendRow({new LevelTreeItem(LevelTreeItem::PunctuationMarks, "50431849-061a-4984-b3c7-f4f168291fd5",  QObject::tr("Punctuation Marks"), ":/Words/first600ShortMediumLongWords.txt"), new EmptyTreeItem()});
    appendRow({new LevelTreeItem(LevelTreeItem::Text, "ab1575a2-c508-4565-8e25-3515c9e22aef",  QObject::tr("Text")), new EmptyTreeItem()});
}

LevelsTreeItem::~LevelsTreeItem()
{

}
