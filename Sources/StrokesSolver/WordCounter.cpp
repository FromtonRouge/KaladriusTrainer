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

#include "WordCounter.h"
#include "../Utils/CountdownTimer.h"
#include <QtCore/QDebug>
#include <chrono>
#include <cmath>

WordCounter::WordCounter(CountdownTimer* pCountdownTimer, QObject* pParent)
    : QObject(pParent)
    , _pCountdownTimer(pCountdownTimer)
{

}

void WordCounter::reset()
{
    _uiValidCharacters = 0;
    _errors.clear();
    _buffer.clear();
    _validChords.clear();
    _typingTestResult.clear();
}

void WordCounter::typingTestDone()
{
    _typingTestResult.compute();
}

float WordCounter::getWPM() const
{
    const int AVERAGE_WORD_LENGTH = 5; // Same as www.10fastfingers.com
    const float fWords = float(_uiValidCharacters) / AVERAGE_WORD_LENGTH;
    float fSeconds = _pCountdownTimer->getTotalTimeInSeconds();
    if (_pCountdownTimer->getRemainingTime() > 0)
    {
        fSeconds = float(_pCountdownTimer->getElapsedTime()) / 1000;
    }
    return 60.f * fWords / fSeconds;
}

float WordCounter::getSPM() const
{
    float fSeconds = _pCountdownTimer->getTotalTimeInSeconds();
    if (_pCountdownTimer->getRemainingTime() > 0)
    {
        fSeconds = float(_pCountdownTimer->getElapsedTime()) / 1000;
    }
    return 60.f * _validChords.count() / fSeconds;
}

void WordCounter::registerError(int iIndex)
{
    _errors.insert(iIndex);
}

void WordCounter::registerValidCharacters(int iCharacters)
{
    _uiValidCharacters = iCharacters;
}

void WordCounter::startChord(const QChar& inputChar, const Word& word, qint64 iTimestamp)
{
    _buffer.clear();

    CharWithTimestamp data;
    data.character = inputChar;
    data.wordBeingCompleted = word;
    data.timestamp = iTimestamp;
    _buffer.push(data);
}

void WordCounter::continueChord(const QChar& c, qint64 iTimestamp)
{
    CharWithTimestamp data;
    data.character = c;
    data.timestamp = iTimestamp;
    _buffer.push(data);
}

void WordCounter::markBecomeError()
{
    Q_ASSERT(!_buffer.isEmpty());

    // Add info on the first character of the chord
    CharWithTimestamp& rFirstCharacter = _buffer.front();
    rFirstCharacter.errorState = CharWithTimestamp::BecomeError;
}

void WordCounter::markStillError()
{
    Q_ASSERT(!_buffer.isEmpty());

    // Add info on the current character of the chord
    _buffer.top().errorState = CharWithTimestamp::StillError;
}

void WordCounter::endChord()
{
    if (!_buffer.isEmpty())
    {
        const CharWithTimestamp& firstData = _buffer.front();

        QString sChord;
        for (const CharWithTimestamp& data : _buffer)
        {
            sChord += data.character;
        }

        if (!sChord.contains("\b"))
        {
            ChordData chordData;
            chordData.timestamp = firstData.timestamp;
            chordData.chord = sChord;

            if (firstData.errorState == CharWithTimestamp::NoError)
            {
                _validChords << chordData;
                _typingTestResult.addSuccess(firstData.wordBeingCompleted, chordData);
            }
            else if (firstData.errorState == CharWithTimestamp::BecomeError)
            {
                _typingTestResult.addError(firstData.wordBeingCompleted, chordData);
            }
        }
    }
}

qint64 WordCounter::getLastTimestamp() const
{
    Q_ASSERT(!_buffer.isEmpty());
    return _buffer.top().timestamp;
}

float WordCounter::getAccuracy() const
{
    const uint uiErrors = _errors.count();
    return _uiValidCharacters==0 ? 0 : 100.f*(qMax<uint>(_uiValidCharacters-uiErrors, 0))/_uiValidCharacters;
}

float WordCounter::getViscosity() const
{
    // We need at least 2 strokes to mesure the time between them
    if (_validChords.count() < 2)
    {
        return 0.f;
    }

    // Compute the "needed time to stroke" average
    float fSum = 0;
    QVector<float> timesBetweenStrokes;
    const int iChords = _validChords.count();
    for (int i = 1; i < iChords; ++i)
    {
        const ChordData& chordData = _validChords[i];
        const float fDelta = chordData.timestamp - _validChords[i-1].timestamp;
        fSum += fDelta;
        timesBetweenStrokes << fDelta;
    }
    const float fAverage = fSum / timesBetweenStrokes.count();

    // Compute the variance
    float fVarianceNumerator = 0;
    for (float fXi : timesBetweenStrokes)
    {
        fVarianceNumerator += (fXi - fAverage)*(fXi - fAverage);
    }

    // Compute the standard deviation of the "needed time to stroke" metric
    const float fVariance = fVarianceNumerator / timesBetweenStrokes.count();
    const float fStandardDeviation = sqrt(fVariance);

    // Finally we compute the "Coefficient of variance" of the "needed time to stroke" metric
    // and we scale it 60 times, we call it the "Viscosity" (the lower the better, -> 0 means "fluid")
    return 60.f*fStandardDeviation/fAverage;
}
