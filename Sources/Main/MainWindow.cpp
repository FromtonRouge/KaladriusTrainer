// ======================================================================
// This file is a part of the KaladriusTrainer project
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
#include "Application.h"
#include "../Serialization/Serialize.h"
#include "../Streams/Iostream.h"
#include "../Keyboards/KeyboardGraphicsView.h"
#include "../Keyboards/KeyboardGraphicsScene.h"
#include "../Keyboards/Models/KeyboardModel.h"
#include "../Keyboards/Models/UndoableKeyboardModel.h"
#include "../Theories/Models/TheoryModel.h"
#include "ui_MainWindow.h"
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QTextEdit>
#include <QtCore/QEvent>
#include <QtCore/QTimer>

MainWindow::MainWindow(QWidget* pParent)
    : QMainWindow(pParent)
    , _pUi(new Ui::MainWindow)
{
    _pUi->setupUi(this);
    setDockOptions(dockOptions() | DockOption::GroupedDragging | DockOption::AllowNestedDocks);
    setWindowFlags(Qt::Widget);

    auto pTheoryModel = qApp->getTheoryModel();
    auto pKeyboardModel = qApp->getKeyboardModel();
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
    auto pKeyboardGraphicsScene = qApp->getKeyboardGraphicsScene();
    if (pStrokesSolverTextEdit)
    {
        connect(pStrokesSolverTextEdit, SIGNAL(dictionaryMatch(QString, QVector<QBitArray>)), pKeyboardModel, SLOT(selectLinkedKeys(QString, QVector<QBitArray>)));
        connect(pStrokesSolverTextEdit, SIGNAL(notifySpecialKeys(HashSpecialKeysStates)), pKeyboardModel, SLOT(selectLinkedSpecialKeys(HashSpecialKeysStates)));
        connect(pStrokesSolverTextEdit, SIGNAL(solverStarted()), pKeyboardGraphicsScene, SLOT(clearSelection()));
    }

    // Default dock states
    _pUi->dockWidgetDictionaries3->hide();
    _pUi->dockWidgetDictionaries4->hide();

    connect(pKeyboardModel, SIGNAL(keyboardLoaded()), _pUi->graphicsView, SLOT(fitKeyboardInView()));
    _pUi->graphicsView->setScene(pKeyboardGraphicsScene);
}

MainWindow::~MainWindow()
{
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
