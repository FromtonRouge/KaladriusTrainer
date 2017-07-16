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

#include "KeyboardGraphicsView.h"
#include <QtWidgets/QGraphicsScene>
#include <QtSvg/QGraphicsSvgItem>
#include <QtSvg/QSvgRenderer>
#include <QtDebug>

KeyboardGraphicsView::KeyboardGraphicsView(QWidget* pParent)
    : QGraphicsView(pParent)
{
    auto pScene = new QGraphicsScene(this);
    _pSvgRenderer = new QSvgRenderer(QLatin1String(":/Svgs/ergodox.svg"), pScene);

    QStringList elementsToLoad =
    {
        "g100",
        "g110",
        "g120",
        "g130",
        "g140",
        "g150",
        "g160",
        "g170",
        "g180",
        "g190",
        "g200",
        "g210",
        "g220",
        "g230",
        "g240",
        "g250",
        "g260",
        "g270",
        "g280",
        "g290",
        "g300",
        "g310",
        "g320",
        "g330",
        "g340",
        "g350",
        "g360",
        "g370",
        "g380",
        "g390",
        "g40",
        "g400",
        "g410",
        "g420",
        "g430",
        "g440",
        "g450",
        "g460",
        "g470",
        "g480",
        "g490",
        "g50",
        "g500",
        "g510",
        "g520",
        "g530",
        "g540",
        "g550",
        "g560",
        "g570",
        "g580",
        "g590",
        "g60",
        "g600",
        "g610",
        "g620",
        "g630",
        "g640",
        "g650",
        "g660",
        "g670",
        "g680",
        "g690",
        "g70",
        "g700",
        "g710",
        "g720",
        "g730",
        "g740",
        "g750",
        "g760",
        "g770",
        "g780",
        "g790",
        "g80",
        "g90"
    };

    for (const auto& sElementId : elementsToLoad)
    {
        auto pSvgItem = new QGraphicsSvgItem();
        pSvgItem->setFlag(QGraphicsItem::ItemIsSelectable);
        pSvgItem->setSharedRenderer(_pSvgRenderer);
        pSvgItem->setElementId(sElementId);
        const auto& r = _pSvgRenderer->boundsOnElement(sElementId);
        pSvgItem->setPos(r.topLeft());
        pScene->addItem(pSvgItem);
    }

    setScene(pScene);
}

KeyboardGraphicsView::~KeyboardGraphicsView()
{
}
