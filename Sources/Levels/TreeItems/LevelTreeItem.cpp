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
#include "../../Tree/Models/ItemDataRole.h"
#include "../../Main/Application.h"
#include "../../Database/Database.h"
#include <QtSql/QSqlQuery>
#include <QtCore/QRandomGenerator>
#include <QtCore/QFile>
#include <QtCore/QRegularExpression>
#include <QtCore/QSettings>
#include <QtCore/QDebug>

LevelTreeItem::LevelTreeItem(LevelType levelType,
                             const QUuid& uuid,
                             const QString& sLabel,
                             const QString& sWordsFilePath)
    : _uuid(uuid)
    , _sWordsFilePath(sWordsFilePath)
    , _levelType(levelType)
{
    setText(sLabel);
    setData(QString("Level %1").arg(_uuid.toString(QUuid::WithoutBraces)), LevelTableNameRole);
    setData(QString("Words %1").arg(_uuid.toString(QUuid::WithoutBraces)), LevelWordsTableNameRole);

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
}

LevelTreeItem::~LevelTreeItem()
{

}

void LevelTreeItem::loadWords()
{
    _words.clear();

    QFile file(_sWordsFilePath);
    if (file.open(QIODevice::ReadOnly))
    {
        const QString& sWords = file.readAll();
        const QRegularExpression re("(\r\n)|(\n)|([ \t])");
        _words = sWords.split(re);

        // Insert words in database if empty
        const QString& sLevelWordsTableName = data(LevelWordsTableNameRole).toString();

        auto pDatabase = qApp->getDatabase();
        if (pDatabase->getCount(sLevelWordsTableName) == 0)
        {
            for (const QString& sWord : _words)
            {
                if (!sWord.isEmpty())
                {
                    QMap<QString, QVariant> values;
                    values["Word"] = sWord;
                    values["Progression"] = -1;
                    values["Occurences"] = 0;
                    values["AverageErrorsCount"] = 0.f;
                    values["AverageChordsCount"] = 0.f;
                    values["AverageTimeSpentForFirstStroke"] = 0.f;
                    values["AverageTimeSpentToComplete"] = 0.f;
                    pDatabase->insertValues(sLevelWordsTableName, values);
                }
            }
        }
    }
}

QStringList LevelTreeItem::get5WordsToPractice(QVector<int>* pProgressValues) const
{
    if (pProgressValues)
    {
        pProgressValues->clear();
    }

    // Get 5 words with a Progression < 100
    auto pDatabase = qApp->getDatabase();
    QStringList wordsToPractice;
    const QString& sLevelWordsTableName = data(LevelWordsTableNameRole).toString();

    QString sQuery = QString("SELECT Word, Progression FROM \"%1\" WHERE Progression < 100 LIMIT 5").arg(sLevelWordsTableName);
    QSqlQuery query = pDatabase->execute(sQuery);
    while (query.next())
    {
        wordsToPractice << query.value(0).toString();

        if (pProgressValues)
        {
            (*pProgressValues) << query.value(1).toInt();
        }
    }

    if (wordsToPractice.size() < 5)
    {
        QString sQuery = QString("SELECT Word, Progression FROM \"%1\" WHERE Progression == 100 ORDER BY AverageTimeSpentToComplete/AverageChordsCount DESC LIMIT %2").arg(sLevelWordsTableName).arg(5-wordsToPractice.size());
        QSqlQuery query = pDatabase->execute(sQuery);
        while (query.next())
        {
            wordsToPractice << query.value(0).toString();

            if (pProgressValues)
            {
                (*pProgressValues) << query.value(1).toInt();
            }
        }
    }

    return wordsToPractice;
}

QStringList LevelTreeItem::getRandomWords() const
{
    QStringList result;

    auto pDatabase = qApp->getDatabase();
    const QString& sLevelWordsTableName = data(LevelWordsTableNameRole).toString();
    const int iTotalWords = pDatabase->getCount(sLevelWordsTableName);

    // Get words to practice
    const QStringList& wordsToPractice = get5WordsToPractice();

    // Get words we don't practice often with a Progression == 100
    QStringList otherWordsToPractice;
    QString sQuery = QString("SELECT Word FROM \"%1\" WHERE Progression == 100 ORDER BY Occurences ASC LIMIT %2").arg(sLevelWordsTableName).arg(iTotalWords/3);
    QSqlQuery query = pDatabase->execute(sQuery);
    while (query.next())
    {
        otherWordsToPractice << query.value(0).toString();
    }

    auto getRandomWord = [](const QStringList& words) -> QString
    {
        if (!words.isEmpty())
        {
            return words[QRandomGenerator::global()->bounded(words.size())];
        }
        return QString();
    };

    for (uint16_t i=0; i<_uiRandomWordsCount; i++)
    {
        QString sWord;

        // Note: avoid empty words and repetitions
        do
        {
            // Get a random number between 1 and 100
            const int iDice = QRandomGenerator::global()->bounded(1, 101);
            if (iDice >=1 && iDice <= 50)
            {
                sWord = getRandomWord(wordsToPractice);
            }
            else
            {
                sWord = getRandomWord(otherWordsToPractice);
            }

        } while (sWord.isEmpty() || (!result.isEmpty() && !sWord.isEmpty() && sWord == result.back()));

        result << sWord;
    }
    return result;
}

float LevelTreeItem::getProgressionPercentage() const
{
    auto pDatabase = qApp->getDatabase();

    const QString& sLevelWordsTableName = data(LevelWordsTableNameRole).toString();
    const int iTotal = pDatabase->getCount(sLevelWordsTableName);
    if (iTotal)
    {
        const QString sQuery = QString("SELECT COUNT(*) FROM \"%1\" WHERE Progression == 100").arg(sLevelWordsTableName);
        QSqlQuery query = pDatabase->execute(sQuery);
        if (query.next())
        {
            return 100.f*float(query.value(0).toInt())/iTotal;
        }
    }
    return 0.f;
}
