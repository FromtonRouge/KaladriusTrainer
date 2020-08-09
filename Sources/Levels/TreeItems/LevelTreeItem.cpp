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

#include "LevelTreeItem.h"
#include <QtCore/QRandomGenerator>
#include <QtCore/QFile>

LevelTreeItem::LevelTreeItem(LevelType levelType,
                             const QUuid& uuid,
                             const QString& sLabel,
                             const QString& sWordsFilePath)
    : _levelType(levelType)
    , _uuid(uuid)
{
    setText(sLabel);

    QString sIconPath;
    switch (_levelType)
    {
    case TimedRandomWords:
        {
            sIconPath = ":/Icons/clock--arrow.png";
            break;
        }
    case FreeText:
        {
            sIconPath = ":/Icons/document-text.png";
            break;
        }
    default:
        {
            break;
        }
    }

    setIcon(QIcon(sIconPath));
    setEditable(false);

    loadWords(sWordsFilePath);
}

LevelTreeItem::~LevelTreeItem()
{

}

void LevelTreeItem::loadWords(const QString& sWordsFilePath)
{
    _words.clear();

    QFile file(sWordsFilePath);
    if (file.open(QIODevice::ReadOnly))
    {
        const QString& sWords = file.readAll();
        _words = sWords.split('\n');
    }
}

QStringList LevelTreeItem::getRandomWords(int iCount) const
{
    QStringList result;
    if (!_words.isEmpty())
    {
        // Get 600 words it should be enough
        for (int i=0; i<iCount; i++)
        {
            const int iIndex = QRandomGenerator::global()->bounded(_words.size());
            result << _words[iIndex];
        }
    }
    return result;
}
