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
#include <QtCore/QFile>
#include <QtCore/QRegularExpression>
#include <QtCore/QDebug>
#include <boost/spirit/include/qi.hpp>
#include <string>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

template <typename Iterator>
struct InternalParser : qi::grammar <Iterator, ascii::space_type, std::string>
{
    InternalParser() : InternalParser::base_type(start)
    {
        using qi::space;
        using qi::int_;
        using qi::alnum;
        using qi::alpha;
        using qi::lit;
        using qi::lexeme;

        identifier = lexeme[(alpha | '_') > +(alnum|'_')]; // C identifier
        mod = identifier; // eg: LSFT
        keycode = (mod >> '(' >> identifier >> ')') | identifier; // eg: LSFT(_Y) or _Y 
        no_entry = lit("NO_ENTRY");
        entry = identifier >> '(' >> (keycode % ',') >> ')'; // eg: _3(_Y, _E, _S)
        dimension = lit('[') >> (int_ | identifier)  >> ']'; // eg: [256] or [MAX_LETTERS]
        type = identifier; // eg: uint16_t, int etc...

        table = lit("const") >> type >> lit("PROGMEM") >> identifier >> dimension >> dimension >> '='
            >> '{'
            >> (no_entry | entry) % ','
            >> '}' >> ';';

        start = +table;
    }

    qi::rule<Iterator, ascii::space_type, std::string> start;
    qi::rule<Iterator, ascii::space_type, std::string> table;
    qi::rule<Iterator, ascii::space_type, std::string> dimension;
    qi::rule<Iterator, ascii::space_type, std::string> no_entry;
    qi::rule<Iterator, ascii::space_type, std::string> entry;
    qi::rule<Iterator, ascii::space_type, std::string> mod;
    qi::rule<Iterator, ascii::space_type, std::string> keycode;
    qi::rule<Iterator, ascii::space_type, std::string> type;
    qi::rule<Iterator, ascii::space_type, std::string> identifier;
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

        // File cleaning
        sFileContent.remove(QRegularExpression("#include.*"));
        sFileContent.remove(QRegularExpression("//.*"));

        using qi::expectation_failure;
        std::string sFilteredContent = sFileContent.toStdString();
        auto it = sFilteredContent.begin();
        try
        {
            bResult = qi::phrase_parse(it, sFilteredContent.end(), *_pInternalParser, ascii::space);
            std::string sParsedString(sFilteredContent.begin(), it);
            qDebug() << "Parsed string" << QString::fromStdString(sParsedString);
        }
        catch (const expectation_failure<IteratorType>&)
        {
            // TODO: e.what_.tag.c_str();
        }
    }
    return bResult;
}
