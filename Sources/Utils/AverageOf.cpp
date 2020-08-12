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

#include "AverageOf.h"

AverageOf::AverageOf(uint8_t uiSize)
    : _uiSize(uiSize)
{

}

void AverageOf::popValue()
{
    if (size() == _uiSize)
    {
        _fAverage -= top();
        pop();
    }
}

bool AverageOf::pushValue(float fValue)
{
    const float fValueToPush = fValue/_uiSize;
    _fAverage += fValueToPush;
    push(fValueToPush);
    return size() == _uiSize;
}
