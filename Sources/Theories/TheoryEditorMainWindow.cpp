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

#include "Theories/TheoryEditorMainWindow.h"
#include "Theories/Models/TheoryModel.h"
#include "Theories/Models/UndoableTheoryModel.h"
#include "Keyboards/Models/KeyboardModel.h"
#include "Keyboards/KeyboardGraphicsScene.h"
#include "Main/Application.h"
#include "Dictionaries/DictionaryParser.h"
#include "Serialization/Serialize.h"
#include "Streams/Iostream.h"
#include "ui_TheoryEditorMainWindow.h"
#include <QtWidgets/QUndoStack>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QTabWidget>
#include <QtCore/QSettings>
#include <QtCore/QTemporaryFile>
#include <QtCore/QFile>

TheoryEditorMainWindow::TheoryEditorMainWindow(QWidget *parent)
    : MainTabWindow(parent)
    , _pUi(new Ui::TheoryEditorMainWindow)
{
    _pUi->setupUi(this);
    setWindowTitle(tr("Theory Editor[*]"));

    // Init undo stack
    auto pUndoStack = qApp->getUndoableTheoryModel()->getUndoStack();
    _pUi->listViewUndo->setStack(pUndoStack);
    connect(pUndoStack, SIGNAL(cleanChanged(bool)), this, SLOT(onUndoCleanChanged(bool)));
    _pUi->menuEdit->addAction(createUndoAction(pUndoStack));
    _pUi->menuEdit->addAction(createRedoAction(pUndoStack));

    _pUi->treeViewTheory->setModel(qApp->getUndoableTheoryModel());

    // Load last .theory file if any
    QSettings settings;
    const QString& sLastTheory = settings.value("lastTheory").toString();
    if (!sLastTheory.isEmpty() && QFile::exists(sLastTheory))
    {
        loadTheory(sLastTheory);
    }
    else
    {
        _pUi->actionLoad_Default->trigger();
    }

    auto pTheoryModel = qApp->getTheoryModel();
    auto pKeyboardModel = qApp->getKeyboardModel();
    _pUi->widgetDictionaries->setTheoryModel(pTheoryModel);
    connect(_pUi->widgetDictionaries, SIGNAL(dictionaryEntrySelected(QString, QVector<QBitArray>)), pKeyboardModel, SLOT(selectLinkedKeys(QString, QVector<QBitArray>)));
    connect(_pUi->widgetDictionaries, SIGNAL(notifySpecialKeys(HashSpecialKeysStates)), pKeyboardModel, SLOT(selectLinkedSpecialKeys(HashSpecialKeysStates)));
    connect(pKeyboardModel, SIGNAL(keyboardLoaded()), _pUi->graphicsView, SLOT(fitKeyboardInView()));
    _pUi->graphicsView->setScene(qApp->getKeyboardGraphicsScene());
}

TheoryEditorMainWindow::~TheoryEditorMainWindow()
{

}

void TheoryEditorMainWindow::on_actionLoad_triggered()
{
    QSettings settings;
    const QString& sLastTheoryFile = settings.value("lastTheory").toString();
    const QString& sTheoryFile = QFileDialog::getOpenFileName(this, tr("Theory"), sLastTheoryFile, "*.theory");
    if (!sTheoryFile.isEmpty())
    {
        loadTheory(sTheoryFile);
    }
}

void TheoryEditorMainWindow::on_actionLoad_Default_triggered()
{
    QFile resourceFile(":/Theories/ProgrammerSteno.theory");
    auto pTemporaryFile = QTemporaryFile::createNativeFile(resourceFile);
    if (pTemporaryFile)
    {
        loadTheory(pTemporaryFile->fileName(), ClearSettings);
    }
}

void TheoryEditorMainWindow::on_actionSave_triggered()
{
    QSettings settings;
    const QString& sLastTheory = settings.value("lastTheory").toString();
    if (!sLastTheory.isEmpty())
    {
        if (Serialization::Save(qApp->getTheoryModel(), sLastTheory))
        {
            COUT(tr("Theory saved to file %1").arg(sLastTheory));
            _pUi->listViewUndo->stack()->setClean();
        }
    }
    else
    {
        _pUi->actionSave_As->trigger();
    }
}

void TheoryEditorMainWindow::on_actionSave_As_triggered()
{
    QSettings settings;
    QString sTheoryFileName;
    const QString& sLastTheoryFile = settings.value("lastTheory").toString();
    QFileDialog saveDlg(this, tr("Theory"), sLastTheoryFile, "*.theory");
    saveDlg.setDefaultSuffix("theory");
    saveDlg.setAcceptMode(QFileDialog::AcceptSave);
    if (saveDlg.exec())
    {
        sTheoryFileName = saveDlg.selectedFiles().front();
        settings.setValue("lastTheory", sTheoryFileName);
    }

    if (!sTheoryFileName.isEmpty())
    {
        _pUi->actionSave->trigger();
    }
}

void TheoryEditorMainWindow::on_actionImport_Dictionaries_triggered()
{
    QSettings settings;
    const QString& sLastImportDirectory = settings.value("lastImportDirectory").toString();
    const QString& sDirectory = QFileDialog::getExistingDirectory(this, tr("Dictionaries"), sLastImportDirectory);
    if (!sDirectory.isEmpty())
    {
        settings.setValue("lastImportDirectory", sDirectory);

        _dictionaries.clear();
        QDir dir(sDirectory);
        QStringList filters = QStringList()  << "shelton_tables.c" << "user_tables.c";
        const auto& entries = dir.entryInfoList(filters);
        if (entries.isEmpty())
        {
            CERR(tr("Can't found files %1 in directory %2").arg(filters.join(", ")).arg(sDirectory));
            return;
        }

        for (const auto& entry : entries)
        {
            DictionaryParser parser(entry.absoluteFilePath());
            parser.parse();
            _dictionaries.unite(parser.getDictionaries());
        }

        qApp->getTheoryModel()->setDictionaries(_dictionaries);

        COUT((tr("%1 dictionaries loaded").arg(_dictionaries.size())));
    }
}

void TheoryEditorMainWindow::on_actionWrite_Markdown_Files_triggered()
{
    QSettings settings;
    const QString& sDirectory = settings.value("lastMarkdownFilesDirectory").toString();
    if (!sDirectory.isEmpty())
    {
        if (!_dictionaries.isEmpty())
        {
            struct DumpInfo
            {
                typedef QMultiMap<QString, QString> OrderedEntries;
                Dictionary dictionary;
                OrderedEntries orderedEntries;
            };

            QMap<QString, DumpInfo> orderedDictionaries;
            auto it = _dictionaries.begin();
            while (it != _dictionaries.end())
            {
                QMultiMap<QString, QString> keycodesToKeysLabels;
                const Dictionary& dictionary = it++.value();
                const auto& entries = dictionary.getKeyBitsToEntry();
                for (int i = 0; i < entries.size(); ++i)
                {
                    const auto& entry = entries[i];
                    if (entry.hasKeycodes())
                    {
                        const QString& sKeysLabels = dictionary.getKeysLabelsInReadingOrder(entry);
                        const QString& sKeycodes = entry.keycodesAsUserString;
                        keycodesToKeysLabels.insert(sKeycodes, sKeysLabels);
                    }
                }

                orderedDictionaries.insert(dictionary.getName(), { dictionary, keycodesToKeysLabels });
            }

            QFile fileAllDictionaries(QString("%1/dict_all.md").arg(sDirectory));
            if (fileAllDictionaries.open(QFile::WriteOnly))
            {
                auto itOrdered = orderedDictionaries.begin();
                while (itOrdered != orderedDictionaries.end())
                {
                    const auto& dumpInfo = itOrdered++.value();
                    const Dictionary& dictionary = dumpInfo.dictionary;
                    const auto& orderedEntries = dumpInfo.orderedEntries;

                    QFile fileDictionary(QString("%1/%2").arg(sDirectory).arg(dictionary.getMarkdownFileName()));
                    if (fileDictionary.open(QFile::WriteOnly))
                    {
                        QString sContent;
                        QTextStream stream(&sContent);
                        stream << "### " << dictionary.getName() << "\n\n";
                        stream << "```\n";
                        auto itKeyCodes = orderedEntries.begin();
                        while (itKeyCodes != orderedEntries.end())
                        {
                            const QString& sKey = itKeyCodes.key();
                            const QString& sValue = itKeyCodes++.value();
                            stream << QString("\t%1 = %2\n").arg(sKey).arg(sValue);
                        }
                        stream << "```\n";
                        stream << "\n";

                        const QByteArray& utf8 = sContent.toUtf8();
                        fileDictionary.write(utf8);
                        fileAllDictionaries.write(utf8);
                        fileDictionary.close();
                    }
                }
                fileAllDictionaries.close();
            }
            COUT(tr("%1 markdown files written").arg(_dictionaries.size()));
        }
    }
}

void TheoryEditorMainWindow::on_actionWrite_Markdown_Files_To_triggered()
{
    QSettings settings;
    const QString& sLastMarkdownFilesDirectory = settings.value("lastMarkdownFilesDirectory").toString();
    const QString& sDirectory = QFileDialog::getExistingDirectory(this, tr("Markdown dictionaries directory"), sLastMarkdownFilesDirectory);
    if (!sDirectory.isEmpty())
    {
        settings.setValue("lastMarkdownFilesDirectory", sDirectory);
        _pUi->actionWrite_Markdown_Files->trigger();
    }
}

void TheoryEditorMainWindow::loadTheory(const QString& sTheoryFileName, TheoryEditorMainWindow::SettingsOperation settingsOperation)
{
    if (Serialization::Load(qApp->getTheoryModel(), sTheoryFileName))
    {
        QSettings().setValue("lastTheory", settingsOperation == ClearSettings ? QVariant() : sTheoryFileName);
        COUT(tr("Theory loaded from file %1").arg(sTheoryFileName));
        _pUi->treeViewTheory->expandToDepth(1);
        _pUi->treeViewTheory->resizeColumnToContents(0);
        _pUi->listViewUndo->stack()->clear();
    }
}
