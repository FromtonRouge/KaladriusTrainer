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

#include "QRectF.h"
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/nvp.hpp>
#include "../ExplicitInstanciation.h"

EXPLICIT_INSTANCIATION(QRectF)

namespace boost
{
    namespace serialization
    {
        template<class Archive> void serialize(Archive& ar, QRectF& obj,  const unsigned int fileVersion)
        {
            split_free(ar, obj, fileVersion);
        }

        template<class Archive> void save(Archive& ar, const QRectF& obj,  const unsigned int)
        {
            const qreal x = obj.x();
            ar << make_nvp("x", x);

            const qreal y = obj.y();
            ar << make_nvp("y", y);

            const qreal width = obj.width();
            ar << make_nvp("width", width);

            const qreal height = obj.height();
            ar << make_nvp("height", height);
        }

        template<class Archive> void load(Archive& ar, QRectF& obj,  const unsigned int)
        {
            qreal x = 0.;
            ar >> make_nvp("x", x);

            qreal y = 0.;
            ar >> make_nvp("y", y);

            qreal width = 0.;
            ar >> make_nvp("width", width);

            qreal height = 0.;
            ar >> make_nvp("height", height);

            obj.setX(x);
            obj.setY(y);
            obj.setWidth(width);
            obj.setHeight(height);
        }
    }
}
