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

void WordCounter::pushChord(const QString& sChord, uint16_t uiTimeToStroke)
{
    ChordData chord;
    chord.chord = sChord;
    chord.uiTimeToStroke = uiTimeToStroke;
    _chords.push(chord);
}

void WordCounter::popChord()
{
    if (!_chords.isEmpty())
    {
        _chords.pop();
    }
}

float WordCounter::getAccuracy() const
{
    const uint uiErrors = _errors.count();
    return _uiValidCharacters==0 ? 0 : 100.f*(qMax<uint>(_uiValidCharacters-uiErrors, 0))/_uiValidCharacters;
}

float WordCounter::getViscosity() const
{
    // Compute the "needed time to stroke" average
    float fSum = 0;
    for (const ChordData& chordData : _chords)
    {
        fSum += float(chordData.uiTimeToStroke);
    }
    const float fAverage = fSum /_chords.count();

    // Compute the variance
    float fVarianceNumerator = 0;
    for (const ChordData& chordData : _chords)
    {
        const float fXi = chordData.uiTimeToStroke;
        fVarianceNumerator += (fXi - fAverage)*(fXi - fAverage);
    }

    // Compute the standard deviation of the "needed time to stroke" metric
    // Its called "Viscosity" (0 means "fluid")
    const float fVariance = fVarianceNumerator / _chords.count();
    const float fStandardDeviation = sqrt(fVariance);
    return fStandardDeviation;
}
