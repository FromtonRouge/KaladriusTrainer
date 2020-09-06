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

#include "Database.h"
#include "../Levels/TreeItems/LevelTreeItem.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlField>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>
#include <QtCore/QStandardPaths>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QVariant>
#include <QtCore/QDebug>
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
                // Activate foreign_keys
                execute("PRAGMA foreign_keys = ON;");
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

QSqlQuery Database::execute(const QString& sQuery) const
{
    QSqlQuery query(QSqlDatabase::database());
    if (!query.exec(sQuery))
    {
        QString sError = QString("SQL query error: %1").arg(query.lastError().text());
        std::cerr << sError.toStdString() << std::endl;
    }
    return query;
}

bool Database::createTextsTable() const
{
    QVector<QPair<QString, QString>> columns;
    columns << QPair<QString, QString>("Id", "INTEGER PRIMARY KEY");
    columns << QPair<QString, QString>("Enabled", "INTEGER");
    columns << QPair<QString, QString>("Text", "TEXT");
    columns << QPair<QString, QString>("Characters", "INTEGER");
    columns << QPair<QString, QString>("HasQuotes", "INTEGER");
    columns << QPair<QString, QString>("TextFileId", "INTEGER NOT NULL");

    const QString sConstraints = "FOREIGN KEY (TextFileId) REFERENCES 'Text Files' (Id)";
    return createTable("Texts", columns, sConstraints);
}

bool Database::createTextFilesTable() const
{
    QVector<QPair<QString, QString>> columns;
    columns << QPair<QString, QString>("Id", "INTEGER PRIMARY KEY");
    columns << QPair<QString, QString>("Filename", "TEXT UNIQUE");
    return createTable("Text Files", columns);
}

bool Database::createLevelTable(const QString& sTableName, int iLevelType) const
{
    QVector<QPair<QString, QString>> columns;
    columns << QPair<QString, QString>("Date", "TEXT");
    columns << QPair<QString, QString>("Wpm", "REAL");
    columns << QPair<QString, QString>("Spm", "REAL");
    columns << QPair<QString, QString>("Accuracy", "REAL");
    columns << QPair<QString, QString>("Viscosity", "REAL");

    QString sConstraints;
    if (iLevelType == LevelTreeItem::Text)
    {
        columns << QPair<QString, QString>("TextId", "INTEGER NOT NULL");
        sConstraints = "FOREIGN KEY (TextId) REFERENCES 'Texts' (Id)";
    }
    else
    {
        columns << QPair<QString, QString>("Progress", "REAL");
    }

    return createTable(sTableName, columns, sConstraints);
}

bool Database::createLevelWordsTable(const QString& sTableName) const
{
    QVector<QPair<QString, QString>> columns;
    columns << QPair<QString, QString>("Word", "TEXT PRIMARY KEY");
    columns << QPair<QString, QString>("Progression", "INTEGER"); // from -1 to 100 (-1 means NEW word to learn)
    columns << QPair<QString, QString>("Occurences", "INTEGER"); // Number of times you see the word
    columns << QPair<QString, QString>("AverageErrorsCount", "REAL"); // Average of errors you did on this word
    columns << QPair<QString, QString>("AverageChordsCount", "REAL"); // Average of chords needed to complete the whole word
    columns << QPair<QString, QString>("AverageTimeSpentToComplete", "REAL"); // Average time spent to do all chords for the whole word
    columns << QPair<QString, QString>("AverageTimeSpentForFirstStroke", "REAL"); // Average time in ms needed to do the first chord of the word
    return createTable(sTableName, columns);
}

bool Database::hasColumn(const QString& sTableName, const QString& sColumnName) const
{
    bool bHasColumn = false;
    QString sQuery = "SELECT COUNT(*) FROM pragma_table_info('%1') WHERE name == '%2'";
    sQuery = sQuery.arg(sTableName).arg(sColumnName);
    QSqlQuery query = execute(sQuery);
    if (query.next())
    {
        bHasColumn = query.value(0).toInt() > 0;
    }
    return bHasColumn;
}

bool Database::insertValues(const QString& sTableName, const QStringList& columns, const QVector<QVariantList>& rows) const
{
    QStringList fixedColumns;
    QStringList fixedRows;

    // Fix column string format
    for (const QString& sColumnName : columns)
    {
        fixedColumns << QString("\"%1\"").arg(sColumnName);
    }

    const QSqlDatabase& db = QSqlDatabase::database();

    // Fix string format in rows
    for (const QVariantList& row : rows)
    {
        QSqlField field("", QVariant::String);
        QStringList fixedRow;
        for (const QVariant& value : row)
        {
            if (value.type() == QVariant::String)
            {
                field.setValue(value);
                fixedRow << db.driver()->formatValue(field); // escape string for sql etc...
            }
            else
            {
                fixedRow << value.toString();
            }
        }

        fixedRows << QString("(%1)").arg(fixedRow.join(", "));
    }

    QString sQuery = "INSERT INTO \"%1\" (%2) VALUES %3;";
    sQuery = sQuery.arg(sTableName).arg(fixedColumns.join(",")).arg(fixedRows.join(","));
    QSqlQuery query(QSqlDatabase::database());
    if (!query.exec(sQuery))
    {
        QString sError = QString("Can't insert result: %1").arg(query.lastError().text());
        std::cerr << sError.toStdString() << std::endl;
        return false;
    }
    return true;
}

int Database::getCount(const QString& sTableName) const
{
    const auto& db = QSqlDatabase::database();
    if (!db.tables().contains(sTableName))
    {
        return 0;
    }

    QSqlQuery query(db);
    const QString sQuery = QString("SELECT COUNT(*) FROM \"%1\"").arg(sTableName);
    if (!query.exec(sQuery))
    {
        QString sError = QString("Can't execute query: %1").arg(query.lastError().text());
        std::cerr << sError.toStdString() << std::endl;
    }
    return query.next() ? query.value(0).toInt() : -1;
}

float Database::getSumOfCount(const QString& sTableName, const QString& sColumnName, uint16_t uiCount) const
{
    QSqlQuery query(QSqlDatabase::database());
    QString sQuery = "SELECT \"%1\" FROM \"%2\" ORDER BY ROWID DESC LIMIT %3";
    sQuery = sQuery.arg(sColumnName).arg(sTableName).arg(uiCount);
    if (!query.exec(sQuery))
    {
        QString sError = QString("Can't execute query: %1").arg(query.lastError().text());
        std::cerr << sError.toStdString() << std::endl;
        return 0;
    }

    float fResult = 0;
    while (query.next())
    {
        fResult += query.value(0).toFloat();
    }
    return fResult;
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

bool Database::createTable(const QString& sTableName, const QVector<QPair<QString, QString> >& columns, const QString& sTableConstraints) const
{
    Q_ASSERT(!sTableName.isEmpty());
    const QSqlDatabase& db = QSqlDatabase::database();
    if (!db.tables().contains(sTableName))
    {
        QStringList buffer;
        for (const auto& pair : columns)
        {
            buffer << QString("\"%1\" %2").arg(pair.first).arg(pair.second);
        }

        QString sCreateDatabase = "CREATE TABLE IF NOT EXISTS \"%1\" (%2%3);";
        sCreateDatabase = sCreateDatabase.arg(sTableName).arg(buffer.join(",")).arg(sTableConstraints.isEmpty() ? "" : QString(", %1").arg(sTableConstraints));

        QSqlQuery query(db);
        if (!query.exec(sCreateDatabase))
        {
            QString sError = QString("Can't create table: %1").arg(query.lastError().text());
            std::cerr << sError.toStdString() << std::endl;
            return false;
        }
    }
    return true;
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
