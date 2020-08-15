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
    _textBuffer.clear();
    _errorsAtPosition.clear();
}

void TypingTestResult::compute()
{
    wordData.clear();

    // Build word data from the text buffer
    qint64 iPreviousTimestamp = 0;
    ChordData currentChord;
    DataAtLocation* pDataAtLocation = nullptr;
    const int iCharacters = _textBuffer.size();
    for (int i = 0; i < iCharacters; ++i)
    {
        const CharData& charData = _textBuffer[i];
        if (charData.state == CharData::ValidState)
        {
            switch (charData.kind)
            {
            case CharData::ChordStart:
                {
                    Q_ASSERT(pDataAtLocation == nullptr);
                    const Word& expectedWord = charData.expectedWord;
                    WordData& rWordData = wordData[expectedWord.text];
                    pDataAtLocation = &(rWordData.dataAtLocation[expectedWord.position]);

                    // Save errors done on this word
                    auto itErrors = _errorsAtPosition.find(expectedWord.position);
                    if (itErrors != _errorsAtPosition.end())
                    {
                        pDataAtLocation->errors = itErrors.value();
                    }

                    if (pDataAtLocation->chords.isEmpty())
                    {
                        pDataAtLocation->timestamp = iPreviousTimestamp;
                    }

                    currentChord.clear();
                    currentChord.characters << charData;
                    iPreviousTimestamp = charData.timestamp;
                    break;
                }
            case CharData::NormalChar:
                {
                    Q_ASSERT(pDataAtLocation);
                    currentChord.characters << charData;

                    // Check next character to know if we are at the end of the chord
                    bool bEndOfTheChord = false;
                    if (i+1 < iCharacters)
                    {
                        const CharData& nextChar = _textBuffer[i+1];
                        bEndOfTheChord = nextChar.kind == CharData::ChordStart;
                    }
                    else
                    {
                        // End of buffer
                        bEndOfTheChord = true;
                    }

                    if (bEndOfTheChord)
                    {
                        // Save the chord
                        pDataAtLocation->chords << currentChord;

                        // Reset data
                        pDataAtLocation = nullptr;
                        currentChord.clear();
                    }
                    break;
                }
            default:
                {
                    break;
                }
            }
        }
        else
        {
            break;
        }
    }

    auto it = wordData.begin();
    while (it != wordData.end())
    {
        WordData& rWordData = it++.value();

        auto itLocation = rWordData.dataAtLocation.begin();
        while (itLocation != rWordData.dataAtLocation.end())
        {
            const int iLocation = itLocation.key();
            const DataAtLocation& dataAtLoc = itLocation++.value();

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

void TypingTestResult::addValidChord(const ChordData& chordData)
{
    Q_ASSERT(_textBuffer.size() == chordData.position());
    _textBuffer << chordData.characters;
}

void TypingTestResult::addErrorChord(const ChordData& chordData)
{
    Q_ASSERT(_textBuffer.size() == chordData.position());
    _textBuffer << chordData.characters;
    const CharData& firstCharOfTheChord = chordData.characters.front();
    Q_ASSERT(firstCharOfTheChord.kind == CharData::ChordStart);
    _errorsAtPosition[firstCharOfTheChord.expectedWord.position]++;
}

void TypingTestResult::addUndoChord(const ChordData& chordData)
{
    const int iCharactersToRemove = chordData.characters.size();
    for (int i = 0; i < iCharactersToRemove; ++i)
    {
        if (!_textBuffer.isEmpty())
        {
            _textBuffer.pop_back();
        }
    }
}

QPair<int, int> TypingTestResult::computeValidChordsAndCharactersCount()
{
    QPair<int, int> result; // first = Chords, second = Characters
    for (const CharData& charData : _textBuffer)
    {
        if (charData.state == CharData::ValidState)
        {
            result.second++;
            if (charData.kind == CharData::ChordStart)
            {
                result.first++;
            }
        }
    }
    return result;
}
