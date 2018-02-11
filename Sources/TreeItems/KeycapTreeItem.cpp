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

#include "KeycapTreeItem.h"
#include "AttributeTreeItem.h"
#include "ValueTreeItem.h"
#include <QtGui/QFont>
#include <QtGui/QColor>

KeycapTreeItem::KeycapTreeItem(const QString& sKeycapId)
{
    setIcon(QIcon(":/Icons/keyboard.png"));
    setKeycapId(sKeycapId);
    setEditable(false);

    auto pLabelItem = addAttribute(QObject::tr("Label"), sKeycapId);
    QFont defaultFont;
    defaultFont.setPixelSize(12);
    pLabelItem->addAttribute(QObject::tr("Font"), defaultFont);
    pLabelItem->addAttribute(QObject::tr("X"), qreal(0));
    pLabelItem->addAttribute(QObject::tr("Y"), qreal(0));
    addAttribute(QObject::tr("Color"), QColor());
}

KeycapTreeItem::~KeycapTreeItem()
{

}

QString KeycapTreeItem::getKeycapId() const
{
    return text();
}

void KeycapTreeItem::setKeycapId(const QString& sKeycapId)
{
    setText(sKeycapId);
}
