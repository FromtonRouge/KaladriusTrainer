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

#pragma once

#include "Streams/StreamSink.h"
#include <QtWidgets/QApplication>
#include <boost/iostreams/stream_buffer.hpp>

#if defined(qApp)
#undef qApp
#endif
#define qApp (static_cast<Application*>(QApplication::instance()))

class KeyboardModel;
class TheoryModel;
class UndoableTheoryModel;
class UndoableKeyboardModel;
class Application : public QApplication
{
    Q_OBJECT

signals:
    void logs(const QString&, int);

public:
    Application(int& argc, char** argv);
    ~Application();

    TheoryModel* getTheoryModel() const {return _pTheoryModel;}
    UndoableTheoryModel* getUndoableTheoryModel() const {return _pUndoableTheoryModel;}
    KeyboardModel* getKeyboardModel() const {return _pKeyboardModel;}
    UndoableKeyboardModel* getUndoableKeyboardModel() const {return _pUndoableKeyboardModel;}

private:
    void toLogs(const QString& sText, int iWarningLevel = 0);

private:
    boost::iostreams::stream_buffer<StreamSink> _streamBufferCout;
    std::streambuf* _pOldStreambufCout;
    boost::iostreams::stream_buffer<StreamSink> _streamBufferCerr;
    std::streambuf* _pOldStreambufCerr;

    TheoryModel* _pTheoryModel;
    UndoableTheoryModel* _pUndoableTheoryModel;
    KeyboardModel* _pKeyboardModel;
    UndoableKeyboardModel* _pUndoableKeyboardModel;
};
