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

#include "DictionaryParser.h"
#include "Keycode.h"
#include <QtCore/QFile>
#include <QtCore/QRegularExpression>
#include <QtCore/QDebug>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <string>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace spirit = boost::spirit;
namespace phoenix = boost::phoenix;

namespace Ast
{
    struct Keycode
    {
        Keycode(const std::string& key = std::string())
        {
            this->key = key;
        }
        std::string mods;
        std::string key;
    };

    struct Entry
    {
        Entry(const std::string& s = std::string())
        {
            if (!s.empty())
            {
                Keycode keycode;
                keycode.key = s;
                keycodes.push_back(keycode);
            }
        }
        std::vector<Keycode> keycodes;
    };

    struct Table
    {
        std::string tableName;
        std::vector<Entry> entries;
    };
}

BOOST_FUSION_ADAPT_STRUCT(Ast::Keycode, (std::string, mods) (std::string, key))
BOOST_FUSION_ADAPT_STRUCT(Ast::Entry, (std::vector<Ast::Keycode>, keycodes))
BOOST_FUSION_ADAPT_STRUCT(Ast::Table, (std::string, tableName) (std::vector<Ast::Entry>, entries))

template <typename Iterator>
struct InternalParser : qi::grammar <Iterator, ascii::space_type, std::vector<Ast::Table>()>
{
    InternalParser() : InternalParser::base_type(start)
    {
        using qi::_val;
        using qi::_1;
        using qi::space;
        using qi::int_;
        using qi::alnum;
        using qi::alpha;
        using qi::lit;
        using qi::string;
        using qi::lexeme;
        using ascii::char_;
        using phoenix::at_c;
        using phoenix::push_back;

        identifier = lexeme[(alpha | char_('_')) > +(alnum|char_('_'))]; // C identifier
        mod = lit("LSFT"); // eg: LSFT
        keycode = (mod[at_c<0>(_val) = _1] >> '(' >> identifier[at_c<1>(_val) = _1] >> ')') | identifier[at_c<1>(_val) = _1]; // eg: LSFT(_Y) or _Y 
        no_entry = string("NO_ENTRY")[push_back(at_c<0>(_val), _1)];
        entry = identifier >> '(' >> (keycode[push_back(at_c<0>(_val), _1)] % ',') >> ')'; // eg: _3(_Y, _E, _S)
        any_entry = (no_entry | entry);
        dimension = lit('[') >> (int_ | identifier)  >> ']'; // eg: [256] or [MAX_LETTERS]
        type = identifier; // eg: uint16_t, int etc...

        table = *lit("const") >> identifier >> lit("PROGMEM") >> identifier[at_c<0>(_val) = _1] >> dimension >> dimension >> '='
            >> '{'
            >> any_entry[push_back(at_c<1>(_val), _1)] % ',' >> *lit(',') // Note: Optional last comma
            >> '}' >> ';';

        start = +table[push_back(_val, _1)];
    }

    qi::rule<Iterator, ascii::space_type, std::vector<Ast::Table>()> start;
    qi::rule<Iterator, ascii::space_type, Ast::Table()> table;
    qi::rule<Iterator, ascii::space_type, std::string()> dimension;
    qi::rule<Iterator, ascii::space_type, Ast::Entry()> any_entry;
    qi::rule<Iterator, ascii::space_type, Ast::Entry()> no_entry;
    qi::rule<Iterator, ascii::space_type, Ast::Entry()> entry;
    qi::rule<Iterator, ascii::space_type, std::string()> mod;
    qi::rule<Iterator, ascii::space_type, Ast::Keycode()> keycode;
    qi::rule<Iterator, ascii::space_type, std::string()> type;
    qi::rule<Iterator, ascii::space_type, std::string()> identifier;
};

// Hardcoded known dictionaries from the firmware
DictionaryParser::KnownDictionaries DictionaryParser::KNOWN_DICTIONARIES =
{
    {"g_left_controls_table",       {"Left Controls Dictionary",        "dict_left_controls.md",       {"L0", "L1", "L2", "L3", "L4", "L5"}, {5, 4, 3, 2, 1, 0}}},
    {"g_left_hand_table",           {"Left Hand Shelton Dictionary",    "dict_left_hand.md",           {"N", "R", "W", "H", "C", "T", "A", "S"}, {6, 7, 4, 5, 2, 3, 0, 1}}},
    {"g_left_pinky_table",          {"Left Pinky Dictionary",           "dict_left_pinky.md",          {"O", "U", "I"}, {2, 1, 0}}},
    {"g_left_punctuation_table",    {"Left Punctuation Dictionary",     "dict_left_punctuation.md",    {"LP0", "LP1", "LP2", "LP3", "LP4", "LP5", "LP6", "LP7"}, {6, 7, 4, 5, 2, 3, 0, 1}}},
    {"g_right_controls_table",      {"Right Controls Dictionary",       "dict_right_controls.md",      {"R0", "R1", "R2", "R3", "R4"}}},
    {"g_right_hand_table",          {"Right Hand Shelton Dictionary",   "dict_right_hand.md",          {"R", "N", "L", "G", "C", "H", "T", "S"}}},
    {"g_right_pinky_table",         {"Right Pinky Dictionary",          "dict_right_pinky.md",         {"E", "Y", "S"}}},
    {"g_right_punctuation_table",   {"Right Punctuation Dictionary",    "dict_right_punctuation.md",   {"RP0", "RP1", "RP2", "RP3", "RP4", "RP5", "RP6", "RP7"}}},
    {"g_thumbs_bigrams_table",      {"Thumbs Bigrams Dictionary",       "dict_thumbs_bigrams.md",      {"E", "O", "A", "U", "I"}}},
    {"g_thumbs_table",              {"Thumbs Dictionary",               "dict_thumbs.md",              {"E", "O", "A", "U", "I"}}}
};

DictionaryParser::DictionaryParser(const QString& sFilePath)
    : _pInternalParser(new InternalParser<IteratorType>())
    , _sFilePath(sFilePath)
{
}

DictionaryParser::~DictionaryParser()
{
}

bool DictionaryParser::parse()
{
    bool bResult = false;
    QFile file(_sFilePath);
    if (file.open(QFile::ReadOnly))
    {
        QString sFileContent = file.readAll();
        file.close();

        // Removing comments and #include lines
        sFileContent.remove(QRegularExpression("#include.*"));
        sFileContent.remove(QRegularExpression("//.*"));

        using qi::expectation_failure;
        const std::string& sFilteredContent = sFileContent.toStdString();
        auto it = sFilteredContent.begin();
        try
        {
            std::vector<Ast::Table> tables;
            bResult = qi::phrase_parse(it, sFilteredContent.end(), *_pInternalParser, ascii::space, tables);

            for (const auto& table : tables)
            {
                const QString& sTableFirmwareName = QString::fromStdString(table.tableName);
                auto it = KNOWN_DICTIONARIES.find(sTableFirmwareName);
                if (it != KNOWN_DICTIONARIES.end())
                {
                    const auto& knownDictionary = it.value();

                    const uint nTableSize = static_cast<uint>(table.entries.size());
                    Dictionary dictionary(knownDictionary.userName,
                                          knownDictionary.markdownFileName,
                                          nTableSize,
                                          knownDictionary.keysLabels,
                                          knownDictionary.keyBitsReadingOrder);

                    for (size_t i = 0; i < table.entries.size(); ++i)
                    {
                        const auto& entry = table.entries[i];
                        Keycodes keycodes;
                        for (const auto& keycode : entry.keycodes)
                        {
                            QStringList vMods;
                            if (!keycode.mods.empty())
                            {
                                vMods << QString::fromStdString(keycode.mods);
                            }
                            keycodes << Keycode(vMods, QString::fromStdString(keycode.key));
                        }
                        dictionary.addEntry(keycodes, uint(i));
                    }
                    _dictionaries.insert(dictionary.getName(), dictionary);
                }
            }
        }
        catch (const expectation_failure<IteratorType>&)
        {
        }
    }
    return bResult;
}
