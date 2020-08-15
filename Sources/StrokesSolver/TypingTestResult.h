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
#include <QtCore/QVector>
#include <QtCore/QString>
#include <QtCore/QDebug>

struct Word
{
    int position = 0;
    QString word;
};

struct CharData
{
    int position = 0;
    qint64 timestamp = 0;
    QChar character;
};

struct ChordData
{
    QVector<CharData> characters;

    int position() const {return characters.isEmpty() ? -1 : characters.front().position;}
    quint64 timestampAtBegin() const {return characters.isEmpty() ? 0 : characters.front().timestamp;}
    quint64 timestampAtEnd() const {return characters.isEmpty() ? 0 : characters.back().timestamp;}
    bool contains(const QChar& c) const { return text().contains(c); }
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
    qint64 timestamp = 0; // Last known time before the first chord
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

    void addValidChord(const Word& word, const ChordData& chordData);
    void addErrorChord(const Word& word, const ChordData& chordData);
    void addUndoChord(const Word& word, const ChordData& chordData);

private:
    qint64 _iLastTimestamp = 0; // Used to fill DataAtLocation::timestamp
};

