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

QDebug operator<<(QDebug debug, const Word& w)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "([" << w.position << "] : " << w.text << ")";
    return debug;
}

QDebug operator<<(QDebug debug, const ChordData& c)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "([" << c.position() << "][" << c.timestampAtBegin() << "] : " << c.text() << ")";
    return debug;
}

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
            const qint64 iTimeSpentForFirstStroke = dataAtLoc.chords.front().timestampAtBegin() - iTimestamp;
            const qint64 iTimeSpentToComplete = iTimeSpentForFirstStroke + (dataAtLoc.chords.back().timestampAtBegin() - dataAtLoc.chords.front().timestampAtBegin());

            rWordData.averageTimeSpentForFirstStroke += iTimeSpentForFirstStroke;
            rWordData.averageTimeSpentToComplete += iTimeSpentToComplete;

            for (const ChordData& chordData : dataAtLoc.chords)
            {
                const qint64 iElapsed = chordData.timestampAtBegin() - iTimestamp;
                Q_UNUSED(iElapsed); // TODO
                iTimestamp = chordData.timestampAtBegin();
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

void TypingTestResult::addValidChord(const Word& word, const ChordData& chordData)
{
    WordData& rWordData = wordData[word.text];
    DataAtLocation& rDataAtLocation = rWordData.dataAtLocation[word.position];

    // Append chord for this word
    if (rDataAtLocation.chords.isEmpty())
    {
        rDataAtLocation.timestamp = _iLastTimestamp;
    }
    else
    {
        // Check if it's a redo of one chord in the current word
        // (the user can do a valid chord then undo one or more chords and redo a valid chord again)
        // It's a redo if the position of a chordData is <= at the last saved chord
        const int iPositionNewChord = chordData.position();
        if (iPositionNewChord <= rDataAtLocation.chords.back().position())
        {
            // Find in the chords list a chord at position iPositionNewChord
            auto itChordToDelete = rDataAtLocation.chords.begin();
            while (itChordToDelete != rDataAtLocation.chords.end())
            {
                const ChordData& oldChord = *(itChordToDelete);
                if (oldChord.position() == iPositionNewChord)
                {
                    // We have to replace all chords from this one
                    break;
                }
                itChordToDelete++;
            }

            // Remove all chords from itChordToDelete
            if (itChordToDelete != rDataAtLocation.chords.end())
            {
                rDataAtLocation.chords.erase(itChordToDelete, rDataAtLocation.chords.end());
            }
        }
    }

    rDataAtLocation.chords << chordData;

    _iLastTimestamp = chordData.timestampAtBegin();
}

void TypingTestResult::addErrorChord(const Word& word, const ChordData&)
{
    WordData& rWordData = wordData[word.text];
    DataAtLocation& rDataAtLocation = rWordData.dataAtLocation[word.position];
    rDataAtLocation.errors++;
}

void TypingTestResult::addUndoChord(const Word& word, const ChordData& chordData)
{
    Q_UNUSED(word);
    Q_UNUSED(chordData);
}
