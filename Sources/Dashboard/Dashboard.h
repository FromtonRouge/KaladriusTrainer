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

class Dashboard : public QObject
{
    Q_OBJECT

    Q_PROPERTY(float lastWpm READ getLastWpm WRITE setLastWpm NOTIFY lastWpmChanged)
    Q_PROPERTY(float lastSpm READ getLastSpm WRITE setLastSpm NOTIFY lastSpmChanged)
    Q_PROPERTY(float lastAccuracy READ getLastAccuracy WRITE setLastAccuracy NOTIFY lastAccuracyChanged)
    Q_PROPERTY(float maxWpm READ getMaxWpm WRITE setMaxWpm NOTIFY maxWpmChanged)
    Q_PROPERTY(float maxSpm READ getMaxSpm WRITE setMaxSpm NOTIFY maxSpmChanged)
    Q_PROPERTY(float maxAccuracy READ getMaxAccuracy WRITE setMaxAccuracy NOTIFY maxAccuracyChanged)

signals:
    void lastWpmChanged(float value) const;
    void lastSpmChanged(float value) const;
    void lastAccuracyChanged(float value) const;
    void maxWpmChanged(float value) const;
    void maxSpmChanged(float value) const;
    void maxAccuracyChanged(float value) const;

public:
    Dashboard(QObject* pParent = nullptr);

    float getLastWpm() const {return _fLastWpm;}
    void setLastWpm(float fValue);
    float getLastSpm() const {return _fLastSpm;}
    void setLastSpm(float fValue);
    float getLastAccuracy() const {return _fLastAccuracy;}
    void setLastAccuracy(float fValue);

    float getMaxWpm() const {return _fMaxWpm;}
    void setMaxWpm(float fValue);
    float getMaxSpm() const {return _fMaxSpm;}
    void setMaxSpm(float fValue);
    float getMaxAccuracy() const {return _fMaxAccuracy;}
    void setMaxAccuracy(float fValue);

private:
    float _fLastWpm = 0;
    float _fLastSpm = 0;
    float _fLastAccuracy = 0;
    float _fMaxWpm = 0;
    float _fMaxSpm = 0;
    float _fMaxAccuracy = 0;
};
