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
#include <QtCore/QRegularExpression>
#include <QtCore/QSettings>

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

    loadProgression();
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
        const QRegularExpression re("(\r\n)|(\n)|([ \t])");
        _words = sWords.split(re);
    }
}

QStringList LevelTreeItem::getCurrentWords() const
{
    return getWords(0, _uiProgression);
}

QStringList LevelTreeItem::getWords(uint16_t uiMin, uint16_t uiMax) const
{
    Q_ASSERT(uiMin < uiMax);
    QStringList result;
    const uint16_t uiBound = qMin<uint16_t>(uiMax, uint16_t(_words.size()));
    for (uint16_t ui = uiMin; ui < uiBound; ++ui)
    {
        result << _words[ui];
    }
    return result;
}

QStringList LevelTreeItem::getRandomWords() const
{
    QStringList result;
    if (!_words.isEmpty())
    {
        const uint16_t uiBound = qMin<uint16_t>(_uiProgression , uint16_t(_words.size()));
        for (uint16_t i=0; i<_uiRandomWordsCount; i++)
        {
            QString sWord;

            // Note: avoid empty words and repetitions
            do
            {
                const int iIndex = QRandomGenerator::global()->bounded(uiBound);
                sWord = _words[iIndex];
            } while (sWord.isEmpty() || (!result.isEmpty() && !sWord.isEmpty() && sWord == result.back()));

            result << sWord;
        }
    }
    return result;
}

void LevelTreeItem::setProgression(uint16_t uiProgression)
{
    _uiProgression = qBound<uint16_t>(5, uiProgression, _words.count());
}

float LevelTreeItem::getProgressionPercentage() const
{
    const int iTotalWords = _words.count();
    return iTotalWords == 0 ? 0 : 100.f*float(_uiProgression)/iTotalWords;
}

void LevelTreeItem::loadProgression()
{
    _uiProgression = QSettings().value(QString("Levels/%1/progress").arg(_uuid.toString()), _uiProgression).toUInt();
}

void LevelTreeItem::saveProgression() const
{
    QSettings().setValue(QString("Levels/%1/progress").arg(_uuid.toString()), _uiProgression);
}
