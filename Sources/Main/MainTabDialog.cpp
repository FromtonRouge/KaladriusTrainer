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

#include "MainTabDialog.h"
#include "Application.h"
#include "Streams/Iostream.h"
#include "ui_MainTabDialog.h"
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QTabBar>
#include <QtCore/QSettings>

MainTabDialog::MainTabDialog(QWidget* pParent)
    : QDialog(pParent)
    , _pUi(new Ui::MainTabDialog)
{
    _pUi->setupUi(this);
    setWindowFlags(Qt::Window);

    connect(qApp, SIGNAL(logs(QString, int)), this, SLOT(logs(QString,int)));

    // Restore geometry and window state
    QSettings settings;
    const QByteArray& byteArray = settings.value("MainTabDialog/geometry").toByteArray();
    if (byteArray.isEmpty())
    {
        // First time
        showMaximized();
    }
    else
    {
        restoreGeometry(byteArray);
        if (isMaximized())
        {
            // Fix for restoring properly a maximized window before a show()
            setGeometry(QApplication::desktop()->availableGeometry(this));
        }
        show();
    }

    // Restore main windows geometries and states
    const auto& vMainWindows = findChildren<QMainWindow*>();
    for (auto pMainWindow : vMainWindows)
    {
        const QString& sSettingPath = QString("%1/windowState").arg(pMainWindow->objectName());
        pMainWindow->restoreState(settings.value(sSettingPath).toByteArray());
    }

    // Restore tab order
    const QStringList& orderedTabs = settings.value("orderedTabs").toStringList();
    if (!orderedTabs.isEmpty())
    {
        int iTabIndex = 0;
        auto pTabBar = _pUi->tabWidget->tabBar();
        for (const QString& sObjectName : orderedTabs)
        {
            auto pWidget = _pUi->tabWidget->findChild<QWidget*>(sObjectName);
            if (pWidget)
            {
                const int iCurrentIndex = _pUi->tabWidget->indexOf(pWidget);
                if (iTabIndex < pTabBar->count())
                {
                    pTabBar->moveTab(iCurrentIndex, iTabIndex++);
                }
            }
        }
    }

    // Restore current tab index
    const int iCurrentTabIndex = settings.value("currentTabIndex", 0).toInt();
    _pUi->tabWidget->setCurrentIndex(iCurrentTabIndex);
}

MainTabDialog::~MainTabDialog()
{
}

QTabWidget*MainTabDialog::getTabWidget() const
{
    return _pUi->tabWidget;
}

void MainTabDialog::logs(const QString& sText, int iWarningLevel)
{
    // Write logs to the text edit with the appropriate color
    QTextCursor cursor(_pUi->textEditLogs->textCursor());
    QTextCharFormat format(cursor.charFormat());
    format.setForeground(QBrush(iWarningLevel == 0 ? Qt::black : Qt::red));
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(sText, format);
    _pUi->textEditLogs->setTextCursor(cursor);

    if (iWarningLevel == 2)
    {
        _pUi->tabWidget->setCurrentWidget(_pUi->tabLogs);
    }
}

void MainTabDialog::closeEvent(QCloseEvent* pEvent)
{
    QSettings settings;
    settings.setValue("MainTabDialog/geometry", saveGeometry());

    // Save all main windows states
    const auto& vMainWindows = findChildren<QMainWindow*>();
    for (auto pMainWindow : vMainWindows)
    {
        const QString& sSettingPath = QString("%1/windowState").arg(pMainWindow->objectName());
        settings.setValue(sSettingPath, pMainWindow->saveState());
    }

    // Save tab order
    QStringList orderedTabs;
    const int iTabCount = _pUi->tabWidget->count();
    for (int iTab = 0; iTab < iTabCount; ++iTab)
    {
        QWidget* pWidget = _pUi->tabWidget->widget(iTab);
        Q_ASSERT(!pWidget->objectName().isEmpty());
        orderedTabs << pWidget->objectName();
    }
    settings.setValue("orderedTabs", orderedTabs);

    // Save current tab
    settings.setValue("currentTabIndex", _pUi->tabWidget->currentIndex());

    QDialog::closeEvent(pEvent);
}
