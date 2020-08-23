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

#pragma once

#include <QtCore/QString>
#include <QtCore/QSharedPointer>
#include <boost/iostreams/concepts.hpp> 
#include <boost/signals2.hpp>

class StreamSink : public boost::iostreams::sink
{
public:
    typedef boost::signals2::signal<void(const QString&)> SignalWrite;

public:
    StreamSink(const SignalWrite::slot_function_type& slot)
        : _pSignalWrite(new SignalWrite())
    {
        _pSignalWrite->connect(slot);
    }

    /**
     * Write the incoming string.
     * We just send the signal.
     */
    std::streamsize write(const char* s, std::streamsize n)
    {
        // The signal is thread safe
        (*_pSignalWrite)(QString(QByteArray(s, static_cast<int>(n))));
        return n;
    }

private:
    QSharedPointer<SignalWrite> _pSignalWrite;
};

