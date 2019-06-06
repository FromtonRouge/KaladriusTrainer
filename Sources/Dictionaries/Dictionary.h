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

#include "Keycode.h"

#include <QtCore/QHash>
#include <QtCore/QMap>
#include <QtCore/QVector>
#include <QtCore/QStringList>
#include <QtCore/QString>
#include <QtCore/qhashfunctions.h>
#include <boost/dynamic_bitset.hpp>

class Dictionary
{
public:
    struct Entry
    {
        typedef boost::dynamic_bitset<> KeyBits;
        Entry(const Keycodes& keycodes = Keycodes(), uint nBitsCount = 0, uint nKeyBits = 0);
        bool operator==(const Entry& e) const { return keycodesAsUserString == e.keycodesAsUserString && bitsAsString == e.bitsAsString; }
        bool hasKeycodes() const { return !keycodesAsUserString.isEmpty(); }
        Keycodes keycodes;
        KeyBits bits;
        QString keycodesAsUserString;
        QString bitsAsString;
    };
    typedef QVector<Entry> KeyBitsToEntry;
    typedef QHash<Entry, Entry::KeyBits> EntryToKeyBits;
    typedef QVector<QString> KeysLabels;
    typedef QVector<int> KeyBitsReadingOrder;

public:
    Dictionary(const QString& sName = QString(),
               const QString& sMarkdownFileName = QString(),
               uint uiSize = 0,
               const KeysLabels& keysLabels = KeysLabels(),
               const KeyBitsReadingOrder& keyBitsReadingOrder = KeyBitsReadingOrder());
    ~Dictionary();

    uint getBitsCount() const {return _uiBitsCount;}
    const QString& getName() const { return _sName; }
    const QString& getMarkdownFileName() const { return _sMarkdownFileName; }
    void addEntry(const Keycodes& keycodes, uint uiKeyBits);
    const auto& getKeyBitsToEntry() const { return _keyBitsToEntry; }
    const auto& getEntryToKeyBits() const { return _entryToKeyBits; }
    QString getKeysLabelsInReadingOrder(const Entry& entry, const QString& sSep = QString("|")) const;
    const KeysLabels& getKeysLabels() const {return _keysLabels;}

private:
    QString _sName;
    QString _sMarkdownFileName;
    uint _uiBitsCount;
    KeysLabels _keysLabels;
    KeyBitsReadingOrder _keyBitsReadingOrder;
    KeyBitsToEntry _keyBitsToEntry;
    EntryToKeyBits _entryToKeyBits;
};

typedef QMap<QString, Dictionary> Dictionaries;
inline uint qHash(const Dictionary::Entry& entry, uint nSeed = 0)
{
    QtPrivate::QHashCombine hash;
    for (const auto& keycode : entry.keycodes)
    {
        nSeed = hash(nSeed, keycode.getFirmwareString());
    }
    return nSeed;
}
