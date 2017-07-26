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

#include "FontEditor.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QFontDialog>
#include <QtWidgets/QFontComboBox>
#include <QtWidgets/QApplication>

FontEditor::FontEditor(QWidget* pParent)
    : QWidget(pParent)
{
    auto pLayout = new QHBoxLayout();
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSpacing(0);

    _pFontComboBox = new QFontComboBox(this);
    connect(_pFontComboBox, SIGNAL(currentFontChanged(QFont)), this, SLOT(onFontChanged(QFont)));
    _pFontComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pLayout->addWidget(_pFontComboBox);

    _pPushButton = new QPushButton(this);
    connect(_pPushButton, SIGNAL(clicked(bool)), this, SLOT(onPushButton()));
    _pPushButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    pLayout->addWidget(_pPushButton);
    setLayout(pLayout);
}

FontEditor::~FontEditor()
{

}

void FontEditor::setFont(const QFont& font)
{
    _font = font;
    QString sText(tr("...").arg(_font.family()));
    _pFontComboBox->setCurrentFont(_font);
    _pPushButton->setText(sText);
}

void FontEditor::onPushButton()
{
    QFontDialog dlg(_font, this);
    if (dlg.exec())
    {
        _font = dlg.selectedFont();

        // Send a user event, it can be interpreted in a custom QStyledItemDelegate to commit data
        QEvent userEvent(QEvent::User);
        QApplication::sendEvent(this, &userEvent);
    }
}

void FontEditor::onFontChanged(const QFont& font)
{
    _font = font;
    // Send a user event, it can be interpreted in a custom QStyledItemDelegate to commit data
    QEvent userEvent(QEvent::User);
    QApplication::sendEvent(this, &userEvent);
}
