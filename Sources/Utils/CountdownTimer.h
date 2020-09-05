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
#include <QtCore/QElapsedTimer>
#include <qqml.h>

class CountdownTimer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString remainingTimeString READ getRemainingTimeString)
    Q_PROPERTY(QString elapsedTimeString READ getElapsedTimeString)
    Q_PROPERTY(int remainingTime READ getRemainingTime)
    Q_PROPERTY(int totalTime READ getTotalTime)
    Q_PROPERTY(bool done READ isDone)

signals:
    void remainingTimeChanged() const;
    void done() const;

public:
    enum class Mode
    {
        Countdown,
        Timer,
    };


public:
    CountdownTimer(QObject* pParent = nullptr);

    QString getRemainingTimeString() const;
    QString getElapsedTimeString() const;
    int getRemainingTime() const;
    int getElapsedTime() const;
    float getTotalTime() const {return _iTotalTime;}
    float getTotalTimeInSeconds() const {return float(_iTotalTime) / 1000;}
    bool isDone();
    bool isRunning() const {return _timer.isValid();}

public slots:
    void start();
    void stop();
    void reset(int iMode);
    void setTotalTime(float fSeconds);
    bool isCountdownMode() const {return _mode == Mode::Countdown;}

private:
    Mode _mode = Mode::Countdown;
    bool _bDone = false;
    int _iTotalTime = 60000; // in ms
    QElapsedTimer _timer;
    qint64 _elapsedTimeOnStop = 0;
};
