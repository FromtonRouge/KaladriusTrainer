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

#pragma once

#include <QtCharts/QChartView>
#include <QtWidgets/QAction>
#include <QtGui/QColor>
#include <QtCore/QVector>

namespace QtCharts
{
    class QChart;
    class QLineSeries;
}

class ChartView : public QtCharts::QChartView
{
public:
    ChartView(QWidget* pParent = nullptr);
    void createChart(const QString& sLevelName = QString());

    float getLastAo5Spm() const;

protected:
    void contextMenuEvent(QContextMenuEvent* pEvent) override;

private:
    QtCharts::QLineSeries* createSeries(const QString& sName, const QColor& color, int iWidth = 3) const;

private:
    struct SeriesConfiguration
    {
        SeriesConfiguration(const QString& sName,
                            const QColor& color,
                            bool bDefaultVisibility,
                            const QColor& colorAo5 = QColor())
            : name(sName), color(color), bDefaultVisibility(bDefaultVisibility), colorAo5(colorAo5)
        {
            pAction = new QAction(sName);
            pAction->setCheckable(true);
        }

        QString name;
        QColor color;
        bool bDefaultVisibility = true;
        QColor colorAo5;
        QAction* pAction = nullptr;
    };

    QString _sTableName;
    QVector<SeriesConfiguration> _seriesConfigurations;
    QAction* _pActionAo5 = nullptr;
    QtCharts::QChart* _pChart = nullptr;
};
