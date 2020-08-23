// ======================================================================
// This file is a part of the KaladriusTrainer project
//
// Copyright (C) 2020  Vissale NEANG <fromtonrouge at gmail dot com>
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

FontEditor::FontEditor(QWidget* pParent)
    : UserEditor(pParent)
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

void FontEditor::setValue(const QVariant& value)
{
    _value = value;
    const QFont& font = _value.value<QFont>();
    QString sText(tr("...").arg(font.family()));
    _pFontComboBox->setCurrentFont(font);
    _pPushButton->setText(sText);
}

void FontEditor::onPushButton()
{
    QFontDialog dlg(_value.value<QFont>(), this);
    if (dlg.exec())
    {
        _value = dlg.selectedFont();
        apply();
    }
}

void FontEditor::onFontChanged(const QFont& font)
{
    _value = font;
    apply();
}
