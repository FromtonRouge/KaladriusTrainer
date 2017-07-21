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

#include <QtSvg/QGraphicsSvgItem>

class QGraphicsSimpleTextItem;
class QSvgRenderer;
class QColor;
class KeycapGraphicsItem : public QGraphicsSvgItem
{
public:
    KeycapGraphicsItem(const QString& sKeycapId, float fAngle, QSvgRenderer* pSvgRenderer, QGraphicsItem* pParent = nullptr);
    ~KeycapGraphicsItem();

    void setColor(const QColor& color);
    void setText(const QString& sText);
    void setTextFont(const QFont& font);
    void setTextOffsetX(qreal fX);
    void setTextOffsetY(qreal fY);

protected:
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    virtual void paint(QPainter* pPainter, const QStyleOptionGraphicsItem* pOption, QWidget* pWidget) override;

private:
    void centerText();

private:
    QGraphicsSimpleTextItem* _pTextItem;
    float _fAngle;
    int _iTextPixelSize;
    QPointF _textOffset;
};
