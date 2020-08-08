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
    _inputStates.clear();
    _sCurrentWord.clear();
    _iCurrentWordErrors = 0;
    _validWords.clear();
}

void WordCounter::pushInputState(const QChar& expected, const QChar& input)
{
    _inputStates.push(InputState{expected, input});

    if (expected != " ")
    {
        _sCurrentWord.push_back(expected);

        if (input != expected)
        {
            _iCurrentWordErrors++;
        }
    }
    else
    {
        // Word validation
        if (_iCurrentWordErrors == 0)
        {
            _validWords << _sCurrentWord;
        }
        else
        {
            _iCurrentWordErrors = 0; // reset for next word
        }
        _sCurrentWord.clear();
    }
}

void WordCounter::popInputState()
{
    if (!_inputStates.isEmpty())
    {
        const InputState topState = _inputStates.top();

        if (topState.expected != " ")
        {
            _sCurrentWord.remove(_sCurrentWord.size()-1, 1);

            if (topState.expected != topState.input)
            {
                _iCurrentWordErrors--;
            }
        }
        else
        {
            // TODO
        }

        _inputStates.pop();
    }
}

int WordCounter::getWPM() const
{
    const int iWords = getValidWordsCount();
    float fSeconds = 60.f;
    if (_pCountdownTimer->getRemainingTime() > 0)
    {
        fSeconds = float(_pCountdownTimer->getElapsedTime()) / 1000;
    }
    const int iWPM = int(60.f * float(iWords) / fSeconds);
    return iWPM;
}
