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
#include "Serialization.h"
#include "MainWindow.h"
#include "KeyboardGraphicsView.h"
#include "KeyboardGraphicsScene.h"
#include "KeyboardPropertiesModel.h"
#include "UndoableProxyModel.h"
#include "DictionariesModel.h"
#include "DictionaryParser.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMdiSubWindow>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QUndoStack>
#include <QtGui/QTextCursor>
#include <QtGui/QTextCharFormat>
#include <QtGui/QBrush>
#include <QtGui/QColor>
#include <QtGui/QKeySequence>
#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QEvent>
#include <QtCore/QStringList>
#include <QtCore/QSet>
#include <QtCore/QFile>
#include <QtCore/QMultiMap>
#include <QtCore/QTextStream>
#include <QtCore/QTimer>
#include <functional>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , _pUi(new Ui::MainWindow)
    , _pOldStreambufCout(std::cout.rdbuf())
    , _pOldStreambufCerr(std::cerr.rdbuf())
    , _pDictionariesModel(new DictionariesModel(this))
    , _pKeyboardModel(new KeyboardPropertiesModel(this))
    , _pUndoableKeyboardModel(new UndoableProxyModel(this))
    , _pKeyboardGraphicsScene(new KeyboardGraphicsScene(this))
    , _pUndoStack(new QUndoStack(this))
{
    _pUi->setupUi(this);

    // Setup std::cout redirection
    _streamBufferCout.open(StreamSink(std::bind(&MainWindow::toLogs, this, std::placeholders::_1, 0)));
    std::cout.rdbuf(&_streamBufferCout);

    // Setup std::cerr redirection
    _streamBufferCerr.open(StreamSink(std::bind(&MainWindow::toLogs, this, std::placeholders::_1, 2)));
    std::cerr.rdbuf(&_streamBufferCerr);

    // Undo/redo
    _pUi->listViewUndo->setStack(_pUndoStack);
    _pUndoableKeyboardModel->setSourceModel(_pKeyboardModel);
    _pUndoableKeyboardModel->setUndoStack(_pUndoStack);

    QAction* pUndoAction = _pUndoStack->createUndoAction(this);
    pUndoAction->setIcon(QIcon(":/Icons/arrow-curve-180-left.png"));
    pUndoAction->setShortcut(QKeySequence("Ctrl+Z"));
    _pUi->menuEdit->addAction(pUndoAction);

    QAction* pRedoAction = _pUndoStack->createRedoAction(this);
    pRedoAction->setIcon(QIcon(":/Icons/arrow-curve.png"));
    pRedoAction->setShortcut(QKeySequence("Ctrl+Y"));
    _pUi->menuEdit->addAction(pRedoAction);

    _pUi->actionReload_Dictionaries->trigger();

    _pUi->widgetDictionaries1->setDictionariesModel(_pDictionariesModel);
    _pUi->widgetDictionaries2->setDictionariesModel(_pDictionariesModel);
    _pUi->widgetDictionaries3->setDictionariesModel(_pDictionariesModel);
    _pUi->widgetDictionaries4->setDictionariesModel(_pDictionariesModel);

    _pUi->treeViewKeyboardProperties->setModel(_pUndoableKeyboardModel);
    _pKeyboardGraphicsScene->setKeyboardProperties(_pUi->treeViewKeyboardProperties);
    _pUi->widgetKeycapProperties->setKeyboardProperties(_pUi->treeViewKeyboardProperties);

    // Default dock states
    _pUi->dockWidgetKeyboardProperties->hide();
    _pUi->dockWidgetKeycapProperties->hide();
    _pUi->dockWidgetDictionaries3->hide();
    _pUi->dockWidgetDictionaries4->hide();
    _pUi->dockWidgetLogs->hide();
    _pUi->dockWidgetUndo->hide();

    // Restore geometry
    QSettings settings;
    restoreGeometry(settings.value("geometry").toByteArray());

    // Load keyboard svg
    const QString& sLastKeyboardSvg = settings.value("lastKeyboardSvg", ":/Svgs/ergodox.svg").toString();
    _pKeyboardModel->loadKeyboardSvgFile(sLastKeyboardSvg);
    _pUi->actionKeyboard_Window->trigger();
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

    if (iWarningLevel == 2)
    {
        _pUi->dockWidgetLogs->setVisible(true);
        _pUi->dockWidgetLogs->raise();
    }
}

void MainWindow::closeEvent(QCloseEvent* pEvent)
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    QMainWindow::closeEvent(pEvent);
}

bool MainWindow::event(QEvent *pEvent)
{
    switch (pEvent->type())
    {
    case QEvent::Polish:
        {
            QTimer::singleShot(80, this, SLOT(delayedRestoreState()));
            break;
        }
    default:
        {
            break;
        }
    }
    return QMainWindow::event(pEvent);
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionImport_Keyboard_Svg_triggered()
{
    QSettings settings;
    const QString& sLastKeyboardSvg = settings.value("lastKeyboardSvg").toString();
    const QString& sKeyboardSvg = QFileDialog::getOpenFileName(this, tr("Keyboard Svg"), sLastKeyboardSvg, "*.svg");
    if (!sKeyboardSvg.isEmpty())
    {
        _pKeyboardModel->loadKeyboardSvgFile(sKeyboardSvg);
        settings.setValue("lastKeyboardSvg", sKeyboardSvg);
    }
}

void MainWindow::on_actionImport_Default_Keyboard_Svg_triggered()
{
    _pKeyboardModel->loadKeyboardSvgFile(":/Svgs/ergodox.svg");
    QSettings settings;
    settings.setValue("lastKeyboardSvg", ":/Svgs/ergodox.svg");
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

        statusBar()->showMessage(tr("%1 dictionaries loaded").arg(_dictionaries.size()));
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
                Dictionary dictionary;
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
                    const Dictionary& dictionary = dumpInfo.dictionary;
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

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox messageBox(this);
    messageBox.setIcon(QMessageBox::Information);
    messageBox.setWindowTitle(tr("About"));
    messageBox.setTextFormat(Qt::RichText);

    QString sText = tr("%1 %2").arg(QApplication::applicationName()).arg(QApplication::applicationVersion());
    sText += tr("<p>Author: %1</p>").arg(QApplication::organizationName());
    sText += tr("<p>Some icons by <a href='http://p.yusukekamiyamane.com/'>Yusuke Kamiyamane</a>. Licensed under a <a href='http://creativecommons.org/licenses/by/3.0/'>Creative Commons Attribution 3.0 License</a>.</p>");
    sText += tr("<p>Application icon by <a href='http://chromatix.deviantart.com/'>Chromatix</a>. Licensed under a <a href='http://creativecommons.org/licenses/by-nc-nd/4.0/'>Creative Commons Attribution-Noncommercial-NoDerivate 4.0 License</a>.</p>");
    sText += tr("<p>Record icon made by <a href='http://www.freepik.com/'>Freepik</a> from <a href='http://www.flaticon.com/'>www.flaticon.com</a>.</p>");
    messageBox.setText(sText);
    messageBox.exec();
}

void MainWindow::on_actionKeyboard_Window_triggered()
{
    auto pGraphicsView = new KeyboardGraphicsView();
    connect(_pKeyboardModel, SIGNAL(keyboardLoaded()), pGraphicsView, SLOT(fitKeyboardInView()));
    connect(_pUi->actionRecord_Keyboard, SIGNAL(triggered(bool)), pGraphicsView, SLOT(recordKeyboardInputs(bool)));
    auto pSubWindow = _pUi->mdiArea->addSubWindow(pGraphicsView);
    pSubWindow->setWindowTitle(tr("Keyboard"));
    pGraphicsView->setParent(pSubWindow);
    pGraphicsView->setScene(_pKeyboardGraphicsScene);
    pSubWindow->showMaximized();
}

void MainWindow::on_actionLoad_Keyboard_triggered()
{
    QSettings settings;
    const QString& sLastKeyboard = settings.value("lastKeyboard").toString();
    const QString& sKeyboardFileName = QFileDialog::getOpenFileName(this, tr("Keyboard"), sLastKeyboard, "*.kbd");
    if (!sKeyboardFileName.isEmpty())
    {
        settings.setValue("lastKeyboard", sKeyboardFileName);

        if (Serialization::Load(_pKeyboardModel, sKeyboardFileName))
        {
            const QString sMsg = tr("Keyboard loaded from file %1").arg(sKeyboardFileName);
            std::cout << sMsg.toStdString() << std::endl;
        }
    }
}

void MainWindow::on_actionSave_Keyboard_as_triggered()
{
    QSettings settings;
    QString sKeyboardFileName;
    {
        const QString& sLastKeyboard = settings.value("lastKeyboard").toString();
        QFileDialog saveDlg(this, tr("Keyboard"), sLastKeyboard, "*.kbd");
        saveDlg.setDefaultSuffix("kbd");
        saveDlg.setAcceptMode(QFileDialog::AcceptSave);
        if (saveDlg.exec())
        {
            sKeyboardFileName = saveDlg.selectedFiles().front();
            settings.setValue("lastKeyboard", sKeyboardFileName);
        }
    }

    if (!sKeyboardFileName.isEmpty())
    {
        _pUi->actionSave_Keyboard->trigger();
    }
}

void MainWindow::on_actionSave_Keyboard_triggered()
{
    QSettings settings;
    const QString& sLastKeyboard = settings.value("lastKeyboard").toString();
    if (!sLastKeyboard.isEmpty())
    {
        if (Serialization::Save(_pKeyboardModel, sLastKeyboard))
        {
            const QString sMsg = tr("Keyboard saved to file %1").arg(sLastKeyboard);
            std::cout << sMsg.toStdString() << std::endl;
        }
    }
    else
    {
        _pUi->actionSave_Keyboard_as->trigger();
    }
}

void MainWindow::delayedRestoreState()
{
    // Restore dock states
    QSettings settings;
    restoreState(settings.value("windowState").toByteArray());
}
