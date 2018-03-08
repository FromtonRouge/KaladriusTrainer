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

#include "DictionariesWidget.h"
#include "DictionariesModel.h"
#include "Theories/Models/TheoryModel.h"
#include "ui_DictionariesWidget.h"
#include <QtWidgets/QMenu>
#include <QtCore/QSortFilterProxyModel>
#include <QtCore/QTimer>
#include <QtCore/QSettings>
#include <QtCore/QList>
#include <QtCore/QBitArray>

/**
 * Accept dictionary selected by the combo box.
 */
struct DictionaryFilter : public QSortFilterProxyModel
{
    DictionaryFilter(QObject* pParent) : QSortFilterProxyModel(pParent), selectedDictionary(0) {}
    virtual bool filterAcceptsRow(int iSourceRow, const QModelIndex& sourceParent) const override
    {
        if (selectedDictionary < 0)
        {
            return true;
        }

        if (!sourceParent.isValid())
        {
            return iSourceRow == selectedDictionary;
        }
        return true;
    }
    int selectedDictionary;
};

/**
 * Always accept top dictionaries elements.
 */
struct AcceptTopRowsFilter : public QSortFilterProxyModel
{
    AcceptTopRowsFilter(QObject* pParent) : QSortFilterProxyModel(pParent) {}
    virtual bool filterAcceptsRow(int iSourceRow, const QModelIndex& sourceParent) const override
    {
        if (!sourceParent.isValid())
        {
            return true;
        }
        return QSortFilterProxyModel::filterAcceptsRow(iSourceRow, sourceParent);
    }
};

/**
 * Remove <no_entry> elements.
 */
struct NoEntriesFilter : public QSortFilterProxyModel
{
    NoEntriesFilter(QObject* pParent) : QSortFilterProxyModel(pParent), enabled(true) {}
    virtual bool filterAcceptsRow(int iSourceRow, const QModelIndex& sourceParent) const override
    {
        const QModelIndex& sourceIndex = sourceModel()->index(iSourceRow, 0, sourceParent);
        if (enabled && sourceIndex.data().toString() == "<no_entry>")
        {
            return false;
        }
        return QSortFilterProxyModel::filterAcceptsRow(iSourceRow, sourceParent);
    }

    void setEnabled(bool bEnabled)
    {
        enabled = bEnabled;
        invalidateFilter();
    }

private:
    bool enabled;
};

DictionariesWidget::DictionariesWidget(QWidget* pParent)
    : QWidget(pParent)
    , _pUi(new Ui::DictionariesWidget())
    , _pTheoryModel(nullptr)
    , _pDictionariesModel(new DictionariesModel(this))
    , _pSortFilterDictionary(new DictionaryFilter(this))
    , _pSortFilterNoEntries(new NoEntriesFilter(this))
    , _pSortFilterSearch(new AcceptTopRowsFilter(this))
    , _pSortFilterAlphabeticalOrder(new QSortFilterProxyModel(this))
    , _iPreviousDictionary(-1)
    , _pTimer(new QTimer(this))
    , _bBuildingDictionaries(false)
    , _pActionShowUnusedEntries(nullptr)
{
    _pUi->setupUi(this);
    _pUi->treeView->setAlternatingRowColors(true);

    auto pMenu = new QMenu(this);
    _pActionShowUnusedEntries = pMenu->addAction(tr("Show unused entries"));
    _pActionShowUnusedEntries->setCheckable(true);
    connect(_pActionShowUnusedEntries, SIGNAL(toggled(bool)), this, SLOT(onShowUnusedEntriesToggled(bool)));

    _pUi->toolButtonSettings->setMenu(pMenu);
    _pSortFilterDictionary->setSourceModel(_pDictionariesModel);
    _pSortFilterNoEntries->setSourceModel(_pSortFilterDictionary);
    _pSortFilterSearch->setSourceModel(_pSortFilterNoEntries);
    _pSortFilterSearch->setFilterCaseSensitivity(Qt::CaseInsensitive);
    _pSortFilterAlphabeticalOrder->setSourceModel(_pSortFilterSearch);
    _pUi->treeView->setModel(_pSortFilterAlphabeticalOrder);
    _pUi->treeView->setSortingEnabled(true);
    connect(_pUi->treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(onCurrentChanged(QModelIndex, QModelIndex)));
    connect(_pTimer, SIGNAL(timeout()), this, SLOT(applyFilter()));
    _pTimer->setSingleShot(true);
    _pTimer->setInterval(500);
}

DictionariesWidget::~DictionariesWidget()
{
    if (_pUi->lineEdit->text().isEmpty())
    {
        if (_pUi->comboBox->currentIndex() == 0)
        {
            saveExpandedIndexes();
        }
    }
}

void DictionariesWidget::setTheoryModel(TheoryModel* pModel)
{
    _bBuildingDictionaries = true;

    if (_pTheoryModel)
    {
        _pTheoryModel->disconnect(this);
    }

    _pTheoryModel = pModel;

    if (_pTheoryModel)
    {
        connect(_pTheoryModel, SIGNAL(dictionariesLoaded()), this, SLOT(onDictionariesLoaded()));
        onDictionariesLoaded();
    }

    _bBuildingDictionaries = false;
}

void DictionariesWidget::on_lineEdit_textChanged()
{
    _pTimer->start();
}

void DictionariesWidget::on_comboBox_currentIndexChanged(int iCurrent)
{
    QSettings settings;
    const QString sSettingPath = QString("%1/selectedDictionary").arg(objectName());
    settings.setValue(sSettingPath, iCurrent);

    const auto pFilter = static_cast<DictionaryFilter*>(_pSortFilterDictionary);
    pFilter->selectedDictionary = iCurrent - 1;

    const auto pModel = _pUi->treeView->model();

    const QModelIndex& sourceIndexDictionaries = _pTheoryModel->getDictionariesIndex();
    Q_ASSERT(sourceIndexDictionaries.isValid());

    if (pFilter->selectedDictionary < 0)
    {
        _pUi->treeView->setRootIsDecorated(true);
        pFilter->invalidate();
        _pUi->treeView->setRootIndex(QModelIndex());
        restoreExpandedIndexes();
    }
    else
    {
        _pUi->treeView->setRootIsDecorated(false);
        if (_iPreviousDictionary < 0)
        {
            saveExpandedIndexes();
        }

        pFilter->invalidate();
        _pUi->treeView->setRootIndex(pModel->index(0, 0));
        _pUi->treeView->expandAll();
    }
    _pUi->treeView->resizeColumnToContents(0);
    _iPreviousDictionary = pFilter->selectedDictionary;
}

void DictionariesWidget::applyFilter()
{
    // Save expand states if the filter was empty
    if (_sPreviousFilter.isEmpty())
    {
        if (_pUi->comboBox->currentIndex() == 0)
        {
            saveExpandedIndexes();
        }
    }

    const QString& sFilter = _pUi->lineEdit->text();
    _pSortFilterSearch->setFilterRegExp(sFilter);
    _pSortFilterSearch->invalidate();
    _pUi->treeView->expandAll();

    // Restore expand states if the filter becomes empty
    if (sFilter.isEmpty())
    {
        if (_pUi->comboBox->currentIndex() == 0)
        {
            restoreExpandedIndexes();
        }
    }

    _sPreviousFilter = sFilter;
}

void DictionariesWidget::onDictionariesLoaded()
{
    QSettings settings;
    const QString sSettingPath = QString("%1/selectedDictionary").arg(objectName());
    int iSelectedDictionary = qMax(settings.value(sSettingPath, 0).toInt(), 0);

    QStringList dictionaries;
    dictionaries << tr("All Dictionaries");
    const QModelIndex& indexDictionaries = _pTheoryModel->getDictionariesIndex();
    Q_ASSERT(indexDictionaries.isValid());
    const int iRows = _pTheoryModel->rowCount(indexDictionaries);
    for (int iRow = 0; iRow < iRows; ++iRow)
    {
        const QModelIndex& index = _pTheoryModel->index(iRow, 0, indexDictionaries);
        dictionaries << index.data().toString();
    }
    _pUi->comboBox->clear();
    _pUi->comboBox->addItems(dictionaries);

    const bool bShowUnusedEntries = settings.value(QString("%1/showUnusedEntries").arg(objectName()), false).toBool();
    _pActionShowUnusedEntries->setChecked(bShowUnusedEntries);

    _pDictionariesModel->setSourceModel(_pTheoryModel);
    _pSortFilterAlphabeticalOrder->sort(0);
    _pUi->treeView->resizeColumnToContents(0);

    // Out of range check
    if (iSelectedDictionary >= _pUi->comboBox->count())
    {
        iSelectedDictionary = 0;
    }

    _pUi->comboBox->setCurrentIndex(iSelectedDictionary);

    if (iSelectedDictionary == 0)
    {
        restoreExpandedIndexes();
    }
}

void DictionariesWidget::onCurrentChanged(const QModelIndex& current, const QModelIndex& previous)
{
    if (current.isValid())
    {
        // Get both the name and value indexes
        const QModelIndex& indexName = current.sibling(current.row(), 0);
        const QModelIndex& indexValue = current.sibling(current.row(), 1);
        const QVariant& variant = indexValue.data(InputKeyBitsRole);
        if (variant.isValid())
        {
            const QBitArray& bits = variant.toBitArray();
            const QModelIndex& sourceIndexName = indexName.data(TreeItemIndexRole).toModelIndex();
            if (sourceIndexName.isValid())
            {
                // Get the dictionary name for that entry
                const QString& sDictionaryName = sourceIndexName.parent().parent().data().toString();
                if (!sDictionaryName.isEmpty())
                {
                    // Send signal to the keyboard model
                    emit dictionaryEntrySelected(sDictionaryName, {bits});

                    // Send the mandatory special key for this dictionary (if any)
                    const QString& sMandatorySpecialKey = _pTheoryModel->getCachedDictionaries()[sDictionaryName].sMandatorySpecialKey;
                    if (!sMandatorySpecialKey.isEmpty())
                    {
                        emit notifySpecialKeys({{sMandatorySpecialKey, true}});
                    }
                }
            }
        }
    }
}

void DictionariesWidget::onShowUnusedEntriesToggled(bool bChecked)
{
    auto pNoEntriesFilter = static_cast<NoEntriesFilter*>(_pSortFilterNoEntries);
    pNoEntriesFilter->setEnabled(!bChecked);
    QSettings().setValue(QString("%1/showUnusedEntries").arg(objectName()), bChecked);
}

void DictionariesWidget::saveExpandedIndexes()
{
    if (_bBuildingDictionaries)
    {
        return;
    }

    QSettings settings;
    const QString sSettingPath = QString("%1/expandedIndexes").arg(objectName());
    const QRegExp& regexp = _pSortFilterSearch->filterRegExp();
    if (regexp.isEmpty())
    {
        const auto* const pModel = _pUi->treeView->model();
        const int iRows = pModel->rowCount();
        if (iRows)
        {
            QList<QVariant> expandedIndexes;
            for (int iRow = 0; iRow < iRows; ++iRow)
            {
                if (_pUi->treeView->isExpanded(pModel->index(iRow, 0)))
                {
                    expandedIndexes << iRow;
                }
            }
            settings.setValue(sSettingPath, expandedIndexes);
        }
    }
}

void DictionariesWidget::restoreExpandedIndexes()
{
    _pUi->treeView->collapseAll();
    QSettings settings;
    const QString sSettingPath = QString("%1/expandedIndexes").arg(objectName());
    const auto& expandedIndexes = settings.value(sSettingPath).toList();
    const auto* const pModel = _pUi->treeView->model();
    for (const auto& variant : expandedIndexes)
    {
        const int iIndex = variant.toInt();
        if (iIndex < pModel->rowCount())
        {
            _pUi->treeView->expand(pModel->index(iIndex, 0));
        }
    }
}
