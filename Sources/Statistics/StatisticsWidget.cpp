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

#include "StatisticsWidget.h"
#include "StatisticsView.h"
#include "../Tree/Models/ItemDataRole.h"
#include "../Levels/Models/LevelsModel.h"
#include "../Levels/TreeItems/LevelTreeItem.h"
#include "../Main/Application.h"
#include <QtCharts/QLineSeries>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtWidgets/QGraphicsLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSizePolicy>
#include <QtCore/QItemSelection>
#include <QtCore/QDebug>

StatisticsWidget::StatisticsWidget(QWidget *parent)
    : QWidget(parent)
    , _pStatisticsView(new StatisticsView(this))
    , _pChartSelector(new QComboBox(this))
{
    auto pLayout = new QVBoxLayout;

    auto pHLayout = new QHBoxLayout();
    pHLayout->addWidget(new QLabel(tr("Data"),this));
    _pChartSelector->addItem(QIcon(), tr("Wpm"));
    _pChartSelector->addItem(QIcon(), tr("Spm"));
    _pChartSelector->addItem(QIcon(), tr("Accuracy"));
    _pChartSelector->addItem(QIcon(), tr("Viscosity"));
    _pChartSelector->addItem(QIcon(), tr("Progress"));
    connect(_pChartSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &StatisticsWidget::onChartSelectorCurrentIndexChanged);
    pHLayout->addWidget(_pChartSelector);
    pHLayout->addStretch();

    pLayout->addLayout(pHLayout);

    _pStatisticsView->setRenderHint(QPainter::Antialiasing);
    pLayout->addWidget(_pStatisticsView);

    setLayout(pLayout);

    onChartSelectorCurrentIndexChanged(_pChartSelector->currentIndex());
}

StatisticsWidget::~StatisticsWidget()
{

}

void StatisticsWidget::onChartSelectorCurrentIndexChanged(int)
{
    const QList<QModelIndex>& levels = _series.keys();
    if (!levels.isEmpty())
    {
        QtCharts::QChart* pChart = _pStatisticsView->chart();
        if (pChart)
        {
            pChart->removeAllSeries();
            _series.clear();

            for (const QModelIndex& indexLevel : levels)
            {
                addSeries(indexLevel);
            }
        }
    }
}

void StatisticsWidget::addSelectedLevels(const QItemSelection& selectedLevels)
{
    const QModelIndexList& levels = getLevelsFromSelection(selectedLevels);
    for (const QModelIndex& indexLevel : levels)
    {
        addSeries(indexLevel);
    }
}

void StatisticsWidget::removeDeselectedLevels(const QItemSelection& deselectedLevels)
{
    const QModelIndexList& levels = getLevelsFromSelection(deselectedLevels);
    for (const QModelIndex& indexLevel : levels)
    {
        removeSeries(indexLevel);
    }
}

QModelIndexList StatisticsWidget::getLevelsFromSelection(const QItemSelection& selection) const
{
    QModelIndexList levels;
    for (const QModelIndex& selectedIndex : selection.indexes())
    {
        if (selectedIndex.data(TreeItemTypeRole).toInt() == TreeItem::Level)
        {
            levels << selectedIndex;
        }
    }
    return levels;
}

void StatisticsWidget::addSeries(const QModelIndex& indexLevel)
{
    auto pCurrentChart = _pStatisticsView->chart();

    if (!pCurrentChart)
    {
        pCurrentChart = new QtCharts::QChart;
        pCurrentChart->legend()->hide();
        _pStatisticsView->setChart(pCurrentChart);
    }

    const QString& sDataType = getCurrentDataType();
    const QSqlDatabase& db = QSqlDatabase::database();

    QSqlQuery query(db);
    QString sQuery = "SELECT \"%1\" FROM \"main\".\"%2\"";
    sQuery = sQuery.arg(sDataType).arg(indexLevel.data(LevelTableNameRole).toString());
    query.exec(sQuery);

    auto pSeries = new QtCharts::QLineSeries();
    _series[indexLevel] = pSeries;
    const QString sSeriesName = QString("%1 (%2)").arg(sDataType).arg(indexLevel.data(Qt::DisplayRole).toString());
    pSeries->setName(sSeriesName);

    qreal rX = 0;
    while (query.next())
    {
        const float fValue = query.value(0).toFloat();
        pSeries->append(rX, qreal(fValue));
        rX++;
    }

    pCurrentChart->addSeries(pSeries);

    pCurrentChart->createDefaultAxes();

    const auto& axes = pCurrentChart->axes();
    if (axes.isEmpty() == false)
    {
        axes[0]->setLabelsVisible(false);
        axes[1]->setMin(0);
    }
}

void StatisticsWidget::removeSeries(const QModelIndex& indexLevel)
{
    auto pCurrentChart = _pStatisticsView->chart();
    if (pCurrentChart)
    {
        auto it = _series.find(indexLevel);
        if (it != _series.end())
        {
            QtCharts::QLineSeries* pSeries = it.value().data();
            if (pSeries)
            {
                pCurrentChart->removeSeries(pSeries);
                _series.remove(indexLevel);
            }
        }
    }
}

QString StatisticsWidget::getCurrentDataType() const
{
    return _pChartSelector->currentText();
}
