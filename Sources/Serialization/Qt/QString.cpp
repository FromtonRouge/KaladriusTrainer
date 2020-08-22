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

#include "QString.h"
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include "ExplicitInstanciation.h"

EXPLICIT_INSTANCIATION(QString)

namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, QString& obj,  const unsigned int fileVersion)
        {
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const QString& obj,  const unsigned int)
        {
            const std::string& sString = obj.toStdString();
            ar << make_nvp("string", sString);
        }

        template<class Archive> void load(Archive& ar, QString& obj,  const unsigned int)
        {
            std::string sString;
            ar >> make_nvp("string", sString);
            obj = QString::fromStdString(sString);
        }
    }
}
