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

#include "Application.h"
#include "../Values/Editors/UserEditorFactory.h"
#include "../Theories/Models/TheoryModel.h"
#include "../Theories/Models/UndoableTheoryModel.h"
#include "../Keyboards/Models/KeyboardModel.h"
#include "../Keyboards/Models/UndoableKeyboardModel.h"
#include "../Keyboards/KeyboardGraphicsScene.h"
#include <QtCore/QCommandLineParser>
#include <QtCore/QStandardPaths>
#include <QtSql/QSqlDatabase>
#include <QtWidgets/QUndoStack>
#include <iostream>

Application::Application(int& argc, char** argv)
    : QApplication(argc, argv)
    , _pOldStreambufCout(std::cout.rdbuf())
    , _pOldStreambufCerr(std::cerr.rdbuf())
    , _pTheoryModel(new TheoryModel(this))
    , _pUndoableTheoryModel(new UndoableTheoryModel(this))
    , _pKeyboardModel(new KeyboardModel(this))
    , _pUndoableKeyboardModel(new UndoableKeyboardModel(this))
    , _pKeyboardGraphicsScene(new KeyboardGraphicsScene(this))
{
    // Setup std::cout redirection
    _streamBufferCout.open(StreamSink(std::bind(&Application::toLogs, this, std::placeholders::_1, 0)));
    std::cout.rdbuf(&_streamBufferCout);

    // Setup std::cerr redirection
    _streamBufferCerr.open(StreamSink(std::bind(&Application::toLogs, this, std::placeholders::_1, 2)));
    std::cerr.rdbuf(&_streamBufferCerr);

    // Application settings
    setOrganizationName("FromtonRouge");
    setApplicationName("KaladriusTrainer");

#define PROJECT_VERSION_MAJOR 0
#define PROJECT_VERSION_MINOR 1
#define PROJECT_VERSION_PATCH 0
    setApplicationVersion(QString("v%1.%2.%3").arg(PROJECT_VERSION_MAJOR).arg(PROJECT_VERSION_MINOR).arg(PROJECT_VERSION_PATCH));

    // Command line options (--help, --version)
    QCommandLineParser parser;
    parser.setApplicationDescription(QObject::tr("Kaladrius keyboard learning tool"));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(*this);

    // Default factory for all item delegates
    QItemEditorFactory::setDefaultFactory(new UserEditorFactory());

    _pUndoableTheoryModel->setUndoStack(new QUndoStack(this));
    _pUndoableTheoryModel->setSourceModel(_pTheoryModel);

    _pUndoableKeyboardModel->setUndoStack(new QUndoStack(this));
    _pUndoableKeyboardModel->setSourceModel(_pKeyboardModel);
}

Application::~Application()
{
    std::cerr.rdbuf(_pOldStreambufCerr);
    std::cout.rdbuf(_pOldStreambufCout);
}

bool Application::openDatabase()
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

QString Application::getDatabaseFilePath() const
{
    const QString& sAppLocalPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    if (sAppLocalPath.isEmpty() == false)
    {
        return QString("%1/%2.sqlite").arg(sAppLocalPath).arg("Statistics");
    }
    return QString();
}

void Application::toLogs(const QString& sText, int iWarningLevel)
{
    emit logs(sText, iWarningLevel);
}
