
/*
   Copyright(c) 2023 Alain Royer.
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
#include "AddingAudio.h"
#include "Utility.h"
//#include "qmediaplayer.h"
//#include <QAudioOutput>
//#include <QAudioDecoder>

// ************************************************************************************************
// *
// *  Slot
// *
// ************************************************************************************************

void MainWindow::on_ButtonAddAudio_clicked()
{
    QString Path;

    Path   = m_AudioDir.absolutePath();
    m_pLoadAudio = new AddingAudio(Path, m_DisplaySize);
    connect(m_pLoadAudio, SIGNAL(AddAudio(sLoadingAudioInfo)), this, SLOT(AddAudio(sLoadingAudioInfo)));
    connect(m_pLoadAudio, SIGNAL(CloseAddAudio()), this, SLOT(CloseAddAudio()));
    connect(m_pLoadAudio, SIGNAL(SaveConfig(QString)), this, SLOT(on_SetNewPathAudio(QString)));

    m_pLoadAudio->show();
}

// ************************************************************************************************

void MainWindow::CloseAddAudio()
{
    disconnect(m_pLoadAudio, SIGNAL(AddAudio(sLoadingAudioInfo)), this, SLOT(AddAudio(sLoadingAudioInfo)));
    disconnect(m_pLoadAudio, SIGNAL(CloseAddAudio()), this, SLOT(CloseAddAudio()));
    disconnect(m_pLoadAudio, SIGNAL(SaveConfig(QString)), this, SLOT(on_SetNewPathAudio(QString)));
    delete m_pLoadAudio;
}

// ************************************************************************************************

void MainWindow::on_ButtonRemoveAudio_clicked()
{
    int row;

    // Get the selected row
    row = ui->TableAudio->currentIndex().row();

    //Remove Row
    ui->TableAudio->removeRow(row);

    // Remove raw data element
    uint32_t Index = m_AudioInfo[row].RawIndex;
    size_t   Size  = m_AudioInfo[row].DataSize;
    m_SkinSize    -= Size;

    // If skin size is 0, there is nothing to save
    setSkinHasUnsavedData((m_SkinSize != 0) ? true : false);

    m_RawAudio.remove(Index, Size);

    // Remove element from info structure
    ID_Code ID(m_AudioInfo[row].ID);
    clearStateID_Code(m_pInUseCode, ID.getCode());
    m_AudioInfo.remove(row);

    // Readjust all index in m_AudioInfo
    int Count = m_AudioInfo.count();
    for(int i = 0; i < Count; i++)
    {
        if(m_AudioInfo[i].RawIndex > Index)
        {
            m_AudioInfo[i].RawIndex -= Size;
        }
    }

    row--;

    // Update Image GUI for all widget
    if(row >= ui->TableAudio->rowCount())
    {
        ui->TableAudio->selectRow(row - 1);
    }

    UpdateAudioGUI(row);
}

// ************************************************************************************************

void MainWindow::on_ButtonUpAudio_clicked()
{
    sAudioInfo AudioInfoCopy;
    int row;

    // Get the selected row
    row = ui->TableAudio->currentIndex().row();

    // Swap info structure
    AudioInfoCopy        = m_AudioInfo[row - 1];
    m_AudioInfo[row - 1] = m_AudioInfo[row];
    m_AudioInfo[row]     = AudioInfoCopy;

    // Swap element in table
    InsertNewRowInTableAudio(row - 1,
                             ui->TableAudio->item(row, 0)->text(),
                             ui->TableAudio->item(row, 1)->text(),
                             ui->TableAudio->item(row, 2)->text(),
                             ui->TableAudio->item(row, 3)->text());

    ui->TableAudio->selectRow(row - 1);
    ui->TableAudio->removeRow(row + 1);
}

// ************************************************************************************************

void MainWindow::on_ButtonDownAudio_clicked()
{
    sAudioInfo AudioInfoCopy;
    int Row;
    //int RowCount;

    // Get the selected row
    Row = ui->TableAudio->currentIndex().row();

    if((Row + 1) < ui->TableAudio->rowCount())
    {
        // Swap info structure
        AudioInfoCopy        = m_AudioInfo[Row + 1];
        m_AudioInfo[Row + 1] = m_AudioInfo[Row];
        m_AudioInfo[Row]     = AudioInfoCopy;

        // Swap element in table
        InsertNewRowInTableAudio(Row + 2,
                                 ui->TableAudio->item(Row, 0)->text(),
                                 ui->TableAudio->item(Row, 1)->text(),
                                 ui->TableAudio->item(Row, 2)->text(),
                                 ui->TableAudio->item(Row, 3)->text());

        ui->TableAudio->removeRow(Row);
        ui->TableAudio->selectRow(Row + 1);
    }
}

// ************************************************************************************************

void MainWindow::on_TableAudio_cellClicked(int row, int column)
{
    Q_UNUSED(column);
    UpdateAudioGUI(row);
}

// ************************************************************************************************

void MainWindow::on_TableAudio_cellDoubleClicked(int row, int column)
{
    QString Item;
    int     Value;
    UpdateAudioGUI(row);

    Item  = ui->TableAudio->item(row, column)->text();
    if(column == 1)
    {
        Value = getWidgetTypeFromText(Item);
        m_pAudioComboBoxDelegate->setEditorValue(Value);
    }
}

// ************************************************************************************************

void MainWindow::on_TableAudio_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    Q_UNUSED(currentColumn);
    Q_UNUSED(previousRow);
    Q_UNUSED(previousColumn);
    UpdateAudioGUI(currentRow);
}

// ************************************************************************************************

void MainWindow::on_TableAudio_itemChanged(QTableWidgetItem *item)
{
    QString CellData;
    int     Row;
    int     Column;

    Row    = item->row();
    Column = item->column();

    if((Row + 1) < ui->TableAudio->rowCount())
    {

        CellData = ui->TableAudio->item(Row, Column)->text();

        if(Column == 1)
        {
            // Get type from ComboBox
            eWidgetType Type = getWidgetTypeFromText(CellData);
            ID_Code ID(Type, 0);
            int FreeID = getNextFreeNumber_Up(m_pInUseCode, ID.getCode());
            ID.setNumber(FreeID);

            if(m_AudioInfo[Row].ID != ID.getCode())
            {
                // Clear old code
                ID_Code OldID(m_ImageInfo[Row].ID);
                clearStateID_Code(m_pInUseCode, OldID);

                // Set new code
                m_AudioInfo[Row].ID = ID.getCode();
                setStateID_Code(m_pInUseCode, ID);

                QTableWidgetItem *CodeID_Item  = new QTableWidgetItem(ID.getCodeText());
                CodeID_Item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
                ui->TableAudio->setItem(Row, 2, CodeID_Item);
                setSkinHasUnsavedData(true);
            }
        }

        if(Column == 2)
        {
            ID_Code ID(CellData);

            if(m_ImageInfo[Row].ID != ID.getCode())
            {
                setStateID_Code(m_pInUseCode, ID.getCode());
                m_ImageInfo[Row].ID = ID.getCode();
                setSkinHasUnsavedData(true);
            }
        }
    }
}

// ************************************************************************************************

void MainWindow::AddAudio(sLoadingAudioInfo LoadingInfo)
{
    uint16_t    ItemCount;
    sAudioInfo  AudioInfo;
   // QAudio      Audio

    // reload Path, if it was change by LoadImage class
   // m_currentDir.setPath(GetPathFromXML());  We nedd to set a path for Audio

    ID_Code ID(TYPE_AUDIO, 0);
    int FreeID = getNextFreeNumber_Up(m_pInUseCode, ID.getCode());
    ID.setNumber(FreeID);

    m_SkinSize += LoadingInfo.DataSize;
    setSkinHasUnsavedData(true);

    // Create audio entry
    AudioInfo.ID           = ID.getCode();
    //ImageInfo.Description  = "-";
    AudioInfo.Filename     = LoadingInfo.Filename;
    //AudioInfo.Size         = LoadingInfo.Size;
    AudioInfo.DataSize     = LoadingInfo.DataSize;
    //AudioInfo.PixelFormat  = LoadingInfo.PixelFormat;
    AudioInfo.RawIndex     = m_RawAudio.size();

   // Audio.load(LoadingInfo.PathAndFilename);                                                        // Load Audio

    // Add this image entry structure in vector
    m_AudioInfo.append(AudioInfo);

    // Fill Table
    ItemCount = ui->TableAudio->rowCount();

    InsertNewRowInTableImage(ItemCount,
                             LoadingInfo.Filename,
                             tr("AU - Audio"),
                             ID.getCodeText(),
                             tr("%1 kB").arg(int((LoadingInfo.DataSize + 1023) / 1024)));


    UpdateAudioGUI(m_AudioInfo.size() - 1);
}


// ************************************************************************************************
// *
// *  Function
// *
// ************************************************************************************************

void MainWindow::InitAudio()
{
    ui->TableAudio->setColumnWidth(0,390);             // Filename
    ui->TableAudio->setColumnWidth(1,120);             // Code type
    ui->TableAudio->setColumnWidth(2,120);             // Code ID
    ui->TableAudio->setColumnWidth(3,49);              // Data size

    ui->TableAudio->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);    // Only tab with file is resizable
    ui->TableAudio->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    ui->TableAudio->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);

    m_pAudioComboBoxDelegate->clear();
  //  m_pAudioComboBoxDelegate->addItems("BK - Background");
  //  m_pAudioComboBoxDelegate->addItems("BT - Button");
  //  m_pAudioComboBoxDelegate->addItems("FT - Font");
  //  m_pAudioComboBoxDelegate->addItems("GL - Glyph");
  //  m_pAudioComboBoxDelegate->addItems("IC - Icon");
  //  m_pAudioComboBoxDelegate->addItems("IM - Image");

    // controls is the ui stuff
   // connect(controls, SIGNAL(play()), m_Player, SLOT(play()));
   // connect(controls, SIGNAL(pause()), m_Player, SLOT(pause()));
   // connect(controls, SIGNAL(stop()), m_Player, SLOT(stop()));
   // connect(controls, SIGNAL(changeVolume(int)), m_Player, SLOT(setVolume(int)));

    // for the slot.. use
    //controls->setVolume(m_Player->volume());

    ResetAudioGUI();
}

// ************************************************************************************************

void MainWindow::ResetAudioGUI()
{
    ui->ButtonUpAudio->setDisabled(true);
    ui->ButtonDownAudio->setDisabled(true);
    ui->ButtonRemoveAudio->setDisabled(true);
    ui->LabelFilenameAudio->setText("");
    //ui->LabelSizeAudio->setText("");
   // ui->LabelPixelFormatAudio->setText("");
   // ui->LabelRawDataSizeAudio->setText("");
    ClearTable(ui->TableAudio);
//    ClearSceneAudio();  // ???
}

// ************************************************************************************************
/*
void MainWindow::ClearSceneAudio()
{
    m_SceneAudio.clear();
    ui->graphicsViewAImage->setSceneRect(0, 0, m_DisplaySize.width(), m_DisplaySize.height());
    CheckerPattern(&m_SceneImage);
    ui->graphicsViewImage->setScene(&m_SceneImage);
}
*/
// ************************************************************************************************

void MainWindow::UpdateAudioGUI(int row)
{
    // Update Button
    if((ui->TableAudio->rowCount() > 0) && (ui->TableAudio->currentIndex().row() >= 0))
    {
        // An image is selected then button remove is active
        ui->ButtonRemoveAudio->setEnabled(true);
    }
    else
    {
        ui->ButtonRemoveAudio->setDisabled(true);
    }

    if(row > 0)
    {
        ui->ButtonUpAudio->setEnabled(true);
    }
    else
    {
        ui->ButtonUpAudio->setDisabled(true);
    }

    if((row + 1) < ui->TableAudio->rowCount())
    {
        ui->ButtonDownAudio->setEnabled(true);
    }
    else
    {
        ui->ButtonDownAudio->setDisabled(true);
    }

    if(ui->TableAudio->verticalScrollBar()->isVisible() == true)
    {
        ui->TableAudio->setColumnWidth(1,110);  // Resize Define name so size appear correctly in window
        ui->TableAudio->setColumnWidth(2,110);
    }
    else
    {
        ui->TableAudio->setColumnWidth(1,120);
        ui->TableAudio->setColumnWidth(2,120);
    }

    // Update view port
    //m_SceneImage.clear();

    if(row >= 0)
    {
        // Update Label
        ui->LabelFilenameAudio->setText(m_ImageInfo[row].Filename);
        //ui->LabelPixelFormatImage->setText(GetFormat(m_ImageInfo[row].PixelFormat));
        //i->LabelSizeAudio->setText(PrintSize(m_ImageInfo[row].Size));
        //ui->LabelRawDataSizeImage->setText(QString("%1 Bytes").arg(m_ImageInfo[row].DataSize));


        // Loading raw data in QImage (Patch cannot get a pointer from QVector)
        //QImage Image( m_ImageInfo[row].Size, m_AudioInfo[row]..PixelFormat);

        int Count = m_AudioInfo[row].RawIndex;


     //   QGraphicsPixmapItem *PixmapItem = m_SceneImage.addPixmap(QPixmap::fromImage(Image));                // Add the image on top of the checker pattern
      //  QPoint Point = CenterPoint(m_ImageInfo[row].Size, m_DisplaySize);                                   // Calculate Center position for the image
     //  PixmapItem->setPos(Point);                                                                          // Then set it
    }

    UpdateStatusBar();
}

// ************************************************************************************************

void MainWindow::InsertNewRowInTableAudio(int row, QString File, QString CodeType, QString CodeID, QString Size)
{
    ui->TableAudio->insertRow(row);
    QTableWidgetItem *fileNameItem = new QTableWidgetItem(File);
    QTableWidgetItem *CodeTypeItem = new QTableWidgetItem(CodeType);
    QTableWidgetItem *CodeID_Item  = new QTableWidgetItem(CodeID);
    QTableWidgetItem *SizeItem     = new QTableWidgetItem(Size);
    fileNameItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    CodeTypeItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
    CodeID_Item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
    SizeItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

    ui->TableAudio->setItem(row, 0, fileNameItem);
    ui->TableAudio->setItem(row, 1, CodeTypeItem);
    ui->TableAudio->setItem(row, 2, CodeID_Item);
    ui->TableAudio->setItem(row, 3, SizeItem);

    ID_Code ID(CodeID);
    setStateID_Code(m_pInUseCode, ID.getCode());
}

// ************************************************************************************************

