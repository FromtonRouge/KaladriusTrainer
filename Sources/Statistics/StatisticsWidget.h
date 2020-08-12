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

#include <QtWidgets/QWidget>
#include <QtCore/QStringList>
#include <QtCore/QHash>
#include <QtCore/QPointer>
#include <QtCore/QModelIndexList>

class StatisticsView;
class QComboBox;
class QItemSelection;
namespace QtCharts
{
    class QLineSeries;
}

class StatisticsWidget : public QWidget
{
    Q_OBJECT

public:
    StatisticsWidget(QWidget *parent = nullptr);
    ~StatisticsWidget();

public slots:
    void onChartSelectorCurrentIndexChanged(int iCurrentIndex);
    void addSelectedLevels(const QItemSelection& selectedLevels);
    void removeDeselectedLevels(const QItemSelection& deselectedLevels);

private:
    QModelIndexList getLevelsFromSelection(const QItemSelection& selection) const;
    void addSeries(const QModelIndex& indexLevel);
    void removeSeries(const QModelIndex& indexLevel);
    QString getCurrentDataType() const;

private:
    StatisticsView* _pStatisticsView = nullptr;
    QComboBox* _pChartSelector = nullptr;
    QHash<QModelIndex, QPointer<QtCharts::QLineSeries>> _series;
};
