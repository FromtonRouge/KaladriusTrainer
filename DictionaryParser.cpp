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
        using qi::alnum;
        using qi::lit;
        using qi::lexeme;

        keycode = +(alnum | '_');
        valid_entry = lexeme['_' > +alnum > '('] >> (keycode % ',') >> ')';
        no_entry = lit("NO_ENTRY");
        element = (no_entry | valid_entry) >> *lit(",");
        array = lexeme['[' > +(alnum | '_') > ']'];
        variable = lexeme["g_" > +(alnum | '_')];

        table = lit("const") >> (lit("uint16_t") | lit("uint8_t")) >> lit("PROGMEM") >> variable >> array >> array >> '='
            >> '{'
            >> +(element)
            >> '}' >> ';';

        start = +table;
    }

    qi::rule<Iterator, ascii::space_type, std::string> start;
    qi::rule<Iterator, ascii::space_type, std::string> table;
    qi::rule<Iterator, ascii::space_type, std::string> variable;
    qi::rule<Iterator, ascii::space_type, std::string> array;
    qi::rule<Iterator, ascii::space_type, std::string> element;
    qi::rule<Iterator, ascii::space_type, std::string> no_entry;
    qi::rule<Iterator, ascii::space_type, std::string> valid_entry;
    qi::rule<Iterator, ascii::space_type, std::string> keycode;
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

        std::string sFilteredContent = sFileContent.toStdString();
        auto it = sFilteredContent.begin();
        bResult = qi::phrase_parse(it, sFilteredContent.end(), *_pInternalParser, ascii::space);
        std::string sParsedString(sFilteredContent.begin(), it);
        qDebug() << "Parsed string" << QString::fromStdString(sParsedString);
    }
    return bResult;
}
