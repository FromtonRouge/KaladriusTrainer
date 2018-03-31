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

#include "Lessons/LessonsEditorMainWindow.h"
#include "Lessons/LessonsTreeView.h"
#include "Lessons/Models/UndoableLessonsModel.h"
#include "Lessons/TextFormatProperties/TextFormatPropertiesModel.h"
#include "Streams/Iostream.h"
#include "Main/Application.h"
#include "Serialization/Serialize.h"
#include "ui_LessonsEditorMainWindow.h"
#include <QtWidgets/QFontComboBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QActionGroup>
#include <QtWidgets/QColorDialog>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QUndoStack>
#include <QtGui/QFontInfo>
#include <QtGui/QTextCursor>
#include <QtGui/QTextTable>
#include <QtGui/QTextList>
#include <QtGui/QFont>
#include <QtGui/QPixmap>
#include <QtCore/QSignalBlocker>
#include <QtCore/QSettings>

LessonsEditorMainWindow::LessonsEditorMainWindow(QWidget* pParent)
    : MainTabWindow(pParent)
    , _pUi(new Ui::LessonsEditorMainWindow())
    , _pFontComboBox(new QFontComboBox(this))
    , _pComboBoxSize(new QComboBox(this))
    , _pFramePropertiesModel(new TextFormatPropertiesModel(this))
    , _pBlockPropertiesModel(new TextFormatPropertiesModel(this))
    , _pBlockCharPropertiesModel(new TextFormatPropertiesModel(this))
    , _pTextPropertiesModel(new TextFormatPropertiesModel(this))
    , _pListPropertiesModel(new TextFormatPropertiesModel(this))
{
    _pUi->setupUi(this);
    setWindowTitle(tr("Lessons Editor[*]"));

    connect(_pFontComboBox, QOverload<const QString &>::of(&QComboBox::activated), this, [&](const QString& sText)
    {
        QTextCharFormat fmt;
        fmt.setFontFamily(sText);
        mergeFormatOnSelection(fmt);
        _pUi->textEdit->setFocus();
    });

    _pUi->toolBarFormat->insertWidget(_pUi->actionBold, _pFontComboBox);
    _pUi->toolBarFormat->insertWidget(_pUi->actionBold, _pComboBoxSize);

    QActionGroup* pAlignGroup = new QActionGroup(this);
    pAlignGroup->addAction(_pUi->actionAlign_Left);
    pAlignGroup->addAction(_pUi->actionAlign_Center);
    pAlignGroup->addAction(_pUi->actionAlign_Right);
    pAlignGroup->addAction(_pUi->actionAlign_Justify);
    connect(pAlignGroup, &QActionGroup::triggered, this, [&](QAction* pAction)
    {
        if (pAction == _pUi->actionAlign_Left)
        {
            _pUi->textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
        }
        else if (pAction == _pUi->actionAlign_Center)
        {
            _pUi->textEdit->setAlignment(Qt::AlignHCenter);
        }
        else if (pAction == _pUi->actionAlign_Right)
        {
            _pUi->textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
        }
        else if (pAction == _pUi->actionAlign_Justify)
        {
            _pUi->textEdit->setAlignment(Qt::AlignJustify);
        }
        _pUi->textEdit->setFocus();
    });

    auto cursor = _pUi->textEdit->textCursor();
    auto blockCharFormat = cursor.blockCharFormat();
    blockCharFormat.setFontPointSize(18);
    blockCharFormat.setBackground(Qt::white);
    cursor.setBlockCharFormat(blockCharFormat);
    _pUi->textEdit->setFont(blockCharFormat.font());
    _pUi->textEdit->setTextBackgroundColor(Qt::white);

    fontChanged(blockCharFormat.font());
    foregroundColorChanged(_pUi->textEdit->textColor());
    backgroundColorChanged(_pUi->textEdit->textBackgroundColor());

    _pComboBoxSize->setEditable(true);

    const QList<int>& standardSizes = QFontDatabase::standardSizes();
    for (int size : standardSizes)
    {
        _pComboBoxSize->addItem(QString::number(size));
    }
    _pComboBoxSize->setCurrentIndex(standardSizes.indexOf(blockCharFormat.fontPointSize()));
    connect(_pComboBoxSize, QOverload<const QString &>::of(&QComboBox::activated), this, [&](const QString& sText)
    {
        qreal pointSize = sText.toFloat();
        if (sText.toFloat() > 0)
        {
            QTextCharFormat fmt;
            fmt.setFontPointSize(pointSize);
            mergeFormatOnSelection(fmt);
        }
        _pUi->textEdit->setFocus();
    });

    auto pUndoableLessonsModel = qApp->getUndoableLessonsModel();
    auto pLessonsTreeView = _pUi->widgetLessons->findChild<LessonsTreeView*>();
    pLessonsTreeView->setModel(pUndoableLessonsModel);
    pLessonsTreeView->expandAll();
    pLessonsTreeView->resizeColumnToContents(0);

    // Init undo stack
    auto pUndoStack = pUndoableLessonsModel->getUndoStack();
    _pUi->listViewUndo->setStack(pUndoStack);
    connect(pUndoStack, SIGNAL(cleanChanged(bool)), this, SLOT(onUndoCleanChanged(bool)));
    _pUi->menuEdit->addAction(createUndoAction(pUndoStack));
    _pUi->menuEdit->addAction(createRedoAction(pUndoStack));

    _pUi->treeViewFrameProperties->setModel(_pFramePropertiesModel);
    _pUi->treeViewBlockProperties->setModel(_pBlockPropertiesModel);
    _pUi->treeViewBlockCharProperties->setModel(_pBlockCharPropertiesModel);
    _pUi->treeViewTextProperties->setModel(_pTextPropertiesModel);
    _pUi->treeViewListProperties->setModel(_pListPropertiesModel);

    tabifyDockWidget(_pUi->dockWidgetFrameProperties, _pUi->dockWidgetBlockProperties);
    tabifyDockWidget(_pUi->dockWidgetBlockProperties, _pUi->dockWidgetBlockCharProperties);
    tabifyDockWidget(_pUi->dockWidgetBlockCharProperties, _pUi->dockWidgetTextProperties);
    tabifyDockWidget(_pUi->dockWidgetTextProperties, _pUi->dockWidgetListProperties);
    _pUi->dockWidgetTextProperties->raise();

    on_textEdit_cursorPositionChanged();
    QMetaObject::invokeMethod(_pUi->actionReload, "trigger", Qt::QueuedConnection);
}

LessonsEditorMainWindow::~LessonsEditorMainWindow()
{

}

void LessonsEditorMainWindow::mergeFormatOnSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = _pUi->textEdit->textCursor();
    cursor.mergeCharFormat(format);
    _pUi->textEdit->mergeCurrentCharFormat(format);
}

void LessonsEditorMainWindow::on_textEdit_currentCharFormatChanged(const QTextCharFormat& format)
{
    fontChanged(format.font());
    foregroundColorChanged(format.foreground().color());
    backgroundColorChanged(format.background().color());
}

void LessonsEditorMainWindow::on_textEdit_cursorPositionChanged()
{
    alignmentChanged(_pUi->textEdit->alignment());
    updateTextFormats();
}

void LessonsEditorMainWindow::on_actionBold_toggled(bool bChecked)
{
    QTextCharFormat format;
    format.setFontWeight(bChecked?QFont::Bold:QFont::Normal);
    mergeFormatOnSelection(format);
    _pUi->textEdit->setFocus();
}

void LessonsEditorMainWindow::on_actionItalic_toggled(bool bChecked)
{
    QTextCharFormat format;
    format.setFontItalic(bChecked);
    mergeFormatOnSelection(format);
    _pUi->textEdit->setFocus();
}

void LessonsEditorMainWindow::on_actionUnderline_toggled(bool bChecked)
{
    QTextCharFormat format;
    format.setFontUnderline(bChecked);
    mergeFormatOnSelection(format);
    _pUi->textEdit->setFocus();
}

void LessonsEditorMainWindow::on_actionForeground_Color_triggered(bool bChecked)
{
    QColor col = QColorDialog::getColor(_pUi->textEdit->textColor(), this);
    if (col.isValid())
    {
        QTextCharFormat fmt;
        fmt.setForeground(col);
        mergeFormatOnSelection(fmt);
    }
    _pUi->textEdit->setFocus();
}

void LessonsEditorMainWindow::on_actionBackground_Color_triggered(bool bChecked)
{
    QColor col = QColorDialog::getColor(_pUi->textEdit->textColor(), this);
    if (col.isValid())
    {
        QTextCharFormat fmt;
        fmt.setBackground(col);
        mergeFormatOnSelection(fmt);
    }
    _pUi->textEdit->setFocus();
}

void LessonsEditorMainWindow::on_actionInsert_List_triggered()
{
    QTextCursor cursor = _pUi->textEdit->textCursor();
    cursor.beginEditBlock();
    QTextListFormat fmt;
    fmt.setStyle(QTextListFormat::ListDisc);
    cursor.insertList(fmt);
    cursor.endEditBlock();
    _pUi->textEdit->setFocus();
}

void LessonsEditorMainWindow::on_actionInsert_Ordered_List_triggered()
{
    QTextCursor cursor = _pUi->textEdit->textCursor();
    cursor.beginEditBlock();
    QTextListFormat fmt;
    fmt.setStyle(QTextListFormat::ListDecimal);
    cursor.insertList(fmt);
    cursor.endEditBlock();
    _pUi->textEdit->setFocus();
}

void LessonsEditorMainWindow::on_actionInsert_Table_triggered()
{
    QTextCursor cursor = _pUi->textEdit->textCursor();
    cursor.beginEditBlock();
    const int nRows = 2;
    const int nColumns = 2;
    QTextTableFormat fmt;
    fmt.setCellSpacing(-1);
    fmt.setCellPadding(5);
    fmt.setBorderBrush(Qt::black);
    auto pTextTable = cursor.insertTable(nRows, nColumns, fmt);
    cursor.endEditBlock();
    _pUi->textEdit->setTextCursor(pTextTable->cellAt(0, 0).firstCursorPosition());
    _pUi->textEdit->setFocus();
}

void LessonsEditorMainWindow::on_actionInsert_Frame_triggered()
{
    QTextCursor cursor = _pUi->textEdit->textCursor();
    cursor.beginEditBlock();
    QTextFrameFormat fmt;
    fmt.setBorder(1);
    fmt.setLeftMargin(8);
    fmt.setPadding(8);
    cursor.insertFrame(fmt);
    cursor.endEditBlock();
    _pUi->textEdit->setFocus();
}

void LessonsEditorMainWindow::on_actionInsert_Image_triggered()
{
    QTextCursor cursor = _pUi->textEdit->textCursor();
    cursor.beginEditBlock();
    QSettings settings;
    QString sDirectory = settings.value("lastImagesDirectory").toString();
    const QStringList& files = QFileDialog::getOpenFileNames(this, tr("Select images"), sDirectory, tr("Images (*.png)"));

    if (!files.isEmpty())
    {
        QFileInfo fileInfo(files.front());
        sDirectory = fileInfo.dir().absolutePath();
        settings.setValue("lastImagesDirectory", sDirectory);
    }

    for (const QString& sFile : files)
    {
        QTextImageFormat fmt;
        fmt.setName(sFile);
        cursor.insertImage(fmt);
    }
    cursor.endEditBlock();
    _pUi->textEdit->setFocus();

}

void LessonsEditorMainWindow::on_actionSave_triggered()
{
    QSettings settings;
    const QString& sLastCourse = settings.value("lastCourse").toString();
    if (!sLastCourse.isEmpty())
    {
        if (Serialization::Save(_pUi->textEdit->document(), sLastCourse))
        {
            COUT(tr("Course saved to file %1").arg(sLastCourse));
        }
    }
    else
    {
        _pUi->actionSave_As->trigger();
    }
}

void LessonsEditorMainWindow::on_actionSave_As_triggered()
{
    QSettings settings;
    QString sCourseFileName;
    const QString& sLastCourse = settings.value("lastCourse").toString();
    QFileDialog saveDlg(this, tr("Course"), sLastCourse, "*.crs");
    saveDlg.setDefaultSuffix("crs");
    saveDlg.setAcceptMode(QFileDialog::AcceptSave);
    if (saveDlg.exec())
    {
        sCourseFileName = saveDlg.selectedFiles().front();
        settings.setValue("lastCourse", sCourseFileName);
    }

    if (!sCourseFileName.isEmpty())
    {
        _pUi->actionSave->trigger();
    }
}

void LessonsEditorMainWindow::on_actionReload_triggered()
{
    QSettings settings;
    const QString& sLastCourse = settings.value("lastCourse").toString();
    Serialization::Load(_pUi->textEdit->document(), sLastCourse);
}

void LessonsEditorMainWindow::on_actionNew_triggered()
{
    _pUi->textEdit->clear();
}

void LessonsEditorMainWindow::on_actionIncrease_Indent_triggered()
{
    QTextCursor cursor = _pUi->textEdit->textCursor();
    cursor.beginEditBlock();
    QTextBlockFormat fmt;
    fmt.setIndent(cursor.blockFormat().indent() + 1);
    cursor.mergeBlockFormat(fmt);
    cursor.endEditBlock();
    _pUi->textEdit->setFocus();
}

void LessonsEditorMainWindow::on_actionDecrease_Indent_triggered()
{
    QTextCursor cursor = _pUi->textEdit->textCursor();
    cursor.beginEditBlock();
    QTextBlockFormat fmt;
    fmt.setIndent(qMax(cursor.blockFormat().indent() - 1, 0));
    cursor.mergeBlockFormat(fmt);
    cursor.endEditBlock();
    _pUi->textEdit->setFocus();
}

void LessonsEditorMainWindow::updateTextFormats()
{
    QTextCursor cursor = _pUi->textEdit->textCursor();
    auto pTextFrame = cursor.currentFrame();
    if (pTextFrame)
    {
        _pFramePropertiesModel->setTextFormat(pTextFrame->frameFormat());
    }
    else
    {
        _pFramePropertiesModel->setTextFormat(QTextFormat());
    }
    _pBlockPropertiesModel->setTextFormat(cursor.blockFormat());
    _pBlockCharPropertiesModel->setTextFormat(cursor.blockCharFormat());
    _pTextPropertiesModel->setTextFormat(cursor.charFormat());

    auto pCurrentList = cursor.currentList();
    if (pCurrentList)
    {
        _pListPropertiesModel->setTextFormat(pCurrentList->format());
    }
    else
    {
        _pListPropertiesModel->setTextFormat(QTextFormat());
    }
}

void LessonsEditorMainWindow::fontChanged(const QFont& f)
{
    QSignalBlocker blocker1(_pUi->actionBold);
    QSignalBlocker blocker2(_pUi->actionItalic);
    QSignalBlocker blocker3(_pUi->actionUnderline);
    QSignalBlocker blocker4(_pFontComboBox);
    QSignalBlocker blocker5(_pComboBoxSize);

    _pFontComboBox->setCurrentIndex(_pFontComboBox->findText(QFontInfo(f).family()));
    _pComboBoxSize->setCurrentIndex(_pComboBoxSize->findText(QString::number(f.pointSize())));
    _pUi->actionBold->setChecked(f.bold());
    _pUi->actionItalic->setChecked(f.italic());
    _pUi->actionUnderline->setChecked(f.underline());
}

void LessonsEditorMainWindow::foregroundColorChanged(const QColor& c)
{
    QPixmap pix(16, 16);
    pix.fill(c);
    _pUi->actionForeground_Color->setIcon(pix);
}

void LessonsEditorMainWindow::backgroundColorChanged(const QColor& c)
{
    QPixmap pix(16, 16);
    pix.fill(c);
    _pUi->actionBackground_Color->setIcon(pix);
}

void LessonsEditorMainWindow::alignmentChanged(Qt::Alignment a)
{
    if (a & Qt::AlignLeft)
    {
        _pUi->actionAlign_Left->setChecked(true);
    }
    else if (a & Qt::AlignHCenter)
    {
        _pUi->actionAlign_Center->setChecked(true);
    }
    else if (a & Qt::AlignRight)
    {
        _pUi->actionAlign_Right->setChecked(true);
    }
    else if (a & Qt::AlignJustify)
    {
        _pUi->actionAlign_Justify->setChecked(true);
    }
}
