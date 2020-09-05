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

#include "WordCounter.h"
#include "../Utils/CountdownTimer.h"
#include <QtCore/QDebug>
#include <cmath>

WordCounter::WordCounter(CountdownTimer* pCountdownTimer, QObject* pParent)
    : QObject(pParent)
    , _pCountdownTimer(pCountdownTimer)
{

}

void WordCounter::reset()
{
    _errors.clear();
    _recordedChord.clear();
    _typingTestResult.clear();
    _iValidChordsCount = 0;
    _iValidCharactersCount = 0;
}

void WordCounter::typingTestDone()
{
    _typingTestResult.compute();
}

float WordCounter::getWpm() const
{
    const int AVERAGE_WORD_LENGTH = 5; // Same as www.10fastfingers.com
    const float fWords = float(_iValidCharactersCount) / AVERAGE_WORD_LENGTH;
    float fSeconds = _pCountdownTimer->getTotalTimeInSeconds();
    if (_pCountdownTimer->isCountdownMode())
    {
        if (_pCountdownTimer->isRunning() && _pCountdownTimer->getRemainingTime() > 0)
        {
            fSeconds = float(_pCountdownTimer->getElapsedTime()) / 1000;
        }
    }
    else
    {
        fSeconds = float(_pCountdownTimer->getElapsedTime()) / 1000;
        if (fSeconds == 0.f)
        {
            return 0.f;
        }
    }
    return 60.f * fWords / fSeconds;
}

float WordCounter::getSpm() const
{
    float fSeconds = _pCountdownTimer->getTotalTimeInSeconds();
    if (_pCountdownTimer->isCountdownMode())
    {
        if (_pCountdownTimer->isRunning() && _pCountdownTimer->getRemainingTime() > 0)
        {
            fSeconds = float(_pCountdownTimer->getElapsedTime()) / 1000;
        }
    }
    else
    {
        fSeconds = float(_pCountdownTimer->getElapsedTime()) / 1000;
        if (fSeconds == 0.f)
        {
            return 0.f;
        }
    }
    return 60.f * _iValidChordsCount / fSeconds;
}

float WordCounter::getAccuracy() const
{
    const uint uiErrors = _errors.count();
    return _iValidCharactersCount==0 ? 0 : 100.f*(qMax<uint>(_iValidCharactersCount-uiErrors, 0))/_iValidCharactersCount;
}

void WordCounter::registerError(int iIndex)
{
    // TODO: remove _errors and use data in _typingTestResult
    _errors.insert(iIndex);
}

void WordCounter::startChord(int iPosition, const QChar& inputChar, const Word& word, qint64 iTimestamp)
{
    _recordedChord.clear();
    _recordedChord.chordData.expectedWord = word;

    CharData data;
    data.kind = CharData::ChordStart;
    data.position = iPosition;
    data.timestamp = iTimestamp;
    data.character = inputChar;
    _recordedChord.chordData.characters << data;
}

void WordCounter::continueChord(int iPosition, const QChar& inputChar, qint64 iTimestamp)
{
    CharData data;
    data.position = iPosition;
    data.timestamp = iTimestamp;
    data.character = inputChar;
    _recordedChord.chordData.characters << data;
}

void WordCounter::markError()
{
    _recordedChord.isError = true;
}

void WordCounter::endChord()
{
    ChordData chordData = _recordedChord.chordData;

    if (chordData.contains('\b'))
    {
        _typingTestResult.addUndoChord(chordData);
    }
    else if (_recordedChord.isError)
    {
        for (CharData& rData : chordData.characters) { rData.state = CharData::ErrorState; }
        _typingTestResult.addErrorChord(chordData);
    }
    else
    {
        for (CharData& rData : chordData.characters) { rData.state = CharData::ValidState; }
        _typingTestResult.addValidChord(&chordData);
    }

    computeLiveData();
}

qint64 WordCounter::getLastTimestamp() const
{
    return _recordedChord.chordData.timestampAtEnd();
}

bool WordCounter::hasLastRecordedChar() const
{
    return !_recordedChord.chordData.characters.isEmpty();
}

CharData WordCounter::getLastRecordedChar() const
{
    return _recordedChord.chordData.characters.back();
}

void WordCounter::computeLiveData()
{
    const auto& pair = _typingTestResult.computeValidChordsAndCharactersCount();
    _iValidChordsCount = pair.first;
    _iValidCharactersCount = pair.second;
}

float WordCounter::getViscosity() const
{
    const auto& validChords = _typingTestResult.getValidChords();

    // We need at least 2 strokes to mesure the time between them
    if (validChords.count() < 2)
    {
        return 0.f;
    }

    // Compute the "needed time to stroke" average
    float fSum = 0;
    QVector<float> timesBetweenStrokes;
    const int iChords = validChords.count();
    for (int i = 1; i < iChords; ++i)
    {
        const ChordData& chordData = validChords[i];
        Q_ASSERT(chordData.timeToStroke != 0);
        fSum += chordData.timeToStroke;
        timesBetweenStrokes << chordData.timeToStroke;
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
    const float fCoefficientOfVariance = fStandardDeviation/fAverage;

    // Square it to emphasize amplitude and scale it. We call it the "Viscosity" (the lower the better, -> 0 means "fluid")
    const float fViscosity = fCoefficientOfVariance*fCoefficientOfVariance * 50.f;
    return fViscosity;
}
