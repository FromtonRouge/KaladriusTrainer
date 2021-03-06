// ======================================================================
// This file is a part of the KaladriusTrainer project
//
// Copyright (C) 2020  Vissale NEANG <fromtonrouge at gmail dot com>
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

#include <QtWidgets/QWidget>
#include <QtCore/QScopedPointer>

namespace Ui
{
    class StrokesSolverWidget;
}

class WordsToImprove;

class StrokesSolverWidget : public QWidget
{
    Q_OBJECT

signals:
    void restartNeeded() const;

public:
    StrokesSolverWidget(QWidget* pParent = nullptr);
    ~StrokesSolverWidget();

    WordsToImprove* getWordsToImprove() const {return _pWordsToImprove;}

public slots:
    void restart(const QString& sText, int iLevelType, int iTextId);

protected slots:
    void on_fontComboBox_currentFontChanged(QFont font);
    void on_comboBoxFontSize_currentTextChanged(const QString& sText);
    void on_pushButtonRestart_released();
    void on_spinBoxMinimumCharacters_valueChanged(int i);
    void on_spinBoxMaximumCharacters_valueChanged(int i);
    void on_lineEditContains_textChanged(const QString& sText);
    void on_lineEditDoesNotContain_textChanged(const QString& sText);

private:
    QScopedPointer<Ui::StrokesSolverWidget> _pUi;
    WordsToImprove* _pWordsToImprove = nullptr;
};
