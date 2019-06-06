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

#include <QtCore/QStringList>
#include <QtCore/QVector>
#include <QtCore/QHash>
#include <QtCore/QDebug>
#include <QtCore/qhashfunctions.h>

class Keycode
{
public:
    Keycode(const QStringList& mods = QStringList(), const QString& sKeycode = QString());
    ~Keycode();
    
    const QStringList& getMods() const { return _mods; }
    const QString& getKeycode() const { return _sKeycode; }
    const QString& getFirmwareString() const { return _sFirmwareString; }
    const QString& getUserString() const { return _sUserString; }
    bool operator==(const Keycode& c) const { return _sFirmwareString == c._sFirmwareString; }

private:
    QStringList _mods;
    QString _sKeycode;
    QString _sFirmwareString;
    QString _sUserString;
    static QHash<QString, QString> FIRMWARE_STRING_TO_USER_STRING;
};

typedef QVector<Keycode> Keycodes;

QDebug operator<<(QDebug dbg, const Keycode& keycode);
inline uint qHash(const Keycode& keycode, uint nSeed = 0)
{
    QtPrivate::QHashCombine hash;
    nSeed = hash(nSeed, keycode.getFirmwareString());
    return nSeed;
}
