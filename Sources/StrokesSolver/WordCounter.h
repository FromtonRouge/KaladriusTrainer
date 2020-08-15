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
#include <QtCore/QHash>
#include <QtCore/QChar>
#include <QtCore/QStack>
#include <QtCore/QStringList>
#include <QtCore/QSet>
#include <QtCore/QObject>

class CountdownTimer;
class WordCounter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float wpm READ getWPM)
    Q_PROPERTY(float spm READ getSPM)
    Q_PROPERTY(float accuracy READ getAccuracy)
    Q_PROPERTY(float viscosity READ getViscosity)

public:
    WordCounter(CountdownTimer* pCountdownTimer, QObject* pParent = nullptr);

    void reset();
    void typingTestDone();
    const TypingTestResult& getTypingTestResult() const {return _typingTestResult;}

    float getWPM() const;
    float getSPM() const;
    float getAccuracy() const;
    float getViscosity() const;

    void registerError(int iIndex);
    void registerValidCharacters(int iCharacters);

    // Chords count process
    void startChord(const QChar& inputChar, const Word& word, qint64 iTimestamp);
    void continueChord(const QChar& c, qint64 iTimestamp);
    void endChord();
    void markBecomeError();
    void markStillError();
    qint64 getLastTimestamp() const;

private:
    CountdownTimer* _pCountdownTimer = nullptr;
    uint _uiValidCharacters = 0;
    QSet<int> _errors;
    TypingTestResult _typingTestResult;

    struct CharWithTimestamp
    {
        enum ErrorState
        {
            NoError,
            BecomeError,
            StillError
        };
        QChar character;
        Word wordBeingCompleted;
        qint64 timestamp = 0;
        ErrorState errorState = NoError;
    };
    QStack<CharWithTimestamp> _buffer;
    QStack<ChordData> _validChords;
};
