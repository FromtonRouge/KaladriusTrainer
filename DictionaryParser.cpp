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
#include "Ast.h"
#include <QtCore/QFile>
#include <QtCore/QTextStream>
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
#include <boost/fusion/include/io.hpp>
#include <string>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace spirit = boost::spirit;
namespace phoenix = boost::phoenix;

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

        table = lit("const") >> identifier >> lit("PROGMEM") >> identifier[at_c<0>(_val) = _1] >> dimension >> dimension >> '='
            >> '{'
            >> any_entry[push_back(at_c<1>(_val), _1)] % ','
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
        const std::string& sFilteredContent = sFileContent.toStdString();
        auto it = sFilteredContent.begin();
        try
        {
            std::vector<Ast::Table> tables;
            bResult = qi::phrase_parse(it, sFilteredContent.end(), *_pInternalParser, ascii::space, tables);

            QFile fileParseResult("result.txt");
            if (fileParseResult.open(QFile::WriteOnly))
            {
                QTextStream textStream(&fileParseResult);
                for (const auto& table : tables)
                {
                    textStream << "Table " << QString::fromStdString(table.tableName) << "\n";
                    uint nIndex = 0;
                    for (const auto& entry : table.entries)
                    {
                        QStringList keycodes;
                        for (const auto& keycode : entry.keycodes)
                        {
                            if (!keycode.mods.empty())
                            {
                                keycodes << QString("%1(%2)").arg(QString::fromStdString(keycode.mods)).arg(QString::fromStdString(keycode.key));
                            }
                            else
                            {
                                keycodes << QString::fromStdString(keycode.key);
                            }
                        }
                        textStream << "\t" << QString("[%1] = ").arg(nIndex) << keycodes.join(", ") << "\n";
                        nIndex++;
                    }
                }
                fileParseResult.close();
            }
        }
        catch (const expectation_failure<IteratorType>&)
        {
        }
    }
    return bResult;
}
