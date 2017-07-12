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

#include <vector>
#include <string>

namespace Ast
{
    struct Keycode
    {
        Keycode(const std::string& key = std::string())
        {
            this->key = key;
        }
        std::string mods;
        std::string key;
    };

    struct Entry
    {
        Entry(const std::string& s = std::string())
        {
            if (!s.empty())
            {
                Keycode keycode;
                keycode.key = s;
                keycodes.push_back(keycode);
            }
        }
        std::vector<Keycode> keycodes;
    };

    struct Table
    {
        std::string tableName;
        std::vector<Entry> entries;
    };
}