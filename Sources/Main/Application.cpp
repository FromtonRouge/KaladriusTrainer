// ======================================================================
// This file is a part of the ProgrammerStenoTutor project
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

#include "Main/Application.h"
#include "ProjectConfig.h"
#include "ValueEditors/UserEditorFactory.h"
#include "Theories/TheoryModel.h"
#include "Theories/UndoableTheoryModel.h"
#include "Keyboards/KeyboardModel.h"
#include "Keyboards/UndoableKeyboardModel.h"
#include "Keyboards/KeyboardGraphicsScene.h"
#include "Lessons/LessonModel.h"
#include "Lessons/UndoableLessonModel.h"
#include <QtCore/QCommandLineParser>
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
    , _pLessonModel(new LessonModel(this))
    , _pUndoableLessonModel(new UndoableLessonModel(this))
{
    // Setup std::cout redirection
    _streamBufferCout.open(StreamSink(std::bind(&Application::toLogs, this, std::placeholders::_1, 0)));
    std::cout.rdbuf(&_streamBufferCout);

    // Setup std::cerr redirection
    _streamBufferCerr.open(StreamSink(std::bind(&Application::toLogs, this, std::placeholders::_1, 2)));
    std::cerr.rdbuf(&_streamBufferCerr);

    // Application settings
    setOrganizationName("FromtonRouge");
    setApplicationName(PROJECT_APPLICATION_NAME);
    setApplicationVersion(QString("v%1.%2.%3").arg(PROJECT_VERSION_MAJOR).arg(PROJECT_VERSION_MINOR).arg(PROJECT_VERSION_PATCH));

    // Command line options (--help, --version)
    QCommandLineParser parser;
    parser.setApplicationDescription(QObject::tr("Programmer Steno learning tool"));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(*this);

    // Default factory for all item delegates
    QItemEditorFactory::setDefaultFactory(new UserEditorFactory());

    _pUndoableTheoryModel->setUndoStack(new QUndoStack(this));
    _pUndoableTheoryModel->setSourceModel(_pTheoryModel);

    _pUndoableKeyboardModel->setUndoStack(new QUndoStack(this));
    _pUndoableKeyboardModel->setSourceModel(_pKeyboardModel);

    _pUndoableLessonModel->setUndoStack(new QUndoStack(this));
    _pUndoableLessonModel->setSourceModel(_pLessonModel);
}

Application::~Application()
{
    std::cerr.rdbuf(_pOldStreambufCerr);
    std::cout.rdbuf(_pOldStreambufCout);
}

void Application::toLogs(const QString& sText, int iWarningLevel)
{
    emit logs(sText, iWarningLevel);
}
