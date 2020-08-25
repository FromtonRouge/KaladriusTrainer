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

#include "QFont.h"
#include "QString.h"
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/nvp.hpp>
#include "../ExplicitInstanciation.h"

EXPLICIT_INSTANCIATION(QFont)

namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, QFont& obj,  const unsigned int fileVersion)
        {
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const QFont& obj,  const unsigned int)
        {
            QString s = obj.family();
            ar & make_nvp("family", s);

            int i = obj.pointSize();
            ar & make_nvp("pointSize", i);

            i = obj.weight();
            ar & make_nvp("weight", i);

            bool b = obj.italic();
            ar & make_nvp("italic", b);

            b = obj.strikeOut();
            ar & make_nvp("strikeOut", b);

            b = obj.underline();
            ar & make_nvp("underline", b);

            s = obj.styleName();
            ar & make_nvp("styleName", s);
        }

        template<class Archive> void load(Archive& ar, QFont& obj,  const unsigned int)
        {
            QString s;
            ar & make_nvp("family", s);
            obj.setFamily(s);

            int i = 0;
            ar & make_nvp("pointSize", i);
            obj.setPointSize(i);

            ar & make_nvp("weight", i);
            obj.setWeight(i);

            bool b = false;
            ar & make_nvp("italic", b);
            obj.setItalic(b);

            ar & make_nvp("strikeOut", b);
            obj.setStrikeOut(b);

            ar & make_nvp("underline", b);
            obj.setUnderline(b);

            ar & make_nvp("styleName", s);
            obj.setStyleName(s);
        }
    }
}
