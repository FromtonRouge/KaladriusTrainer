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

#include "ui_MainWindow.h"
#include "MainWindow.h"
#include "DictionariesModel.h"
#include "DictionaryParser.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QStatusBar>
#include <QtGui/QTextCursor>
#include <QtGui/QTextCharFormat>
#include <QtGui/QBrush>
#include <QtGui/QColor>
#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QtCore/QSet>
#include <QtCore/QFile>
#include <QtCore/QMultiMap>
#include <QtCore/QTextStream>
#include <functional>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , _pUi(new Ui::MainWindow)
    , _pOldStreambufCout(std::cout.rdbuf())
    , _pOldStreambufCerr(std::cerr.rdbuf())
    , _pDictionariesModel(new DictionariesModel(this))
{
    _pUi->setupUi(this);

    // Setup std::cout redirection
    _streamBufferCout.open(StreamSink(std::bind(&MainWindow::toLogs, this, std::placeholders::_1, 0)));
    std::cout.rdbuf(&_streamBufferCout);

    // Setup std::cerr redirection
    _streamBufferCerr.open(StreamSink(std::bind(&MainWindow::toLogs, this, std::placeholders::_1, 2)));
    std::cerr.rdbuf(&_streamBufferCerr);

    _pUi->actionReload_Dictionaries->trigger();

    QSettings settings;
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    _pUi->widgetDictionaries1->setDictonariesModel(_pDictionariesModel);
    _pUi->widgetDictionaries2->setDictonariesModel(_pDictionariesModel);
}

MainWindow::~MainWindow()
{
	std::cerr.rdbuf(_pOldStreambufCerr);
	std::cout.rdbuf(_pOldStreambufCout);
}

void MainWindow::toLogs(const QString& sText, int iWarningLevel)
{
    // Write logs to the text edit with the appropriate color
    QTextCursor cursor(_pUi->textEditLogs->textCursor());
    QTextCharFormat format(cursor.charFormat());
    format.setForeground(QBrush(iWarningLevel == 0 ? Qt::black : Qt::red));
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(sText, format);
    _pUi->textEditLogs->setTextCursor(cursor);
}

void MainWindow::closeEvent(QCloseEvent* pEvent)
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    QMainWindow::closeEvent(pEvent);
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
    statusBar()->clearMessage();
    QSettings settings;
    const QString& sLastImportDirectory = settings.value("lastImportDirectory").toString();
    if (!sLastImportDirectory.isEmpty())
    {
        _dictionaries.clear();
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
                _dictionaries.unite(parser.getDictionaries());
            }
        }

        _pDictionariesModel->setDictionaries(_dictionaries);

        statusBar()->showMessage(tr("%1 dictonaries loaded").arg(_dictionaries.size()));
    }
}

void MainWindow::on_actionWrite_Markdown_Files_To_triggered()
{
    QSettings settings;
    const QString& sLastMarkdownFilesDirectory = settings.value("lastMarkdownFilesDirectory").toString();
    const QString& sDirectory = QFileDialog::getExistingDirectory(this, tr("Markdown dictionaries directory"), sLastMarkdownFilesDirectory);
    if (!sDirectory.isEmpty())
    {
        settings.setValue("lastMarkdownFilesDirectory", sDirectory);
        _pUi->actionWrite_Markdown_Files->trigger();
    }
}

void MainWindow::on_actionWrite_Markdown_Files_triggered()
{
    statusBar()->clearMessage();
    QSettings settings;
    const QString& sDirectory = settings.value("lastMarkdownFilesDirectory").toString();
    if (!sDirectory.isEmpty())
    {
        if (!_dictionaries.isEmpty())
        {
            struct DumpInfo
            {
                typedef QMultiMap<QString, QString> OrderedEntries;
                Dictionary dictonary;
                OrderedEntries orderedEntries;
            };

            QMap<QString, DumpInfo> orderedDictionaries;
            auto it = _dictionaries.begin();
            while (it != _dictionaries.end())
            {
                QMultiMap<QString, QString> keycodesToKeysLabels;
                const Dictionary& dictionary = it++.value();
                const auto& entries = dictionary.getKeyBitsToEntry();
                for (int i = 0; i < entries.size(); ++i)
                {
                    const auto& entry = entries[i];
                    if (entry.hasKeycodes())
                    {
                        const QString& sKeysLabels = dictionary.getKeysLabels(entry);
                        const QString& sKeycodes = entry.keycodesAsUserString;
                        keycodesToKeysLabels.insert(sKeycodes, sKeysLabels);
                    }
                }

                orderedDictionaries.insert(dictionary.getName(), { dictionary, keycodesToKeysLabels });
            }

            QFile fileAllDictionaries(QString("%1/dict_all.md").arg(sDirectory));
            if (fileAllDictionaries.open(QFile::WriteOnly))
            {
                auto itOrdered = orderedDictionaries.begin();
                while (itOrdered != orderedDictionaries.end())
                {
                    const auto& dumpInfo = itOrdered++.value();
                    const Dictionary& dictionary = dumpInfo.dictonary;
                    const auto& orderedEntries = dumpInfo.orderedEntries;

                    QFile fileDictionary(QString("%1/%2").arg(sDirectory).arg(dictionary.getMarkdownFileName()));
                    if (fileDictionary.open(QFile::WriteOnly))
                    {
                        QString sContent;
                        QTextStream stream(&sContent);
                        stream << "### " << dictionary.getName() << "\n\n";
                        stream << "```\n";
                        auto itKeyCodes = orderedEntries.begin();
                        while (itKeyCodes != orderedEntries.end())
                        {
                            const QString& sKey = itKeyCodes.key();
                            const QString& sValue = itKeyCodes++.value();
                            stream << QString("\t%1 = %2\n").arg(sKey).arg(sValue);
                        }
                        stream << "```\n";
                        stream << "\n";

                        const QByteArray& utf8 = sContent.toUtf8();
                        fileDictionary.write(utf8);
                        fileAllDictionaries.write(utf8);
                        fileDictionary.close();
                    }
                }
                fileAllDictionaries.close();
            }
            statusBar()->showMessage(tr("%1 markdown files written").arg(_dictionaries.size()));
        }
    }
}
