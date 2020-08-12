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
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
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
}

void ChartView::createChart(const QString& sTableName)
{
    _pChart->removeAllSeries();
    const QSqlDatabase& db = QSqlDatabase::database();
    QSqlQuery query(db);
    QString sQuery = "SELECT \"Wpm\", \"Spm\" FROM \"main\".\"%1\"";
    sQuery = sQuery.arg(sTableName);
    query.exec(sQuery);
    qreal rX = 0;

    auto pWpmSeries = new QtCharts::QLineSeries();
    pWpmSeries->setName("Wpm");
    QPen penWpm = pWpmSeries->pen();
    penWpm.setWidth(3);
    penWpm.setColor(Qt::blue);
    pWpmSeries->setPen(penWpm);

    auto pAo5WpmSeries = new QtCharts::QLineSeries();
    pAo5WpmSeries->setName("Ao5 (Wpm)");
    QPen penAo5Wpm = pAo5WpmSeries->pen();
    penAo5Wpm.setWidth(1);
    penAo5Wpm.setColor(Qt::red);
    pAo5WpmSeries->setPen(penAo5Wpm);

    auto pSpmSeries = new QtCharts::QLineSeries();
    pSpmSeries->setName("Spm");
    QPen penSpm = pSpmSeries->pen();
    penSpm.setWidth(3);
    penSpm.setColor(QColor("#add8e6"));
    pSpmSeries->setPen(penSpm);

    _pAo5SpmSeries = new QtCharts::QLineSeries();
    _pAo5SpmSeries->setName("Ao5 (Spm)");
    QPen penAo5Spm = _pAo5SpmSeries->pen();
    penAo5Spm.setWidth(1);
    penAo5Spm.setColor(QColor("#ffa500"));
    _pAo5SpmSeries->setPen(penAo5Spm);

    AverageOf ao5Wpm(5);
    AverageOf ao5Spm(5);
    while (query.next())
    {
        // Wpm
        ao5Wpm.popValue();
        const float fWpm = query.value(0).toFloat();
        pWpmSeries->append(rX, fWpm);
        if (ao5Wpm.pushValue(fWpm))
        {
            pAo5WpmSeries->append(rX, ao5Wpm.getAverage());
        }

        // Spm
        ao5Spm.popValue();
        const float fSpm = query.value(1).toFloat();
        pSpmSeries->append(rX, fSpm);
        if (ao5Spm.pushValue(fSpm))
        {
            _pAo5SpmSeries->append(rX, ao5Spm.getAverage());
        }

        rX++;
    }

    // Wpm
    _pChart->addSeries(pWpmSeries);
    _pChart->addSeries(pAo5WpmSeries);

    // Spm
    _pChart->addSeries(pSpmSeries);
    _pChart->addSeries(_pAo5SpmSeries);

    _pChart->createDefaultAxes();
    if (_pChart->axisX())
    {
        _pChart->axisX()->setLabelsVisible(false);
    }

    if (_pChart->axisY())
    {
        _pChart->axisY()->setMin(0);
    }
}

float ChartView::getLastAo5Spm() const
{
    float fResult = 0.f;
    if (_pAo5SpmSeries && _pAo5SpmSeries->count())
    {
        const QPointF& lastAo5Spm = _pAo5SpmSeries->at(_pAo5SpmSeries->count()-1);
        fResult = lastAo5Spm.y();
    }
    return fResult;
}
