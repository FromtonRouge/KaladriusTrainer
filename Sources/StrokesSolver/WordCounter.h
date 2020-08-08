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

#include <QtCore/QChar>
#include <QtCore/QStack>
#include <QtCore/QStringList>
#include <QtCore/QObject>

class CountdownTimer;
class WordCounter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int wpm READ getWPM)

public:
    WordCounter(CountdownTimer* pCountdownTimer, QObject* pParent = nullptr);

    void reset();
    void pushInputState(const QChar& expected, const QChar& input);
    void popInputState();
    int getWPM() const;

private:
    struct InputState
    {
        QChar expected;
        QChar input;
    };

    CountdownTimer* _pCountdownTimer = nullptr;
    QStack<InputState> _inputStates;
    QString _sCurrentWord;
    int _iCurrentWordErrors = 0;
    QStringList _validWords;
    uint _iValidCharacters = 0;
    const int AVERAGE_WORD_LENGTH = 4; // Same as www.10fastfingers.com
};
