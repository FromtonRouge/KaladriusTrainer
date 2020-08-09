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

#pragma once

#include <QtWidgets/QMainWindow>
#include <QtCore/QScopedPointer>

namespace Ui
{
    class MainWindow;
}

class CountdownTimer;
class WordCounter;
class LevelsModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* pParent = nullptr);
    ~MainWindow();

    void Init();

protected slots:
    void on_actionAbout_triggered();
    void delayedRestoreState();
    void onCountdownTimerDone();

protected:
    virtual bool event(QEvent* pEvent) override;

private:
    QScopedPointer<Ui::MainWindow> _pUi;
    LevelsModel* _pLevelsModel = nullptr;
    CountdownTimer* _pCountdownTimer = nullptr;
    WordCounter* _pWordCounter = nullptr;
};
