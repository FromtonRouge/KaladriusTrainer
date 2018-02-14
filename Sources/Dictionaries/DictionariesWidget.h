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

#pragma once

#include "Keycaps/KeycapState.h"
#include <QtWidgets/QWidget>
#include <QtCore/QScopedPointer>
#include <QtCore/QBitArray>
#include <QtCore/QVector>

namespace Ui
{
    class DictionariesWidget;
}

class TheoryModel;
class DictionariesModel;
class QSortFilterProxyModel;
class QTimer;
class DictionariesWidget : public QWidget
{
    Q_OBJECT

signals:
    void dictionaryEntrySelected(const QString& sDictionaryName, const QVector<QBitArray>& possibleBits) const;
    void notifySpecialKeys(const HashSpecialKeysStates& specialKeysStates) const;

public:
    DictionariesWidget(QWidget* pParent = nullptr);
    ~DictionariesWidget();

    void setTheoryModel(TheoryModel* pModel);

protected slots:
    void on_lineEdit_textChanged();
    void on_comboBox_currentIndexChanged(int iCurrent);
    void applyFilter();
    void onDictionariesLoaded();
    void onCurrentChanged(const QModelIndex& current, const QModelIndex& previous);
    void onShowUnusedEntriesToggled(bool);

private:
    void saveExpandedIndexes();
    void restoreExpandedIndexes();

private:
    QScopedPointer<Ui::DictionariesWidget> _pUi;
    TheoryModel* _pTheoryModel;
    DictionariesModel* _pDictionariesModel;
    QSortFilterProxyModel* _pSortFilterDictionary;
    QSortFilterProxyModel* _pSortFilterNoEntries;
    QSortFilterProxyModel* _pSortFilterSearch;
    QSortFilterProxyModel* _pSortFilterAlphabeticalOrder;
    int _iPreviousDictionary;
    QString _sPreviousFilter;
    QTimer* _pTimer;
    bool _bBuildingDictionaries;
};
