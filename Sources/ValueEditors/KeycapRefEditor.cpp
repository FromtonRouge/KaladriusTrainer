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

#include "KeycapRefEditor.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QComboBox>
#include <QtCore/QAbstractItemModel>

KeycapRefEditor::KeycapRefEditor(QAbstractItemModel* pKeycapsRefModel, QWidget* pParent)
    : UserEditor(pParent)
    , _pComboBox(new QComboBox(this))
{
    _pComboBox->setModel(pKeycapsRefModel);
    connect(_pComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(onCurrentIndexChanged(QString)));
    auto pLayout = new QHBoxLayout();
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->addWidget(_pComboBox);
    setLayout(pLayout);
}

void KeycapRefEditor::setValue(const QVariant& value)
{
    _value = value;
    int iCurrentIndex = -1;
    auto pModel = _pComboBox->model();
    if (pModel->hasChildren())
    {
        iCurrentIndex = 0; // <no keycap> entry by default
        const auto& keycapRef = qvariant_cast<KeycapRef>(value);
        const auto& matches = pModel->match(pModel->index(0,0), Qt::DisplayRole, keycapRef.keycapId, 1, Qt::MatchExactly);
        if (!matches.isEmpty())
        {
            iCurrentIndex = matches.front().row();
        }
    }
    _pComboBox->setCurrentIndex(iCurrentIndex);
}

void KeycapRefEditor::onCurrentIndexChanged(const QString& sKeycapId)
{
    if (_pComboBox->currentIndex() == 0)
    {
        _value = qVariantFromValue(KeycapRef());
    }
    else
    {
        _value = qVariantFromValue(KeycapRef(sKeycapId));
    }
    apply();
}
