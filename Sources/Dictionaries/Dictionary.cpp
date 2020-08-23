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

#include "Dictionary.h"
#include <cmath>

Dictionary::Entry::Entry(const Keycodes& keycodes, uint nBitsCount, uint nKeyBits)
    : keycodes(keycodes)
    , bits(nBitsCount, nKeyBits)
{
    std::string sBitset;
    boost::to_string(bits, sBitset);
    bitsAsString = QString::fromStdString(sBitset);

    QStringList vKeycodes;
    for (const auto& keycode : keycodes)
    {
        const QString& sUserString = keycode.getUserString();
        if (!sUserString.isEmpty())
        {
            vKeycodes << keycode.getUserString();
        }
    }
    keycodesAsUserString = vKeycodes.join("");
}

Dictionary::Dictionary(const QString& sName, 
                       const QString& sMarkdownFileName,
                       uint uiSize,
                       const KeysLabels& keysLabels, 
                       const KeyBitsReadingOrder& keyBitsReadingOrder)
    : _sName(sName)
    , _sMarkdownFileName(sMarkdownFileName)
    , _uiBitsCount(static_cast<uint>(std::floor(std::log2(uiSize + 1))))
    , _keysLabels(keysLabels)
    , _keyBitsReadingOrder(keyBitsReadingOrder)
{
}

Dictionary::~Dictionary()
{
}

void Dictionary::addEntry(const Keycodes& keycodes, uint nKeyBits)
{
    Entry entry(keycodes, _uiBitsCount, nKeyBits);
    _keyBitsToEntry << entry;
}

QString Dictionary::getKeysLabelsInReadingOrder(const Entry& entry, const QString& sSep) const
{
    QStringList result;
    if (int(entry.bits.size()) == _keysLabels.size())
    {
        for (Entry::KeyBits::size_type i = 0; i < entry.bits.size(); ++i)
        {
            int iReadIndex = static_cast<int>(i);
            if (!_keyBitsReadingOrder.isEmpty() && _keyBitsReadingOrder.size() == int(entry.bits.size()))
            {
                iReadIndex = _keyBitsReadingOrder[iReadIndex];
            }

            const bool bEnabled = entry.bits.test(size_t(iReadIndex));
            if (bEnabled)
            {
                result << _keysLabels[iReadIndex];
            }
        }
    }
    return result.isEmpty() ? entry.bitsAsString : result.join(sSep);
}
