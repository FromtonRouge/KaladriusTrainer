// ======================================================================
// This file is a part of the KaladriusTrainer project
//
// Copyright (C) 2020  Vissale NEANG <fromtonrouge at gmail dot com>
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

#include "TextsManagerMainWindow.h"
#include "TextsModel.h"
#include "../Main/Application.h"
#include "../Database/Database.h"
#include <ui_TextsManagerMainWindow.h>
#include <QtWidgets/QProgressDialog>
#include <QtWidgets/QFileDialog>
#include <QtCore/QSettings>
#include <QtCore/QTextStream>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QSet>
#include <QtCore/QDebug>
#include <iostream>
#include <functional>

TextsManagerMainWindow::TextsManagerMainWindow(QWidget* pParent)
    : MainTabWindow(pParent)
    , _pUi(new Ui::TextsManagerMainWindow)
    , _pTextsModel(new TextsModel(this))
{
    _pUi->setupUi(this);
    _pUi->textEdit->setReadOnly(true);
    _pUi->textEdit->setFontPointSize(15);

    connect(_pUi->treeViewTexts, &TextsTreeView::sendText, this, &TextsManagerMainWindow::sendText);
    connect(_pUi->treeViewTexts, &TextsTreeView::importRequest, [this](const QStringList& files)
    {
        importFiles(files);
        _pTextsModel->reset();
        _pUi->textEdit->clear();
    });
}

TextsManagerMainWindow::~TextsManagerMainWindow()
{

}

void TextsManagerMainWindow::Init()
{
    auto pDatabase = qApp->getDatabase();
    pDatabase->createTextFilesTable();
    pDatabase->createTextsTable();

    _pUi->treeViewTexts->setModel(_pTextsModel);
    auto pItemSelectionModel = _pUi->treeViewTexts->selectionModel();
    connect(pItemSelectionModel, &QItemSelectionModel::currentChanged, [&](const QModelIndex& current, const QModelIndex&)
    {
        if (current.parent().isValid())
        {
            const int iTextId = current.data(Qt::UserRole + 1).toInt();

            QString sQuery = "SELECT Text FROM \"Texts\" WHERE Id == %1";
            sQuery = sQuery.arg(iTextId);
            QSqlQuery query = pDatabase->execute(sQuery);
            if (query.next())
            {
                const QString& sText = query.value(0).toString();
                _pUi->textEdit->setText(sText);
            }
        }
    });
}

void TextsManagerMainWindow::on_actionAbout_triggered()
{
    qApp->showAboutDialog();
}

void TextsManagerMainWindow::on_actionImport_Text_triggered()
{
    QSettings settings;
    const QString& sLastTextImportDirectory = settings.value("lastTextImportDirectory").toString();
    const QString& sTextFilePath = QFileDialog::getOpenFileName(this, tr("Import Text..."), sLastTextImportDirectory, "*.*");
    if (!sTextFilePath.isEmpty())
    {
        importTextFile(sTextFilePath);

        QFileInfo fileInfo(sTextFilePath);
        settings.setValue("lastTextImportDirectory", fileInfo.absoluteDir().absolutePath());

        _pTextsModel->reset();
        _pUi->textEdit->clear();
    }
}

void TextsManagerMainWindow::on_actionImport_Directory_triggered()
{
    QSettings settings;
    const QString& sLastTextImportDirectory = settings.value("lastTextImportDirectory").toString();
    const QString& sDirectory = QFileDialog::getExistingDirectory(this, tr("Import Texts..."), sLastTextImportDirectory);
    if (!sDirectory.isEmpty())
    {
        importDirectory(sDirectory);

        settings.setValue("lastTextImportDirectory", sDirectory);

        _pTextsModel->reset();
        _pUi->textEdit->clear();
    }
}

void TextsManagerMainWindow::on_actionImport_Default_Texts_triggered()
{
    importDirectory(":/Texts/");
    _pTextsModel->reset();
    _pUi->textEdit->clear();
}

void TextsManagerMainWindow::importDirectory(const QString& sDirectory)
{
    QDir dir(sDirectory);
    if (!dir.exists())
    {
        return;
    }

    QStringList files;
    const auto& filesInfo = dir.entryInfoList({"*"}, QDir::NoDotAndDotDot|QDir::Dirs|QDir::Files);
    for (const QFileInfo& fi : filesInfo)
    {
        files << fi.absoluteFilePath();
    }

    importFiles(files);
}

void TextsManagerMainWindow::importTextFile(const QString& sFilePath)
{
    // TODO: Specific import from file extension
    QFile file(sFilePath);
    if (!file.open(QFile::ReadOnly))
    {
        return;
    }

    QTextStream stream(&file);

    // Dict that converts unicode characters to a latin1
    // character we can type on keyboard
    QHash<ushort, QVector<char>> converter;
    converter[339] = {'o', 'e'}; // Small ligature oe
    converter[8216] = {'\''}; // Left single quotation mark
    converter[8217] = {'\''}; // Right single quotation mark
    converter[8220] = {'"'}; // Left double quotation mark
    converter[8221] = {'"'}; // Right double quotation mark
    converter[8212] = {'-'}; // Em Dash

    QVector<QChar> last4Chars;
    QChar currentChar;
    QStringList texts;
    QString sText;
    QSet<QString> nonConvertedCharacters;

    auto processChar = [&](const QChar& c)
    {
        sText += c;

        bool bSplitText = false;
        if (stream.atEnd())
        {
            bSplitText = true;
        }
        else if (c == '\n')
        {
            for (const QChar& lastChar : last4Chars)
            {
                if (lastChar == '\n')
                {
                    bSplitText = true;
                    break;
                }
            }
        }

        if (bSplitText)
        {
            if (sText.size() > 50)
            {
                QString sSimplified = sText.simplified();

                if (sSimplified.startsWith('"'))
                {
                    int iDoubleQuoteCount = 0;
                    for (const QChar& character : sSimplified)
                    {
                        if (character == '"')
                        {
                            iDoubleQuoteCount++;
                        }
                    }

                    if (iDoubleQuoteCount == 1 || iDoubleQuoteCount == 2)
                    {
                        sSimplified.remove('"');
                    }
                }

                texts << sSimplified;
            }

            sText.clear();
        }

        // Memorize last 4 characters only to detect double eol
        last4Chars.push_back(c);
        if (last4Chars.size() > 4)
        {
            last4Chars.pop_front();
        }
    };

    while (stream.atEnd() == false)
    {
        stream >> currentChar;

        // Convert to "typable" character
        if (currentChar.toLatin1() == 0)
        {
            auto it = converter.find(currentChar.unicode());
            if (it != converter.end())
            {
                const auto& characters = it.value();
                for (const QChar& c : characters)
                {
                    processChar(c);
                }
            }
            else
            {
                nonConvertedCharacters.insert(currentChar);
                processChar(' ');
            }
        }
        else
        {
            processChar(currentChar);
        }
    }

    file.close();

    if (texts.isEmpty() == false)
    {
        QFileInfo fi(sFilePath);
        auto pDatabase = qApp->getDatabase();
        auto getTextFileId = [&]() -> int
        {
            QString sQuery = "SELECT Id FROM \"Text Files\" WHERE Filename == \"%1\"";
            sQuery = sQuery.arg(fi.fileName());
            int iRowId = -1;
            QSqlQuery query = pDatabase->execute(sQuery);
            if (query.next())
            {
                iRowId = query.value(0).toInt();
            }
            return iRowId;
        };

        int iTextFileId = getTextFileId();
        if (iTextFileId == -1)
        {
            pDatabase->insertValues("Text Files", {"Filename"}, {{fi.fileName()}});
            iTextFileId = getTextFileId();

            if (iTextFileId != -1)
            {
                QVector<QVariantList> rows;
                for (const QString& sText : texts)
                {
                    QVariantList values = {true, sText, sText.size(), iTextFileId};
                    rows << values;
                }

                if (pDatabase->insertValues("Texts", {"Enabled", "Text", "Characters", "TextFileId"}, rows))
                {
                    const QString sOutput = QString("%1 texts imported from \"%2\"").arg(texts.size()).arg(fi.fileName());
                    std::cout << sOutput.toStdString() << std::endl;
                }
            }
        }
        else
        {
            const QString sError = QString("Texts from \"%1\" already imported").arg(fi.fileName());
            std::cerr << sError.toStdString() << std::endl;
        }
    }

    if (nonConvertedCharacters.isEmpty() == false)
    {
        qDebug() << "Non converted characters" << nonConvertedCharacters;
        Q_ASSERT(false);
    }
}

void TextsManagerMainWindow::importFiles(const QStringList& files)
{
    const int iEntries = files.size();
    if (iEntries > 0)
    {
        QProgressDialog progressDialog(tr("Import Texts..."), tr("Cancel"), 0, iEntries, this);
        progressDialog.setWindowModality(Qt::WindowModal);

        std::function<void (const QFileInfo&)> import = [this, &import](const QFileInfo& fi)
        {
            if (fi.isDir())
            {
                const QDir dir = fi.absoluteFilePath();
                const auto& filesInfo = dir.entryInfoList({"*"}, QDir::NoDotAndDotDot|QDir::Dirs|QDir::Files);
                for (const QFileInfo& fileInfo : filesInfo)
                {
                    import(fileInfo);
                }
            }
            else
            {
                importTextFile(fi.absoluteFilePath());
            }
        };

        for (int i = 0; i < iEntries; ++i)
        {
            const QFileInfo fi(files[i]);
            progressDialog.setValue(i);
            progressDialog.setLabelText(fi.fileName());
            if (progressDialog.wasCanceled())
            {
                break;
            }

            import(fi);
        }

        progressDialog.setValue(iEntries);
    }
}
