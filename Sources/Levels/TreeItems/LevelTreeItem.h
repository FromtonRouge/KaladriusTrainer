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

#include "../../Tree/TreeItems/TreeItem.h"
#include <QtCore/QStringList>
#include <QtCore/QUuid>

class LevelTreeItem : public TreeItem
{
public:
    enum LevelType
    {
        TimedRandomWords,
        FreeText,
    };

public:
    LevelTreeItem(LevelType levelType,
                  const QUuid& uuid,
                  const QString& sLabel,
                  const QString& sWordsFilePath);
    ~LevelTreeItem();

    virtual int type() const override {return Level;}
    const QUuid& getUuid() const {return _uuid;}

    void loadWords(const QString& sWordsFilePath);
    const QStringList& getAllWords() const {return _words;}
    QStringList getWords(uint16_t uiMin, uint16_t uiMax) const;
    QStringList getCurrentWords() const;
    QStringList getRandomWords() const;

    uint16_t getProgression() const {return _uiProgression;}
    void setProgression(uint16_t uiProgression);
    float getProgressionPercentage() const;

    void loadProgression();
    void saveProgression() const;

    uint16_t getWPMNeededToProgress() const {return _uiWPMNeededToProgress;}

private:
    QUuid _uuid;
    LevelType _levelType;
    QStringList _words;
    uint16_t _uiRandomWordsCount = 1000;
    uint16_t _uiProgression = 5;
    uint16_t _uiWPMNeededToProgress = 50;
};
