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

#include "TypingTestResult.h"
#include <QtCore/QObject>
#include <QtCore/QSet>
#include <QtCore/QChar>

class CountdownTimer;
class WordCounter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float wpm READ getWpm)
    Q_PROPERTY(float spm READ getSpm)
    Q_PROPERTY(float accuracy READ getAccuracy)
    Q_PROPERTY(float viscosity READ getViscosity)

public:
    WordCounter(CountdownTimer* pCountdownTimer, QObject* pParent = nullptr);

    void reset();
    void typingTestDone();
    const TypingTestResult& getTypingTestResult() const {return _typingTestResult;}

    float getWpm() const;
    float getSpm() const;
    float getAccuracy() const;
    float getViscosity() const;

    void registerError(int iIndex);

    // Chords count process
    void startChord(int iPosition, const QChar& inputChar, const Word& word, qint64 iTimestamp);
    void continueChord(int iPosition, const QChar& inputChar, qint64 iTimestamp);
    void endChord();
    void markError();
    qint64 getLastTimestamp() const;

    bool hasLastRecordedChar() const;
    CharData getLastRecordedChar() const;

private:
    void computeLiveData();

private:
    CountdownTimer* _pCountdownTimer = nullptr;
    QSet<int> _errors;
    TypingTestResult _typingTestResult;

    struct RichChordData
    {
        ChordData chordData;
        bool isError = false;
        void clear() {chordData.clear(); isError = false;}
    };

    RichChordData _recordedChord;
    int _iValidChordsCount = 0;
    int _iValidCharactersCount = 0;
};
