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

#include "KeycapGraphicsItem.h"
#include "KeycapColorEffect.h"
#include <QtWidgets/QStyleOptionGraphicsItem>
#include <QtWidgets/QGraphicsSimpleTextItem>
#include <QtGui/QFont>
#include <QtSvg/QSvgRenderer>

KeycapGraphicsItem::KeycapGraphicsItem(const QString& sKeycapId, float fAngle, QSvgRenderer* pSvgRenderer, QGraphicsItem* pParent)
    : QGraphicsSvgItem(pParent)
    , _fAngle(fAngle)
    , _iTextPixelSize(20)
{
    setSharedRenderer(pSvgRenderer);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setElementId(sKeycapId);
    setPos(pSvgRenderer->boundsOnElement(sKeycapId).topLeft());
    setGraphicsEffect(new KeycapColorEffect(this));
    setColor(QColor());

    _pTextItem = new QGraphicsSimpleTextItem(this);
}

KeycapGraphicsItem::~KeycapGraphicsItem()
{
}

void KeycapGraphicsItem::setColor(const QColor& color)
{
    auto pGraphicsEffect = static_cast<KeycapColorEffect*>(graphicsEffect());
    if (!color.isValid())
    {
        pGraphicsEffect->setEnabled(false);
    }
    else
    {
        pGraphicsEffect->setEnabled(true);
        pGraphicsEffect->setColor(color);
    }
}

void KeycapGraphicsItem::setText(const QString& sText)
{
    _pTextItem->setText(sText);
    centerText();
}

void KeycapGraphicsItem::setTextSize(int iSize)
{
    _iTextPixelSize = iSize;
    QFont font = _pTextItem->font();
    font.setPixelSize(_iTextPixelSize);
    _pTextItem->setFont(font);
    centerText();
}

QVariant KeycapGraphicsItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
    switch (change)
    {
    case QGraphicsItem::GraphicsItemChange::ItemSelectedChange:
        {
            if (value.toBool())
            {
                setColor(Qt::blue);
            }
            else
            {
                setColor(QColor());
            }
            return value;
        }
    default:
        {
            break;
        }
    }
    return QGraphicsSvgItem::itemChange(change, value);
}

void KeycapGraphicsItem::paint(QPainter* pPainter, const QStyleOptionGraphicsItem* pOption, QWidget* pWidget)
{
    QStyleOptionGraphicsItem options(*pOption);
    options.state &= ~QStyle::State_Selected;
    return QGraphicsSvgItem::paint(pPainter, &options, pWidget);
}

void KeycapGraphicsItem::centerText()
{
    const auto& rectKeycap = boundingRect();
    const auto& rectText = _pTextItem->boundingRect();
    _pTextItem->setTransformOriginPoint(rectText.center());
    _pTextItem->setRotation(_fAngle);
    _pTextItem->setPos(rectKeycap.center()-rectText.center()-QPointF(0, 4));
}
