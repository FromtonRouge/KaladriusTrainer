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

#include "ChartView.h"
#include "../Utils/AverageOf.h"
#include <QtCharts/QLineSeries>
#include <QtWidgets/QGraphicsLayout>
#include <QtWidgets/QMenu>
#include <QtGui/QCursor>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtCore/QSettings>
#include <QtCore/QDebug>

ChartView::ChartView(QWidget* pParent)
    : QChartView(pParent)
    , _pChart(new QtCharts::QChart)
{
    _pChart->setMargins(QMargins());
    _pChart->setBackgroundRoundness(0);
    _pChart->layout()->setContentsMargins(0, 0, 0, 0);
    setChart(_pChart);
    setRenderHint(QPainter::Antialiasing);

    _seriesConfigurations << SeriesConfiguration("Wpm", Qt::blue, true, Qt::red);
    _seriesConfigurations << SeriesConfiguration("Spm", "#add8e6", true, "#ffa500");
    _seriesConfigurations << SeriesConfiguration("Accuracy", "#ee82ee", false);
    _seriesConfigurations << SeriesConfiguration("Viscosity", "#2e8b57", false);
    _seriesConfigurations << SeriesConfiguration("Progress", Qt::darkMagenta, false);

    _pActionAo5 = new QAction("Average of 5");
    _pActionAo5->setCheckable(true);

    QSettings settings;
    for (const auto& config : _seriesConfigurations)
    {
        const bool bChecked = settings.value(QString("ChartView/%1").arg(config.name), config.bDefaultVisibility).toBool();
        config.pAction->setChecked(bChecked);
    }
    _pActionAo5->setChecked(settings.value("ChartView/ao5", false).toBool());
}

void ChartView::createChart(const QString& sLevelName)
{
    if (!sLevelName.isEmpty())
    {
        _sTableName = sLevelName;
    }

    if (_sTableName.isEmpty())
    {
        return;
    }

    _pChart->removeAllSeries();

    const QSqlDatabase& db = QSqlDatabase::database();
    QSqlQuery query(db);

    for (const auto& config : _seriesConfigurations)
    {
        if (!config.pAction->isChecked())
        {
            continue;
        }

        const QString& sDataType = config.name;
        QString sQuery = "SELECT \"%1\" FROM \"main\".\"%2\"";
        sQuery = sQuery.arg(sDataType).arg(_sTableName);
        qreal rX = 0;

        auto pSeries = createSeries(sDataType, config.color, 2);

        QtCharts::QLineSeries* pAo5Series = nullptr;
        if (_pActionAo5->isChecked() && config.colorAo5.isValid())
        {
            pAo5Series = createSeries(QString("Ao5 (%1)").arg(sDataType), config.colorAo5, 4);
        }

        AverageOf ao5(5);
        query.exec(sQuery);
        while (query.next())
        {
            ao5.popValueBack();
            const float fValue = query.value(0).toFloat();
            pSeries->append(rX, fValue);
            if (pAo5Series && ao5.pushValueFront(fValue))
            {
                pAo5Series->append(rX, ao5.getAverage());
            }

            rX++;
        }

        _pChart->addSeries(pSeries);

        if (pAo5Series)
        {
            _pChart->addSeries(pAo5Series);
        }
    }

    _pChart->createDefaultAxes();
    const auto& axes = _pChart->axes();
    if (axes.isEmpty() == false)
    {
        axes[0]->setLabelsVisible(false);
        axes[1]->setMin(0);
    }
}

void ChartView::contextMenuEvent(QContextMenuEvent*)
{
    QMenu menu;

    for (const auto& config : _seriesConfigurations)
    {
        menu.addAction(config.pAction);
    }

    menu.addSeparator();
    menu.addAction(_pActionAo5);

    QAction* pResult = menu.exec(QCursor::pos());
    if (pResult)
    {
        QSettings settings;
        for (const auto& config : _seriesConfigurations)
        {
            settings.setValue(QString("ChartView/%1").arg(config.name), config.pAction->isChecked());
        }
        settings.setValue("ChartView/ao5", _pActionAo5->isChecked());

        createChart();
    }
}

QtCharts::QLineSeries* ChartView::createSeries(const QString& sName, const QColor& color, int iWidth) const
{
    auto pLineSeries = new QtCharts::QLineSeries();
    pLineSeries->setName(sName);
    QPen pen = pLineSeries->pen();
    pen.setWidth(iWidth);
    pen.setColor(color);
    pLineSeries->setPen(pen);
    return pLineSeries;
}
