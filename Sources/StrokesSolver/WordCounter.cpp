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

WordCounter::WordCounter(CountdownTimer* pCountdownTimer, QObject* pParent)
    : QObject(pParent)
    , _pCountdownTimer(pCountdownTimer)
{

}

void WordCounter::reset()
{
    _uiValidCharacters = 0;
    _errors.clear();
}

int WordCounter::getWPM() const
{
    const int AVERAGE_WORD_LENGTH = 5; // Same as www.10fastfingers.com
    const float fWords = float(_uiValidCharacters) / AVERAGE_WORD_LENGTH;
    float fSeconds = _pCountdownTimer->getTotalTime();
    if (_pCountdownTimer->getRemainingTime() > 0)
    {
        fSeconds = float(_pCountdownTimer->getElapsedTime()) / 1000;
    }
    return int(60.f * fWords / fSeconds);
}

void WordCounter::registerError(int iIndex)
{
    _errors.insert(iIndex);
}

void WordCounter::registerValidCharacters(int iCharacters)
{
    _uiValidCharacters = iCharacters;
}

float WordCounter::getAccuracy() const
{
    const uint uiErrors = _errors.count();
    return _uiValidCharacters==0 ? 0 : 100.f*(qMax<uint>(_uiValidCharacters-uiErrors, 0))/_uiValidCharacters;
}
