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

#include "MainWindow.h"
#include "DictionaryParser.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>
#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QtCore/QSet>
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , _pUi(new Ui::MainWindow)
{
    _pUi->setupUi(this);
    _pUi->actionReload_Dictionaries->trigger();
}

MainWindow::~MainWindow()
{
    delete _pUi;
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionImport_Dictionaries_triggered()
{
    QSettings settings;
    const QString& sLastImportDirectory = settings.value("lastImportDirectory").toString();
    const QString& sDirectory = QFileDialog::getExistingDirectory(this, tr("Dictionaries"), sLastImportDirectory);
    if (!sDirectory.isEmpty())
    {
        settings.setValue("lastImportDirectory", sDirectory);
        _pUi->actionReload_Dictionaries->trigger();
    }
}

void MainWindow::on_actionReload_Dictionaries_triggered()
{
    QSettings settings;
    const QString& sLastImportDirectory = settings.value("lastImportDirectory").toString();
    if (!sLastImportDirectory.isEmpty())
    {
        QDir dir(sLastImportDirectory);
        QSet<QString> dictionariesFiles;
        dictionariesFiles.insert("shelton_tables.c");
        dictionariesFiles.insert("user_tables.c");
        const auto& entries = dir.entryInfoList(QStringList() << "*.c");
        for (const auto& entry : entries)
        {
            if (dictionariesFiles.contains(entry.fileName()))
            {
                DictionaryParser parser(entry.absoluteFilePath());
                parser.parse();
            }
        }
    }
}
