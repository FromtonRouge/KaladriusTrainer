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
#include <QtWidgets/QApplication>

ColorEditor::ColorEditor(QWidget* pParent)
    : QWidget(pParent)
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

void ColorEditor::setColor(const QColor& color)
{
    _color = color;
    _pPushButton->setText(color.name());
    _pPushButton->setStyleSheet(QString("QPushButton { background-color: %1; color: %2}").arg(_color.name(QColor::HexArgb)).arg(_color.lightnessF() > 0.5 ? "black" : "white"));
}

void ColorEditor::onPushButton()
{
    QColorDialog dlg(_color, this);
    if (dlg.exec())
    {
        _color = dlg.selectedColor();

        // Send a user event, it can be interpreted in a custom QStyledItemDelegate to commit data
        QEvent userEvent(QEvent::User);
        QApplication::sendEvent(this, &userEvent);
    }
}
