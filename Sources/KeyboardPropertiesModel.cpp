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

#include "KeyboardPropertiesModel.h"
#include <QtXml/QDomDocument>
#include <QtGui/QStandardItem>
#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtCore/QRegularExpression>
#include <iostream>

KeyboardPropertiesModel::KeyboardPropertiesModel(QObject* pParent)
    : QStandardItemModel(pParent)
{
}

KeyboardPropertiesModel::~KeyboardPropertiesModel()
{
}

void KeyboardPropertiesModel::loadKeyboardSvg(const QString& sSvgFilePath)
{
    _sKeyboardSvgFilePath = sSvgFilePath;

    clear(); // modelReset() signal sent here
    setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Value"));

    QDomDocument document;
    QFile fileKeyboard(_sKeyboardSvgFilePath);
    if (fileKeyboard.open(QFile::ReadOnly))
    {
        document.setContent(&fileKeyboard);
        fileKeyboard.close();
    }

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

        auto pKeysRoot = new QStandardItem(QIcon(":/Icons/keyboard-full.png"), tr("Keys"));
        pKeysRoot->setData(int(PropertyType::KeycapsRoot), int(UserRole::PropertyTypeRole));
        pKeysRoot->setEditable(false);

        for (const auto& element : elements)
        {
            const QString& sKeyId = element.attribute("id");

            // Check for transform
            float fAngle = 0;
            if (element.hasAttribute("transform"))
            {
                QString sTransform = element.attribute("transform");
                if (sTransform.contains("rotate"))
                {
                    sTransform.replace(QRegularExpression("(rotate\\()|\\)"), "");
                    const QStringList& params = sTransform.split(" ");
                    if (!params.isEmpty())
                    {
                        const QString& sAngle = params.front();
                        fAngle = sAngle.toFloat();
                    }
                }
            }

            auto pItem1 = new QStandardItem(QIcon(":/Icons/keyboard.png"), sKeyId);
            pItem1->setData(int(PropertyType::Keycap), int(UserRole::PropertyTypeRole));
            pItem1->setData(fAngle, int(UserRole::AngleRole));
            pItem1->setEditable(false);
            auto pItem2 = new QStandardItem();
            pItem2->setEditable(false);
            pItem2->setSelectable(false);
            pKeysRoot->appendRow({pItem1, pItem2}); // no signal sent
        }

        auto pEmptyItem  = new QStandardItem();
        pEmptyItem->setEditable(false);
        pEmptyItem->setSelectable(false);
        appendRow({pKeysRoot, pEmptyItem}); // rowsInserted() signal sent here
    }

    emit keyboardLoaded();
}
