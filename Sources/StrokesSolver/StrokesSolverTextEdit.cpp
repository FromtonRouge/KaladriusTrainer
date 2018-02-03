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
#include "Main/Application.h"
#include <QtCore/QRegularExpression>
#include <QtCore/QDebug>

StrokesSolverTextEdit::StrokesSolverTextEdit(QWidget* pParent)
    : QTextEdit(pParent)
{
    setAcceptRichText(false);
    setText("An orthographic theory of shorthand is based on spelling rather than phonetics. The advantage is that you can (in theory) write any word you know how to spell at least as fast as you can type it, because you do not need a dictionary to convert it from phonetic to written form. This is a sketch for an orthographic theory that might perhaps be implemented on a SOFTHRUF or a Planck, Atreus or other small keyboard that allows easy chording. It is intended to be a less specialised tool than Plover, easier to learn and to use as a general personal keyboard, while hopefully having more significant advantages than a simple change to key layout like Dvorak or Colemak.");
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChanged()));
}

void StrokesSolverTextEdit::onCursorPositionChanged()
{
    emit solverStarted();

    // Hard coded at the moment
    QStringList dictionariesToParse;
    dictionariesToParse << "Left Hand Shelton Dictionary";
    dictionariesToParse << "Thumbs Bigrams Dictionary";
    dictionariesToParse << "Thumbs Dictionary";
    dictionariesToParse << "Right Hand Shelton Dictionary";
    dictionariesToParse << "Right Controls Dictionary";
    dictionariesToParse << "Right Pinky Dictionary";

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

    QString sTextToSolve = cursor.selectedText();
    if (sTextToSolve == " ")
    {
        cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
        sTextToSolve = cursor.selectedText();
    }

    if (!sTextToSolve.isEmpty())
    {
        sTextToSolve = sTextToSolve.trimmed().toUpper(); // Remove leading space

        if (!solve(sTextToSolve, dictionaries, dictionariesToParse))
        {
            solve(sTextToSolve, dictionaries, {"Left Punctuation Dictionary", "Right Punctuation Dictionary"});
        }
    }
}

bool StrokesSolverTextEdit::solve(QString sText,
                                  const TheoryModel::CacheDictionaries& cachedDictionaries,
                                  const QStringList& orderedDictionaries) const
{
    bool bAtLeastOneMatch = false;

    // Try to find a match for each dictionary
    auto itDictionary = orderedDictionaries.begin();
    while (itDictionary != orderedDictionaries.end())
    {
        QBitArray bits;
        auto entries = cachedDictionaries[*itDictionary];
        const int iSize = sText.size();
        for (int iChar = iSize - 1; iChar >= 0; --iChar)
        {
            const QString& subString = sText.left(iChar + 1);
            auto itEntry = entries.find(subString);
            if (itEntry != entries.end())
            {
                bAtLeastOneMatch = true;
                bits = itEntry.value();
                sText = sText.right(iSize - iChar - 1);
                break;
            }
        }

        if (!bits.isNull())
        {
            emit dictionaryMatch(*itDictionary, bits);
        }
        itDictionary++;
    }
    return bAtLeastOneMatch;
}
