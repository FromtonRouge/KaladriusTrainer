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

#pragma once

#include <QtCore/QHash>
#include <QtCore/QStack>
#include <QtCore/QVector>
#include <QtCore/QString>
#include <QtCore/QPair>
#include <QtCore/QDebug>

struct Word
{
    int position = 0;
    QString text;
    void clear() {text.clear(); position = 0;}
};
QDebug operator<<(QDebug debug, const Word& w);

struct CharData
{
    enum Kind
    {
        NormalChar,
        ChordStart,
    };

    enum State
    {
        DefaultState,
        ValidState,
        ErrorState,
    };

    Kind kind = NormalChar;
    State state = DefaultState;
    int position = 0;
    qint64 timestamp = 0;
    QChar character;
};

struct ChordData
{
    QVector<CharData> characters;
    Word expectedWord;
    qint64 timeToStroke = 0;

    int position() const {return characters.isEmpty() ? -1 : characters.front().position;}
    quint64 timestampAtBegin() const {return characters.isEmpty() ? 0 : characters.front().timestamp;}
    quint64 timestampAtEnd() const {return characters.isEmpty() ? 0 : characters.back().timestamp;}
    bool contains(const QChar& c) const { return text().contains(c); }
    void clear() {characters.clear(); timeToStroke = 0; expectedWord.clear();}
    QString text() const
    {
        QString sResult;
        for (const CharData& data : characters)
        {
            sResult += data.character;
        }
        return sResult;
    }
};
QDebug operator<<(QDebug debug, const ChordData& c);

struct DataAtLocation
{
    int errors = 0; // Number of failed chords for this word at this location
    QVector<ChordData> chords; // Chords needed to complete the whole word
};

struct WordData
{
    QHash<int, DataAtLocation> dataAtLocation; // key = word position

    int occurences = 0;
    float averageChordsCount = 0.f;
    float averageErrorsCount = 0.f;
    float averageTimeSpentForFirstStroke = 0.f;
    float averageTimeSpentToComplete = 0.f;
};

class TypingTestResult
{
public:
    QHash<QString, WordData> wordData;

public:
    TypingTestResult();
    ~TypingTestResult();

    void clear();
    void compute();

    void addValidChord(ChordData* pChordData);
    void addErrorChord(const ChordData& chordData);
    void addUndoChord(const ChordData& chordData);

    QPair<int, int> computeValidChordsAndCharactersCount();
    const QStack<ChordData>& getValidChords() const {return _validChords;}

private:
    QVector<CharData> _textBuffer;
    QHash<int, int> _errorsAtPosition;
    QStack<ChordData> _validChords;
};

