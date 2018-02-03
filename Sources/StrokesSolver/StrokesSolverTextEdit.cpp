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

#include "StrokesSolverTextEdit.h"
#include "Theories/TheoryModel.h"
#include "Main/Application.h"
#include <QtCore/QRegularExpression>
#include <QtCore/QDebug>

StrokesSolverTextEdit::StrokesSolverTextEdit(QWidget* pParent)
    : QTextEdit(pParent)
{
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChanged()));
}

void StrokesSolverTextEdit::onCursorPositionChanged()
{
    // Hard coded at the moment
    QStringList dictionariesToParse;
    dictionariesToParse << "Left Hand Shelton Dictionary";
    dictionariesToParse << "Thumbs Bigrams Dictionary";
    dictionariesToParse << "Thumbs Dictionary";
    dictionariesToParse << "Right Hand Shelton Dictionary";
    dictionariesToParse << "Right Controls Dictionary";
    dictionariesToParse << "Right Pinky Dictionary";
    //dictionariesToParse << "Left Punctuation Dictionary";
    //dictionariesToParse << "Right Punctuation Dictionary";

    for (const auto& sDictionaryName : dictionariesToParse)
    {
        emit strokeFound(sDictionaryName, QBitArray());
    }

    // Check first if we have cached dictionaries data
    auto pTheoryModel = qApp->getTheoryModel();
    const auto& dictionaries = pTheoryModel->getCachedDictionaries();
    if (dictionaries.isEmpty())
    {
        return;
    }

    // Get the next word to solve from the current cursor position
    auto cursor = textCursor();
    QTextCursor findCursor = document()->find(QRegularExpression("[ \\t]"), cursor);
    if (findCursor.isNull())
    {
        cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
    }
    else
    {
        cursor.setPosition(findCursor.position(), QTextCursor::KeepAnchor);
    }

    QString sTextToSolve = cursor.selectedText().toUpper();
    if (sTextToSolve == " ")
    {
        cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
        sTextToSolve = cursor.selectedText().toUpper();
    }

    if (!sTextToSolve.isEmpty())
    {
        sTextToSolve = sTextToSolve.trimmed(); // Remove leading space

        // Try to find a match for the first dictionary
        auto itDictionary = dictionariesToParse.begin();
        while (itDictionary != dictionariesToParse.end())
        {
            QBitArray bits;
            auto entries = dictionaries[*itDictionary];
            const int iSize = sTextToSolve.size();
            for (int iChar = iSize - 1; iChar >= 0; --iChar)
            {
                const QString& subString = sTextToSolve.left(iChar + 1);
                auto itEntry = entries.find(subString);
                if (itEntry != entries.end())
                {
                    bits = itEntry.value();
                    sTextToSolve = sTextToSolve.right(iSize - iChar - 1);
                    break;
                }
            }

            if (!bits.isNull())
            {
                emit strokeFound(*itDictionary, bits);
            }
            itDictionary++;
        }
    }
}
