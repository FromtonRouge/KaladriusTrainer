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

#include "Database.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtCore/QStandardPaths>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QVariant>
#include <iostream>

Database::Database(QObject *pParent)
    : QObject(pParent)
{

}

Database::~Database()
{

}

bool Database::open()
{
    const QString& sDatabaseFilePath = getDatabaseFilePath();
    if (sDatabaseFilePath.isEmpty() == false)
    {
        QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
        if (database.isValid())
        {
            database.setDatabaseName(sDatabaseFilePath);
            if (database.open())
            {
                return true;
            }
            else
            {
                QString sError = QString("Can't open database \"%1\"").arg(sDatabaseFilePath);
                std::cerr << sError.toStdString() << std::endl;
            }
        }
        else
        {
            QString sError = QString("Invalid database: %1. Available drivers=[%2]").arg(database.driverName()).arg(database.drivers().join(", "));
            std::cerr << sError.toStdString() << std::endl;
        }
    }
    return false;
}

float Database::getLastWpm(const QString& sTableName) const
{
    QSqlQuery query = getLastRecord(sTableName);
    return query.next() ? query.value("Wpm").toFloat() : 0.f;
}

float Database::getLastSpm(const QString& sTableName) const
{
    QSqlQuery query = getLastRecord(sTableName);
    return query.next() ? query.value("Spm").toFloat() : 0.f;
}

float Database::getLastAccuracy(const QString& sTableName) const
{
    QSqlQuery query = getLastRecord(sTableName);
    return query.next() ? query.value("Accuracy").toFloat() : 0.f;
}

float Database::getMaxWpm(const QString& sTableName) const
{
    QSqlQuery query = getMax("Wpm", sTableName);
    return query.next() ? query.value(0).toFloat() : 0.f;
}

float Database::getMaxSpm(const QString& sTableName) const
{
    QSqlQuery query = getMax("Spm", sTableName);
    return query.next() ? query.value(0).toFloat() : 0.f;
}

float Database::getMaxAccuracy(const QString& sTableName) const
{
    QSqlQuery query = getMax("Accuracy", sTableName);
    return query.next() ? query.value(0).toFloat() : 0.f;
}

QSqlQuery Database::getLastRecord(const QString& sTableName) const
{
    QSqlQuery query(QSqlDatabase::database());
    QString sQuery = "SELECT * FROM \"%1\" ORDER by ROWID DESC LIMIT 1";
    sQuery = sQuery.arg(sTableName);
    if (!query.exec(sQuery))
    {
        QString sError = QString("Can't execute query: %1").arg(query.lastError().text());
        std::cerr << sError.toStdString() << std::endl;
    }
    return query;
}

QSqlQuery Database::getMax(const QString& sColumnName, const QString& sTableName) const
{
    QSqlQuery query(QSqlDatabase::database());
    QString sQuery = "SELECT MAX(\"%1\") FROM \"%2\"";
    sQuery = sQuery.arg(sColumnName).arg(sTableName);
    if (!query.exec(sQuery))
    {
        QString sError = QString("Can't execute query: %1").arg(query.lastError().text());
        std::cerr << sError.toStdString() << std::endl;
    }
    return query;
}

QString Database::getDatabaseFilePath() const
{
    const QString& sAppLocalPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    if (sAppLocalPath.isEmpty() == false)
    {
        // On linux we have to create the path first
        if (!QFile::exists(sAppLocalPath))
        {
            QDir dir;
            if (!dir.mkpath(sAppLocalPath))
            {
                return QString();
            }
        }

        return QString("%1/%2.sqlite").arg(sAppLocalPath).arg("Statistics");
    }
    return QString();
}
