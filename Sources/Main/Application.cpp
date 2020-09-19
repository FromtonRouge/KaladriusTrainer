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

#include "Application.h"
#include "../Values/Editors/UserEditorFactory.h"
#include "../Theories/Models/TheoryModel.h"
#include "../Theories/Models/UndoableTheoryModel.h"
#include "../Keyboards/Models/KeyboardModel.h"
#include "../Keyboards/Models/UndoableKeyboardModel.h"
#include "../Keyboards/KeyboardGraphicsScene.h"
#include "../Levels/Models/LevelsModel.h"
#include "../Database/Database.h"
#include <QtWidgets/QUndoStack>
#include <QtWidgets/QMessageBox>
#include <QtCore/QCommandLineParser>
#include <iostream>

Application::Application(int& argc, char** argv)
    : QApplication(argc, argv)
    , _pOldStreambufCout(std::cout.rdbuf())
    , _pOldStreambufCerr(std::cerr.rdbuf())
    , _pDatabase(new Database(this))
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
#define PROJECT_VERSION_MINOR 2
#define PROJECT_VERSION_PATCH 2
    setApplicationVersion(QString("v%1.%2.%3").arg(PROJECT_VERSION_MAJOR).arg(PROJECT_VERSION_MINOR).arg(PROJECT_VERSION_PATCH));

    // Command line options (--help, --version)
    QCommandLineParser parser;
    parser.setApplicationDescription(QObject::tr("Kaladrius keyboard learning tool"));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(*this);

    // Default factory for all item delegates
    QItemEditorFactory::setDefaultFactory(new UserEditorFactory());

    _pLevelsModel = new LevelsModel(this);

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

void Application::showAboutDialog()
{
    QMessageBox messageBox(activeWindow());
    messageBox.setIcon(QMessageBox::Information);
    messageBox.setWindowTitle(tr("About"));
    messageBox.setTextFormat(Qt::RichText);

    QString sText = tr("%1 %2").arg(Application::applicationName()).arg(Application::applicationVersion());
    sText += tr("<p>Author: %1</p>").arg(Application::organizationName());
    sText += tr("<p>Some icons by <a href='http://p.yusukekamiyamane.com/'>Yusuke Kamiyamane</a>. Licensed under a <a href='http://creativecommons.org/licenses/by/3.0/'>Creative Commons Attribution 3.0 License</a>.</p>");
    sText += tr("<p>Application icon by <a href='http://chromatix.deviantart.com/'>Chromatix</a>. Licensed under a <a href='http://creativecommons.org/licenses/by-nc-nd/4.0/'>Creative Commons Attribution-Noncommercial-NoDerivate 4.0 License</a>.</p>");
    sText += tr("<p>Record icon made by <a href='http://www.freepik.com/'>Freepik</a> from <a href='http://www.flaticon.com/'>www.flaticon.com</a>.</p>");
    sText += tr("<p>Words list from <a href='http://github.com/first20hours/google-10000-english'>google-10000-english</a>.</p>");
    sText += tr("<p>All texts from <a href='http://www.gutenberg.org/'>Project Gutenberg</a>.</p>");
    messageBox.setText(sText);
    messageBox.exec();
}

void Application::toLogs(const QString& sText, int iWarningLevel)
{
    emit logs(sText, iWarningLevel);
}
