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

#include "FingerSelector.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QComboBox>
#include <QtCore/QAbstractItemModel>

FingerSelector::FingerSelector(QWidget* pParent)
    : UserEditor(pParent)
    , _pComboBox(new QComboBox(this))
{
    _pComboBox->addItems({tr("None"), tr("Thumb"), tr("Index"), tr("Middle"), tr("Ring"), tr("Pinky")});
    connect(_pComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
    auto pLayout = new QHBoxLayout();
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->addWidget(_pComboBox);
    setLayout(pLayout);
}

void FingerSelector::setValue(const QVariant& value)
{
    _value = value;
    _pComboBox->setCurrentIndex(int(qvariant_cast<Finger>(value).id) + 1);
}

void FingerSelector::onCurrentIndexChanged(int iIndex)
{
    _value = qVariantFromValue(Finger(Finger::Id(iIndex - 1)));
    apply();
}
