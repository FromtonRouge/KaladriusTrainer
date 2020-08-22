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

#pragma once

#include <QtCore/QVector>
#include <boost/serialization/version.hpp>
#include <boost/serialization/nvp.hpp>

// Generic QVector<T> serialization
namespace boost
{
    namespace serialization
    {
        template<class T> struct version<QVector<T>>
        {
            typedef mpl::int_<0> type; // Increment the version number here, must be between 0 and 255
            typedef mpl::integral_c_tag tag;
            BOOST_STATIC_CONSTANT(int, value = version::type::value);
        };

        template<class Archive, class T> void serialize(Archive& ar, QVector<T>& obj, const unsigned int fileVersion)
        {
            split_free(ar, obj, fileVersion);
        }

        template<class Archive, class T> void save(Archive& ar, const QVector<T>& obj,  const unsigned int fileVersion)
        {
            const int iCount = obj.size();
            ar << make_nvp("count", iCount);
            for (const auto& element : obj)
            {
                ar << make_nvp("element", element);
            }
        }

        template<class Archive, class T> void load(Archive& ar, QVector<T>& obj,  const unsigned int fileVersion)
        {
            int iCount;
            ar >> make_nvp("count", iCount);
            for (int i = 0; i < iCount; ++i)
            {
                T element;
                ar >> make_nvp("element", element);
                obj << element;
            }
        }
    }
}
