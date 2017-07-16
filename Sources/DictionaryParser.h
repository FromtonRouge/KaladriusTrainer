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

#include "Dictionary.h"
#include <QtCore/QScopedPointer>
#include <QtCore/QHash>
#include <QtCore/QString>

template <typename Iterator> struct InternalParser;
class DictionaryParser
{
public:
    DictionaryParser(const QString& sFilePath);
    ~DictionaryParser();

    bool parse();
    const Dictionaries& getDictionaries() const { return _dictionaries; }

private:
    struct KnownDictionary
    {
        QString userName;
        QString markdownFileName;
        QVector<QString> keysLabels;
        QVector<int> keyBitsReadingOrder;
    };
    typedef QHash<QString, KnownDictionary> KnownDictionaries;
    typedef std::string::const_iterator IteratorType;
    QScopedPointer<InternalParser<IteratorType>> _pInternalParser;
    QString _sFilePath;
    Dictionaries _dictionaries;
    static KnownDictionaries KNOWN_DICTIONARIES;
};
