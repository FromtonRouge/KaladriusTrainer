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

#include "Application.h"
#include "ProjectConfig.h"
#include "UserEditorFactory.h"
#include <QtCore/QCommandLineParser>

Application::Application(int& argc, char** argv)
    : QApplication(argc, argv)
{
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
}

Application::~Application()
{

}
