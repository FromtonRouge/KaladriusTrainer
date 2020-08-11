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

#include <QtCore/QObject>
#include <QtCore/QElapsedTimer>
#include <qqml.h>

class CountdownTimer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString remainingTimeString READ getRemainingTimeString)
    Q_PROPERTY(int remainingTime READ getRemainingTime)
    Q_PROPERTY(int totalTime READ getTotalTime)
    Q_PROPERTY(bool done READ isDone)

signals:
    void remainingTimeChanged() const;
    void done() const;

public:
    CountdownTimer(QObject* pParent = nullptr);

    QString getRemainingTimeString() const;
    int getRemainingTime() const;
    int getElapsedTime() const;
    float getTotalTime() const {return _iTotalTime;}
    float getTotalTimeInSeconds() const {return float(_iTotalTime) / 1000;}
    bool isDone();

public slots:
    void start();
    void reset();
    void setTotalTime(float fSeconds);

private:
    bool _bDone = false;
    int _iTotalTime = 60000; // in ms
    QElapsedTimer _timer;
};
