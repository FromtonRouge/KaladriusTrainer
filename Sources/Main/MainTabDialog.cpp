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

#include "MainTabDialog.h"
#include "Application.h"
#include "ui_MainTabDialog.h"
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QDesktopWidget>
#include <QtCore/QSettings>

MainTabDialog::MainTabDialog(QWidget* pParent)
    : QDialog(pParent)
    , _pUi(new Ui::MainTabDialog)
{
    _pUi->setupUi(this);
    setWindowFlags(Qt::Window);

    connect(qApp, SIGNAL(logs(QString, int)), this, SLOT(logs(QString,int)));

    showMaximized();

    // Restore geometry and window state
    QSettings settings;
    restoreGeometry(settings.value("MainTabDialog/geometry").toByteArray());
    if (isMaximized())
    {
        setGeometry(QApplication::desktop()->availableGeometry(this));
    }

    // Restore main windows geometries and states
    _pUi->mainWindowLearningMode->restoreState(settings.value("windowState").toByteArray());
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
        //_pUi->dockWidgetLogs->setVisible(true);
        //_pUi->dockWidgetLogs->raise();
    }
}

void MainTabDialog::closeEvent(QCloseEvent* pEvent)
{
    QSettings settings;
    settings.setValue("MainTabDialog/geometry", saveGeometry());
    auto pMainWindow = qobject_cast<QMainWindow*>(_pUi->tabWidget->widget(0)->layout()->itemAt(0)->widget());
    if (pMainWindow)
    {
        settings.setValue("windowState", pMainWindow->saveState());
    }
    QDialog::closeEvent(pEvent);
}
