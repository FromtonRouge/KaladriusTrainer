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

#include <QtCore/QString>
#include <QtCore/QMetaType>
#include <QtCore/QVariant>

struct ListValue
{
    enum NamingPolicy
    {
        NameIsNotEditable,
        NameIsEditable,
        NameIsAutoIndexed, ///< eg: [0], [1] etc....
    };

    ListValue(const QString& sLabel = QString(),
              const QVariant& defaultValue = QVariant(),
              NamingPolicy namingPolicy = NameIsAutoIndexed,
              const QString& sDefaultIconResource = QString(":/Icons/document-attribute.png"))
        : sLabel(sLabel)
        , defaultValue(defaultValue)
        , namingPolicy(namingPolicy)
        , sDefaultIconResource(sDefaultIconResource)
    {}

    QString sLabel;
    QVariant defaultValue;
    NamingPolicy namingPolicy;
    QString sDefaultIconResource;
};

Q_DECLARE_METATYPE(ListValue)
