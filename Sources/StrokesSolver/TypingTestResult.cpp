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
    _textBuffer.clear();
    _errorsAtPosition.clear();
    _validChords.clear();
}

void TypingTestResult::compute()
{
    wordData.clear();

    // Store errors and chords at word locations
    for (const ChordData& chordData : _validChords)
    {
        const Word& expectedWord = chordData.expectedWord;
        WordData& rWordData = wordData[expectedWord.text];
        DataAtLocation& rDataAtLocation = rWordData.dataAtLocation[expectedWord.position];

        // Save errors done on this word
        auto itErrors = _errorsAtPosition.find(expectedWord.position);
        if (itErrors != _errorsAtPosition.end())
        {
            rDataAtLocation.errors = itErrors.value();
        }

        // Save all chords done for this word
        rDataAtLocation.chords << chordData;
    }

    // Compute words statistics
    auto it = wordData.begin();
    while (it != wordData.end())
    {
        WordData& rWordData = it++.value();

        auto itLocation = rWordData.dataAtLocation.begin();
        while (itLocation != rWordData.dataAtLocation.end())
        {
            const DataAtLocation& dataAtLoc = itLocation++.value();

            rWordData.averageChordsCount += dataAtLoc.chords.count();
            rWordData.averageErrorsCount += dataAtLoc.errors;

            // Compute the time needed to do all chords for this word
            qint64 iTimeSpentToComplete = 0;
            for (const ChordData& chordData : dataAtLoc.chords)
            {
                iTimeSpentToComplete += chordData.timeToStroke;
            }

            rWordData.averageTimeSpentForFirstStroke += dataAtLoc.chords.front().timeToStroke;
            rWordData.averageTimeSpentToComplete += iTimeSpentToComplete;
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

void TypingTestResult::addValidChord(ChordData* pChordData)
{
    Q_ASSERT(_textBuffer.size() == pChordData->position());

    // Compute the time elapsed between this valid chord with the last one
    if (!_validChords.isEmpty())
    {
        const qint64 iLastValidTimestamp = _validChords.top().timestampAtBegin();
        pChordData->timeToStroke = pChordData->timestampAtBegin() - iLastValidTimestamp;
    }

    _textBuffer << pChordData->characters;
    _validChords.push(*pChordData);
}

void TypingTestResult::addErrorChord(const ChordData& chordData)
{
    Q_ASSERT(_textBuffer.size() == chordData.position());
    _textBuffer << chordData.characters;
    _errorsAtPosition[chordData.expectedWord.position]++;
}

void TypingTestResult::addUndoChord(const ChordData& chordData)
{
    const int iCharactersToRemove = chordData.characters.size();
    for (int i = 0; i < iCharactersToRemove; ++i)
    {
        if (!_textBuffer.isEmpty())
        {
            // Pop char from the buffer, if the char is a beginning of a valid chord
            // we also remove the valid chord from the stack
            const CharData& charData = _textBuffer.back();
            if (charData.kind == CharData::ChordStart && charData.state == CharData::ValidState)
            {
                const ChordData& lastValidChord = _validChords.top();
                Q_ASSERT(lastValidChord.position() == charData.position);
                _validChords.pop();
            }

            _textBuffer.pop_back();
        }
    }
}

QPair<int, int> TypingTestResult::computeValidChordsAndCharactersCount()
{
    QPair<int, int> result; // first = Chords, second = Characters
    result.first = _validChords.count();
    for (const ChordData& chordData : _validChords)
    {
        result.second += chordData.characters.count();
    }
    return result;
}
