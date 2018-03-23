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

#include <QtCore/QByteArray>
#include <QtCore/QList>

class QStandardItem;
class KeyboardModel;
class TheoryModel;
class QString;
class QTextDocument;
namespace Serialization
{
    bool Save(KeyboardModel* pModel, const QString& sFilePath);
    bool Load(KeyboardModel* pModel, const QString& sFilePath);

    bool Save(TheoryModel* pModel, const QString& sFilePath);
    bool Load(TheoryModel* pModel, const QString& sFilePath);

    bool Save(QTextDocument* pTextDocument, const QString& sFilePath);
    bool Load(QTextDocument* pTextDocument, const QString& sFilePath);

    QByteArray Save(const QList<QStandardItem*>& itemsRow);
    QList<QStandardItem*> Load(const QByteArray& branchData);
}
