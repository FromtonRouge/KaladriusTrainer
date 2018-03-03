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

#include "Keyboards/KeyboardEditorMainWindow.h"
#include "Keyboards/UndoableKeyboardModel.h"
#include "Keyboards/KeyboardModel.h"
#include "Keyboards/KeyboardGraphicsScene.h"
#include "Main/Application.h"
#include "Serialization/Serialize.h"
#include "Streams/Iostream.h"
#include "ui_KeyboardEditorMainWindow.h"
#include <QtWidgets/QUndoStack>
#include <QtWidgets/QFileDialog>
#include <QtCore/QSettings>
#include <QtCore/QTemporaryFile>
#include <QtCore/QFile>
#include <QtCore/QDebug>

KeyboardEditorMainWindow::KeyboardEditorMainWindow(QWidget *pParent)
    : MainTabWindow(pParent)
    , _pUi(new Ui::KeyboardEditorMainWindow)
{
    _pUi->setupUi(this);
    setWindowTitle(tr("Keyboard Editor[*]"));

    // Init undo stack
    auto pUndoStack = qApp->getUndoableKeyboardModel()->getUndoStack();
    _pUi->listViewUndo->setStack(pUndoStack);
    connect(pUndoStack, SIGNAL(cleanChanged(bool)), this, SLOT(onUndoCleanChanged(bool)));
    _pUi->menuEdit->addAction(createUndoAction(pUndoStack));
    _pUi->menuEdit->addAction(createRedoAction(pUndoStack));

    auto pKeyboardModel = qApp->getKeyboardModel();
    auto pKeyboardGraphicsScene = qApp->getKeyboardGraphicsScene();
    connect(pKeyboardModel, SIGNAL(linkedKeycapsStates(QVector<KeycapsStates>)), pKeyboardGraphicsScene, SLOT(setKeycapsStates(QVector<KeycapsStates>)));
    connect(pKeyboardModel, SIGNAL(linkedKeycapsStates(KeycapsStates)), pKeyboardGraphicsScene, SLOT(setKeycapsStates(KeycapsStates)));

    _pUi->treeViewKeyboardProperties->setModel(qApp->getUndoableKeyboardModel());
    pKeyboardGraphicsScene->setKeyboardProperties(_pUi->treeViewKeyboardProperties);
    _pUi->widgetKeycapProperties->setKeyboardProperties(_pUi->treeViewKeyboardProperties);

    // Load last .kbd file if any
    QSettings settings;
    const QString& sLastKeyboard = settings.value("lastKeyboard").toString();
    if (!sLastKeyboard.isEmpty() && QFile::exists(sLastKeyboard))
    {
        loadKeyboard(sLastKeyboard);
    }
    else
    {
        _pUi->actionLoad_Default->trigger();
    }

    connect(_pUi->actionRecord_Keyboard, SIGNAL(triggered(bool)), _pUi->graphicsView, SLOT(recordKeyboardInputs(bool)));
    connect(qApp->getKeyboardModel(), SIGNAL(keyboardLoaded()), _pUi->graphicsView, SLOT(fitKeyboardInView()));
    _pUi->graphicsView->setScene(qApp->getKeyboardGraphicsScene());
}

KeyboardEditorMainWindow::~KeyboardEditorMainWindow()
{

}

void KeyboardEditorMainWindow::loadKeyboard(const QString& sKeyboardFileName, SettingsOperation settingsOperation)
{
    if (Serialization::Load(qApp->getKeyboardModel(), sKeyboardFileName))
    {
        QSettings().setValue("lastKeyboard", settingsOperation == ClearSettings ? QVariant() : sKeyboardFileName);
        COUT(tr("Keyboard loaded from file %1").arg(sKeyboardFileName));
        _pUi->listViewUndo->stack()->clear();
    }
}

void KeyboardEditorMainWindow::on_actionLoad_triggered()
{
    QSettings settings;
    const QString& sLastKeyboard = settings.value("lastKeyboard").toString();
    const QString& sKeyboardFileName = QFileDialog::getOpenFileName(this, tr("Keyboard"), sLastKeyboard, "*.kbd");
    if (!sKeyboardFileName.isEmpty())
    {
        loadKeyboard(sKeyboardFileName);
    }
}

void KeyboardEditorMainWindow::on_actionLoad_Default_triggered()
{
    QFile resourceFile(":/Keyboards/ErgoDox.kbd");
    auto pTemporaryFile = QTemporaryFile::createNativeFile(resourceFile);
    if (pTemporaryFile)
    {
        loadKeyboard(pTemporaryFile->fileName(), ClearSettings);
    }
}

void KeyboardEditorMainWindow::on_actionSave_As_triggered()
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
        _pUi->actionSave->trigger();
    }
}

void KeyboardEditorMainWindow::on_actionSave_triggered()
{
    QSettings settings;
    const QString& sLastKeyboard = settings.value("lastKeyboard").toString();
    if (!sLastKeyboard.isEmpty())
    {
        if (Serialization::Save(qApp->getKeyboardModel(), sLastKeyboard))
        {
            COUT(tr("Keyboard saved to file %1").arg(sLastKeyboard));
            _pUi->listViewUndo->stack()->setClean();
        }
    }
    else
    {
        _pUi->actionSave_As->trigger();
    }
}

void KeyboardEditorMainWindow::on_actionImport_Svg_triggered()
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

void KeyboardEditorMainWindow::on_actionImport_Default_Svg_triggered()
{
    qApp->getKeyboardModel()->loadKeyboardSvgFile(":/Svgs/ergodox.svg");
    QSettings settings;
    settings.setValue("lastKeyboardSvg", ":/Svgs/ergodox.svg");
    settings.setValue("lastKeyboard", QString());
}
