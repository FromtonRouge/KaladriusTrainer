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

#include "TreeItems/AttributeTreeItem.h"
#include <QtGui/QIcon>

class ArrayElementTreeItem : public AttributeTreeItem
{
public:
    ArrayElementTreeItem() {}
    ~ArrayElementTreeItem() {}
    virtual int type() const override {return ArrayElement;}
};

class ArrayTreeItem : public TreeItem
{
public:
    ArrayTreeItem(const QIcon& icon = QIcon(), const QString& sName = QString());
    ~ArrayTreeItem();
    virtual int type() const override {return Array;}
    ArrayElementTreeItem* addElement(const QVariant& value);

private:
    virtual ArrayElementTreeItem* addAttribute(const QString& ignored, const QVariant& value) override;
};
