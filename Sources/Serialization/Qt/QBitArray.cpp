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

#include "QBitArray.h"
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/vector.hpp>
#include "../ExplicitInstanciation.h"
#include <vector>

EXPLICIT_INSTANCIATION(QBitArray)

namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, QBitArray& obj,  const unsigned int fileVersion)
        {
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const QBitArray& obj,  const unsigned int)
        {
            const int iSize = obj.size();
            std::vector<bool> bits(iSize, false);
            for (int i=0; i<iSize; ++i)
            {
                bits[i] = obj.at(i);
            }
            ar << make_nvp("bits", bits);
        }

        template<class Archive> void load(Archive& ar, QBitArray& obj,  const unsigned int)
        {
            std::vector<bool> bits;
            ar >> make_nvp("bits", bits);

            const int iSize = int(bits.size());
            obj.clear();
            obj.resize(iSize);
            for (int i=0; i<iSize; ++i)
            {
                obj.setBit(i, bits[i]);
            }
        }
    }
}
