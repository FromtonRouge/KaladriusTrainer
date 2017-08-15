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
#include "AttributeValueTreeItem.h"
#include <QtGui/QFont>
#include <QtGui/QColor>

KeycapTreeItem::KeycapTreeItem(const QString& sKeycapId)
{
    setIcon(QIcon(":/Icons/keyboard.png"));
    setText(sKeycapId);
    setEditable(false);

    // Keycap attributes item
    {
        auto addAttribute = [](TreeItem* pParentItem, const QString& sName, const QVariant& value) -> AttributeTreeItem*
        {
            auto pAttribute = new AttributeTreeItem(sName);
            pParentItem->appendRow({pAttribute, new AttributeValueTreeItem(value)});
            return pAttribute;
        };

        auto pLabelItem = addAttribute(this, QObject::tr("Label"), sKeycapId);
        QFont defaultFont;
        defaultFont.setPixelSize(12);
        addAttribute(pLabelItem, QObject::tr("Font"), defaultFont);
        addAttribute(pLabelItem, QObject::tr("X"), qreal(0));
        addAttribute(pLabelItem, QObject::tr("Y"), qreal(0));
        addAttribute(this, QObject::tr("Color"), QColor());
    }
}

KeycapTreeItem::~KeycapTreeItem()
{

}
