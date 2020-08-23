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

#include "Dashboard.h"

Dashboard::Dashboard(QObject *pParent)
    : QObject(pParent)
{

}

void Dashboard::setLastWpm(float fValue)
{
    _fLastWpm = fValue;
    emit lastWpmChanged(_fLastWpm);
}

void Dashboard::setLastSpm(float fValue)
{
    _fLastSpm = fValue;
    emit lastSpmChanged(_fLastSpm);
}

void Dashboard::setLastAccuracy(float fValue)
{
    _fLastAccuracy = fValue;
    emit lastAccuracyChanged(_fLastAccuracy);
}

void Dashboard::setMaxWpm(float fValue)
{
    _fMaxWpm = fValue;
    emit maxWpmChanged(_fMaxWpm);
}

void Dashboard::setMaxSpm(float fValue)
{
    _fMaxSpm = fValue;
    emit maxSpmChanged(_fMaxSpm);
}

void Dashboard::setMaxAccuracy(float fValue)
{
    _fMaxAccuracy = fValue;
    emit maxAccuracyChanged(_fMaxAccuracy);
}
