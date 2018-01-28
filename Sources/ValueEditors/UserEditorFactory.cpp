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

#include "UserEditorFactory.h"
#include "ColorEditor.h"
#include "FontEditor.h"
#include "Main/Application.h"
#include "KeycapRefEditor.h"
#include "Keyboards/KeyboardModel.h"
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QStandardItemEditorCreator>
#include <QtGui/QStandardItemModel>
#include <cfloat>

UserEditorFactory::UserEditorFactory()
    : _pKeycapsRefModel(new QStandardItemModel())
{
    registerEditor(QVariant::Color, new QStandardItemEditorCreator<ColorEditor>());
    registerEditor(QVariant::Font, new QStandardItemEditorCreator<FontEditor>());
}

UserEditorFactory::~UserEditorFactory()
{

}

QWidget* UserEditorFactory::createEditor(int iUserType, QWidget* pParent) const
{
    switch (iUserType)
    {
    case QVariant::String:
        {
            auto pLineEdit = new QLineEdit(pParent);
            pLineEdit->setFrame(false);
            return pLineEdit;
        }
    case QVariant::Double:
        {
            auto pDoubleSpinBox = new QDoubleSpinBox(pParent);
            pDoubleSpinBox->setFrame(false);
            pDoubleSpinBox->setSingleStep(0.1);
            pDoubleSpinBox->setMinimum(-DBL_MAX);
            pDoubleSpinBox->setMaximum(DBL_MAX);
            QObject::connect(pDoubleSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                             [=]() {
                QEvent userEvent(QEvent::User);
                QApplication::sendEvent(pDoubleSpinBox, &userEvent);
            });
            return pDoubleSpinBox;
        }
    default:
        {
            if (iUserType == qMetaTypeId<KeycapRef>())
            {
                auto pKeyboardModel = qApp->getKeyboardModel();
                if (pKeyboardModel)
                {
                    const QModelIndex& indexKeycaps = pKeyboardModel->getKeycapsIndex();
                    if (indexKeycaps.isValid())
                    {
                        if (!_pKeycapsRefModel->hasChildren())
                        {
                            _pKeycapsRefModel->appendRow(new QStandardItem(QObject::tr("<no keycap>")));
                            const int iRows = pKeyboardModel->rowCount(indexKeycaps);
                            for (int iRow = 0; iRow < iRows; ++iRow)
                            {
                                const QModelIndex& indexKeycap = pKeyboardModel->index(iRow, 0, indexKeycaps);
                                const QString& sKeycapId = indexKeycap.data(Qt::DisplayRole).toString();
                                _pKeycapsRefModel->appendRow(new QStandardItem(sKeycapId));
                            }
                        }

                        return new KeycapRefEditor(_pKeycapsRefModel.data(), pParent);
                    }
                }
            }
            else
            {
                return QItemEditorFactory::createEditor(iUserType, pParent);
            }
        }
    }
    return nullptr;
}
