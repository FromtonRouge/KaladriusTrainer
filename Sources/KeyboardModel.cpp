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

#include "KeyboardModel.h"
#include "TreeItems/KeyboardTreeItem.h"
#include "Iostream.h"
#include <QtXml/QDomDocument>
#include <QtGui/QStandardItem>
#include <QtGui/QFont>
#include <QtCore/QRectF>
#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtCore/QRegularExpression>

KeyboardModel::KeyboardModel(QObject* pParent)
    : QStandardItemModel(pParent)
{
    setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Value"));
}

KeyboardModel::~KeyboardModel()
{
}

QModelIndex KeyboardModel::getParentKeycap(const QModelIndex& indexInKeycapHierarchy)
{
    QModelIndex indexKeycap = indexInKeycapHierarchy;
    while (indexKeycap.isValid() && (indexKeycap.data(PropertyTypeRole).toInt() != Keycap))
    {
        indexKeycap = indexKeycap.parent();
    }
    return indexKeycap;
}

void KeyboardModel::loadKeyboardSvgFile(const QString& sSvgFilePath)
{
    QByteArray svgContent;
    QFile fileKeyboard(sSvgFilePath);
    if (fileKeyboard.open(QFile::ReadOnly))
    {
        svgContent = fileKeyboard.readAll();
        fileKeyboard.close();
    }

    if (svgContent.isEmpty())
    {
        CERR(tr("Can't load svg file %1").arg(sSvgFilePath));
    }
    else
    {
        loadKeyboardSvg(svgContent);
    }
}

void KeyboardModel::loadKeyboardSvg(const QByteArray& svgContent)
{
    _svgContent = svgContent;

    clear(); // modelReset() signal sent here

    QDomDocument document;
    document.setContent(_svgContent);

    if (document.hasChildNodes())
    {
        QList<QDomElement> elements;
        const QDomNode& nodeDocument = document.documentElement();
        QDomNode node = nodeDocument.firstChild();
        while (!node.isNull())
        {
            QDomElement element = node.toElement();
            if (!element.isNull())
            {
                const QString& sName = element.tagName();
                if (sName == "g")
                {
                    const QString& sClass = element.attribute("class");
                    if (!sClass.isEmpty())
                    {
                        if (sClass.contains("keycap"))
                        {
                            const QString& sKeyId = element.attribute("id");
                            if (!sKeyId.isEmpty())
                            {
                                elements << element;
                            }
                        }
                    }
                    else
                    {
                        node = node.firstChild();
                        continue;
                    }
                }
            }
            node = node.nextSibling();
        }

        auto pKeysRoot = new KeyboardTreeItem();
        pKeysRoot->setData(KeycapsRoot, PropertyTypeRole);

        for (const auto& element : elements)
        {
            const QString& sKeyId = element.attribute("id");

            // Check for transform
            QVector<qreal> rotateTransform;
            if (element.hasAttribute("transform"))
            {
                QString sTransform = element.attribute("transform");
                if (sTransform.contains("rotate"))
                {
                    sTransform.replace(QRegularExpression("(rotate\\()|\\)"), "");
                    for (const QString& sToken : sTransform.split(" "))
                    {
                        rotateTransform << sToken.toDouble();
                    }
                }
            }

            auto pKeycapItem = new QStandardItem(QIcon(":/Icons/keyboard.png"), sKeyId);
            pKeycapItem->setData(Keycap, PropertyTypeRole);
            if (rotateTransform.size() == 3)
            {
                pKeycapItem->setData(rotateTransform[0], RotationAngleRole);
                pKeycapItem->setData(QPointF(rotateTransform[1], rotateTransform[2]), RotationOriginRole);
            }

            // Get the child outer border rect element
            const auto& rectElement = element.firstChildElement("rect");
            if (!rectElement.isNull() && rectElement.attribute("class") == "outer border")
            {
                QRectF rectOuterBorder;
                rectOuterBorder.setX(rectElement.attribute("x").toDouble());
                rectOuterBorder.setY(rectElement.attribute("y").toDouble());
                rectOuterBorder.setWidth(rectElement.attribute("width").toDouble());
                rectOuterBorder.setHeight(rectElement.attribute("height").toDouble());
                pKeycapItem->setData(rectOuterBorder, int(UserRole::OuterBorderRole));
            }

            pKeycapItem->setEditable(false);

            // Keycap attributes item
            {
                auto addAttribute = [](QStandardItem* pParentItem, const QString& sName, const QVariant& value) -> QStandardItem*
                {
                    auto pAttrCol0 = new QStandardItem(QIcon(":/Icons/document-attribute.png"), sName);
                    pAttrCol0->setData(Attribute, PropertyTypeRole);
                    pAttrCol0->setEditable(false);
                    auto pAttrCol1 = new QStandardItem();
                    pAttrCol1->setData(value, Qt::EditRole);
                    pAttrCol1->setData(Attribute, PropertyTypeRole);
                    pParentItem->appendRow({pAttrCol0, pAttrCol1}); // no signal sent
                    return pAttrCol0;
                };

                auto pLabelItem = addAttribute(pKeycapItem, tr("Label"), sKeyId);
                QFont defaultFont;
                defaultFont.setPixelSize(12);
                addAttribute(pLabelItem, tr("Font"), defaultFont);
                addAttribute(pLabelItem, tr("X"), qreal(0));
                addAttribute(pLabelItem, tr("Y"), qreal(0));
                addAttribute(pKeycapItem, tr("Color"), QColor());
            }

            pKeysRoot->appendRow({pKeycapItem, new EmptyTreeItem()}); // no signal sent
        }

        appendRow({pKeysRoot, new EmptyTreeItem()}); // rowsInserted() signal sent here
        emit keyboardLoaded();
    }
    else
    {
        CERR(tr("Empty document"));
    }
    setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Value"));
}

void KeyboardModel::setSvgContent(const QByteArray& svgContent)
{
    _svgContent = svgContent;
    clear();
    setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Value"));
}
