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
    _pChart->setTitle(tr("Wpm"));
    _pChart->legend()->hide();
    setChart(_pChart);
    setRenderHint(QPainter::Antialiasing);
}

void ChartView::display(const QString& sTableName)
{
    _pChart->removeAllSeries();
    const QSqlDatabase& db = QSqlDatabase::database();
    QSqlQuery query(db);
    QString sQuery = "SELECT \"Wpm\" FROM \"main\".\"%1\"";
    sQuery = sQuery.arg(sTableName);
    query.exec(sQuery);
    qreal rX = 0;
    auto pLineSeries = new QtCharts::QLineSeries();
    while (query.next())
    {
        const int iWpm = query.value(0).toInt();
        pLineSeries->append(rX++, qreal(iWpm));
    }
    _pChart->addSeries(pLineSeries);
    _pChart->createDefaultAxes();
}
