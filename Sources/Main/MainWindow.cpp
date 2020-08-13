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
#include "../StrokesSolver/StrokesSolverTextEdit.h"
#include "../StrokesSolver/StrokesSolverWidget.h"
#include "../StrokesSolver/WordCounter.h"
#include "../Utils/CountdownTimer.h"
#include "../Levels/Models/LevelsModel.h"
#include "../Levels/TreeItems/LevelTreeItem.h"
#include "../Levels/LevelsTreeView.h"
#include "../Dashboard/Dashboard.h"
#include "../Database/Database.h"
#include "ui_MainWindow.h"
#include <QtWidgets/QTextEdit>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtQml/QQmlContext>
#include <QtCore/QEvent>
#include <QtCore/QTimer>
#include <QtCore/QDateTime>

MainWindow::MainWindow(QWidget* pParent)
    : QMainWindow(pParent)
    , _pUi(new Ui::MainWindow)
    , _pCountdownTimer(new CountdownTimer(this))
    , _pWordCounter(new WordCounter(_pCountdownTimer, this))
    , _pDashboard(new Dashboard(this))
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

    auto pStrokesSolverTextEdit = _pUi->widgetStrokesSolver->findChild<StrokesSolverTextEdit*>("textEdit");
    auto pKeyboardGraphicsScene = qApp->getKeyboardGraphicsScene();
    if (pStrokesSolverTextEdit)
    {
        pStrokesSolverTextEdit->setWordCounter(_pWordCounter);
        connect(pStrokesSolverTextEdit, SIGNAL(dictionaryMatch(QString, QVector<QBitArray>)), pKeyboardModel, SLOT(selectLinkedKeys(QString, QVector<QBitArray>)));
        connect(pStrokesSolverTextEdit, SIGNAL(notifySpecialKeys(HashSpecialKeysStates)), pKeyboardModel, SLOT(selectLinkedSpecialKeys(HashSpecialKeysStates)));
        connect(pStrokesSolverTextEdit, SIGNAL(solverStarted()), pKeyboardGraphicsScene, SLOT(clearSelection()));
        connect(pStrokesSolverTextEdit, &StrokesSolverTextEdit::reset, _pCountdownTimer, &CountdownTimer::reset);
        connect(pStrokesSolverTextEdit, &StrokesSolverTextEdit::started, _pCountdownTimer, &CountdownTimer::start);
        connect(_pCountdownTimer, &CountdownTimer::done, this, &MainWindow::onCountdownTimerDone);
        connect(_pUi->treeViewLevels, &LevelsTreeView::sendText, pStrokesSolverTextEdit, &StrokesSolverTextEdit::restart);
        connect(_pUi->widgetStrokesSolver, &StrokesSolverWidget::restartNeeded, _pUi->treeViewLevels, &LevelsTreeView::restart);
    }

    auto pRootContext = _pUi->quickWidgetDashboard->rootContext();
    pRootContext->setContextProperty("dashboard", _pDashboard);
    pRootContext->setContextProperty("countdownTimer", _pCountdownTimer);
    pRootContext->setContextProperty("wordCounter", _pWordCounter);
    _pUi->quickWidgetDashboard->setSource(QUrl("qrc:/Qml/Dashboard.qml"));

    // Default dock states
    _pUi->dockWidgetDictionaries1->hide();
    _pUi->dockWidgetDictionaries2->hide();
    _pUi->dockWidgetDictionaries3->hide();
    _pUi->dockWidgetDictionaries4->hide();

    connect(pKeyboardModel, SIGNAL(keyboardLoaded()), _pUi->graphicsView, SLOT(fitKeyboardInView()));
    _pUi->graphicsView->setScene(pKeyboardGraphicsScene);
}

MainWindow::~MainWindow()
{
}

void MainWindow::Init()
{
    connect(_pUi->treeViewLevels, &LevelsTreeView::restarted, [&](const QString& sTableName)
    {
        _pUi->chartView->createChart(sTableName);
        auto pDatabase = qApp->getDatabase();
        _pDashboard->setLastWpm(pDatabase->getLastWpm(sTableName));
        _pDashboard->setLastSpm(pDatabase->getLastSpm(sTableName));
        _pDashboard->setLastAccuracy(pDatabase->getLastAccuracy(sTableName));

        _pDashboard->setMaxWpm(pDatabase->getMaxWpm(sTableName));
        _pDashboard->setMaxSpm(pDatabase->getMaxSpm(sTableName));
        _pDashboard->setMaxAccuracy(pDatabase->getMaxAccuracy(sTableName));
    });

    _pUi->treeViewLevels->setModel(qApp->getLevelsModel());
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
    qApp->showAboutDialog();
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

void MainWindow::onCountdownTimerDone()
{
    auto pStrokesSolverTextEdit = _pUi->widgetStrokesSolver->findChild<StrokesSolverTextEdit*>("textEdit");
    pStrokesSolverTextEdit->stopTraining();

    // Save result in database
    const QModelIndex& indexCurrentLevel = _pUi->treeViewLevels->currentIndex();
    if (indexCurrentLevel.isValid())
    {
        const int iTreeItemType = indexCurrentLevel.data(TreeItemTypeRole).toInt();
        switch (iTreeItemType)
        {
        case TreeItem::Level:
            {
                auto pLevelTreeItem = static_cast<LevelTreeItem*>(qApp->getLevelsModel()->itemFromIndex(indexCurrentLevel));
                const QUuid& uuidLevel = pLevelTreeItem->getUuid();
                const QString sTableName = QString("Level %1").arg(uuidLevel.toString(QUuid::WithoutBraces));
                const QDateTime& currentTime = QDateTime::currentDateTime();
                const QString& sCurrentTime = currentTime.toString();
                const float fWpm = _pWordCounter->getWPM();
                const float fSpm = _pWordCounter->getSPM();
                const float fAccuracy = _pWordCounter->getAccuracy();

                // Update progression
                const uint16_t uiProgression = pLevelTreeItem->getProgression();

                // The progression is based on the Spm (Strokes per minute) average of 5
                // or if the current Spm is 70% above the needed Spm
                const float fNeededSpm = pLevelTreeItem->getSPMNeededToProgress();
                const bool bForceProgression = fSpm > (1.7*fNeededSpm);

                auto pDatabase = qApp->getDatabase();
                const float fAo5Spm = (pDatabase->getSumOfCount(sTableName, "Spm", 4) + fSpm)/5;
                if (fAo5Spm > 0.f || bForceProgression)
                {
                    const float fSpmDelta = (bForceProgression ? fSpm : fAo5Spm) - fNeededSpm;
                    const float fDeltaPercent = fSpmDelta/fNeededSpm;
                    if (fDeltaPercent > 0 && fSpm > fNeededSpm)
                    {
                        uint16_t uiNewProgress = uiProgression + 5 + 10*fDeltaPercent; // increase faster for good results
                        qApp->getLevelsModel()->setProgression(indexCurrentLevel, uiNewProgress);
                        pLevelTreeItem->saveProgression();
                    }
                }

                // Insert values in database
                QMap<QString, QVariant> values;
                values["Date"] = sCurrentTime;
                values["Wpm"] = fWpm;
                values["Spm"] = fSpm;
                values["Accuracy"] = fAccuracy;
                values["Progress"] = pLevelTreeItem->getProgressionPercentage();
                if (pDatabase->insertValues(sTableName, values))
                {
                    _pUi->chartView->createChart(sTableName);
                }

                // Update last and pb
                _pDashboard->setLastWpm(pDatabase->getLastWpm(sTableName));
                _pDashboard->setLastSpm(pDatabase->getLastSpm(sTableName));
                _pDashboard->setLastAccuracy(pDatabase->getLastAccuracy(sTableName));
                _pDashboard->setMaxWpm(pDatabase->getMaxWpm(sTableName));
                _pDashboard->setMaxSpm(pDatabase->getMaxSpm(sTableName));
                _pDashboard->setMaxAccuracy(pDatabase->getMaxAccuracy(sTableName));
                break;
            }
        default:
            {
                break;
            }
        }
    }
}
