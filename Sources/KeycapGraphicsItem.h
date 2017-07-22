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
#include <QtCore/QRectF>
#include <QtCore/QPointF>

class QGraphicsSimpleTextItem;
class QGraphicsRectItem;
class QGraphicsRotation;
class QSvgRenderer;
class QColor;
class KeycapGraphicsItem : public QGraphicsSvgItem
{
public:
    KeycapGraphicsItem( const QString& sKeycapId,
                        qreal dRotationAngle,
                        const QPointF& rotationOrigin,
                        const QRectF& rectOuterBorder,
                        QSvgRenderer* pSvgRenderer,
                        QGraphicsItem* pParent = nullptr);
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
    QGraphicsRectItem* _pOuterBorderItem;
    QGraphicsSimpleTextItem* _pTextItem;
    qreal _dRotationAngle;
    QPointF _rotationOrigin;
    int _iTextPixelSize;
    QPointF _textOffset;
    QRectF _rectOuterBorder;
    QGraphicsRotation* _pRotation;
};
