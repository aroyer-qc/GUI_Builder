/*
   Copyright(c) 2020 Alain Royer.
   Email: aroyer.qc@gmail.com

   Permission is hereby granted, free of charge, to any person obtaining a copy of this software
   and associated documentation files (the "Software"), to deal in the Software without
   restriction, including without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all copies or
   substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
   INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
   AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
   DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Utility.h"
#include <QFont>
#include <QPixmap>
#include <QRegExp>


struct FontMeta {
    QString Name;
    int Size;
};

// ************************************************************************************************
// *
// *  Slot
// *
// ************************************************************************************************

void MainWindow::on_ButtonAddFont_clicked()
{
    QFontDialog fontDialog;
    QFont Font;
    int row;
    bool ok;

    //fontDialog.setOption(QFontDialog::DontUseNativeDialog, true);
    //fontDialog.resize(800, 600);
    fontDialog.setStyleSheet("QPushButton {color: #DFD;background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6B82AC, stop: 0.49 #566D97, stop: 0.5 #445B85, stop: 1 #566D97); border-width: 3px; border-color: #778EB8; border-style: solid; border-radius: 7; padding: 3px; font: bold \"Ubuntu\"; font-size: 12px; padding-left: 5px; padding-right: 5px; }");
    fontDialog.repaint();

    Font = fontDialog.getFont(&ok, QFont("Arial", 12), this);
    if(ok)
    {
        setSkinHasUnsavedData(true);
        m_Font.append(Font);
        m_SamplingFont.append(SAMPLING_DEFAULT_FONT);
        row = m_Font.size() - 1;

        ui->TableFont->insertRow(row);
        QTableWidgetItem *FontNameItem       = new QTableWidgetItem(Font.family());
        QTableWidgetItem *FontSizeItem       = new QTableWidgetItem(QString("%1").arg(Font.pointSize()));
        FontNameItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        FontSizeItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
        ui->TableFont->setItem(row, 0, FontNameItem);
        ui->TableFont->setItem(row, 1, FontSizeItem);
        SetTableProperties(row);
        ui->TableFont->setCurrentCell(row, 0);

        LoadFont(row);
    }
}

/*
        QFileDialog fileDialog(this, tr("Open file"), m_SkinDir.absolutePath(), tr("Skin file (*.skn)"));
        fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);
        fileDialog.resize(800, 600);
        fileDialog.setStyleSheet("QPushButton {color: #DFD;background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6B82AC, stop: 0.49 #566D97, stop: 0.5 #445B85, stop: 1 #566D97); border-width: 3px; border-color: #778EB8; border-style: solid; border-radius: 7; padding: 3px; font: bold \"Ubuntu\"; font-size: 12px; padding-left: 5px; padding-right: 5px; }");

        QListView *listView = fileDialog.findChild<QListView*>("listView");
        if(listView)
        {
            listView->setMinimumSize(400, 300); // Adjust the minimum size of the list view pane
        }

        QTreeView *treeView = fileDialog.findChild<QTreeView*>("treeView");
        if(treeView)
        {
            treeView->setMinimumSize(400, 300); // Adjust the minimum size of the tree view pane
        }

        QList<QPushButton*> buttons = fileDialog.findChildren<QPushButton*>();
        for(QPushButton *button : buttons)
        {
            button->setMinimumWidth(150); // Set the minimum width of the buttons
        }

        if (fileDialog.exec() == QDialog::Accepted)
        {
            QString File = fileDialog.selectedFiles().first();
            if (!File.isEmpty())
            {
                Open(File);
            }
        }
    }
    else
    {
        m_IsNeedCompleteFileOpen = true;            // Will trigger file open at the end saving
    }

*/


// ************************************************************************************************

void MainWindow::on_ButtonRemoveFont_clicked()
{
    int row;

    ui->TableFont->blockSignals(true);

    // Get the selected row
    row = ui->TableFont->currentIndex().row();

    // Remove Font from vector
    m_Font.remove(row);
    m_SamplingFont.remove(row);

    // Remove Row
    ui->TableFont->removeRow(row);

    // If skin size is 0, and no more font there is nothing to save
    setSkinHasUnsavedData(((m_SkinSize != 0) || (m_Font.size() != 0)) ? true : false);

    //row--;

    // Update Image GUI for all widget
    if(row >= ui->TableFont->rowCount())
    {
        row =  ui->TableFont->rowCount() - 1;
        ui->TableFont->selectRow(row);
    }

    UpdateFontGUI(row);
    LoadFont(row);

    ui->TableFont->blockSignals(false);
}

// ************************************************************************************************

void MainWindow::on_ButtonUpFont_clicked()
{
    QFont   FontCopy;
    uint8_t SamplingCopy;
    int row;

    ui->TableFont->blockSignals(true);

    // Get the selected row
    row = ui->TableFont->currentIndex().row();

    // Swap info structure
    FontCopy                    = m_Font[row - 1];
    SamplingCopy                = m_SamplingFont[row - 1];

    m_Font[row - 1]             = m_Font[row];
    m_SamplingFont[row - 1]     = m_SamplingFont[row];

    m_Font[row]                 = FontCopy;
    m_SamplingFont[row]         = SamplingCopy;

    // Swap element in table
    InsertNewRowInTableFont(row - 1,
                        ui->TableFont->item(row, 0)->text(),
                        ui->TableFont->item(row, 1)->text(),
                        ui->TableFont->item(row, 2)->text());

    ui->TableFont->selectRow(row - 1);
    ui->TableFont->removeRow(row + 1);

    ui->TableFont->blockSignals(false);

    UpdateFontGUI(row - 1);
    setSkinHasUnsavedData(true);
}

// ************************************************************************************************

void MainWindow::on_ButtonDownFont_clicked()
{
    QFont   FontCopy;
    uint8_t SamplingCopy;
    int row;

    ui->TableFont->blockSignals(true);

    // Get the selected row
    row = ui->TableFont->currentIndex().row();

    // Swap info structure
    FontCopy                    = m_Font[row + 1];
    SamplingCopy                = m_SamplingFont[row + 1];

    m_Font[row + 1]             = m_Font[row];
    m_SamplingFont[row + 1]     = m_SamplingFont[row];

    m_Font[row]                 = FontCopy;
    m_SamplingFont[row]         = SamplingCopy;

    // Swap element in table
    InsertNewRowInTableFont(row + 2,
                        ui->TableFont->item(row, 0)->text(),
                        ui->TableFont->item(row, 1)->text(),
                        ui->TableFont->item(row, 2)->text());

    ui->TableFont->removeRow(row);
    ui->TableFont->selectRow(row + 1);

    ui->TableFont->blockSignals(false);

    UpdateFontGUI(row + 1);
    setSkinHasUnsavedData(true);
}

// ************************************************************************************************

void MainWindow::on_TableFont_cellActivated(int row, int column)
{
    Q_UNUSED(column);
    LoadFont(row);
}

// ************************************************************************************************

void MainWindow::on_TableFont_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    Q_UNUSED(currentColumn);
    Q_UNUSED(previousRow);
    Q_UNUSED(previousColumn);
    LoadFont(currentRow);
}

// ************************************************************************************************

void MainWindow::on_TableFont_cellDoubleClicked(int row, int column)
{
    QString Item;
    int     Value;

    Item  = ui->TableFont->item(row, column)->text();
    if(column == 1)
    {
        Value = getPointSizeFontFromText(Item);
        m_pFontComboBoxDelegate->setEditorValue(Value);
    }
}

// ************************************************************************************************

void MainWindow::on_TableFont_itemChanged(QTableWidgetItem *item)
{
    QString CellData;
    int     Row;
    int     Column;

    Row    = item->row();
    Column = item->column();

    CellData = ui->TableFont->item(Row, Column)->text();

    if(Column == 1)
    {
        m_Font[Row].setPointSize(CellData.toInt());
        LoadFont(Row);
    }
}

// ************************************************************************************************

// Trig a reload
void MainWindow::on_checkBoxAlphaFont_clicked(bool checked)
{
    int row = ui->TableFont->currentRow();

    if(checked == true) m_SamplingFont[row] |=  SAMPLING_ALPHA;
    else                m_SamplingFont[row] &= ~SAMPLING_ALPHA;
    LoadFont(row);
    setSkinHasUnsavedData(true);
}

// ************************************************************************************************

void MainWindow::on_checkBoxNumericFont_clicked(bool checked)
{
    int row = ui->TableFont->currentRow();

    if(checked == true)
    {
        m_SamplingFont[row] |=  SAMPLING_NUMERIC;
        ui->checkBoxFixed->setEnabled(true);
    }
    else
    {
        m_SamplingFont[row] &= ~SAMPLING_NUMERIC;
        ui->checkBoxFixed->setChecked(false);
        ui->checkBoxFixed->setEnabled(false);
    }
    LoadFont(row);
    setSkinHasUnsavedData(true);
}

// ************************************************************************************************

void MainWindow::on_checkBoxSymbolFont_clicked(bool checked)
{
    int row = ui->TableFont->currentRow();

    if(checked == true) m_SamplingFont[row] |=  SAMPLING_SYMBOL;
    else                m_SamplingFont[row] &= ~SAMPLING_SYMBOL;
    LoadFont(row);
    setSkinHasUnsavedData(true);
}

// ************************************************************************************************

void MainWindow::on_checkBoxExtraSymbolFont_clicked(bool checked)
{
    int row = ui->TableFont->currentRow();

    if(checked == true) m_SamplingFont[row] |=  SAMPLING_EXTRA_SYMBOL;
    else                m_SamplingFont[row] &= ~SAMPLING_EXTRA_SYMBOL;
    LoadFont(row);
    setSkinHasUnsavedData(true);
}

// ************************************************************************************************

void MainWindow::on_checkBoxLatinFont_clicked(bool checked)
{
    int row = ui->TableFont->currentRow();

    if(checked == true) m_SamplingFont[row] |=  SAMPLING_LATIN;
    else                m_SamplingFont[row] &= ~SAMPLING_LATIN;
    LoadFont(row);
    setSkinHasUnsavedData(true);
}

// ************************************************************************************************

void MainWindow::on_checkBoxBold_clicked(bool checked)
{
    int row = ui->TableFont->currentRow();

    if(row >= 0)
    {
        m_Font[row].setBold(checked);
        LoadFont(row);
    }
}

// ************************************************************************************************

void MainWindow::on_checkBoxItalic_clicked(bool checked)
{
    int row = ui->TableFont->currentRow();

    if(row >= 0)
    {
        m_Font[row].setItalic(checked);
        LoadFont(row);
    }
}

// ************************************************************************************************

void MainWindow::on_checkBoxFixed_clicked(bool checked)
{
    int row = ui->TableFont->currentRow();

    if(checked == true) m_SamplingFont[row] |=  SAMPLING_FIXED_NUMERIC;
    else                m_SamplingFont[row] &= ~SAMPLING_FIXED_NUMERIC;

    LoadFont(row);
}

// ************************************************************************************************

struct CharMeta
{
    uint32_t Offset;
    uint16_t BitmapSize;
    uint8_t  Code;
    uint8_t  Width;
    uint8_t  Height;
    uint8_t  BearingX;
    uint8_t  BearingY;
    uint8_t  Advance;
};

void MainWindow::on_ButtonConvertFont_clicked()
{
    QVector<uint8_t> RawData;
    QVector<FontMeta> FontMetaList;
    QString BaseName = "StaticFont";
    uint32_t OffsetFontCharCountHeader;
    QVector<uint32_t> OffsetFontHeight;
    QVector<QVector<CharMeta>> FontCharMeta;

    // Create Raw data before write
    Append_uint16(&RawData, (uint16_t)m_Font.count());              // Put font count in compressed data
    for(int Count = 0; Count < m_Font.count(); Count++)
    {
        OffsetFontHeight.append(RawData.size());                    // Kept offset so we can modifed then later
        RawData.append(0x00);                                       // Reserve space for height for this font
        RawData.append(0x00);                                       // Reserve space for interline for this font
    }
    m_TotalCharCount = 0;                                           // Reset value of the character count

    for(int Count = 0; Count < m_Font.count(); Count++)
    {
        m_InFontCharCount = 0;
        m_pFont       = &m_Font.at(Count);
        m_pFontMetric = new QFontMetrics(*m_pFont);

        FontMeta Meta;
        Meta.Name = m_pFont->family();
        Meta.Size = m_pFont->pointSize();
        FontMetaList.append(Meta);

        // Put font count in compressed data
        Append_uint32(&RawData, Count);                             // For now we use count number as ID

        // Reserve space in compressed data for character count
        OffsetFontCharCountHeader = RawData.size();                 // Keep offset for later
        RawData.append(0x00);                                       // Value rewritten when count is knowned

        // Sample all alpha character
        if((m_SamplingFont.at(Count) & SAMPLING_ALPHA) != 0)
        {
            for(uint32_t CharCount = 'a'; CharCount <= 'z'; CharCount++) ExtractFontInfo(&RawData, char(CharCount));
            for(uint32_t CharCount = 'A'; CharCount <= 'Z'; CharCount++) ExtractFontInfo(&RawData, char(CharCount));
        }

        // Sample all numeric character
        if((m_SamplingFont.at(Count) & SAMPLING_NUMERIC) != 0)
        {
            int MaxX;

            for(uint32_t CharCount = '0'; CharCount <= '9'; CharCount++) ExtractFontInfo(&RawData, char(CharCount));
        }

        // Sample all symbol character
        if((m_SamplingFont.at(Count) & SAMPLING_SYMBOL) != 0)
        {
            for(uint32_t CharCount = ' '; CharCount <= '/'; CharCount++) ExtractFontInfo(&RawData, char(CharCount));
            for(uint32_t CharCount = ':'; CharCount <= '@'; CharCount++) ExtractFontInfo(&RawData, char(CharCount));
            for(uint32_t CharCount = '['; CharCount <= '`'; CharCount++) ExtractFontInfo(&RawData, char(CharCount));
            for(uint32_t CharCount = '{'; CharCount <= '~'; CharCount++) ExtractFontInfo(&RawData, char(CharCount));
        }

        // Sample all extra symbol character
        if((m_SamplingFont.at(Count) & SAMPLING_EXTRA_SYMBOL) != 0)
        {
            ExtractFontInfo(&RawData, 153); //'tm'
            ExtractFontInfo(&RawData, 169); //'©'
            ExtractFontInfo(&RawData, 174); //'®'
            ExtractFontInfo(&RawData, 176); //'°'
            ExtractFontInfo(&RawData, 185); //'±'
        }

        // Sample all latin character
        if((m_SamplingFont.at(Count) & SAMPLING_LATIN) != 0)
        {
            for(uint32_t CharCount = 192/*'À'*/; CharCount <= 214/*'Ö'*/; CharCount++) ExtractFontInfo(&RawData, char(CharCount));
            for(uint32_t CharCount = 216/*'Ø'*/; CharCount <= 246/*'ö'*/; CharCount++) ExtractFontInfo(&RawData, char(CharCount));
            for(uint32_t CharCount = 248/*'ø'*/; CharCount <= 254/*'þ'*/; CharCount++) ExtractFontInfo(&RawData, char(CharCount));
        }

        RawData.replace(OffsetFontCharCountHeader, m_InFontCharCount); // Write character count
    }

    m_TotalCharCount = 0;                                      // Reset value of the character count

    for(int Count = 0; Count < m_Font.count(); Count++)
    {
        uint32_t StartOffsetFont = m_TotalCharCount;
        uint32_t StartNumericalOffsetFont;
        uint8_t MinY = 0xFF;
        uint8_t MaxY = 0x00;

        m_pFont       = &m_Font.at(Count);
        m_pFontMetric = new QFontMetrics(*m_pFont);
        m_MaxX_FixedFont = 0;

        // Sample all alpha character
        if((m_SamplingFont.at(Count) & SAMPLING_ALPHA) != 0)
        {
            for(uint32_t CharCount = 'a'; CharCount <= 'z'; CharCount++) SaveEachCharFont(&RawData, char(CharCount));
            for(uint32_t CharCount = 'A'; CharCount <= 'Z'; CharCount++) SaveEachCharFont(&RawData, char(CharCount));
        }

        // Sample all numeric character
        if((m_SamplingFont.at(Count) & SAMPLING_NUMERIC) != 0)
        {
            StartNumericalOffsetFont = m_TotalCharCount;

            for(uint32_t CharCount = '0'; CharCount <= '9'; CharCount++)
            {
                SaveEachCharFont(&RawData, char(CharCount));
            }
        }

        // Sample all symbol character
        if((m_SamplingFont.at(Count) & SAMPLING_SYMBOL) != 0)
        {
            for(uint32_t CharCount = ' '; CharCount <= '/'; CharCount++) SaveEachCharFont(&RawData, char(CharCount));
            for(uint32_t CharCount = ':'; CharCount <= '@'; CharCount++) SaveEachCharFont(&RawData, char(CharCount));
            for(uint32_t CharCount = '['; CharCount <= '`'; CharCount++) SaveEachCharFont(&RawData, char(CharCount));
            for(uint32_t CharCount = '{'; CharCount <= '~'; CharCount++) SaveEachCharFont(&RawData, char(CharCount));
        }

        // Sample all extra symbol character
        if((m_SamplingFont.at(Count) & SAMPLING_EXTRA_SYMBOL) != 0)
        {
            SaveEachCharFont(&RawData, 153); //'tm'
            SaveEachCharFont(&RawData, 169); //'©'
            SaveEachCharFont(&RawData, 174); //'®'
            SaveEachCharFont(&RawData, 176); //'°'
            SaveEachCharFont(&RawData, 185); //'±'
        }

        // Sample all latin character
        if((m_SamplingFont.at(Count) & SAMPLING_LATIN) != 0)
        {
            for(uint32_t CharCount = 192/*'À'*/; CharCount <= 214/*'Ö'*/; CharCount++) SaveEachCharFont(&RawData, char(CharCount));
            for(uint32_t CharCount = 216/*'Ø'*/; CharCount <= 246/*'ö'*/; CharCount++) SaveEachCharFont(&RawData, char(CharCount));
            for(uint32_t CharCount = 248/*'ø'*/; CharCount <= 254/*'þ'*/; CharCount++) SaveEachCharFont(&RawData, char(CharCount));
        }

        // Rescan for lowest Y minimum and
        for(uint32_t CharCount = StartOffsetFont; CharCount < m_TotalCharCount; CharCount++)
        {
            if(MinY > m_MinY[CharCount]) MinY = m_MinY[CharCount];
            if(MaxY < m_MaxY[CharCount]) MaxY = m_MaxY[CharCount];
        }

        // Write the new height for the font
        RawData.replace(OffsetFontHeight[Count], (MaxY - MinY) + 1);


        // Rescan numerical value for max width
        //if((m_pFontSamplingInfo->at(Count) & SAMPLING_FIXED_NUMERIC) != 0)
        {
            for(uint32_t CharCount = StartNumericalOffsetFont; CharCount < (StartNumericalOffsetFont) + 10; CharCount++)
            {
                RawData.replace( m_OffsetFontHeader[CharCount] + 7, m_MaxX_FixedFont);
            }
        }

        // We subtract this absolute minimum from height, and all other minimum and maximum
        for(uint32_t CharCount = StartOffsetFont; CharCount < m_TotalCharCount; CharCount++)
        {
            RawData.replace(m_OffsetFontHeader[CharCount] + 8,  m_MinY[CharCount] -= MinY);
        }
    }


    // Create the header file
    QFile File(BaseName + ".h");

    if(File.open(QIODevice::WriteOnly))
    {
        QTextStream Stream(&File);

        Stream << "//-------------------------------------------\r\n";
        Stream << "// This file was autogenerated by GUI Builder\r\n";
        Stream << "//-------------------------------------------\r\n\r\n";

        Stream << "#include <stdint.h>\r\n";
        Stream << "#include \"digini_cfg.h\"\r\n";
        Stream << "#ifdef DIGINI_USE_GRAFX\r\n\r\n";

        Stream << "//-------------------------------------------\r\n\r\n";
       
        Stream << "struct FONT_Char_t\r\n{\r\n";
        Stream << "    uint8_t  Code;\r\n";
        Stream << "    uint8_t  Width;\r\n";
        Stream << "    uint8_t  Height;\r\n";
        Stream << "    uint8_t  BearingX;\r\n";
        Stream << "    uint8_t  BearingY;\r\n";
        Stream << "    uint8_t  Advance;\r\n";
        Stream << "    uint16_t BitmapSize;\r\n";
        Stream << "    const uint8_t* pBitmap;\r\n";
        Stream << "};\r\n\r\n";

        Stream << "struct FONT_Entry_t\r\n{\r\n";
        Stream << "    uint8_t        Height;\r\n";
        Stream << "    uint8_t        Interline;\r\n";
        Stream << "    uint32_t       FontID;\r\n";
        Stream << "    uint8_t        CharCount;\r\n";
        Stream << "    const FontChar_t* pChars;\r\n";
        Stream << "};\r\n\r\n";

        Stream << "struct FONT_File_t\r\n{\r\n";
        Stream << "    uint16_t           FontCount;\r\n";
        Stream << "    const FontEntry_t* pFonts;\r\n";
        Stream << "};\r\n\r\n";

        Stream << "//-------------------------------------------\r\n\r\n";
        
        Stream << "enum FONT_ListID_t\r\n{\r\n";

        for(int i = 0; i < FontMetaList.size(); i++)
        {
            QString CleanName = FontMetaList[i].Name;
            CleanName.replace(" ", "_");
            CleanName.replace("-", "_");

            Stream << "    FONT_" 
                   << CleanName.toUpper() 
                   << "_" 
                   << FontMetaList[i].Size;

            if(i < FontMetaList.size() - 1)
            {
                Stream << ",\r\n";
            }
            else
            {
                Stream << "\r\n";
            }
        }

        Stream << "};\r\n\r\n";

        Stream << "//-------------------------------------------\r\n\r\n";

        Stream << "extern const uint8_t FONT_RawData[];\r\n";
        Stream << "extern const FontEntry_t FONT_Table[];\r\n\r\n";
        
        Stream << "//-------------------------------------------\r\n\r\n";
        Stream << "#endif // DIGINI_USE_GRAFX\r\n";

        File.close();
    }
        
    // Create th CPP file
    File.setFileName(BaseName + ".cpp");

    if(File.open(QIODevice::WriteOnly))
    {
        QTextStream Stream(&File);

        Stream << "//-------------------------------------------\r\n";
        Stream << "// This file was autogenerated by GUI Builder\r\n";
        Stream << "//-------------------------------------------\r\n\r\n";
        Stream << "#include <stdint.h>\r\n";
        Stream << "#include \"digini_cfg.h\"\r\n";
        Stream << "#ifdef DIGINI_USE_GRAFX\r\n";
        Stream << "#include \"" << BaseName << ".h\"\r\n\r\n";
        Stream << "//-------------------------------------------\r\n\r\n";

        // Raw data array
        Stream << "const uint8_t FONT_RawData[] =\r\n";
        Stream << "{\r\n";

        Stream << "    ";

        for(int i = 0; i < RawData.size(); i++)
        {
            uint8_t value = RawData.at(i);

            Stream << "0x"
                   << QString("%1").arg(value, 2, 16, QLatin1Char('0')).toUpper();

            if(i < RawData.size() - 1)
            {
                Stream << ", ";
            }

            if(((i + 1) % 16) == 0 && (i < RawData.size() - 1))
            {
                Stream << "\r\n    ";
            }
        }

        Stream << "\r\n};\r\n\r\n";

        for(int f = 0; f < FontMetaList.size(); f++)
        {
            Stream << "const FONT_Char_t FONT_" << f << "_Chars[] =\r\n";
            Stream << "{\r\n";

            const auto &CharList = FontCharMeta[f];

            for(int c = 0; c < CharList.size(); c++)
            {
                const CharMeta &M = CharList[c];

                Stream << "    { "
                       << (int)M.Code       << ", "
                       << (int)M.Width      << ", "
                       << (int)M.Height     << ", "
                       << (int)M.BearingX   << ", "
                       << (int)M.BearingY   << ", "
                       << (int)M.Advance    << ", "
                       << M.BitmapSize      << ", "
                       << "&FONT_RawData["  << M.Offset << "] }";

                if(c < CharList.size() - 1)
                {
                    Stream << ",";
                }
                
                Stream << "\r\n";
            }

            Stream << "};\r\n\r\n";
        }

        //---------------------------------------------------------
        // Font table
        //---------------------------------------------------------

        Stream << "const FontEntry_t FONT_Table[] =\r\n";
        Stream << "{\r\n";

        for(int f = 0; f < FontMetaList.size(); f++)
        {
            Stream << "    { "
              //     << (int)FontHeights[f]     << ", "             // TODO fix
              //     << (int)FontInterlines[f]  << ", "
                   << f                       << ", "
                   << FontCharMeta[f].size()  << ", "
                   << "FONT_" << f << "_Chars }";

            if(f < FontMetaList.size() - 1)
            {
                Stream << ",";
            }
            
            Stream << "\r\n";
        }

        Stream << "};\r\n\r\n";

        Stream << "//-------------------------------------------\r\n\r\n";

        Stream << "#endif // DIGINI_USE_GRAFX\r\n";
        File.close();
    }
}


void MainWindow::ExtractFontInfo(QVector<uint8_t>* pRawData, uint8_t Char)
{
    m_OffsetFontHeader.append(pRawData->size());          // Kept the offset for this character header
    pRawData->append(Char);                               // 0     - Put the character value
    Append_uint16(pRawData, (uint16_t)0x0000);            // 1-2   - Reserve space for data size
    pRawData->append(0x00);                               // 3     - Reserve space for left bearing
    pRawData->append(0x00);                               // 4     - Reserve space for right bearing
    pRawData->append(0x00);                               // 5     - Reserve space for width Pixel
    pRawData->append(0x00);                               // 6     - Reserve space for height Pixel
    pRawData->append(0x00);                               // 7     - Reserve space for width
    pRawData->append(0x00);                               // 8     - Reserve space for Offset Y
    pRawData->append(0x00);                               // 9     - Reserve space for compression
    Append_uint32(pRawData, (uint32_t)0x00000000);        // 10-13 - Reserve space for offset in raw data

    m_TotalCharCount++;
    m_InFontCharCount++;
}

void MainWindow::SaveEachCharFont(QVector<uint8_t>* pRawData, uint8_t Char)
{
    QPixmap* pPix = new QPixmap(SAMPLING_BOX_X_SIZE, SAMPLING_BOX_Y_SIZE);
    QPainter* pPainter = new QPainter(pPix);
    QImage Image;
    uint16_t Count = 0;
    uint8_t CompressionMethod;
    QVector<uint8_t> InputData;
    uint32_t OffsetFontHeader = m_OffsetFontHeader[m_TotalCharCount];
    int     Size;
    bool FoundPixel = false;

    // Add new min/max X
    m_MinX.append(SAMPLING_BOX_X_SIZE);
    m_MaxX.append(0);
    m_MinY.append(SAMPLING_BOX_Y_SIZE);
    m_MaxY.append(0);
    m_Width.append(m_pFontMetric->horizontalAdvance(QChar(Char)));
    m_LeftBearing.append(m_pFontMetric->leftBearing(QChar(Char)));
    m_RightBearing.append(m_pFontMetric->leftBearing(QChar(Char)));

    if(Char >= '0' && Char <='9')
    {
        if(m_pFontMetric->horizontalAdvance(QChar(Char)) > m_MaxX_FixedFont)
        {
            m_MaxX_FixedFont = m_pFontMetric->horizontalAdvance(QChar(Char));
        }
    }

    // Prepare Pix map for character drawing white in black
    pPainter->setPen(Qt::white);
    pPainter->fillRect(0, 0, SAMPLING_BOX_X_SIZE, SAMPLING_BOX_Y_SIZE, Qt::black);
    pPainter->setFont(*m_pFont);
    pPainter->drawText(QPoint(20, m_pFontMetric->height()), QString("%1").arg(Char));
    Image = pPix->toImage();

    // Found the sampling rectangle size
    for(uint8_t y = 0; y < SAMPLING_BOX_Y_SIZE; y++)
    {
        for(uint8_t x = 0; x < SAMPLING_BOX_X_SIZE; x++)
        {
            QRgb ColorPixel;

            ColorPixel = Image.pixel(x, y);
            if((ColorPixel & 0x00FFFFFF) != 0x00000000)
            {
                FoundPixel = true;
                if(x < m_MinX[m_TotalCharCount]) m_MinX[m_TotalCharCount] = x;
                if(x > m_MaxX[m_TotalCharCount]) m_MaxX[m_TotalCharCount] = x;
                if(y < m_MinY[m_TotalCharCount]) m_MinY[m_TotalCharCount] = y;
                if(y > m_MaxY[m_TotalCharCount]) m_MaxY[m_TotalCharCount] = y;
            }
        }
    }

    // Write the raw data offset address in character structure only if character has data (example 'SPACE' has no data)
    if(FoundPixel == true)
    {
        Replace_uint32(pRawData, OffsetFontHeader + 10, pRawData->size());      // Write raw data offset for this character
    }

    // Copy rectangle found data in linear data for compression
    for(uint8_t y = m_MinY[m_TotalCharCount]; y <= m_MaxY[m_TotalCharCount]; y++)
    {
        for(uint8_t x = m_MinX[m_TotalCharCount]; x <= m_MaxX[m_TotalCharCount]; x++)
        {
            uint8_t Data;

            Data = qGray(Image.pixel(x, y));
            InputData.append(Data);                                                 // Put data into temporary buffer for compression if true
            Count++;
        }
    }

    if(Count)
    {
        // Copy raw data from InputData to pRawData
        for(uint16_t i = 0; i < Count; i++)
        {
            pRawData->append(InputData[i]);
        }

        // Write datasize for this font
        Replace_uint16(pRawData, OffsetFontHeader + 1, Count);

        int8_t lb = m_pFontMetric->leftBearing(QChar(Char));

        if(lb < 0)
        {
            lb--;
            lb++;
        }

        pRawData->replace(OffsetFontHeader + 3, uint8_t(lb));
        int8_t rb = m_pFontMetric->rightBearing(QChar(Char));
        pRawData->replace(OffsetFontHeader + 4, uint8_t(rb));
        pRawData->replace(OffsetFontHeader + 5,  (m_MaxX[m_TotalCharCount] - m_MinX[m_TotalCharCount]) + 1);
        pRawData->replace(OffsetFontHeader + 6,  (m_MaxY[m_TotalCharCount] - m_MinY[m_TotalCharCount]) + 1);
    }

    pRawData->replace(OffsetFontHeader + 7,  m_pFontMetric->horizontalAdvance(QChar(Char)));     // outside because space need width but has no data
    m_TotalCharCount++;
}

#if 0 

typedef struct
{
    uint8_t  code;        // ASCII ou extended
    uint8_t  width;
    uint8_t  height;
    uint8_t  bearingX;
    uint8_t  bearingY;
    uint8_t  advance;
    uint16_t bitmapSize;  // taille du bitmap en octets
    const uint8_t *bitmap;
} FontChar_t;

typedef struct
{
    uint8_t     height;        // rempli après extraction
    uint8_t     interline;     // rempli après extraction
    uint32_t    fontID;        // Count dans ton code
    uint8_t     charCount;     // nombre de caractères
    const FontChar_t *chars;   // tableau de caractères
} FontEntry_t;


typedef struct
{
    uint16_t      fontCount;
    const FontEntry_t *fonts;
} FontFile_t;



typedef enum
{
    FONT_Roboto_12,
    FONT_Roboto_16,
    FONT_Arial_10,
    FONT_Arial_14,
    FONT_Count
} FontId_t;


const uint8_t g_FontRawData[] =
{
    0x02, 0x00,  // fontCount = 2
    0x0C, 0x02,  // height/interline font 0
    0x10, 0x03,  // height/interline font 1
    ...
    // tout ton pRawData ici
};

const FontEntry_t g_FontTable[FONT_Count] =
{
    [FONT_Roboto_12] =
    {
        .height    = 12,
        .interline = 2,
        .fontID    = 0,
        .charCount = 96,
        .chars     = g_Font0_Chars
    },

    [FONT_Roboto_16] =
    {
        .height    = 16,
        .interline = 3,
        .fontID    = 1,
        .charCount = 96,
        .chars     = g_Font1_Chars
    }
};


#ifndef FONT_DATA_H
#define FONT_DATA_H

#include <stdint.h>

typedef enum
{
    FONT_Roboto_12,
    FONT_Roboto_16,
    FONT_Arial_10,
    FONT_Arial_14,
    FONT_Count
} FontId_t;

extern const uint8_t g_FontRawData[];
extern const FontEntry_t g_FontTable[FONT_Count];

#endif


#include "FontData.h"

const uint8_t g_FontRawData[] =
{
    // ton pRawData ici
};

const FontChar_t g_Font0_Chars[] =
{
    // offsets dans g_FontRawData
};

const FontChar_t g_Font1_Chars[] =
{
    // offsets dans g_FontRawData
};

const FontEntry_t g_FontTable[FONT_Count] =
{
    { 12, 2, 0, 96, g_Font0_Chars },
    { 16, 3, 1, 96, g_Font1_Chars },
};


#endif

// ************************************************************************************************

void MainWindow::on_TableFont_cellClicked(int row, int column)
{
    Q_UNUSED(column);
    LoadFont(row);
}

// ************************************************************************************************
// *
// *  Function
// *
// ************************************************************************************************

void MainWindow::InitFont()
{
    ui->TableFont->setColumnWidth(0,140);            // Font Name
    ui->TableFont->setColumnWidth(1,80);             // Font Size
    ui->TableFont->setColumnWidth(2,200);            // Font Style

    ui->TableFont->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);    // Only tab with file is resizable
    ui->TableFont->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    ui->TableFont->horizontalHeader()->setStretchLastSection(true);

    m_pFontComboBoxDelegate->clear();
    m_pFontComboBoxDelegate->addItems("6");
    m_pFontComboBoxDelegate->addItems("7");
    m_pFontComboBoxDelegate->addItems("8");
    m_pFontComboBoxDelegate->addItems("9");
    m_pFontComboBoxDelegate->addItems("10");
    m_pFontComboBoxDelegate->addItems("11");
    m_pFontComboBoxDelegate->addItems("12");
    m_pFontComboBoxDelegate->addItems("14");
    m_pFontComboBoxDelegate->addItems("16");
    m_pFontComboBoxDelegate->addItems("18");
    m_pFontComboBoxDelegate->addItems("20");
    m_pFontComboBoxDelegate->addItems("22");
    m_pFontComboBoxDelegate->addItems("24");
    m_pFontComboBoxDelegate->addItems("26");
    m_pFontComboBoxDelegate->addItems("28");
    m_pFontComboBoxDelegate->addItems("32");
    m_pFontComboBoxDelegate->addItems("36");
    m_pFontComboBoxDelegate->addItems("42");
    m_pFontComboBoxDelegate->addItems("48");
    m_pFontComboBoxDelegate->addItems("56");
    m_pFontComboBoxDelegate->addItems("72");

    ResetFontGUI();
}

// ************************************************************************************************

void MainWindow::ResetFontGUI()
{
    ui->ButtonAddFont->setEnabled(true);
    ui->ButtonRemoveFont->setDisabled(true);
    ui->ButtonUpFont->setDisabled(true);
    ui->ButtonDownFont->setDisabled(true);
    ui->LabelFontName->setText("");
    ui->LabelFontSize->setText("");
    ui->LabelFontProperties ->setText("");
    ui->checkBoxAlphaFont->setChecked(true);
    ui->checkBoxNumericFont->setChecked(true);
    ui->checkBoxSymbolFont->setChecked(true);
    ui->checkBoxFixed->setChecked(true);
    //ui->checkBoxExtraSymbolFont->setChecked(true);
    //ui->checkBoxLatinFont->setChecked(true);
    ClearTable(ui->TableFont);
    ClearSceneFont();
    CheckBoxActivation(false);
}

// ************************************************************************************************

void MainWindow::ClearSceneFont()
{
    m_SceneFont.clear();
    ui->graphicsViewFont->setScene(&m_SceneFont);
    m_SceneExample.clear();
    ui->graphicsViewExample->setScene(&m_SceneExample);
}

// ************************************************************************************************

void MainWindow::LoadFont(int row)
{
    QGraphicsTextItem* pItem;
    int Height = 0;
    int HeightOff;

    Q_UNUSED(row);

    ClearSceneFont();
    UpdateFontGUI(row);

    if(m_Font.size() != 0)
    {
        if(row == -1)
        {
            row = ui->TableFont->currentRow();
        }

        QFontMetrics FontMetric(m_Font[row]);
        HeightOff = FontMetric.height();

        // Added image to the scene
        if(ui->checkBoxAlphaFont->isChecked() == true)
        {
            pItem = new QGraphicsTextItem();
            pItem->setFont(m_Font[row]);
            pItem->setDefaultTextColor(Qt::black);
            pItem->setPos(0,Height);
            pItem->setPlainText("The quick brown fox jumps over the lazy dog");
            m_SceneFont.addItem(pItem);
            Height += HeightOff;
        }

        if(ui->checkBoxNumericFont->isChecked() == true)
        {
            if(ui->checkBoxFixed->isChecked() == true)                  // Print with fixed spacing
            {
                QFontMetrics FontMetric(m_Font[row]);
                uint8_t MaxSizeChar = 0;
                uint8_t thisSizeChar;
                int OffsetChar = 0;

                for(uint8_t c = '0'; c <= '9'; c++)
                {
                    thisSizeChar = FontMetric.horizontalAdvance(QChar(c));              // TODO This is modded for QT 6.5.. might not work as intended
                    if(thisSizeChar > MaxSizeChar)
                    {
                        MaxSizeChar = thisSizeChar;
                    }
                }
                for(uint8_t c = 0; c <= 9; c++)
                {
                    pItem = new QGraphicsTextItem();
                    pItem->setFont(m_Font[row]);
                    pItem->setDefaultTextColor(Qt::black);
                    pItem->setPos(OffsetChar, Height);
                    pItem->setPlainText(QString("%1").arg(c));
                    OffsetChar += MaxSizeChar;
                    m_SceneFont.addItem(pItem);
                }
            }
            else                                                        // Print in normal spacing
            {
                pItem = new QGraphicsTextItem();
                pItem->setFont(m_Font[row]);
                pItem->setDefaultTextColor(Qt::black);
                pItem->setPos(0,Height);
                pItem->setPlainText("0123456789");
                m_SceneFont.addItem(pItem);
            }
            Height += HeightOff;
        }

        if(ui->checkBoxSymbolFont->isChecked() == true)
        {
            pItem = new QGraphicsTextItem();
            pItem->setFont(m_Font[row]);
            pItem->setDefaultTextColor(Qt::black);
            pItem->setPos(0,Height);
            pItem->setPlainText("!\"#$%'()*+,-./:;<=>?@[\\]^_`{|}");
            m_SceneFont.addItem(pItem);
            Height += HeightOff;
        }

        if(ui->checkBoxExtraSymbolFont->isChecked() == true)
        {
            pItem = new QGraphicsTextItem();
            pItem->setFont(m_Font[row]);
            pItem->setDefaultTextColor(Qt::black);
            pItem->setPos(0,Height);
            pItem->setPlainText("°©®±");
            m_SceneFont.addItem(pItem);
            Height += HeightOff;
        }

        if(ui->checkBoxLatinFont->isChecked() == true)
        {
            pItem = new QGraphicsTextItem();
            pItem->setFont(m_Font[row]);
            pItem->setDefaultTextColor(Qt::black);
            pItem->setPos(0,Height);
            pItem->setPlainText("ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝ\nàáâãäåæèéêëìíîïðñòóôõöøùúûüýÞÿ");
            m_SceneFont.addItem(pItem);
           // Height += HeightOff;          // if in future other if are added for any reason, then uncomment this line
        }

        ui->LabelFontName->setText(m_Font[row].family());
        ui->LabelFontSize->setText(QString("%1").arg(m_Font[row].pointSize()));
        QString Properties = "";
        if(m_Font[row].bold()       == false)               Properties.append("Normal");
        else                                                Properties.append("Bold");
        if(m_Font[row].italic()     == true)                Properties.append(", Italic");
        if(m_SamplingFont[row] & SAMPLING_FIXED_NUMERIC)    Properties.append("\nNumeric Fixed Width");
        ui->LabelFontProperties->setText(Properties);

        if(m_Font[row].bold()       == false) ui->checkBoxBold->setChecked(false);
        else                                  ui->checkBoxBold->setChecked(true);
        if(m_Font[row].italic()     == false) ui->checkBoxItalic->setChecked(false);
        else                                  ui->checkBoxItalic->setChecked(true);

        SetTableProperties(row);
        DisplayExample(row);
    }
}


// ************************************************************************************************

void MainWindow::SetTableProperties(int row)
{
    QString Properties = "";
    if(m_Font[row].bold()       == false)               Properties.append("Normal");
    else                                                Properties.append("Bold");
    if(m_Font[row].italic()     == true)                Properties.append(", Italic");
    if(m_SamplingFont[row] & SAMPLING_FIXED_NUMERIC)    Properties.append(", Numeric Fixed Width");
    QTableWidgetItem *FontPropertiesItem = new QTableWidgetItem(Properties);
    FontPropertiesItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    ui->TableFont->setItem(row, 2, FontPropertiesItem);
}

// ************************************************************************************************

void MainWindow::InsertNewRowInTableFont(int row, QString Name, QString Size, QString Properties)
{
    ui->TableFont->insertRow(row);
    QTableWidgetItem *FontNameItem = new QTableWidgetItem(Name);
    QTableWidgetItem *FontSizeItem = new QTableWidgetItem(Size);
    QTableWidgetItem *FontPropertiesItem  = new QTableWidgetItem(Properties);
    FontNameItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

    // This will put the row name in red since we did not find the Font installed in the system
    if(QFont(Name).exactMatch() == false)
    {
        FontNameItem->setForeground(QColor(255,0,0));
        m_IsAllFontValid = false;
    }

    FontSizeItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
    FontPropertiesItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    ui->TableFont->setItem(row, 0, FontNameItem);
    ui->TableFont->setItem(row, 1, FontSizeItem);
    ui->TableFont->setItem(row, 2, FontPropertiesItem);

    checkValidFont();   // to update StatusBar
}

// ************************************************************************************************

void MainWindow::UpdateFontGUI(int row)
{
    // Update CheckBox

    ui->checkBoxAlphaFont->setChecked       ((m_SamplingFont[row] & SAMPLING_ALPHA)         ? true : false);
    ui->checkBoxNumericFont->setChecked     ((m_SamplingFont[row] & SAMPLING_NUMERIC)       ? true : false);
    ui->checkBoxSymbolFont->setChecked      ((m_SamplingFont[row] & SAMPLING_SYMBOL)        ? true : false);
    ui->checkBoxExtraSymbolFont->setChecked ((m_SamplingFont[row] & SAMPLING_EXTRA_SYMBOL)  ? true : false);
    ui->checkBoxLatinFont->setChecked       ((m_SamplingFont[row] & SAMPLING_LATIN)         ? true : false);

    if(ui->checkBoxNumericFont->isChecked() == true)
    {
        ui->checkBoxFixed->setChecked       ((m_SamplingFont[row] & SAMPLING_FIXED_NUMERIC) ? true : false);
    }
    else
    {
        ui->checkBoxFixed->setChecked(false);
        ui->checkBoxFixed->setEnabled(false);
    }

    // Update Button

    if((ui->TableFont->rowCount() > 0) && (ui->TableFont->currentIndex().row() >= 0))
    {
        // An image is selected then button remove is active
        ui->ButtonRemoveFont->setEnabled(true);

        // Also radio button are activated
        CheckBoxActivation(true);
    }
    else
    {
        // No image selected then button remove is deactivated
        ui->ButtonRemoveFont->setDisabled(true);

        // Also radio button are activated
        CheckBoxActivation(false);

        // Clear all label
        ui->LabelFontName->clear();
        ui->LabelFontSize->clear();
        ui->LabelFontProperties->clear();

        // Clear viewport
    }

    if(row > 0)
    {
        ui->ButtonUpFont->setEnabled(true);
    }
    else
    {
        ui->ButtonUpFont->setDisabled(true);
    }

    if((row + 1) < ui->TableFont->rowCount())
    {
        ui->ButtonDownFont->setEnabled(true);
    }
    else
    {
        ui->ButtonDownFont->setDisabled(true);
    }

}

// ************************************************************************************************

void MainWindow::CheckBoxActivation(bool state)
{
    ui->checkBoxAlphaFont->setEnabled(state);
    ui->checkBoxNumericFont->setEnabled(state);
    ui->checkBoxSymbolFont->setEnabled(state);
    ui->checkBoxExtraSymbolFont->setEnabled(state);
    ui->checkBoxLatinFont->setEnabled(state);
    ui->checkBoxBold->setEnabled(state);
    ui->checkBoxItalic->setEnabled(state);
    if(ui->checkBoxNumericFont->isChecked()) ui->checkBoxFixed->setEnabled(state);
}

// ************************************************************************************************

void MainWindow::DisplayExample(int row)
{
    // helpful to debug font save...

    QPixmap* pPix = new QPixmap(SAMPLING_BOX_X_SIZE, SAMPLING_BOX_Y_SIZE);
    QPainter* pPainter = new QPainter(pPix);
    QFontMetrics FontMetric(m_Font[row]);
    QImage Image;

    pPainter->fillRect(0, 0, SAMPLING_BOX_X_SIZE, SAMPLING_BOX_Y_SIZE, Qt::black);
    pPainter->setFont(m_Font[row]);
    pPainter->setPen(Qt::white);
    pPainter->drawText(QPoint(20, FontMetric.height()), QString("%1").arg('a'));
    Image = pPix->toImage();
    m_SceneExample.clear();
    m_SceneExample.addPixmap(*pPix);
}

// ************************************************************************************************

void MainWindow::checkValidFont()
{
    if(m_IsAllFontValid == false)
    {
        m_SpecialNote = "One or more font are not available on the system.";
        UpdateStatusBar();
    }
}

// ************************************************************************************************
