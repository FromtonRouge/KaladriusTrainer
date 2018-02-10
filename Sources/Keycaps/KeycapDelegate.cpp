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

#include "KeycapDelegate.h"
#include "ValueTypes/KeycapRef.h"
#include "ValueTypes/ArrayValue.h"
#include "Models/DiffModel.h"
#include <QtGui/QFont>
#include <QtCore/QEvent>
#include <QtCore/QMetaProperty>

KeycapDelegate::KeycapDelegate(QObject* pParent)
    : UserItemDelegate(pParent)
{

}

KeycapDelegate::~KeycapDelegate()
{

}

void KeycapDelegate::setModelData(QWidget* pEditor, QAbstractItemModel* pModel, const QModelIndex& index) const
{
    // Set model data only if the value has really changed
    QByteArray name = pEditor->metaObject()->userProperty().name();
    if (!name.isEmpty())
    {
        const QVariant& oldValue = index.data(Qt::EditRole);
        const QVariant& newValue = pEditor->property(name);
        if (oldValue != newValue)
        {
            UserItemDelegate::setModelData(pEditor, pModel, index);
        }
    }
}

void KeycapDelegate::initStyleOption(QStyleOptionViewItem* pOption, const QModelIndex& index) const
{
    UserItemDelegate::initStyleOption(pOption, index);

    if (index.flags().testFlag(Qt::ItemIsEditable))
    {
        auto pDiffModel = qobject_cast<DiffModel*>(const_cast<QAbstractItemModel*>(index.model()));
        if (pDiffModel && pDiffModel->hasDifferentValues(index))
        {
            pOption->text = tr("<different values>");
        }
        else
        {
            const QVariant& value = index.data(Qt::EditRole);
            switch (value.type())
            {
            case QVariant::UserType:
                {
                    const int iUserType = value.userType();
                    if (iUserType == qMetaTypeId<KeycapRef>())
                    {
                        const auto& keycapRef = qvariant_cast<KeycapRef>(value);
                        pOption->text = keycapRef.keycapId;
                        pOption->features.setFlag(QStyleOptionViewItem::HasDecoration);
                        if (pOption->text.isEmpty())
                        {
                            pOption->icon = QIcon(":/Icons/question.png");
                            pOption->text = QObject::tr("<no keycap>");
                        }
                        else
                        {
                            pOption->icon = QIcon(":/Icons/keyboard.png");
                        }
                    }
                    else if (iUserType == qMetaTypeId<ArrayValue>())
                    {
                        const auto& data = qvariant_cast<ArrayValue>(value);
                        pOption->text = data.sLabel;
                        pOption->features.setFlag(QStyleOptionViewItem::HasDecoration);
                        pOption->icon = QIcon(":/Icons/edit-list-order.png");
                    }
                    break;
                }
            case QVariant::Font:
                {
                    const QFont& font = value.value<QFont>();
                    pOption->text = QString("[%1]").arg(font.family());
                    break;
                }
            case QVariant::Color:
                {
                    const QColor& color = value.value<QColor>();
                    QString sText(QObject::tr("[No color]"));
                    if (color.isValid())
                    {
                        pOption->backgroundBrush = QBrush(color);
                        pOption->palette.setBrush(QPalette::Text, QBrush(color.lightnessF() > 0.5 ? Qt::black : Qt::white));
                        sText = QString("[%1]").arg(value.toString());
                    }
                    pOption->text = sText;
                    break;
                }
            default:
                {
                    break;
                }
            }
        }
    }
}
