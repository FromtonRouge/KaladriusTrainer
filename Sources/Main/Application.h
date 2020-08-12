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

#include "../Streams/StreamSink.h"
#include <QtWidgets/QApplication>
#include <boost/iostreams/stream_buffer.hpp>

#if defined(qApp)
#undef qApp
#endif
#define qApp (static_cast<Application*>(QApplication::instance()))

class KeyboardGraphicsScene;
class KeyboardModel;
class UndoableKeyboardModel;
class TheoryModel;
class UndoableTheoryModel;
class LevelsModel;

class Application : public QApplication
{
    Q_OBJECT

signals:
    void logs(const QString&, int);

public:
    Application(int& argc, char** argv);
    ~Application();

    void showAboutDialog();

    TheoryModel* getTheoryModel() const {return _pTheoryModel;}
    UndoableTheoryModel* getUndoableTheoryModel() const {return _pUndoableTheoryModel;}
    KeyboardModel* getKeyboardModel() const {return _pKeyboardModel;}
    UndoableKeyboardModel* getUndoableKeyboardModel() const {return _pUndoableKeyboardModel;}
    KeyboardGraphicsScene* getKeyboardGraphicsScene() const {return _pKeyboardGraphicsScene;}
    LevelsModel* getLevelsModel() const {return _pLevelsModel;}

    bool openDatabase();

private:
    void toLogs(const QString& sText, int iWarningLevel = 0);
    QString getDatabaseFilePath() const;

private:
    boost::iostreams::stream_buffer<StreamSink> _streamBufferCout;
    std::streambuf* _pOldStreambufCout;
    boost::iostreams::stream_buffer<StreamSink> _streamBufferCerr;
    std::streambuf* _pOldStreambufCerr;

    TheoryModel* _pTheoryModel = nullptr;
    UndoableTheoryModel* _pUndoableTheoryModel = nullptr;
    KeyboardModel* _pKeyboardModel = nullptr;
    UndoableKeyboardModel* _pUndoableKeyboardModel = nullptr;
    KeyboardGraphicsScene* _pKeyboardGraphicsScene = nullptr; ///< Only 1 scene and potentialy N views on this scene.
    LevelsModel* _pLevelsModel = nullptr;
};
