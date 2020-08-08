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

#include "StrokesSolverTextEdit.h"
#include "../Main/Application.h"
#include "WordCounter.h"
#include <QtCore/QRegularExpression>
#include <QtGui/QKeyEvent>
#include <QtCore/QTimer>
#include <QtCore/QSet>
#include <QtCore/QDebug>

StrokesSolverTextEdit::StrokesSolverTextEdit(QWidget* pParent)
    : QTextEdit(pParent)
    , _colorOk(0, 255, 0, 128)
    , _colorWarning(250, 200, 50, 200)
    , _colorError(255, 0, 0, 200)
    , _pTimerSolve(new QTimer(this))
{
    setAcceptRichText(false);
    connect(this, &StrokesSolverTextEdit::cursorPositionChanged, this, &StrokesSolverTextEdit::onCursorPositionChanged);
    connect(this, &QTextEdit::textChanged, this, &StrokesSolverTextEdit::onTextChanged);

    _pTimerSolve->setSingleShot(true);
    _pTimerSolve->setInterval(10);
    connect(_pTimerSolve, &QTimer::timeout, this, &StrokesSolverTextEdit::onTimerSolve);
}

void StrokesSolverTextEdit::restart(const QString& sText)
{
    setEnabled(true);

    blockSignals(true);
    setText(sText);
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    auto format = cursor.charFormat();
    format.setBackground(QBrush());
    cursor.setCharFormat(format);
    setTextCursor(cursor);

    cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
    setTextCursor(cursor);
    blockSignals(false);

    _bCleanState = true;
    if (_pWordCounter)
    {
        _pWordCounter->reset();
    }
    setFocus();
    emit reset();
}

void StrokesSolverTextEdit::setWordCounter(WordCounter* pWordCounter)
{
   _pWordCounter = pWordCounter;
}

void StrokesSolverTextEdit::onCursorPositionChanged()
{
    _pTimerSolve->start();
}

void StrokesSolverTextEdit::onTimerSolve()
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
    dictionariesToParse << "Right Controls Dictionary";

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

    bool bNoSpace = true;
    QString sTextToSolve = cursor.selectedText();
    if (sTextToSolve == " ")
    {
        cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
        sTextToSolve = cursor.selectedText();
        bNoSpace = false;
    }
    else if (sTextToSolve.startsWith('.') || sTextToSolve.startsWith(',') || sTextToSolve.startsWith(';'))
    {
        bNoSpace = false;
    }

    if (!sTextToSolve.isEmpty())
    {
        sTextToSolve = sTextToSolve.trimmed().toUpper(); // Remove leading space

        // Get all special keys
        HashSpecialKeysStates hashSpecialKeysStates;
        const QModelIndex& indexSpecialKeys = pTheoryModel->getSpecialKeysIndex();
        if (indexSpecialKeys.isValid())
        {
            const int iSpecialKeys = pTheoryModel->rowCount(indexSpecialKeys);
            for (int iSpecialKey = 0; iSpecialKey < iSpecialKeys; ++iSpecialKey)
            {
                const QString& sSpecialKeyIdentifier = pTheoryModel->index(iSpecialKey, 0, indexSpecialKeys).data(Qt::DisplayRole).toString();
                hashSpecialKeysStates.insert(sSpecialKeyIdentifier, false);
            }
        }

        if (!solve(sTextToSolve, dictionaries, dictionariesToParse, hashSpecialKeysStates))
        {
            solve(sTextToSolve, dictionaries, {"Left Punctuation Dictionary", "Right Punctuation Dictionary"}, hashSpecialKeysStates);
        }

        // To cancel the automatic space insertion, the user has to press the "NO SEPARATOR" key
        if (bNoSpace)
        {
            hashSpecialKeysStates["skSeparator"] = true;
        }

        // Send all special keys states once
        emit notifySpecialKeys(hashSpecialKeysStates);
    }
}

void StrokesSolverTextEdit::keyPressEvent(QKeyEvent* pKeyEvent)
{
    if (!(pKeyEvent->modifiers() & Qt::ControlModifier))
    {
        auto cursor = textCursor();
        auto format = cursor.charFormat();
        switch (pKeyEvent->key())
        {
        case Qt::Key_PageDown:
        case Qt::Key_PageUp:
        case Qt::Key_Home:
        case Qt::Key_End:
        case Qt::Key_Left:
            {
                if (format.background() != QBrush())
                {
                    // Move the cursor backward without selecting the text so
                    // the solve() can work properly
                    cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor);
                    setTextCursor(cursor);

                    // Reset the background color
                    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
                    format.setBackground(QBrush());
                    cursor.setCharFormat(format);
                    setTextCursor(cursor);

                    // Go back again...
                    cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor);
                    setTextCursor(cursor);
                    return;
                }
                // No break
            }
        case Qt::Key_Right:
        case Qt::Key_Down:
        case Qt::Key_Up:
            {
                return QTextEdit::keyPressEvent(pKeyEvent);
            }
        case Qt::Key_Backspace:
            {
                if (format.background() == _colorError)
                {
                    _pWordCounter->popInputState();
                    return QTextEdit::keyPressEvent(pKeyEvent);
                }
                else if (format.background() != QBrush())
                {
                    // Move the cursor backward without selecting the text so
                    // the solve() can work properly
                    cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor);
                    setTextCursor(cursor);

                    // Reset the background color
                    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
                    format.setBackground(QBrush());
                    cursor.setCharFormat(format);
                    setTextCursor(cursor);

                    // Go back again...
                    cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor);
                    setTextCursor(cursor);
                }
                else
                {
                    cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor);
                    setTextCursor(cursor);
                }
                _pWordCounter->popInputState();
                return;
            }
        default:
            {
                break;
            }
        }

        const QString& sInputText = pKeyEvent->text();
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
        const QString& sSelectedText = cursor.selectedText();
        if (sInputText != sSelectedText)
        {
            if (sInputText.toUpper() == sSelectedText.toUpper())
            {
                // Warning
                format.setBackground(_colorWarning);
                cursor.setCharFormat(format);
                cursor.setPosition(cursor.position(), QTextCursor::MoveAnchor);
                setTextCursor(cursor);
            }
            else
            {
                // Error dectected
                cursor = textCursor();
                format = cursor.charFormat();
                format.setBackground(_colorError);
                cursor.setCharFormat(format);
                cursor.insertText(sInputText);
            }
        }
        else
        {
            // Valid input
            format = cursor.charFormat();
            format.setBackground(_colorOk);
            cursor.setCharFormat(format);
            cursor.setPosition(cursor.position(), QTextCursor::MoveAnchor);
            setTextCursor(cursor);

        }

        if (!sSelectedText.isEmpty() && !sInputText.isEmpty())
        {
            _pWordCounter->pushInputState(sSelectedText[0], sInputText[0]);
        }
    }
    else
    {
        QTextEdit::keyPressEvent(pKeyEvent);
    }
}

bool StrokesSolverTextEdit::solve(QString sText,
                                  const TheoryModel::CacheDictionaries& cachedDictionaries,
                                  const QStringList& orderedDictionaries,
                                  HashSpecialKeysStates& rSpecialKeysStates) const
{
    bool bAtLeastOneMatch = false;

    // A dictionary can appears more than once in orderedDictionaries
    QSet<QString> matchedDictionaries;

    // Try to find a match for each dictionary
    auto itDictionary = orderedDictionaries.begin();
    while (itDictionary != orderedDictionaries.end())
    {
        const QString& sDictionaryName = *itDictionary++;
        if (matchedDictionaries.contains(sDictionaryName))
        {
            continue;
        }

        QVector<QBitArray> possibleBits;
        const auto& cachedDictionaryInfo = cachedDictionaries[sDictionaryName];
        auto entries = cachedDictionaryInfo.entries;
        const int iSize = sText.size();
        for (int iChar = iSize - 1; iChar >= 0; --iChar)
        {
            const QString& subString = sText.left(iChar + 1);
            
            // Get all possible bits for the current text
            auto itPossibleBits = entries.find(subString);
            while (itPossibleBits != entries.end() && itPossibleBits.key() == subString)
            {
                possibleBits << itPossibleBits.value();
                itPossibleBits++;
            }
            
            if (!possibleBits.isEmpty())
            {
                matchedDictionaries.insert(sDictionaryName);
                bAtLeastOneMatch = true;
                sText = sText.right(iSize - iChar - 1);
                break;
            }
        }

        if (!possibleBits.isEmpty())
        {
            // Set the mandatory key in the special keys states
            auto it = rSpecialKeysStates.find(cachedDictionaryInfo.sMandatorySpecialKey);
            if (it != rSpecialKeysStates.end())
            {
                it.value() = true;
            }

            emit dictionaryMatch(sDictionaryName, possibleBits);
        }
    }
    return bAtLeastOneMatch;
}

void StrokesSolverTextEdit::onTextChanged()
{
    if (_bCleanState)
    {
        _bCleanState = false;
        emit started();
    }
}

void StrokesSolverTextEdit::stopTraining()
{
    setDisabled(true);
}
