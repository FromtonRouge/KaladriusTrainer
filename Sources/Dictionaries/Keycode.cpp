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

#include "Keycode.h"

QDebug operator<<(QDebug dbg, const Keycode& keycode)
{
    dbg.space() << keycode.getFirmwareString();
	return dbg.space();
}

QHash<QString, QString> Keycode::FIRMWARE_STRING_TO_USER_STRING =
{
	{"CKC_CAMEL", "[camel case mode]"},
	{"CKC_CASE_INNER_LOCKED", "[inner case locked]"},
	{"CKC_CASE_INNER_ONCE", "[inner case once]"},
	{"CKC_CASE_NORMAL", "[normal case]"},
	{"CKC_CASE_UPPER_LOCKED", "[upper case locked]"},
	{"CKC_CASE_UPPER_ONCE", "[upper case once]"},
	{"CKC_RESET_SEP_AND_CASE", "[reset case and separator]"},
    {"CKC_DELWORD", "<lctrl>+<lshift>+<left><bspc>"},
    {"CKC_DLEFT", "<left><left>"},
    {"LSFT", "<lshift>"},
    {"KC_SPC", "<space>"},
    {"KC_LEFT", "<left>"},
    {"KC_ENT", "<enter>"},
    {"KC_END", "<end>"},
    {"KC_ESC", "<esc>"},
    {"KC_HOME", "<home>"},
    {"KC_DEL", "<del>"},
    {"KC_TAB", "<tab>"},
    {"CKC_ENTABOVE", "<up><end><enter>"},
	{"NO_ENTRY", ""},
	{"_A", "A"},
	{"_B", "B"},
	{"_C", "C"},
	{"_D", "D"},
	{"_E", "E"},
	{"_F", "F"},
	{"_G", "G"},
	{"_H", "H"},
	{"_I", "I"},
	{"_J", "J"},
	{"_K", "K"},
	{"_L", "L"},
	{"_M", "M"},
	{"_N", "N"},
	{"_O", "O"},
	{"_P", "P"},
	{"_Q", "Q"},
	{"_R", "R"},
	{"_S", "S"},
	{"_T", "T"},
	{"_U", "U"},
	{"_V", "V"},
	{"_W", "W"},
	{"_X", "X"},
	{"_Y", "Y"},
	{"_Z", "Z"},
	{"_AGRV", "À"},
	{"_AMP", "&"},
    {"_APOS", "'"},
	{"_ASTR", "*"},
	{"_AT", "@"},
	{"_BSLS", "\\"},
	{"_BULT", "BULT"},
	{"_CCED", "Ç"},
	{"_CIRC", "^"},
	{"_COLN", ":"},
	{"_COMM", ","},
	{"_DLR", "$"},
	{"_DOT", "."},
	{"_DQUOT", "\""},
	{"_EACU", "EACU"},
	{"_EGRV", "È"},
	{"_EQL", "="},
	{"_EURO", "€"},
	{"_EXLM", "!"},
	{"_GRTR", ">"},
	{"_GRV", "`"},
    {"_HASH", "#"},
	{"_LBRC", "["},
	{"_LCBR", "{"},
	{"_LESS", "<"},
	{"_LPRN", "("},
	{"_MINS", "-"},
	{"_MU", "µ"},
	{"_N0", "0"},
	{"_N1", "1"},
	{"_N2", "2"},
	{"_N3", "3"},
	{"_N4", "4"},
	{"_N5", "5"},
	{"_N6", "6"},
	{"_N7", "7"},
	{"_N8", "8"},
	{"_N9", "9"},
	{"_OVRR", "OVRR"},
	{"_PERC", "%"},
	{"_PIPE", "|"},
	{"_PLUS", "+"},
	{"_PND", "PND"},
	{"_QUES", "?"},
	{"_RBRC", "]"},
	{"_RCBR", "}"},
	{"_RPRN", ")"},
    {"_SCLN", ";"},
	{"_SECT", "SECT"},
	{"_SLSH", "/"},
	{"_SUP2", "SUP2"},
	{"_TILD", "~"},
	{"_UGRV", "Ù"},
	{"_UMLT", "UMLT"},
	{"_UNDS", "_"}
};

Keycode::Keycode(const QStringList& mods, const QString& sKeycode)
    : _mods(mods)
    , _sKeycode(sKeycode)
{
    _sFirmwareString = _sKeycode;
    if (!_mods.isEmpty())
    {
        _sFirmwareString = QString("%1(%2%3").arg(_mods.join("(")).arg(_sKeycode).arg(QString(")").repeated(_mods.size()));
    }

    // Translate mods
    QStringList vUserMods;
    QString sUserMod;
    for (const QString& sMod : _mods)
    {
        sUserMod = sMod;
        auto it = FIRMWARE_STRING_TO_USER_STRING.find(sMod);
        if (it != FIRMWARE_STRING_TO_USER_STRING.end())
        {
            sUserMod = it.value();
        }
        vUserMods << sUserMod;
    }

    _sUserString = _sKeycode;
    auto it = FIRMWARE_STRING_TO_USER_STRING.find(_sKeycode);
    if (it != FIRMWARE_STRING_TO_USER_STRING.end())
    {
        _sUserString = it.value();
    }

    if (!vUserMods.isEmpty())
    {
        _sUserString = QString("%1+%2").arg(vUserMods.join("+")).arg(_sUserString);
    }
}

Keycode::~Keycode()
{
}
