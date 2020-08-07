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

#include "CountdownTimer.h"
#include <chrono>

CountdownTimer::CountdownTimer(QObject* pParent)
    : QObject(pParent)
{
}

void CountdownTimer::start()
{
    _bDone = false;
    _timer.start();
}

void CountdownTimer::reset()
{
    _timer.invalidate();
    _bDone = false;
}

int CountdownTimer::getRemainingTime() const
{
    const int iRemainingTime = _iTotalTime - (_timer.isValid() ? _timer.elapsed() : 0);
    return qMax(0, iRemainingTime);
}

QString CountdownTimer::getRemainingTimeString() const
{
    using namespace std::chrono;
    milliseconds ms(getRemainingTime());
    const auto iMinutes = duration_cast<minutes>(ms).count();
    const auto iSeconds = duration_cast<seconds>(ms).count() - iMinutes*60;
    const auto iMilliseconds = ms.count() - iMinutes*60*1000 - iSeconds*1000;
    return QString("%1:%2.%3").arg(iMinutes).arg(iSeconds, 2, 10, QChar('0')).arg(iMilliseconds, 3, 10, QChar('0'));
}

bool CountdownTimer::isDone()
{
    const bool bResult = getRemainingTime() == 0;
    if (bResult && !_bDone)
    {
        emit done();
        _bDone = true;
    }
    return bResult;
}

void CountdownTimer::setTotalTime(int seconds)
{
    _iTotalTime = seconds * 1000;
    reset();
}
