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
#include <QtSql/QSqlQuery>

class Database : public QObject
{
    Q_OBJECT

public:
    Database(QObject* pParent = nullptr);
    ~Database();

    bool open();

    float getLastWpm(const QString& sTableName) const;
    float getLastSpm(const QString& sTableName) const;
    float getLastAccuracy(const QString& sTableName) const;

    float getMaxWpm(const QString& sTableName) const;
    float getMaxSpm(const QString& sTableName) const;
    float getMaxAccuracy(const QString& sTableName) const;

private:
    QSqlQuery getLastRecord(const QString& sTableName) const;
    QSqlQuery getMax(const QString& sColumnName, const QString& sTableName) const;

private:
    QString getDatabaseFilePath() const;
};
