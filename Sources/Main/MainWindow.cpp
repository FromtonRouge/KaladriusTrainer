// ======================================================================
// This file is a part of the KaladriusTrainer project
//
// Copyright (C) 2020  Vissale NEANG <fromtonrouge at gmail dot com>
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
#include "../StrokesSolver/WordsToImprove.h"
#include "../Utils/CountdownTimer.h"
#include "../Levels/Models/LevelsModel.h"
#include "../Levels/TreeItems/LevelTreeItem.h"
#include "../Levels/LevelsTreeView.h"
#include "../Dashboard/Dashboard.h"
#include "../Database/Database.h"
#include "../Utils/Functions.h"
#include "ui_MainWindow.h"
#include <QtWidgets/QTextEdit>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtQml/QQmlContext>
#include <QtCore/QEvent>
#include <QtCore/QTimer>
#include <QtCore/QDateTime>
#include <QtCore/QLineF>
#include <QtCore/QHash>

MainWindow::MainWindow(QWidget* pParent)
    : QMainWindow(pParent)
    , _pUi(new Ui::MainWindow)
    , _pCountdownTimer(new CountdownTimer(this))
    , _pDashboard(new Dashboard(this))
    , _pWordCounter(new WordCounter(_pCountdownTimer, this))
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
        connect(pStrokesSolverTextEdit, &StrokesSolverTextEdit::reset, [this](int iTextId)
        {
            CountdownTimer::Mode mode;
            mode = iTextId == 0 ? CountdownTimer::Mode::Countdown : CountdownTimer::Mode::Timer;
            _pCountdownTimer->reset(int(mode));
        });
        connect(pStrokesSolverTextEdit, &StrokesSolverTextEdit::started, _pCountdownTimer, &CountdownTimer::start);
        connect(pStrokesSolverTextEdit, &StrokesSolverTextEdit::done, this, &MainWindow::onTypingTestDone);
        connect(_pCountdownTimer, &CountdownTimer::done, this, &MainWindow::onTypingTestDone);
        connect(_pUi->treeViewLevels, &LevelsTreeView::sendText, _pUi->widgetStrokesSolver, &StrokesSolverWidget::restart);
        connect(_pUi->treeViewLevels, &LevelsTreeView::sendWordsToPractice, pStrokesSolverTextEdit, &StrokesSolverTextEdit::setWordsToPractice);
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

void MainWindow::onTypingTestDone()
{
    auto pStrokesSolverTextEdit = _pUi->widgetStrokesSolver->findChild<StrokesSolverTextEdit*>("textEdit");
    pStrokesSolverTextEdit->stopTraining();
    const int iTextId = pStrokesSolverTextEdit->getTextId();
    _pWordCounter->typingTestDone();

    const TypingTestResult& typingTestResult = _pWordCounter->getTypingTestResult();

    // Save result in database
    const QModelIndex& indexCurrentLevel = _pUi->treeViewLevels->currentIndex();
    if (indexCurrentLevel.isValid())
    {
        const int iTreeItemType = indexCurrentLevel.data(TreeItemTypeRole).toInt();
        switch (iTreeItemType)
        {
        case TreeItem::Level:
            {
                auto pDatabase = qApp->getDatabase();
                auto pLevelTreeItem = static_cast<LevelTreeItem*>(qApp->getLevelsModel()->itemFromIndex(indexCurrentLevel));
                const QString sLevelTableName = indexCurrentLevel.data(LevelTableNameRole).toString();
                const QString sLevelWordsTableName = indexCurrentLevel.data(LevelWordsTableNameRole).toString();

                QStringList columns;
                columns << "Date";
                columns << "Wpm";
                columns << "Spm";
                columns << "Accuracy";
                columns << "Viscosity";

                QVariantList values;
                values << QDateTime::currentDateTime().toString();
                values << _pWordCounter->getWpm();
                values << _pWordCounter->getSpm();
                values << _pWordCounter->getAccuracy();
                values << _pWordCounter->getViscosity();

                if (iTextId == 0)
                {
                    columns << "Progress";
                    values << pLevelTreeItem->getProgressionPercentage();

                    if (pDatabase->insertValues(sLevelTableName, columns, {values}))
                    {
                        _pUi->chartView->createChart(sLevelTableName);
                    }

                    const auto pWordsToImprove = _pUi->widgetStrokesSolver->getWordsToImprove();
                    const QStringList& currentWords = pWordsToImprove->getWordsToImprove();
                    QHash<QString, int> progressValuesByWord;

                    // Initialize progress values for current words to improve first
                    // The user may not have the time to register a result for each "word to improve"
                    // So we need at least to report the current progress value for those words
                    for (const QString& sWordToImprove : currentWords)
                    {
                        QString sQueryGetValues = "SELECT Progression FROM \"%1\" WHERE Word == \"%2\"";
                        sQueryGetValues = sQueryGetValues.arg(sLevelWordsTableName).arg(sWordToImprove);
                        QSqlQuery queryGetValues = pDatabase->execute(sQueryGetValues);

                        if (queryGetValues.next())
                        {
                            const int iProgression =  queryGetValues.value(0).toInt(); // Can be -1 when it's a "NEW" word
                            progressValuesByWord[sWordToImprove] = iProgression;
                        }
                    }

                    // Insert values in the level words table
                    auto itWordData = typingTestResult.wordData.begin();
                    while (itWordData != typingTestResult.wordData.end())
                    {
                        const QString& sWord = itWordData.key();
                        const WordData& wordData = itWordData++.value();

                        QString sQueryGetValues = "SELECT * FROM \"%1\" WHERE Word == \"%2\"";
                        sQueryGetValues = sQueryGetValues.arg(sLevelWordsTableName).arg(sWord);
                        QSqlQuery queryGetValues = pDatabase->execute(sQueryGetValues);

                        int iProgression =  0;
                        int iOccurences = wordData.occurences;
                        float fAverageErrorsCount = wordData.averageErrorsCount;
                        float fAverageChordsCount = wordData.averageChordsCount;
                        float fAverageTimeSpentForFirstStroke = wordData.averageTimeSpentForFirstStroke;
                        float fAverageTimeSpentToComplete = wordData.averageTimeSpentToComplete;

                        if (queryGetValues.next())
                        {
                            iProgression =  queryGetValues.value("Progression").toInt(); // Can be -1 when it's a "NEW" word
                            iOccurences += queryGetValues.value("Occurences").toInt();

                            auto averageIfNeeded = [&queryGetValues](const QString& sColumnName, float& rfAverageForThisTest)
                            {
                                const float fValueInDb = queryGetValues.value(sColumnName).toFloat();
                                if ( fValueInDb > 0)
                                {
                                    rfAverageForThisTest = (rfAverageForThisTest + fValueInDb)/2;
                                }
                            };

                            averageIfNeeded("AverageErrorsCount", fAverageErrorsCount);
                            averageIfNeeded("AverageChordsCount", fAverageChordsCount);
                            averageIfNeeded("AverageTimeSpentToComplete", fAverageTimeSpentToComplete);
                            averageIfNeeded("AverageTimeSpentForFirstStroke", fAverageTimeSpentForFirstStroke);
                        }

                        // Compute the progression delta fx(timePerChord)
                        //  with timePerChord of 500 ms (120 spm) we want a deltaProgress = 100
                        //  with timePerChord of 2000 (30 spm) we want a deltaProgress = 5
                        const float MIN_TIME_PER_CHORD = 500;
                        const float MAX_DELTA_PROGRESSION = 100;
                        const float MAX_TIME_PER_CHORD = 2000;
                        const float MIN_DELTA_PROGRESSION = 5;
                        const QPointF p1(MIN_TIME_PER_CHORD, MAX_DELTA_PROGRESSION);
                        const QPointF p2(MAX_TIME_PER_CHORD, MIN_DELTA_PROGRESSION);

                        const float fAverageStrokeTimePerChord = wordData.averageTimeSpentToComplete / wordData.averageChordsCount;
                        int iDeltaProgression = int(interpolate(p1, p2, fAverageStrokeTimePerChord));

                        // Computing errors penalties
                        const int iErrors = int(wordData.averageErrorsCount * wordData.occurences);
                        const int iPenalty = iErrors * 10; // -10% per error on this word
                        iDeltaProgression -= iPenalty;

                        iProgression += iDeltaProgression;

                        const int iFinalProgression = qBound(0, iProgression, 125); // Max = 100 + [tolerance]
                        progressValuesByWord[sWord] = iFinalProgression;

                        QStringList values;
                        values << QString("\"Progression\" = %1").arg(iFinalProgression); // Progression can't goes back to -1 (which means "NEW" word)
                        values << QString("\"Occurences\" = %1").arg(iOccurences);
                        values << QString("\"AverageErrorsCount\" = %1").arg(fAverageErrorsCount);
                        values << QString("\"AverageChordsCount\" = %1").arg(fAverageChordsCount);
                        values << QString("\"AverageTimeSpentToComplete\" = %1").arg(fAverageTimeSpentToComplete);
                        values << QString("\"AverageTimeSpentForFirstStroke\" = %1").arg(fAverageTimeSpentForFirstStroke);

                        QString sQueryUpdateWord = QString("UPDATE \"%1\" SET %2 WHERE Word == \"%3\"");
                        sQueryUpdateWord = sQueryUpdateWord.arg(sLevelWordsTableName).arg(values.join(",")).arg(sWord);
                        pDatabase->execute(sQueryUpdateWord);
                    }

                    // Update the Level indicator
                    qApp->getLevelsModel()->updateLevelDisplay(indexCurrentLevel);

                    QVector<int> newProgressions;
                    for (const QString& sWord : currentWords)
                    {
                        newProgressions << progressValuesByWord[sWord];
                    }

                    pWordsToImprove->showResults(newProgressions);
                }
                else
                {
                    _pCountdownTimer->stop();

                    columns << "TextId";
                    values << iTextId;

                    if (pDatabase->insertValues(sLevelTableName, columns, {values}))
                    {
                        _pUi->chartView->createChart(sLevelTableName);
                    }
                }

                // Update last and pb
                _pDashboard->setLastWpm(pDatabase->getLastWpm(sLevelTableName));
                _pDashboard->setLastSpm(pDatabase->getLastSpm(sLevelTableName));
                _pDashboard->setLastAccuracy(pDatabase->getLastAccuracy(sLevelTableName));
                _pDashboard->setMaxWpm(pDatabase->getMaxWpm(sLevelTableName));
                _pDashboard->setMaxSpm(pDatabase->getMaxSpm(sLevelTableName));
                _pDashboard->setMaxAccuracy(pDatabase->getMaxAccuracy(sLevelTableName));
                break;
            }
        default:
            {
                break;
            }
        }
    }
}

void MainWindow::onTextId(int iTextId)
{
    _pUi->treeViewLevels->setWantedTextId(iTextId);
}
