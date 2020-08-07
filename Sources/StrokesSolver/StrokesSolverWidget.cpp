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

#include "StrokesSolverWidget.h"
#include "ui_StrokesSolverWidget.h"
#include <QtGui/QFontDatabase>
#include <QtCore/QSettings>
#include <QtCore/QSignalBlocker>
#include <QtCore/QFile>
#include <QtCore/QRandomGenerator>

StrokesSolverWidget::StrokesSolverWidget(QWidget* pParent)
    : QWidget(pParent)
    , _pUi(new Ui::StrokesSolverWidget())
{
    _pUi->setupUi(this);

    // We don't want to trigger the connected slots
    QSignalBlocker blockerFontSize(_pUi->comboBoxFontSize);
    QSignalBlocker blockerFontCombo(_pUi->fontComboBox);

    // Build the font standard sizes combo box
    _pUi->comboBoxFontSize->setEditable(true);
    const auto& sizes = QFontDatabase::standardSizes();
    for (int iSize : sizes)
    {
        _pUi->comboBoxFontSize->addItem(QString::number(iSize));
    }

    // Try to get a saved font if any
    const QVariant& variant = QSettings().value("strokesSolverFont", _pUi->textEdit->font());
    const QFont& wantedFont = qvariant_cast<QFont>(variant);
    _pUi->fontComboBox->setCurrentFont(wantedFont);
    _pUi->comboBoxFontSize->setCurrentIndex(sizes.indexOf(wantedFont.pointSize()));
    _pUi->textEdit->setFont(wantedFont);
    _pUi->pushButtonRestart->click();
}

StrokesSolverWidget::~StrokesSolverWidget()
{

}

void StrokesSolverWidget::on_fontComboBox_currentFontChanged(QFont font)
{
    const int iSize = _pUi->comboBoxFontSize->currentText().toInt();
    font.setPointSize(iSize);
    _pUi->textEdit->setFont(font);
    QSettings().setValue("strokesSolverFont", font);
}

void StrokesSolverWidget::on_comboBoxFontSize_currentTextChanged(const QString& sText)
{
    QFont font = _pUi->textEdit->font();
    font.setPointSize(sText.toInt());
    _pUi->textEdit->setFont(font);
    QSettings().setValue("strokesSolverFont", font);
}

void StrokesSolverWidget::on_pushButtonRestart_released()
{
    QFile file(":/Words/first200ShortWords.txt");
    QStringList vWords;
    if (file.open(QIODevice::ReadOnly))
    {
        const QString sWords = file.readAll();
        vWords = sWords.split('\n');
        file.close();
    }

    QStringList vText;
    if (!vWords.isEmpty())
    {
        // Get 600 words it should be enough
        for (int i=0; i<600; i++)
        {
            const int iIndex = QRandomGenerator::global()->bounded(vWords.size());
            vText << vWords[iIndex];
        }
    }

    _pUi->textEdit->restart(vText.join(" "));
}
