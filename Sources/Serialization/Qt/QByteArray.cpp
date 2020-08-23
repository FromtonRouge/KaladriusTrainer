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

#include "QByteArray.h"
#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_free.hpp>
#include "../ExplicitInstanciation.h"

EXPLICIT_INSTANCIATION(QByteArray)

namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, QByteArray& obj,  const unsigned int fileVersion)
        {
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const QByteArray& obj,  const unsigned int)
        {
            int iSize = obj.size();
            ar << make_nvp("size", iSize);
            void* pBinaryData = const_cast<void*>(static_cast<const void*>(obj.data()));
            ar << make_nvp("bytes", make_binary_object(pBinaryData, size_t(iSize)));
        }

        template<class Archive> void load(Archive& ar, QByteArray& obj,  const unsigned int)
        {
            int iSize = 0;
            ar >> make_nvp("size", iSize);
            obj.resize(iSize);
            ar >> make_nvp("bytes", make_binary_object(obj.data(), size_t(iSize)));
        }
    }
}
