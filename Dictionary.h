// ======================================================================
// This file is a part of the ProgrammerStenoTutor project
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

#include "Keycode.h"

#include <QtCore/QHash>
#include <QtCore/QVector>
#include <QtCore/QString>

class Dictionary
{
public:
    typedef QVector<Keycodes> EntriesByKeyBits;
    typedef QHash<Keycodes, uint> EntriesByStrings;

public:
    Dictionary(const QString& sName = QString());
    ~Dictionary();

    const QString& getName() const { return _sName; }
    void addEntry(const Keycodes& keycodes, uint keyBits);
    const auto& getEntriesByKeyBits() const { return _keyBitsToKeycodes; }
    const auto& getEntriesByKeycodes() const { return _keycodesToKeyBits; }

private:
    QString _sName;
    EntriesByStrings _keycodesToKeyBits;
    EntriesByKeyBits _keyBitsToKeycodes;
};

typedef QHash<QString, Dictionary> Dictionaries;