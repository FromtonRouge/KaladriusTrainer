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

#pragma once

#include "../Tree/TreeItems/ValueTreeItem.h"
#include <boost/serialization/version.hpp>
#include <boost/serialization/export.hpp>

namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, ValueTreeItem& obj,  const unsigned int fileVersion);
    }
}

BOOST_CLASS_VERSION(ValueTreeItem, 0)
BOOST_CLASS_EXPORT_KEY(ValueTreeItem) // For serializing from a base pointer
