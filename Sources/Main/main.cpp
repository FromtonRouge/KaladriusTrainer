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
#include "MainWindow.h"
#include "MainTabDialog.h"
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QDesktopWidget>
#include <QtCore/QSettings>

int main(int argc, char *argv[])
{
    Application a(argc, argv);

    QSettings settings;
    MainTabDialog mainTabDialog;
    MainWindow mainWindow;
    mainTabDialog.getTabWidget()->addTab(&mainWindow, QObject::tr("Developer Mode"));
    mainTabDialog.showMaximized();
    mainTabDialog.restoreGeometry(settings.value("MainTabDialog/geometry").toByteArray());
    if (mainTabDialog.isMaximized())
    {
        mainTabDialog.setGeometry(QApplication::desktop()->availableGeometry(&mainTabDialog));
    }
    mainWindow.restoreState(settings.value("windowState").toByteArray());

    return a.exec();
}
