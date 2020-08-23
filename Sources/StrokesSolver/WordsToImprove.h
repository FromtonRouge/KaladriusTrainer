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

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QVector>

class WordsToImprove : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList wordsToImprove READ getWordsToImprove WRITE setWordsToImprove NOTIFY wordsToImproveChanged)
    Q_PROPERTY(QVector<int> progressValues READ getProgressValues WRITE setProgressValues NOTIFY progressValuesChanged)

signals:
    void wordsToImproveChanged(const QStringList& wordsToImprove) const;
    void progressValuesChanged(const QVector<int>& progressValues) const;
    void showResults(const QVector<int>& progressValues) const;

public:
    WordsToImprove(QObject* pParent = nullptr);
    ~WordsToImprove();

    const QStringList& getWordsToImprove() const {return _wordsToImprove;}
    void setWordsToImprove(const QStringList& wordsToImprove);
    const QVector<int>& getProgressValues() const {return _progressValues;}
    void setProgressValues(const QVector<int>& values);

private:
    QStringList _wordsToImprove;
    QVector<int> _progressValues;
};
