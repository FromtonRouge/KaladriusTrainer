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

#include "TextFormatPropertiesModel.h"
#include <QtGui/QTextFrameFormat>
#include <QtGui/QTextTableFormat>
#include <QtGui/QTextBlockFormat>
#include <QtGui/QTextCharFormat>
#include <QtGui/QTextImageFormat>
#include <QtGui/QTextTableCellFormat>
#include <QtGui/QTextListFormat>
#include <QtGui/QIcon>
#include <QtCore/QMetaEnum>

TextFormatPropertiesModel::TextFormatPropertiesModel(QObject* pParent)
    : QAbstractItemModel(pParent)
{
}

TextFormatPropertiesModel::~TextFormatPropertiesModel()
{

}

void TextFormatPropertiesModel::setTextFormat(const QTextFormat& textFormat)
{
    beginResetModel();
    _textFormat = textFormat;
    _textFormatDescription = QTextFormat();
    endResetModel();
}


void TextFormatPropertiesModel::resetInternalData()
{
    switch (_textFormat.type())
    {
    case QTextFormat::FrameFormat:
        {
            QTextFrameFormat textFrameFormat;
            textFrameFormat.setPosition(textFrameFormat.position());
            textFrameFormat.setBorder(textFrameFormat.border());
            textFrameFormat.setBorderBrush(textFrameFormat.borderBrush());
            textFrameFormat.setBorderStyle(textFrameFormat.borderStyle());
            textFrameFormat.setMargin(textFrameFormat.margin());
            textFrameFormat.setTopMargin(textFrameFormat.topMargin());
            textFrameFormat.setBottomMargin(textFrameFormat.bottomMargin());
            textFrameFormat.setLeftMargin(textFrameFormat.leftMargin());
            textFrameFormat.setRightMargin(textFrameFormat.rightMargin());
            textFrameFormat.setPadding(textFrameFormat.padding());
            textFrameFormat.setWidth(textFrameFormat.width());
            textFrameFormat.setHeight(textFrameFormat.height());
            textFrameFormat.setPageBreakPolicy(textFrameFormat.pageBreakPolicy());

            if (_textFormat.isTableFormat())
            {
                QTextTableFormat textTableFormat;
                textTableFormat.setColumns(textTableFormat.columns());
                textTableFormat.setColumnWidthConstraints(textTableFormat.columnWidthConstraints());
                textTableFormat.setCellSpacing(textTableFormat.cellSpacing());
                textTableFormat.setCellPadding(textTableFormat.cellPadding());
                textTableFormat.setAlignment(textTableFormat.alignment());
                textTableFormat.setHeaderRowCount(textTableFormat.headerRowCount());
                textFrameFormat.merge(textTableFormat);
            }

            _textFormatDescription = textFrameFormat;
            break;
        }
    case QTextFormat::BlockFormat:
        {
            QTextBlockFormat textBlockFormat;
            textBlockFormat.setAlignment(textBlockFormat.alignment());
            textBlockFormat.setTopMargin(textBlockFormat.topMargin());
            textBlockFormat.setBottomMargin(textBlockFormat.bottomMargin());
            textBlockFormat.setLeftMargin(textBlockFormat.leftMargin());
            textBlockFormat.setRightMargin(textBlockFormat.rightMargin());
            textBlockFormat.setTextIndent(textBlockFormat.textIndent());
            textBlockFormat.setIndent(textBlockFormat.indent());
            textBlockFormat.setNonBreakableLines(textBlockFormat.nonBreakableLines());
            textBlockFormat.setPageBreakPolicy(textBlockFormat.pageBreakPolicy());
            textBlockFormat.setLineHeight(textBlockFormat.lineHeight(), textBlockFormat.lineHeightType());
            textBlockFormat.setTabPositions(textBlockFormat.tabPositions());

            _textFormatDescription = textBlockFormat;
            break;
        }
    case QTextFormat::CharFormat:
        {
            QTextCharFormat textCharFormat;
            textCharFormat.setFontFamily(textCharFormat.fontFamily());
            textCharFormat.setFontPointSize(textCharFormat.fontPointSize());
            textCharFormat.setFontWeight(textCharFormat.fontWeight());
            textCharFormat.setFontItalic(textCharFormat.fontItalic());
            textCharFormat.setFontCapitalization(textCharFormat.fontCapitalization());
            textCharFormat.setFontLetterSpacingType(textCharFormat.fontLetterSpacingType());
            textCharFormat.setFontLetterSpacing(textCharFormat.fontLetterSpacing());
            textCharFormat.setFontWordSpacing(textCharFormat.fontWordSpacing());
            textCharFormat.setFontUnderline(textCharFormat.fontUnderline());
            textCharFormat.setFontOverline(textCharFormat.fontOverline());
            textCharFormat.setFontStrikeOut(textCharFormat.fontStrikeOut());
            textCharFormat.setUnderlineColor(textCharFormat.underlineColor());
            textCharFormat.setFontFixedPitch(textCharFormat.fontFixedPitch());
            textCharFormat.setFontStretch(textCharFormat.fontStretch());
            textCharFormat.setFontStyleHint(textCharFormat.fontStyleHint());
            textCharFormat.setFontStyleStrategy(textCharFormat.fontStyleStrategy());
            textCharFormat.setFontHintingPreference(textCharFormat.fontHintingPreference());
            textCharFormat.setFontKerning(textCharFormat.fontKerning());
            textCharFormat.setUnderlineStyle(textCharFormat.underlineStyle());
            textCharFormat.setVerticalAlignment(textCharFormat.verticalAlignment());
            textCharFormat.setTextOutline(textCharFormat.textOutline());
            textCharFormat.setToolTip(textCharFormat.toolTip());
            textCharFormat.setAnchor(textCharFormat.isAnchor());
            textCharFormat.setAnchorHref(textCharFormat.anchorHref());
            textCharFormat.setAnchorName(textCharFormat.anchorName());
            textCharFormat.setAnchorNames(textCharFormat.anchorNames());
            textCharFormat.setTableCellRowSpan(textCharFormat.tableCellRowSpan());
            textCharFormat.setTableCellColumnSpan(textCharFormat.tableCellColumnSpan());

            if (_textFormat.isImageFormat())
            {
                QTextImageFormat textImageFormat;
                textImageFormat.setName(textImageFormat.name());
                textImageFormat.setWidth(textImageFormat.width());
                textImageFormat.setHeight(textImageFormat.height());

                textCharFormat.merge(textImageFormat);
            }
            else if (_textFormat.isTableCellFormat())
            {
                QTextTableCellFormat textTableCellFormat;
                textTableCellFormat.setTopPadding(textTableCellFormat.topPadding());
                textTableCellFormat.setBottomPadding(textTableCellFormat.bottomPadding());
                textTableCellFormat.setLeftPadding(textTableCellFormat.leftPadding());
                textTableCellFormat.setRightPadding(textTableCellFormat.rightPadding());

                textCharFormat.merge(textTableCellFormat);
            }

            _textFormatDescription = textCharFormat;
            break;
        }
    case QTextFormat::ListFormat:
        {
            QTextListFormat textListFormat;
            textListFormat.setStyle(textListFormat.style());
            textListFormat.setIndent(textListFormat.indent());
            textListFormat.setNumberPrefix(textListFormat.numberPrefix());
            textListFormat.setNumberSuffix(textListFormat.numberSuffix());

            _textFormatDescription = textListFormat;
            break;
        }
    default:
        {
            break;
        }
    }

    // Set the QTextFormat part
    if (_textFormatDescription.isValid())
    {
        _textFormatDescription.setLayoutDirection(_textFormatDescription.layoutDirection());
        _textFormatDescription.setBackground(_textFormatDescription.background());
        _textFormatDescription.setForeground(_textFormatDescription.foreground());
    }

    _rowToPropertyId.clear();
    const auto& properties = _textFormatDescription.properties();
    auto it = properties.begin();
    while (it != properties.end())
    {
        _rowToPropertyId << it.key();
        it++;
    }
}

QVariant TextFormatPropertiesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        switch (section)
        {
        case 0:
            {
                return tr("Name");
            }
        case 1:
            {
                return tr("Value");
            }
        default:
            {
                break;
            }
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

QModelIndex TextFormatPropertiesModel::index(int row, int column, const QModelIndex&) const
{
    return createIndex(row, column, row);
}

QModelIndex TextFormatPropertiesModel::parent(const QModelIndex&) const
{
    return QModelIndex();
}

int TextFormatPropertiesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return _textFormatDescription.propertyCount();
}

int TextFormatPropertiesModel::columnCount(const QModelIndex&) const
{
    return 2;
}

QVariant TextFormatPropertiesModel::data(const QModelIndex& index, int iRole) const
{
    QVariant result;
    if (!index.isValid())
    {
        return result;
    }

    const int iPropertyId = _rowToPropertyId[index.row()];

    switch (iRole)
    {
    case Qt::DisplayRole:
        {
            switch (index.column())
            {
            case 0:
                {
                    const auto& metaEnum = QMetaEnum::fromType<QTextFormat::Property>();
                    result = metaEnum.valueToKey(iPropertyId);
                    break;
                }
            case 1:
                {
                    if (_textFormat.hasProperty(iPropertyId))
                    {
                        result = _textFormat.property(iPropertyId).toString();
                    }
                    else
                    {
                        result = _textFormatDescription.property(iPropertyId).toString();
                    }
                    break;
                }
            default:
                {
                    break;
                }
            }
            break;
        }
    case Qt::EditRole:
        {
            switch (index.column())
            {
            case 1:
                {
                    if (_textFormat.hasProperty(iPropertyId))
                    {
                        result = _textFormat.property(iPropertyId);
                    }
                    else
                    {
                        result = _textFormatDescription.property(iPropertyId);
                    }
                    break;
                }
            default:
                {
                    break;
                }
            }
            break;
        }
    case Qt::FontRole:
        {
            QFont font;
            font.setBold(_textFormat.hasProperty(iPropertyId));
            result = font;
            break;
        }
    case Qt::DecorationRole:
        {
            switch (index.column())
            {
            case 0:
                {
                    result = QIcon(":/Icons/document-attribute.png");
                    break;
                }
            default:
                {
                    break;
                }
            }
            break;
        }
    default:
        {
            break;
        }
    }

    return result;
}
