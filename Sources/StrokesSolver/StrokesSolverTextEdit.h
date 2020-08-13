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

#include "../Theories/Models/TheoryModel.h"
#include "../Keycaps/KeycapState.h"
#include <QtWidgets/QTextEdit>
#include <QtGui/QColor>
#include <QtCore/QElapsedTimer>
#include <QtCore/QVector>
#include <QtCore/QChar>

class QTimer;
class WordCounter;
class StrokesSolverTextEdit : public QTextEdit
{
    Q_OBJECT

signals:
    void solverStarted() const;
    void dictionaryMatch(const QString& sDictionaryName, const QVector<QBitArray>& possibleBits) const;
    void notifySpecialKeys(const HashSpecialKeysStates& specialKeysStates) const;
    void reset() const;
    void started() const;

public:
    StrokesSolverTextEdit(QWidget* pParent = nullptr);

    void restart(const QString& sText = QString());
    void setWordCounter(WordCounter* pWordCounter);

public slots:
    void stopTraining();

protected slots:
    void onCursorPositionChanged();
    void onTimerSolve();
    void onTextChanged();

protected:
    void keyPressEvent(QKeyEvent* pKeyEvent) override;
    void mousePressEvent(QMouseEvent*) override {}
    void mouseDoubleClickEvent(QMouseEvent*) override {}
    void mouseMoveEvent(QMouseEvent*) override {}
    void mouseReleaseEvent(QMouseEvent*) override {}
    void contextMenuEvent(QContextMenuEvent*) override {}

private:
    bool solve(QString sText,
               const TheoryModel::CacheDictionaries& cachedDictionaries,
               const QStringList& dictionaries,
               HashSpecialKeysStates& rSpecialKeysStates) const;

    void processChord(const QChar& character);

private:
    QElapsedTimer _keyPressTimer;
    uint _uiInvalidCharacters = 0;
    uint _uiValidCharacters = 0;
    bool _bCleanState = true;
    WordCounter* _pWordCounter = nullptr;
    QColor _colorOk;
    QColor _colorError;
    QTimer* _pTimerSolve = nullptr;
};
