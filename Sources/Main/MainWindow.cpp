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
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QUndoStack>
#include <QtWidgets/QTextEdit>
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
    setWindowFlags(Qt::Widget);

    // Undo/redo
    _pUi->listViewUndo->setStack(_pUndoStack);
    qApp->getUndoableKeyboardModel()->setUndoStack(_pUndoStack);

    QAction* pUndoAction = _pUndoStack->createUndoAction(this);
    pUndoAction->setIcon(QIcon(":/Icons/arrow-curve-180-left.png"));
    pUndoAction->setShortcut(QKeySequence("Ctrl+Z"));
    _pUi->menuEdit->addAction(pUndoAction);

    QAction* pRedoAction = _pUndoStack->createRedoAction(this);
    pRedoAction->setIcon(QIcon(":/Icons/arrow-curve.png"));
    pRedoAction->setShortcut(QKeySequence("Ctrl+Y"));
    _pUi->menuEdit->addAction(pRedoAction);

    auto pTheoryModel = qApp->getTheoryModel();
    auto pKeyboardModel = qApp->getKeyboardModel();
    connect(pKeyboardModel, SIGNAL(linkedKeycapsStates(QVector<KeycapsStates>)), _pKeyboardGraphicsScene, SLOT(setKeycapsStates(QVector<KeycapsStates>)));
    connect(pKeyboardModel, SIGNAL(linkedKeycapsStates(KeycapsStates)), _pKeyboardGraphicsScene, SLOT(setKeycapsStates(KeycapsStates)));
    _pUi->widgetDictionaries1->setTheoryModel(pTheoryModel);
    _pUi->widgetDictionaries2->setTheoryModel(pTheoryModel);
    _pUi->widgetDictionaries3->setTheoryModel(pTheoryModel);
    _pUi->widgetDictionaries4->setTheoryModel(pTheoryModel);
    connect(_pUi->widgetDictionaries1, SIGNAL(dictionaryEntrySelected(QString, QVector<QBitArray>)), pKeyboardModel, SLOT(selectLinkedKeys(QString, QVector<QBitArray>)));
    connect(_pUi->widgetDictionaries2, SIGNAL(dictionaryEntrySelected(QString, QVector<QBitArray>)), pKeyboardModel, SLOT(selectLinkedKeys(QString, QVector<QBitArray>)));
    connect(_pUi->widgetDictionaries3, SIGNAL(dictionaryEntrySelected(QString, QVector<QBitArray>)), pKeyboardModel, SLOT(selectLinkedKeys(QString, QVector<QBitArray>)));
    connect(_pUi->widgetDictionaries4, SIGNAL(dictionaryEntrySelected(QString, QVector<QBitArray>)), pKeyboardModel, SLOT(selectLinkedKeys(QString, QVector<QBitArray>)));
    connect(_pUi->widgetDictionaries1, SIGNAL(notifySpecialKeys(HashSpecialKeysStates)), pKeyboardModel, SLOT(selectLinkedSpecialKeys(HashSpecialKeysStates)));
    connect(_pUi->widgetDictionaries2, SIGNAL(notifySpecialKeys(HashSpecialKeysStates)), pKeyboardModel, SLOT(selectLinkedSpecialKeys(HashSpecialKeysStates)));
    connect(_pUi->widgetDictionaries3, SIGNAL(notifySpecialKeys(HashSpecialKeysStates)), pKeyboardModel, SLOT(selectLinkedSpecialKeys(HashSpecialKeysStates)));
    connect(_pUi->widgetDictionaries4, SIGNAL(notifySpecialKeys(HashSpecialKeysStates)), pKeyboardModel, SLOT(selectLinkedSpecialKeys(HashSpecialKeysStates)));

    auto pStrokesSolverTextEdit = _pUi->widgetStrokesSolver->findChild<QTextEdit*>("textEdit");
    if (pStrokesSolverTextEdit)
    {
        connect(pStrokesSolverTextEdit, SIGNAL(dictionaryMatch(QString, QVector<QBitArray>)), pKeyboardModel, SLOT(selectLinkedKeys(QString, QVector<QBitArray>)));
        connect(pStrokesSolverTextEdit, SIGNAL(notifySpecialKeys(HashSpecialKeysStates)), pKeyboardModel, SLOT(selectLinkedSpecialKeys(HashSpecialKeysStates)));
        connect(pStrokesSolverTextEdit, SIGNAL(solverStarted()), _pKeyboardGraphicsScene, SLOT(clearSelection()));
    }

    _pUi->treeViewKeyboardProperties->setModel(qApp->getUndoableKeyboardModel());
    _pKeyboardGraphicsScene->setKeyboardProperties(_pUi->treeViewKeyboardProperties);
    _pUi->widgetKeycapProperties->setKeyboardProperties(_pUi->treeViewKeyboardProperties);

    // Default dock states
    _pUi->dockWidgetKeyboardProperties->hide();
    _pUi->dockWidgetKeycapProperties->hide();
    _pUi->dockWidgetDictionaries3->hide();
    _pUi->dockWidgetDictionaries4->hide();
    _pUi->dockWidgetUndo->hide();

    // Load last .kbd file if any
    QSettings settings;
    const QString& sLastKeyboard = settings.value("lastKeyboard").toString();
    if (!sLastKeyboard.isEmpty() && QFile::exists(sLastKeyboard))
    {
        loadKeyboard(sLastKeyboard);
    }
    else
    {
        _pUi->actionLoad_Default_Keyboard->trigger();
    }

    connect(qApp->getKeyboardModel(), SIGNAL(keyboardLoaded()), _pUi->graphicsView, SLOT(fitKeyboardInView()));
    connect(_pUi->actionRecord_Keyboard, SIGNAL(triggered(bool)), _pUi->graphicsView, SLOT(recordKeyboardInputs(bool)));
    _pUi->graphicsView->setScene(_pKeyboardGraphicsScene);
}

MainWindow::~MainWindow()
{
}

void MainWindow::loadKeyboard(const QString& sKeyboardFileName, SettingsOperation settingsOperation)
{
    if (Serialization::Load(qApp->getKeyboardModel(), sKeyboardFileName))
    {
        QSettings().setValue("lastKeyboard", settingsOperation == ClearSettings ? QVariant() : sKeyboardFileName);
        COUT(tr("Keyboard loaded from file %1").arg(sKeyboardFileName));
    }
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
    // Set the focus on the StrokesSolverTextEdit by default
    if (_pUi->dockWidgetStrokesSolver->isVisible())
    {
        _pUi->dockWidgetStrokesSolver->raise();
        auto pStrokesSolverTextEdit = _pUi->widgetStrokesSolver->findChild<QTextEdit*>("textEdit");
        if (pStrokesSolverTextEdit)
        {
            pStrokesSolverTextEdit->setFocus();
        }
    }
}
