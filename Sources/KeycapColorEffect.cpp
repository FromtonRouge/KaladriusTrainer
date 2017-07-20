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

#include "KeycapColorEffect.h"
#include <QtGui/QPixmap>
#include <QtGui/QPainter>
#include <QtGui/QBitmap>

KeycapColorEffect::KeycapColorEffect(QObject* pParent)
    : QGraphicsEffect(pParent)
{
    _color.setAlphaF(0.3);
}

KeycapColorEffect::~KeycapColorEffect()
{

}

void KeycapColorEffect::draw(QPainter* pPainter)
{
    if (!_color.isValid())
    {
        drawSource(pPainter);
    }
    else
    {
        QPoint pointOffset;
        const QPixmap& pixmap = sourcePixmap(Qt::LogicalCoordinates, &pointOffset);
        pPainter->drawPixmap(pointOffset, pixmap);

        QPixmap pixmapColor(pixmap.size());
        pixmapColor.fill(_color);
        pixmapColor.setMask(pixmap.createMaskFromColor(Qt::transparent));
        pPainter->drawPixmap(pointOffset, pixmapColor);
    }
}

void KeycapColorEffect::setColor(const QColor& color)
{
    _color.setRed(color.red());
    _color.setGreen(color.green());
    _color.setBlue(color.blue());
    update();
}

void KeycapColorEffect::setAlpha(qreal dAlpha)
{
    _color.setAlphaF(dAlpha);
    update();
}
