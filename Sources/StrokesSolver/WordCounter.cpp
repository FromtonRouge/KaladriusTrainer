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
    _chords.clear();
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
    return 60.f * _chords.count() / fSeconds;
}

void WordCounter::registerError(int iIndex)
{
    _errors.insert(iIndex);
}

void WordCounter::registerValidCharacters(int iCharacters)
{
    _uiValidCharacters = iCharacters;
}

void WordCounter::startChord(const QChar& c, qint64 iTimestamp)
{
    _buffer.clear();

    CharWithTimestamp data;
    data.character = c;
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

void WordCounter::markError()
{
    Q_ASSERT(!_buffer.isEmpty());
    _buffer.top().isError = true;
}

void WordCounter::endChord()
{
    QString sChord;
    bool bError = false;
    for (const CharWithTimestamp& data : _buffer)
    {
        sChord += data.character;
        bError = data.isError;
    }

    if (!sChord.contains("\b") && !bError && !_buffer.isEmpty())
    {
        ChordData chordData;
        chordData.timestamp = _buffer.front().timestamp;
        chordData.chord = sChord;
        _chords << chordData;
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
    if (_chords.count() < 2)
    {
        return 0.f;
    }

    // Compute the "needed time to stroke" average
    float fSum = 0;
    QVector<float> timesBetweenStrokes;
    const int iChords = _chords.count();
    for (int i = 1; i < iChords; ++i)
    {
        const ChordData& chordData = _chords[i];
        const float fDelta = chordData.timestamp - _chords[i-1].timestamp;
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
