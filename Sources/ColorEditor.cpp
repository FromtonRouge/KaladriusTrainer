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

#include "ColorEditor.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QColorDialog>

ColorEditor::ColorEditor(QWidget* pParent)
    : UserEditor(pParent)
{
    auto pLayout = new QHBoxLayout();
    pLayout->setContentsMargins(0, 0, 0, 0);
    _pPushButton = new QPushButton(this);
    _pPushButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(_pPushButton, SIGNAL(clicked(bool)), this, SLOT(onPushButton()));
    pLayout->addWidget(_pPushButton);
    setLayout(pLayout);
}

ColorEditor::~ColorEditor()
{
}

void ColorEditor::setValue(const QVariant& value)
{
    _value = value;
    const QColor& color = _value.value<QColor>();
    QString sColorname;
    if (color.isValid())
    {
        sColorname = color.name();
        _pPushButton->setStyleSheet(QString("QPushButton { background-color: %1; color: %2}").arg(color.name(QColor::HexArgb)).arg(color.lightnessF() > 0.5 ? "black" : "white"));
    }
    _pPushButton->setText(tr("%1Pick color...").arg(sColorname.isEmpty() ? "" : tr("[%1] ").arg(sColorname)));
}

void ColorEditor::onPushButton()
{
    QColorDialog dlg(_value.value<QColor>(), this);
    if (dlg.exec())
    {
        _value = dlg.selectedColor();
        apply();
    }
}
