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

#include "KeycapPropertiesDelegate.h"
#include "DiffModel.h"

KeycapPropertiesDelegate::KeycapPropertiesDelegate(QObject* pParent)
    : QStyledItemDelegate(pParent)
    , _bMultipleEditions(false)
{

}

KeycapPropertiesDelegate::~KeycapPropertiesDelegate()
{

}

void KeycapPropertiesDelegate::initStyleOption(QStyleOptionViewItem* pOption, const QModelIndex& index) const
{
    QStyledItemDelegate::initStyleOption(pOption, index);
    if (_bMultipleEditions && index.flags().testFlag(Qt::ItemIsEditable))
    {
        // TODO: Use a merger model to check different values
        auto pDiffModel = qobject_cast<DiffModel*>(const_cast<QAbstractItemModel*>(index.model()));
        pOption->text = tr("<different values>");
    }
}

void KeycapPropertiesDelegate::paint(QPainter* pPainter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyledItemDelegate::paint(pPainter, option, index);
}
