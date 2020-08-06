// ======================================================================
// This file is a part of the KaladriusTrainer project
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
#include "../../Values/Types/KeycapRef.h"
#include "../../Keyboards/TreeItems/KeyboardTreeItem.h"
#include "../../Keyboards/TreeItems/KeycapTreeItem.h"
#include "../../Tree/TreeItems/AttributeTreeItem.h"
#include "../../Tree/TreeItems/ValueTreeItem.h"
#include "../../Tree/TreeItems/ListTreeItem.h"
#include "../../Streams/Iostream.h"
#include "../../Tree/Models/Utils.h"
#include <QtXml/QDomDocument>
#include <QtGui/QFont>
#include <QtCore/QRectF>
#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtCore/QRegularExpression>
#include <QtCore/QMimeData>
#include <QtCore/QDebug>

KeyboardModel::KeyboardModel(QObject* pParent)
    : TreeItemModel(pParent)
{
    setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Value"));
}

KeyboardModel::~KeyboardModel()
{
}

QModelIndex KeyboardModel::getParentKeycap(const QModelIndex& indexInKeycapHierarchy)
{
    QModelIndex indexKeycap = indexInKeycapHierarchy;
    while (indexKeycap.isValid() && (indexKeycap.data(TreeItemTypeRole).toInt() != TreeItem::Keycap))
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

        auto pKeyboardTreeItem = new KeyboardTreeItem();
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

            auto pKeycapTreeItem = new KeycapTreeItem(sKeyId);
            if (rotateTransform.size() == 3)
            {
                pKeycapTreeItem->setData(rotateTransform[0], RotationAngleRole);
                pKeycapTreeItem->setData(QPointF(rotateTransform[1], rotateTransform[2]), RotationOriginRole);
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
                pKeycapTreeItem->setData(rectOuterBorder, int(OuterBorderRole));
            }

            pKeyboardTreeItem->getKeycaps()->appendRow({pKeycapTreeItem, new EmptyTreeItem()}); // no signal sent
        }

        appendRow({pKeyboardTreeItem, new EmptyTreeItem()}); // rowsInserted() signal sent here
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

KeyboardTreeItem* KeyboardModel::getKeyboardTreeItem() const
{
    if (!hasChildren())
    {
        return nullptr;
    }
    return dynamic_cast<KeyboardTreeItem*>(invisibleRootItem()->child(0));
}

void KeyboardModel::setKeyboardTreeItem(KeyboardTreeItem* pKeyboardTreeItem)
{
    clear();
    appendRow({pKeyboardTreeItem, new EmptyTreeItem()});
    emit keyboardLoaded();
    setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Value"));
}

QModelIndex KeyboardModel::getKeyboardIndex() const
{
    if (hasChildren())
    {
        auto pKeyboardTreeItem = getKeyboardTreeItem();
        return pKeyboardTreeItem ? indexFromItem(pKeyboardTreeItem) : QModelIndex();
    }
    return QModelIndex();
}

QModelIndex KeyboardModel::getKeycapsIndex() const
{
    auto pKeycapsTreeItem = getKeyboardTreeItem()->getKeycaps();
    return pKeycapsTreeItem ? indexFromItem(pKeycapsTreeItem) : QModelIndex();
}

QStringList KeyboardModel::mimeTypes() const
{
    return TreeItemModel::mimeTypes() << "application/prs.stenotutor.keycapid";
}

bool KeyboardModel::canDropMimeData(const QMimeData* pMimeData, Qt::DropAction action, int iRow, int iColumn, const QModelIndex& parent) const
{
    if (pMimeData->hasFormat("application/prs.stenotutor.keycapid") && parent.isValid())
    {
        const QVariant& value = parent.data(Qt::EditRole);
        return value.isValid() && value.userType() == qMetaTypeId<KeycapRef>();
    }
    return TreeItemModel::canDropMimeData(pMimeData, action, iRow, iColumn, parent);
}

void KeyboardModel::selectLinkedKeys(const QString& sDictionaryName, const QVector<QBitArray>& possibleBitsEntries)
{
    if (possibleBitsEntries.isEmpty())
    {
        return;
    }

    const QModelIndex& indexLinkedTheories = getKeyboardTreeItem()->getLinkedTheories()->index();
    if (indexLinkedTheories.isValid() && hasChildren(indexLinkedTheories))
    {
        // Find the first index with the name sDictionaryName
        const auto& matches = match(indexLinkedTheories.child(0, 0), Qt::DisplayRole, sDictionaryName, 1, Qt::MatchExactly|Qt::MatchRecursive);
        if (!matches.isEmpty())
        {
            const QModelIndex& indexLinkedDictionary = matches.front();
            const QModelIndex& indexLinkedKeys = Utils::index(this, "Linked Keys", 0, indexLinkedDictionary);
            if (indexLinkedKeys.isValid())
            {
                const int iLinkedKeycaps = rowCount(indexLinkedKeys);
                typedef QPair<QString, bool> KeycapState;
                typedef QVector<KeycapState> KeycapsStates;
                typedef QVector<KeycapsStates> PossibleKeycapsEntries;
                PossibleKeycapsEntries possibleKeycapsEntries;
                for (const auto& bits : possibleBitsEntries)
                {
                    KeycapsStates keycapsStates(iLinkedKeycaps);
                    for (int iLinkedKeycap = 0; iLinkedKeycap < iLinkedKeycaps; ++iLinkedKeycap)
                    {
                        const QModelIndex& indexKeycapId = indexLinkedKeys.child(iLinkedKeycap, 1);
                        const QVariant& variant = indexKeycapId.data(Qt::EditRole);
                        if (variant.isValid())
                        {
                            const auto& keycapRef = qvariant_cast<KeycapRef>(variant);
                            keycapsStates[iLinkedKeycap].first = keycapRef.keycapId;

                            if (bits.size() > iLinkedKeycap)
                            {
                                keycapsStates[iLinkedKeycap].second = bits.testBit(iLinkedKeycap);
                            }
                            else
                            {
                                keycapsStates[iLinkedKeycap].second = false;
                            }
                        }
                    }
                    possibleKeycapsEntries << keycapsStates;
                }

                emit linkedKeycapsStates(possibleKeycapsEntries);
            }
        }
    }
}

void KeyboardModel::selectLinkedSpecialKeys(const HashSpecialKeysStates& specialKeysStates)
{
    KeycapsStates keycapsStates;

    const QModelIndex& indexLinkedTheories = getKeyboardTreeItem()->getLinkedTheories()->index();
    if (indexLinkedTheories.isValid() && hasChildren(indexLinkedTheories))
    {
        const auto& matches = match(indexLinkedTheories.child(0, 0), Qt::DisplayRole, QStringLiteral("Linked Special Keys"), 1, Qt::MatchExactly|Qt::MatchRecursive);
        if (!matches.isEmpty())
        {
            const QModelIndex& indexLinkedSpecialKeys = matches.front();
            const int iSpecialKeys = rowCount(indexLinkedSpecialKeys);
            for (int iSpecialKey = 0; iSpecialKey < iSpecialKeys; ++iSpecialKey)
            {
                const QModelIndex& indexSpecialKey = index(iSpecialKey, 0, indexLinkedSpecialKeys);
                const QString& sSpecialKey = indexSpecialKey.data(Qt::DisplayRole).toString();
                bool bEnabled = false;
                const int iMappings = rowCount(indexSpecialKey);
                for (int iMapping = 0; iMapping < iMappings; ++iMapping)
                {
                    const QModelIndex& indexMapping = index(iMapping, 1, indexSpecialKey);
                    const QVariant& variant = indexMapping.data(Qt::EditRole);
                    if (variant.isValid())
                    {
                        const KeycapRef& keycapRef = qvariant_cast<KeycapRef>(variant);

                        // TODO: Enable only one keycap
                        if (!bEnabled)
                        {
                            bEnabled = specialKeysStates[sSpecialKey];
                            keycapsStates << KeycapState(keycapRef.keycapId, bEnabled);
                        }
                        else
                        {
                            keycapsStates << KeycapState(keycapRef.keycapId, false);
                        }
                    }
                }
            }
        }
    }

    if (!keycapsStates.isEmpty())
    {
        emit linkedKeycapsStates(keycapsStates);
    }
}
