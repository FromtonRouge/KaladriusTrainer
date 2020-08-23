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

#include "KeycapColorEffect.h"
#include <QtGui/QPixmap>
#include <QtGui/QPainter>
#include <QtGui/QBitmap>
#include <QtGui/QTransform>

KeycapColorEffect::KeycapColorEffect(QObject* pParent)
    : QGraphicsEffect(pParent)
{
}

KeycapColorEffect::~KeycapColorEffect()
{

}

void KeycapColorEffect::draw(QPainter* pPainter)
{
    if (_color == Qt::black && !_selectionColor.isValid())
    {
        drawSource(pPainter);
    }
    else
    {
        // Get and draw the source pixmap first
        const QTransform& oldTransform = pPainter->worldTransform();
        QPoint pointOffset;
        const QPixmap& pixmap = sourcePixmap(Qt::DeviceCoordinates, &pointOffset);
        pPainter->setWorldTransform(QTransform());
        pPainter->drawPixmap(pointOffset, pixmap);

        // Apply a color layer
        QPixmap pixmapColor(pixmap.size());
        const auto& mask = pixmap.createMaskFromColor(Qt::transparent);
        if (_color != Qt::black && _color.isValid())
        {
            QColor color = _color;
            color.setAlphaF(0.3f);
            pixmapColor.fill(color);
            pixmapColor.setMask(mask);
            pPainter->drawPixmap(pointOffset, pixmapColor);
        }

        // Apply a color layer
        if (_selectionColor.isValid())
        {
            QColor color = _selectionColor;
            color.setAlphaF(0.2f);
            pixmapColor.fill(color);
            pixmapColor.setMask(mask);
            pPainter->drawPixmap(pointOffset, pixmapColor);
        }

        pPainter->setWorldTransform(oldTransform);
    }
}

void KeycapColorEffect::setColor(const QColor& color)
{
    _color = color;
    update();
}

void KeycapColorEffect::setSelectionColor(const QColor& color)
{
    _selectionColor = color;
    update();
}
