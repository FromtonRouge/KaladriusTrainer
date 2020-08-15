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

#include "TypingTestResult.h"
#include <QtCore/QDebug>

TypingTestResult::TypingTestResult()
{

}

TypingTestResult::~TypingTestResult()
{

}

void TypingTestResult::clear()
{
    wordData.clear();
    _iLastTimestamp = 0;
}

void TypingTestResult::compute()
{
    auto it = wordData.begin();
    while (it != wordData.end())
    {
        WordData& rWordData = it++.value();

        auto itLocation = rWordData.dataAtLocation.begin();
        while (itLocation != rWordData.dataAtLocation.end())
        {
            const int iLocation = itLocation.key();
            const DataAtLocation& dataAtLoc = itLocation++.value();
            if (dataAtLoc.chords.isEmpty())
            {
                Q_ASSERT(dataAtLoc.errors);
                // It's possible on errors only
                continue;
            }

            rWordData.averageChordsCount += dataAtLoc.chords.count();
            rWordData.averageErrorsCount += dataAtLoc.errors;

            // Compute time to chord for each chord
            qint64 iTimestamp = dataAtLoc.timestamp;
            const qint64 iTimeSpentForFirstStroke = dataAtLoc.chords.front().timestamp - iTimestamp;
            const qint64 iTimeSpentToComplete = iTimeSpentForFirstStroke + (dataAtLoc.chords.back().timestamp - dataAtLoc.chords.front().timestamp);

            rWordData.averageTimeSpentForFirstStroke += iTimeSpentForFirstStroke;
            rWordData.averageTimeSpentToComplete += iTimeSpentToComplete;

            for (const ChordData& chordData : dataAtLoc.chords)
            {
                const qint64 iElapsed = chordData.timestamp - iTimestamp;
                Q_UNUSED(iElapsed); // TODO
                iTimestamp = chordData.timestamp;
            }
        }

        // Compute averages
        const int iDataCount = rWordData.dataAtLocation.count();
        rWordData.occurences = iDataCount;
        rWordData.averageChordsCount /= iDataCount;
        rWordData.averageErrorsCount /= iDataCount;
        rWordData.averageTimeSpentForFirstStroke /= iDataCount;
        rWordData.averageTimeSpentToComplete /= iDataCount;
    }
}

void TypingTestResult::addSuccess(const Word& word, const ChordData& chordData)
{
    WordData& rWordData = wordData[word.word];
    DataAtLocation& rDataAtLocation = rWordData.dataAtLocation[word.position];

    // Append chord for this word
    if (rDataAtLocation.chords.isEmpty())
    {
        rDataAtLocation.timestamp = _iLastTimestamp;
    }
    rDataAtLocation.chords << chordData;

    _iLastTimestamp = chordData.timestamp;
}

void TypingTestResult::addError(const Word& word, const ChordData&)
{
    WordData& rWordData = wordData[word.word];
    DataAtLocation& rDataAtLocation = rWordData.dataAtLocation[word.position];
    rDataAtLocation.errors++;
}
