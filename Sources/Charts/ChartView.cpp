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
#include <QtCharts/QLineSeries>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtCore/QDebug>

ChartView::ChartView(QWidget* pParent)
    : QChartView(pParent)
    , _pChart(new QtCharts::QChart)
{
    setChart(_pChart);
    setRenderHint(QPainter::Antialiasing);
}

void ChartView::display(const QString& sTableName)
{
    _pChart->removeAllSeries();
    const QSqlDatabase& db = QSqlDatabase::database();
    QSqlQuery query(db);
    QString sQuery = "SELECT \"Wpm\",\"Accuracy\" FROM \"main\".\"%1\"";
    sQuery = sQuery.arg(sTableName);
    query.exec(sQuery);
    qreal rX = 0;

    auto pWpmSeries = new QtCharts::QLineSeries();
    pWpmSeries->setName("Wpm");
    QPen penWpm = pWpmSeries->pen();
    penWpm.setWidth(3);
    penWpm.setColor("#add8e6");
    pWpmSeries->setPen(penWpm);

    auto pAccuracySeries = new QtCharts::QLineSeries();
    pAccuracySeries->setName("Accuracy");
    QPen penAccuracy = pAccuracySeries->pen();
    penAccuracy.setWidth(3);
    penAccuracy.setColor("#ee82ee");
    pAccuracySeries->setPen(penAccuracy);

    while (query.next())
    {
        const int iWpm = query.value(0).toInt();
        const float fAccuracy = query.value(1).toInt();
        pWpmSeries->append(rX, qreal(iWpm));
        pAccuracySeries->append(rX, fAccuracy);
        rX++;
    }
    _pChart->addSeries(pWpmSeries);
    _pChart->addSeries(pAccuracySeries);
    _pChart->createDefaultAxes();
    if (_pChart->axisX())
    {
        _pChart->axisX()->setLabelsVisible(false);
    }
}
