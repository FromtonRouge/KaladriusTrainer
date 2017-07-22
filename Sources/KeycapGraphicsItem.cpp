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
#include <QtWidgets/QGraphicsRotation>
#include <QtWidgets/QGraphicsRectItem>
#include <QtWidgets/QGraphicsScene>
#include <QtGui/QFont>
#include <QtGui/QPainter>
#include <QtGui/QMatrix>
#include <QtSvg/QSvgRenderer>
#include <QDebug>

KeycapGraphicsItem::KeycapGraphicsItem( const QString& sKeycapId,
                                        qreal dRotationAngle,
                                        const QPointF& rotationOrigin,
                                        const QRectF& rectOuterBorder,
                                        QSvgRenderer* pSvgRenderer,
                                        QGraphicsItem* pParent)
    : QGraphicsSvgItem(pParent)
    , _iTextPixelSize(20)
    , _dRotationAngle(dRotationAngle)
    , _rotationOrigin(rotationOrigin)
    , _rectOuterBorder(rectOuterBorder)
{
    setSharedRenderer(pSvgRenderer);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setElementId(sKeycapId);

    _matrixScene = pSvgRenderer->matrixForElement(sKeycapId);
    const auto& rectItem = pSvgRenderer->boundsOnElement(sKeycapId);
    const auto& rectScene = _matrixScene.mapRect(rectItem);
    const QPointF& pointScene = rectScene.topLeft();
    setPos(pointScene);
    auto pGraphicsEffect = new KeycapColorEffect(this);
    setGraphicsEffect(pGraphicsEffect);
    pGraphicsEffect->setEnabled(false);

    // We only use this rect as an invisible parent for _pTextItem
    const QPointF& pointOuterBorderInScene = _matrixScene.map(_rectOuterBorder.topLeft());
    const QPointF& pointInParentItem = mapFromScene(pointOuterBorderInScene);
    _pOuterBorderItem = new QGraphicsRectItem(QRectF(QPointF(), _rectOuterBorder.size()), this);
    _pOuterBorderItem->setPos(pointInParentItem);
    const QPointF& pointRotationInParentItem = mapFromScene(_matrixScene.map(_rotationOrigin));
    const QPointF& poinRotationInRectItem = _pOuterBorderItem->mapFromParent(pointRotationInParentItem);
    _pRotation = new QGraphicsRotation(this);
    _pRotation->setAngle(_dRotationAngle);
    _pRotation->setOrigin(QVector3D(poinRotationInRectItem.x(), poinRotationInRectItem.y(), 0));
    _pOuterBorderItem->setTransformations({_pRotation});
    _pOuterBorderItem->setPen(QPen(Qt::NoPen));
    _pOuterBorderItem->setBrush(Qt::transparent);

    // Build text item as a child of the invisible rect that contains the rotation transform
    _pTextItem = new QGraphicsSimpleTextItem(_pOuterBorderItem);
}

KeycapGraphicsItem::~KeycapGraphicsItem()
{
}

void KeycapGraphicsItem::setColor(const QColor& color)
{
    auto pGraphicsEffect = static_cast<KeycapColorEffect*>(graphicsEffect());
    pGraphicsEffect->setEnabled(true);
    pGraphicsEffect->setColor(color);
}

void KeycapGraphicsItem::setText(const QString& sText)
{
    _pTextItem->setText(sText);
    centerText();
}

void KeycapGraphicsItem::setTextFont(const QFont& font)
{
    _pTextItem->setFont(font);
    centerText();
}

void KeycapGraphicsItem::setTextOffsetX(qreal fX)
{
    _textOffset.setX(fX);
    centerText();
}

void KeycapGraphicsItem::setTextOffsetY(qreal fY)
{
    _textOffset.setY(-fY);
    centerText();
}

QPainterPath KeycapGraphicsItem::shape() const
{
    QPainterPath result;

    // Rebuild the rotation transform
    QMatrix4x4 m;
    const QPointF& pointRotationInItem = mapFromScene(_matrixScene.map(_rotationOrigin));
    QGraphicsRotation rotation;
    rotation.setAngle(_dRotationAngle);
    rotation.setOrigin(QVector3D(pointRotationInItem.x(), pointRotationInItem.y(), 0));
    rotation.applyTo(&m);

    // Apply the rotation the the rectangle and build the shape path
    const QTransform& t = m.toTransform();
    QRectF rectOuterBorder = _pOuterBorderItem->rect();
    rectOuterBorder.moveTo(_pOuterBorderItem->pos());
    QPointF p1 = t.map(rectOuterBorder.topLeft());
    QPointF p2 = t.map(rectOuterBorder.topRight());
    QPointF p3 = t.map(rectOuterBorder.bottomRight());
    QPointF p4 = t.map(rectOuterBorder.bottomLeft());
    result.moveTo(p1);
    result.lineTo(p2);
    result.lineTo(p3);
    result.lineTo(p4);
    result.lineTo(p1);
    return result;
}

QVariant KeycapGraphicsItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
    switch (change)
    {
    case QGraphicsItem::GraphicsItemChange::ItemSelectedChange:
        {
            auto pGraphicsEffect = static_cast<KeycapColorEffect*>(graphicsEffect());
            pGraphicsEffect->setEnabled(true);
            if (value.toBool())
            {
                pGraphicsEffect->setSelectionColor(Qt::blue);
            }
            else
            {
                pGraphicsEffect->setSelectionColor(QColor());
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
    QGraphicsSvgItem::paint(pPainter, &options, pWidget);
}

void KeycapGraphicsItem::centerText()
{
    const auto& rectText = _pTextItem->boundingRect();
    QPointF posText;
    posText.setX(_rectOuterBorder.width()/2 - rectText.width()/2);
    posText.setY(_rectOuterBorder.height()/2 - rectText.height()/2);
    const QPointF OFFSET(0, -4);
    _pTextItem->setPos(posText + OFFSET + _textOffset);
}
