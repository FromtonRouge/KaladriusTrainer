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
#include "Serialization/Serialize.h"
#include "Main/Application.h"
#include "MainWindow.h"
#include "Streams/Iostream.h"
#include "Keyboards/KeyboardGraphicsView.h"
#include "Keyboards/KeyboardGraphicsScene.h"
#include "Keyboards/KeyboardModel.h"
#include "Keyboards/UndoableKeyboardModel.h"
#include "Theories/TheoryModel.h"
#include "Models/UndoableProxyModel.h"
#include "Dictionaries/DictionaryParser.h"
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
#include <QtCore/QTemporaryFile>
#include <QtCore/QMultiMap>
#include <QtCore/QTextStream>
#include <QtCore/QTimer>
#include <QtCore/QMap>
#include <functional>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , _pUi(new Ui::MainWindow)
    , _pKeyboardGraphicsScene(new KeyboardGraphicsScene(this))
    , _pUndoStack(new QUndoStack(this))
{
    _pUi->setupUi(this);
    setDockOptions(dockOptions() | DockOption::GroupedDragging | DockOption::AllowNestedDocks);

    connect(qApp, SIGNAL(logs(QString, int)), this, SLOT(logs(QString,int)));

    // Undo/redo
    _pUi->listViewUndo->setStack(_pUndoStack);
    qApp->getUndoableKeyboardModel()->setUndoStack(_pUndoStack);
    qApp->getUndoableTheoryModel()->setUndoStack(_pUndoStack);

    QAction* pUndoAction = _pUndoStack->createUndoAction(this);
    pUndoAction->setIcon(QIcon(":/Icons/arrow-curve-180-left.png"));
    pUndoAction->setShortcut(QKeySequence("Ctrl+Z"));
    _pUi->menuEdit->addAction(pUndoAction);

    QAction* pRedoAction = _pUndoStack->createRedoAction(this);
    pRedoAction->setIcon(QIcon(":/Icons/arrow-curve.png"));
    pRedoAction->setShortcut(QKeySequence("Ctrl+Y"));
    _pUi->menuEdit->addAction(pRedoAction);

    // Load last .theory file if any
    QSettings settings;
    const QString& sLastTheory = settings.value("lastTheory").toString();
    if (!sLastTheory.isEmpty() && QFile::exists(sLastTheory))
    {
        loadTheory(sLastTheory);
    }
    else
    {
        _pUi->actionLoad_Default_Theory->trigger();
    }

    auto pTheoryModel = qApp->getTheoryModel();
    auto pKeyboardModel = qApp->getKeyboardModel();
    connect(pKeyboardModel, SIGNAL(linkedKeycapsStates(QVector<QPair<QString, bool>>)), _pKeyboardGraphicsScene, SLOT(setKeycapsStates(QVector<QPair<QString, bool>>)));
    _pUi->widgetDictionaries1->setTheoryModel(pTheoryModel);
    _pUi->widgetDictionaries2->setTheoryModel(pTheoryModel);
    _pUi->widgetDictionaries3->setTheoryModel(pTheoryModel);
    _pUi->widgetDictionaries4->setTheoryModel(pTheoryModel);
    connect(_pUi->widgetDictionaries1, SIGNAL(dictionaryEntrySelected(QString, QBitArray)), pKeyboardModel, SLOT(selectLinkedKeys(QString, QBitArray)));
    connect(_pUi->widgetDictionaries2, SIGNAL(dictionaryEntrySelected(QString, QBitArray)), pKeyboardModel, SLOT(selectLinkedKeys(QString, QBitArray)));
    connect(_pUi->widgetDictionaries3, SIGNAL(dictionaryEntrySelected(QString, QBitArray)), pKeyboardModel, SLOT(selectLinkedKeys(QString, QBitArray)));
    connect(_pUi->widgetDictionaries4, SIGNAL(dictionaryEntrySelected(QString, QBitArray)), pKeyboardModel, SLOT(selectLinkedKeys(QString, QBitArray)));

    _pUi->treeViewKeyboardProperties->setModel(qApp->getUndoableKeyboardModel());
    _pKeyboardGraphicsScene->setKeyboardProperties(_pUi->treeViewKeyboardProperties);
    _pUi->widgetKeycapProperties->setKeyboardProperties(_pUi->treeViewKeyboardProperties);

    _pUi->treeViewTheory->setModel(qApp->getUndoableTheoryModel());

    // Default dock states
    _pUi->dockWidgetTheory->hide();
    _pUi->dockWidgetKeyboardProperties->hide();
    _pUi->dockWidgetKeycapProperties->hide();
    _pUi->dockWidgetDictionaries3->hide();
    _pUi->dockWidgetDictionaries4->hide();
    _pUi->dockWidgetLogs->hide();
    _pUi->dockWidgetUndo->hide();

    showMaximized();

    // Restore geometry
    restoreGeometry(settings.value("geometry").toByteArray());

    // Load last .kbd file if any
    const QString& sLastKeyboard = settings.value("lastKeyboard").toString();
    if (!sLastKeyboard.isEmpty() && QFile::exists(sLastKeyboard))
    {
        loadKeyboard(sLastKeyboard);
    }
    else
    {
        _pUi->actionLoad_Default_Keyboard->trigger();
    }

    _pUi->actionKeyboard_Window->trigger();
}

MainWindow::~MainWindow()
{
}

void MainWindow::logs(const QString& sText, int iWarningLevel)
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

void MainWindow::loadKeyboard(const QString& sKeyboardFileName, SettingsOperation settingsOperation)
{
    if (Serialization::Load(qApp->getKeyboardModel(), sKeyboardFileName))
    {
        QSettings().setValue("lastKeyboard", settingsOperation == ClearSettings ? QVariant() : sKeyboardFileName);
        COUT(tr("Keyboard loaded from file %1").arg(sKeyboardFileName));
    }
}

void MainWindow::loadTheory(const QString& sTheoryFileName, SettingsOperation settingsOperation)
{
    if (Serialization::Load(qApp->getTheoryModel(), sTheoryFileName))
    {
        QSettings().setValue("lastTheory", settingsOperation == ClearSettings ? QVariant() : sTheoryFileName);
        COUT(tr("Theory loaded from file %1").arg(sTheoryFileName));
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
    Application::quit();
}

void MainWindow::on_actionImport_Keyboard_Svg_triggered()
{
    QSettings settings;
    const QString& sLastKeyboardSvg = settings.value("lastKeyboardSvg").toString();
    const QString& sKeyboardSvg = QFileDialog::getOpenFileName(this, tr("Keyboard Svg"), sLastKeyboardSvg, "*.svg");
    if (!sKeyboardSvg.isEmpty())
    {
        qApp->getKeyboardModel()->loadKeyboardSvgFile(sKeyboardSvg);
        settings.setValue("lastKeyboardSvg", sKeyboardSvg);
        settings.setValue("lastKeyboard", QString());
    }
}

void MainWindow::on_actionImport_Default_Keyboard_Svg_triggered()
{
    qApp->getKeyboardModel()->loadKeyboardSvgFile(":/Svgs/ergodox.svg");
    QSettings settings;
    settings.setValue("lastKeyboardSvg", ":/Svgs/ergodox.svg");
    settings.setValue("lastKeyboard", QString());
}

void MainWindow::on_actionLoad_Theory_triggered()
{
    QSettings settings;
    const QString& sLastTheoryFile = settings.value("lastTheory").toString();
    const QString& sTheoryFile = QFileDialog::getOpenFileName(this, tr("Theory"), sLastTheoryFile, "*.theory");
    if (!sTheoryFile.isEmpty())
    {
        loadTheory(sTheoryFile);
    }
}

void MainWindow::on_actionLoad_Default_Theory_triggered()
{
    QFile resourceFile(":/Theories/Jackdaw.theory");
    auto pTemporaryFile = QTemporaryFile::createNativeFile(resourceFile);
    if (pTemporaryFile)
    {
        loadTheory(pTemporaryFile->fileName(), ClearSettings);
    }
}

void MainWindow::on_actionSave_Theory_as_triggered()
{
    QSettings settings;
    const QString& sLastTheoryFile = settings.value("lastTheory").toString();
    QFileDialog saveDlg(this, tr("Theory"), sLastTheoryFile, "*.theory");
    saveDlg.setDefaultSuffix("theory");
    saveDlg.setAcceptMode(QFileDialog::AcceptSave);
    if (saveDlg.exec())
    {
        const QString sTheoryFile = saveDlg.selectedFiles().front();
        if (Serialization::Save(qApp->getTheoryModel(), sTheoryFile))
        {
            settings.setValue("lastTheory", sTheoryFile);
            COUT(tr("Theory saved to file %1").arg(sTheoryFile));
        }
    }
}

void MainWindow::on_actionImport_Dictionaries_triggered()
{
    QSettings settings;
    const QString& sLastImportDirectory = settings.value("lastImportDirectory").toString();
    const QString& sDirectory = QFileDialog::getExistingDirectory(this, tr("Dictionaries"), sLastImportDirectory);
    if (!sDirectory.isEmpty())
    {
        settings.setValue("lastImportDirectory", sDirectory);

        _dictionaries.clear();
        QDir dir(sDirectory);
        QStringList filters = QStringList()  << "shelton_tables.c" << "user_tables.c";
        const auto& entries = dir.entryInfoList(filters);
        if (entries.isEmpty())
        {
            CERR(tr("Can't found files %1 in directory %2").arg(filters.join(", ")).arg(sDirectory));
            return;
        }

        for (const auto& entry : entries)
        {
            DictionaryParser parser(entry.absoluteFilePath());
            parser.parse();
            _dictionaries.unite(parser.getDictionaries());
        }

        qApp->getTheoryModel()->setDictionaries(_dictionaries);

        COUT((tr("%1 dictionaries loaded").arg(_dictionaries.size())));
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
                        const QString& sKeysLabels = dictionary.getKeysLabelsInReadingOrder(entry);
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
            COUT(tr("%1 markdown files written").arg(_dictionaries.size()));
        }
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox messageBox(this);
    messageBox.setIcon(QMessageBox::Information);
    messageBox.setWindowTitle(tr("About"));
    messageBox.setTextFormat(Qt::RichText);

    QString sText = tr("%1 %2").arg(Application::applicationName()).arg(Application::applicationVersion());
    sText += tr("<p>Author: %1</p>").arg(Application::organizationName());
    sText += tr("<p>Some icons by <a href='http://p.yusukekamiyamane.com/'>Yusuke Kamiyamane</a>. Licensed under a <a href='http://creativecommons.org/licenses/by/3.0/'>Creative Commons Attribution 3.0 License</a>.</p>");
    sText += tr("<p>Application icon by <a href='http://chromatix.deviantart.com/'>Chromatix</a>. Licensed under a <a href='http://creativecommons.org/licenses/by-nc-nd/4.0/'>Creative Commons Attribution-Noncommercial-NoDerivate 4.0 License</a>.</p>");
    sText += tr("<p>Record icon made by <a href='http://www.freepik.com/'>Freepik</a> from <a href='http://www.flaticon.com/'>www.flaticon.com</a>.</p>");
    messageBox.setText(sText);
    messageBox.exec();
}

void MainWindow::on_actionKeyboard_Window_triggered()
{
    auto pGraphicsView = new KeyboardGraphicsView();
    connect(qApp->getKeyboardModel(), SIGNAL(keyboardLoaded()), pGraphicsView, SLOT(fitKeyboardInView()));
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
        loadKeyboard(sKeyboardFileName);
    }
}

void MainWindow::on_actionLoad_Default_Keyboard_triggered()
{
    QFile resourceFile(":/Keyboards/ErgoDox.kbd");
    auto pTemporaryFile = QTemporaryFile::createNativeFile(resourceFile);
    if (pTemporaryFile)
    {
        loadKeyboard(pTemporaryFile->fileName(), ClearSettings);
    }
}

void MainWindow::on_actionSave_Keyboard_as_triggered()
{
    QSettings settings;
    QString sKeyboardFileName;
    const QString& sLastKeyboard = settings.value("lastKeyboard").toString();
    QFileDialog saveDlg(this, tr("Keyboard"), sLastKeyboard, "*.kbd");
    saveDlg.setDefaultSuffix("kbd");
    saveDlg.setAcceptMode(QFileDialog::AcceptSave);
    if (saveDlg.exec())
    {
        sKeyboardFileName = saveDlg.selectedFiles().front();
        settings.setValue("lastKeyboard", sKeyboardFileName);
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
        if (Serialization::Save(qApp->getKeyboardModel(), sLastKeyboard))
        {
            COUT(tr("Keyboard saved to file %1").arg(sLastKeyboard));
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
