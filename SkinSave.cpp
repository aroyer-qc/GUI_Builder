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

//#include "ui_mainwindow.h"
#include "SkinSave.h"
#include "compression.h"
#include "utility.h"
#include "qxmlputget.h"
//#include <iostream>
#include <qfontdatabase.h>
#include <QtEndian>

#include <QDir>
#include <ft2build.h>
#include <freetype/ftsnames.h>
#include FT_FREETYPE_H

// ************************************************************************************************

void MainWindow::Save()
{
    m_pProgress = new Progress("Saving skin file", "Compressing", "Saving", m_SkinName);
    m_pSkinSave = new class SkinSave(m_SkinName, this);
    connect(m_pSkinSave, SIGNAL(SaveProgress(QString,int)), m_pProgress, SLOT(on_UpdateProgress(QString,int)));
    connect(m_pSkinSave, SIGNAL(SaveDone()),                this,        SLOT(on_SaveDone()));
    m_pSkinSave->start();
}

// ************************************************************************************************

void MainWindow::on_SaveDone()
{
    disconnect(m_pSkinSave, SIGNAL(SaveProgress(QString, int)), m_pProgress, SLOT(on_UpdateProgress(QString, int)));
    disconnect(m_pSkinSave, SIGNAL(SaveDone()),                 this,        SLOT(on_SaveDone()));
    m_pSkinSave->exit();

    delete m_pProgress;
    delete m_pSkinSave;

    setSkinHasUnsavedData(false);
    UpdateStatusBar();

    if(m_IsNeedToClearDataAfterSave == true)
    {
        ClearAllData();
        ResetAllSkinTab();
        m_IsNeedToClearDataAfterSave = false;
    }

    if(m_IsNeedToCloseAfterSave == true)
    {
        this->close();
    }

    if(m_IsNeedCompleteFileOpen == true)
    {
        on_actionOpen_Skin_triggered();
        m_IsNeedCompleteFileOpen = false;
    }
}

// ************************************************************************************************
//
//  Class SkinSave ( QThread )
//
// ************************************************************************************************

SkinSave::SkinSave(QString SkinPathAndFileName, QObject* parent) : QThread(parent)
{
    m_SkinPathAndFileName = SkinPathAndFileName;

    m_pSkinConfig         = ((MainWindow*)parent)->GetSkinConfig();

    // Image
    m_pRawImageData       = ((MainWindow*)parent)->getRawImageDataPtr();        // Get pointer on Image data
    m_pImageInfo          = ((MainWindow*)parent)->getImageInfoPtr();           // Get pointer on Image structure information

    // Font
    m_pFontSamplingInfo   = ((MainWindow*)parent)->getFontSamplingInfoPtr();    // Get Sampling information of font
    m_pFontInfo           = ((MainWindow*)parent)->getFontInfoPtr();

    // Audio
    m_pRawAudioData       = ((MainWindow*)parent)->getRawAudioDataPtr();        // Get pointer on Audio data
    m_pAudioInfo          = ((MainWindow*)parent)->getAudioInfoPtr();           // Get pointer on Audio structure information

    // Label
    m_pRawLabelData       = ((MainWindow*)parent)->getRawLabelDataPtr();        // Get pointer on Label data
    m_pLabelInfo          = ((MainWindow*)parent)->getLabelInfoPtr();           // Get pointer on Label structure information

   // Label List
    m_pRawLabelListData   = ((MainWindow*)parent)->getRawLabelListDataPtr();    // Get pointer on Label List data
    m_pLabelListInfo      = ((MainWindow*)parent)->getLabelListInfoPtr();       // Get pointer on Label List structure information
}

// ************************************************************************************************

SkinSave::~SkinSave()
{
}

// ************************************************************************************************

void SkinSave::run(void)
{
    QVector<uint8_t>    FileRawData;
    QFile               File;
    QFileInfo           FileInfo(m_SkinPathAndFileName);
  
    emit SaveProgress("", 0);

    // Process data
    m_ThisBlockOfData     = 0;
    m_PreviousBlockOfData = 0;


    m_ImageCount = m_pImageInfo->size();
    m_FontCount  = m_pFontInfo->count();
    m_AudioCount = m_pAudioInfo->count();
    //m_LabelCount = m_pLabelInfo->count();
    //m_labelListCount = m_plabelListInfo->count();

    File.setFileName(FileInfo.baseName() + ".skn");
    File.open(QIODevice::WriteOnly);

    if(m_ImageCount != 0)
    {
        SaveImageInfo(&FileRawData);                            // Save image info structure
        SaveAllImage(&FileRawData);                             // Try each compression method, and save best for each image in file
        m_PreviousBlockOfData = m_ThisBlockOfData;
        m_ThisBlockOfData = FileRawData.size();
    }

    if(m_FontCount != 0)
    {
        SaveFontInfo(&FileRawData);                             // Save font info structure
        SaveAllFont(&FileRawData);                              // Try each compression method, and save best for each image in file
        m_PreviousBlockOfData = m_ThisBlockOfData;
        m_ThisBlockOfData = FileRawData.size();
    }

    if(m_AudioCount != 0)
    {
        SaveAudioInfo(&FileRawData);                            // Save audio info structure
        SaveAllAudio(&FileRawData);                             // Try each compression method, and save best for each audio in file
        //CompressAllAudio(&FileRawData);                       // Try each compression method, and save best for each audio in file
        m_PreviousBlockOfData = m_ThisBlockOfData;
        m_ThisBlockOfData = FileRawData.size();
    }

#if 0
    if(m_LabelCount != 0)
    {
        SaveLabelInfo(&FileRawData);                            // Save label info structure
        SaveAllLabel(&FileRawData);                             // Try each compression method, and save best for all label in file
        m_PreviousBlockOfData = m_ThisBlockOfData;
        m_ThisBlockOfData = FileRawData.size();
    }

    if(m_LabelListCount != 0)
    {
        SaveLabelListInfo(&FileRawData);                        // Save label list info structure
        SaveAllLabelListFont(&FileRawData);                     // Try each compression method, and save best for all label list in file
        m_PreviousBlockOfData = m_ThisBlockOfData;
        m_ThisBlockOfData = FileRawData.size();
    }
#endif

    // Save all data to skin file
    File.write((const char*)FileRawData.data(), FileRawData.size());

    // Close file and exit
    File.close();
    CreateXML(FileInfo.absolutePath() + "/" + FileInfo.baseName() + ".skn");
    emit SaveProgress("", 100);

    if(m_pSkinConfig->UseBinary == true)
    {
        CreateBinary();
    }

    emit SaveDone();
    exec();
}

// ************************************************************************************************

void SkinSave::SaveImageInfo(QVector<uint8_t>* pFileRawData)            // TODO modify to handle the Binary type
{
    GFX_PixelFormat_e PixelFormat;

    if(m_ThisBlockOfData != 0)
    {
        Replace_uint32(pFileRawData, m_PreviousBlockOfData, pFileRawData->size());
    }

    // Reserve space in compressed data for the next block of data
    Append_uint32(pFileRawData, (uint32_t)0x00000000);

    // Put the type of data block (here Image)
    Append_uint16(pFileRawData, (uint16_t)SkinBlockType_e::SKIN_IMAGE);

    // Put image count in compressed data
    Append_uint16(pFileRawData, (uint16_t)m_ImageCount);

    m_OffsetImageHeader = pFileRawData->size();

    for(int Count = 0; Count < m_ImageCount; Count++)
    {
        Append_uint32(pFileRawData, (uint32_t)m_pImageInfo->at(Count).ID);                // 0  - ID
        Append_uint32(pFileRawData, (uint32_t)m_pImageInfo->at(Count).DataSize);          // 4  - size compressed data

        if(m_pImageInfo->at(Count).PixelFormat == QImage::Format_ARGB32) PixelFormat = ARGB8888;
        if(m_pImageInfo->at(Count).PixelFormat == QImage::Format_RGB16)  PixelFormat = RGB565;

        pFileRawData->append((uint8_t)PixelFormat);                                       // 8  - Pixel format                                                            // Compression
        Append_uint16(pFileRawData, (uint16_t)m_pImageInfo->at(Count).Size.width());      // 9  - Width image
        Append_uint16(pFileRawData, (uint16_t)m_pImageInfo->at(Count).Size.height());     // 11 - Height image
        pFileRawData->append(0);                                                          // 13 - Compression
        Append_uint32(pFileRawData, (uint32_t)0);                                         // 14 - Offset in data
    }
}

// ************************************************************************************************

void SkinSave::SaveFontInfo(QVector<uint8_t>* pFileRawData)            // TODO modify to handle the Binary type
{
    if(m_ThisBlockOfData != 0)
    {
        Replace_uint32(pFileRawData, m_PreviousBlockOfData, pFileRawData->size());
    }

    Append_uint32(pFileRawData, (uint32_t)0x00000000);                  // Reserve space in compressed data for the next block of data
    Append_uint16(pFileRawData, (uint16_t)SkinBlockType_e::SKIN_FONT);  // Put type of data block (here Font)
    Append_uint16(pFileRawData, (uint16_t)m_FontCount);                 // Put font count in compressed data

    m_TotalCharCount = 0;                                               // Reset value of the character count

    for(int Count = 0; Count < m_FontCount; Count++)
    {
        m_OffsetFontHeight.append(pFileRawData->size());                // Kept offset so we can modifed then later
        pFileRawData->append(0x00);                                     // Reserve space for height for this font
        pFileRawData->append(0x00);                                     // Reserve space for interline for this font
    }

    for(int Count = 0; Count < m_FontCount; Count++)
    {
        m_InFontCharCount = 0;

        m_pFont       = &m_pFontInfo->at(Count);
        m_pFontMetric = new QFontMetrics(*m_pFont);

        // Put font count in compressed data
        Append_uint32(pFileRawData, Count);                             // For now we use count number as ID

        // Reserve space in compressed data for character count
        m_OffsetFontCharCountHeader = pFileRawData->size();             // Keep offset for later
        pFileRawData->append(0x00);                                     // Value rewritten when count is knowned

        // Sample all alpha character
        if((m_pFontSamplingInfo->at(Count) & SAMPLING_ALPHA) != 0)
        {
            for(uint32_t CharCount = 'a'; CharCount <= 'z'; CharCount++) ExtractFontInfo(pFileRawData, char(CharCount));
            for(uint32_t CharCount = 'A'; CharCount <= 'Z'; CharCount++) ExtractFontInfo(pFileRawData, char(CharCount));
        }

        // Sample all numeric character
        if((m_pFontSamplingInfo->at(Count) & SAMPLING_NUMERIC) != 0)
        {
            int MaxX;

            for(uint32_t CharCount = '0'; CharCount <= '9'; CharCount++) ExtractFontInfo(pFileRawData, char(CharCount));
        }

        // Sample all symbol character
        if((m_pFontSamplingInfo->at(Count) & SAMPLING_SYMBOL) != 0)
        {
            for(uint32_t CharCount = ' '; CharCount <= '/'; CharCount++) ExtractFontInfo(pFileRawData, char(CharCount));
            for(uint32_t CharCount = ':'; CharCount <= '@'; CharCount++) ExtractFontInfo(pFileRawData, char(CharCount));
            for(uint32_t CharCount = '['; CharCount <= '`'; CharCount++) ExtractFontInfo(pFileRawData, char(CharCount));
            for(uint32_t CharCount = '{'; CharCount <= '~'; CharCount++) ExtractFontInfo(pFileRawData, char(CharCount));
        }

        // Sample all extra symbol character
        if((m_pFontSamplingInfo->at(Count) & SAMPLING_EXTRA_SYMBOL) != 0)
        {
            ExtractFontInfo(pFileRawData, 153); //'tm'
            ExtractFontInfo(pFileRawData, 169); //'©'
            ExtractFontInfo(pFileRawData, 174); //'®'
            ExtractFontInfo(pFileRawData, 176); //'°'
            ExtractFontInfo(pFileRawData, 185); //'±'
        }

        // Sample all latin character
        if((m_pFontSamplingInfo->at(Count) & SAMPLING_LATIN) != 0)
        {
            for(uint32_t CharCount = 192/*'À'*/; CharCount <= 214/*'Ö'*/; CharCount++) ExtractFontInfo(pFileRawData, char(CharCount));
            for(uint32_t CharCount = 216/*'Ø'*/; CharCount <= 246/*'ö'*/; CharCount++) ExtractFontInfo(pFileRawData, char(CharCount));
            for(uint32_t CharCount = 248/*'ø'*/; CharCount <= 254/*'þ'*/; CharCount++) ExtractFontInfo(pFileRawData, char(CharCount));
        }

        pFileRawData->replace(m_OffsetFontCharCountHeader, m_InFontCharCount); // Write character count
    }
}

// ************************************************************************************************

void SkinSave::SaveAudioInfo(QVector<uint8_t>* pFileRawData)            // TODO modify to handle the Binary type
{
    Q_UNUSED(pFileRawData);
}

// ************************************************************************************************
#if 0
void SkinSave::SaveLabelInfo(QVector<uint8_t>* pFileRawData)            // TODO modify to handle the Binary type
{
    Q_UNUSED(pFileRawData);
}
#endif
// ************************************************************************************************
#if 0
void SkinSave::SaveLabelListInfo(QVector<uint8_t>* pFileRawData)        // TODO modify to handle the Binary type
{
    Q_UNUSED(pFileRawData);
}
#endif
// ************************************************************************************************
void SkinSave::SaveAllImage(QVector<uint8_t>* pFileRawData)
{
    int     Progress;
    uint8_t CompressionMethod;
    int     Size;

    // Scan all image and compress all image in memory
    for(int Count = 0; Count < m_ImageCount; Count++)
    {

        Size = pFileRawData->size();
        Replace_uint32(pFileRawData, (Count * IMAGE_HEADER_SIZE) + 14 + m_OffsetImageHeader, Size);     // Write offset for this image data
        ChangeEndianness(Count);                            // Change Endianness of raw data before save in needed

        // Write data for this image
        CompressionMethod = Compress(pFileRawData,
                            m_pRawImageData,
                            m_pImageInfo->at(Count).DataSize,
                            m_pImageInfo->at(Count).RawIndex);

        ChangeEndianness(Count);                            // Replace raw data into original state for more editing if it was needed
        
        // Write compression method for this image
        pFileRawData->replace((Count * IMAGE_HEADER_SIZE) + 13 + m_OffsetImageHeader, CompressionMethod);

        Size = pFileRawData->size() - Size;

        // Write raw compress datasize for this image
        Replace_uint32(pFileRawData, (Count * IMAGE_HEADER_SIZE) + 4 + m_OffsetImageHeader, uint32_t(Size));

        Progress = (Count * 98) / m_ImageCount;
        emit SaveProgress("", Progress);
    }
}

// ************************************************************************************************

void SkinSave::SaveAllFont(QVector<uint8_t>* pFileRawData)
{
    m_TotalCharCount = 0;                                      // Reset value of the character count

    for(int Count = 0; Count < m_FontCount; Count++)
    {
        uint32_t StartOffsetFont = m_TotalCharCount;
        uint32_t StartNumericalOffsetFont;
        uint8_t MinY = 0xFF;
        uint8_t MaxY = 0x00;

        m_pFont       = &m_pFontInfo->at(Count);
        m_pFontMetric = new QFontMetrics(*m_pFont);
        m_MaxX_FixedFont = 0;

        // Sample all alpha character
        if((m_pFontSamplingInfo->at(Count) & SAMPLING_ALPHA) != 0)
        {
            for(uint32_t CharCount = 'a'; CharCount <= 'z'; CharCount++) SaveEachCharFont(pFileRawData, char(CharCount));
            for(uint32_t CharCount = 'A'; CharCount <= 'Z'; CharCount++) SaveEachCharFont(pFileRawData, char(CharCount));
        }

        // Sample all numeric character
        if((m_pFontSamplingInfo->at(Count) & SAMPLING_NUMERIC) != 0)
        {
            StartNumericalOffsetFont = m_TotalCharCount;

            for(uint32_t CharCount = '0'; CharCount <= '9'; CharCount++)
            {
                SaveEachCharFont(pFileRawData, char(CharCount));
            }
        }

        // Sample all symbol character
        if((m_pFontSamplingInfo->at(Count) & SAMPLING_SYMBOL) != 0)
        {
            for(uint32_t CharCount = ' '; CharCount <= '/'; CharCount++) SaveEachCharFont(pFileRawData, char(CharCount));
            for(uint32_t CharCount = ':'; CharCount <= '@'; CharCount++) SaveEachCharFont(pFileRawData, char(CharCount));
            for(uint32_t CharCount = '['; CharCount <= '`'; CharCount++) SaveEachCharFont(pFileRawData, char(CharCount));
            for(uint32_t CharCount = '{'; CharCount <= '~'; CharCount++) SaveEachCharFont(pFileRawData, char(CharCount));
        }

        // Sample all extra symbol character
        if((m_pFontSamplingInfo->at(Count) & SAMPLING_EXTRA_SYMBOL) != 0)
        {
            SaveEachCharFont(pFileRawData, 153); //'tm'
            SaveEachCharFont(pFileRawData, 169); //'©'
            SaveEachCharFont(pFileRawData, 174); //'®'
            SaveEachCharFont(pFileRawData, 176); //'°'
            SaveEachCharFont(pFileRawData, 185); //'±'
        }

        // Sample all latin character
        if((m_pFontSamplingInfo->at(Count) & SAMPLING_LATIN) != 0)
        {
            for(uint32_t CharCount = 192/*'À'*/; CharCount <= 214/*'Ö'*/; CharCount++) SaveEachCharFont(pFileRawData, char(CharCount));
            for(uint32_t CharCount = 216/*'Ø'*/; CharCount <= 246/*'ö'*/; CharCount++) SaveEachCharFont(pFileRawData, char(CharCount));
            for(uint32_t CharCount = 248/*'ø'*/; CharCount <= 254/*'þ'*/; CharCount++) SaveEachCharFont(pFileRawData, char(CharCount));
        }

        // Rescan for lowest Y minimum and
        for(uint32_t CharCount = StartOffsetFont; CharCount < m_TotalCharCount; CharCount++)
        {
            if(MinY > m_MinY[CharCount]) MinY = m_MinY[CharCount];
            if(MaxY < m_MaxY[CharCount]) MaxY = m_MaxY[CharCount];
        }

        // Write the new height for the font
        pFileRawData->replace(m_OffsetFontHeight[Count], (MaxY - MinY) + 1);


        // Rescan numerical value for max width
        //if((m_pFontSamplingInfo->at(Count) & SAMPLING_FIXED_NUMERIC) != 0)
        {
            for(uint32_t CharCount = StartNumericalOffsetFont; CharCount < (StartNumericalOffsetFont) + 10; CharCount++)
            {
                pFileRawData->replace( m_OffsetFontHeader[CharCount] + 7, m_MaxX_FixedFont);
            }
        }

        // We subtract this absolute minimum from height, and all other minimum and maximum
        for(uint32_t CharCount = StartOffsetFont; CharCount < m_TotalCharCount; CharCount++)
        {
             pFileRawData->replace(m_OffsetFontHeader[CharCount] + 8,  m_MinY[CharCount] -= MinY);
        }
    }
}

// ************************************************************************************************

void SkinSave::SaveAllAudio(QVector<uint8_t>* pFileRawData)
{
    Q_UNUSED(pFileRawData);
}

// ************************************************************************************************
#if 0
void SkinSave::SaveAllLabel(QVector<uint8_t>* pFileRawData)
{
    Q_UNUSED(pFileRawData);
}

// ************************************************************************************************

void SkinSave::SaveAllLabelList(QVector<uint8_t>* pFileRawData)
{
    Q_UNUSED(pFileRawData);
}
#endif
// ************************************************************************************************

void SkinSave::ExtractFontInfo(QVector<uint8_t>* pFileRawData, uint8_t Char)
{
    m_OffsetFontHeader.append(pFileRawData->size());          // Kept the offset for this character header

    pFileRawData->append(Char);                               // 0     - Put the character value
    Append_uint16(pFileRawData, (uint16_t)0x0000);            // 1-2   - Reserve space for data size
    pFileRawData->append(0x00);                               // 3     - Reserve space for left bearing
    pFileRawData->append(0x00);                               // 4     - Reserve space for right bearing
    pFileRawData->append(0x00);                               // 5     - Reserve space for width Pixel
    pFileRawData->append(0x00);                               // 6     - Reserve space for height Pixel
    pFileRawData->append(0x00);                               // 7     - Reserve space for width
    pFileRawData->append(0x00);                               // 8     - Reserve space for Offset Y
    pFileRawData->append(0x00);                               // 9     - Reserve space for compression
    Append_uint32(pFileRawData, (uint32_t)0x00000000);        // 10-13 - Reserve space for offset in raw data

    m_TotalCharCount++;
    m_InFontCharCount++;
}

// ************************************************************************************************

void SkinSave::SaveEachCharFont(QVector<uint8_t>* pFileRawData, uint8_t Char)
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
        Replace_uint32(pFileRawData, OffsetFontHeader + 10, pFileRawData->size());      // Write raw data offset for this character
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
        Size = pFileRawData->size();
        CompressionMethod = Compress(pFileRawData, &InputData, Count, 0);
        Size = pFileRawData->size() - Size;
        
        // Write raw compress datasize for this image
        Replace_uint16(pFileRawData, OffsetFontHeader + 1, uint16_t(Size));
        int8_t lb = m_pFontMetric->leftBearing(QChar(Char));

        if(lb < 0)
        {
            lb--;
            lb++;
        }

        pFileRawData->replace(OffsetFontHeader + 3, uint8_t(lb));
        int8_t rb = m_pFontMetric->rightBearing(QChar(Char));
        pFileRawData->replace(OffsetFontHeader + 4, uint8_t(rb));
        pFileRawData->replace(OffsetFontHeader + 5,  (m_MaxX[m_TotalCharCount] - m_MinX[m_TotalCharCount]) + 1);
        pFileRawData->replace(OffsetFontHeader + 6,  (m_MaxY[m_TotalCharCount] - m_MinY[m_TotalCharCount]) + 1);
        pFileRawData->replace(OffsetFontHeader + 9, CompressionMethod);
    }

    pFileRawData->replace(OffsetFontHeader + 7,  m_pFontMetric->horizontalAdvance(QChar(Char)));     // outside because space need width but has no data
    m_TotalCharCount++;
}

// ************************************************************************************************

void SkinSave::CreateXML(QString Path)
{
    QXmlPut xmlPut("Skin");

    xmlPut.putInt("Width", m_pSkinConfig->DisplaySize.width());
    xmlPut.putInt("Height", m_pSkinConfig->DisplaySize.height());
    xmlPut.putString("Endian", (m_pSkinConfig->Endianess == LITTLE_ENDIAN) ? "Little" : "Big");
    xmlPut.putInt("Size", 0);           // Size of the data in skin (raw uncompressed data)
    xmlPut.descend("binary");
    xmlPut.putString("Enable", (m_pSkinConfig->UseBinary == false) ? "No" : "Yes");
    xmlPut.putInt("Memory Offset", m_pSkinConfig->MemoryOffset);
    xmlPut.putInt("Image Max", m_pSkinConfig->ImageMax);
    xmlPut.putInt("Font Max", m_pSkinConfig->FontMax);
    xmlPut.putInt("Audio Max", m_pSkinConfig->AudioMax);
    xmlPut.putInt("Label Max", m_pSkinConfig->LabelMax);
    xmlPut.putInt("Label List Max", m_pSkinConfig->LabelListMax);
    xmlPut.rise();

    //-------------------------------------------------------------------------
    // Image information
    xmlPut.descend("Image");
    xmlPut.putInt("Count", m_ImageCount);
    xmlPut.putInt("Offset", 0);         // Offset of the image data block in skin (raw uncompressed data)
    xmlPut.putInt("Size", 0);           // Size of the image data block in skin (raw uncompressed data)

    for(int i = 0; i < m_ImageCount; i++)
    {
        xmlPut.descend("Data");
        xmlPut.putString("File", m_pImageInfo->at(i).Filename);
        xmlPut.putInt("OffSet", m_pImageInfo->at(i).RawIndex);
        xmlPut.putInt("Size", m_pImageInfo->at(i).DataSize);
        xmlPut.putInt("Format", m_pImageInfo->at(i).PixelFormat);
        xmlPut.putInt("Width", m_pImageInfo->at(i).Size.width());
        xmlPut.putInt("Height", m_pImageInfo->at(i).Size.height());
        xmlPut.rise();
    }
    xmlPut.rise();

    //-------------------------------------------------------------------------
    // Font Information
    xmlPut.descend("Font");
    xmlPut.putInt("Count", m_FontCount);
    xmlPut.putInt("Offset", 0);         // Offset of the font data block in skin (raw uncompressed data)
    xmlPut.putInt("Size", 0);           // Size of the font data block in skin (raw uncompressed data)

    for(int i = 0; i < m_FontCount; i++)
    {
        xmlPut.descend("Data");
        xmlPut.putFont("Family", m_pFontInfo->at(i));
        xmlPut.putInt("Offset", 0);
        xmlPut.putInt("Size", 0);
        xmlPut.putInt("Option", m_pFontSamplingInfo->at(i));
        FontMetaData_t MetaData = ReadFontMetadata(GetFontFile(m_pFontInfo->at(i).family()));
        xmlPut.putString("Filename",MetaData.FileName);
        xmlPut.putString("Manufacturer", MetaData.Manufacturer);
        xmlPut.putString("Designer", MetaData.Designer);
        xmlPut.rise();
    }

    //-------------------------------------------------------------------------
    // Audio Information
    #if 0 // do Audio
    xmlPut.descend("Audio");
    xmlPut.putInt("Count", m_AudioCount);
    xmlPut.putInt("Offset", 0);         // Offset of the audio data block in skin (raw uncompressed data)
    xmlPut.putInt("Size", 0);           // Size of the audio data block in skin (raw uncompressed data)

    for(int i = 0; i < m_AudioCount; i++)
    {
        xmlPut.descend("Data");
        xmlPut.putString("File", m_pAudioInfo->at(i).Filename);
        xmlPut.putString("Type", m_pAudioInfo->at(i).xxx);
        xmlPut.putString("Sampling", m_pAudioInfo->at(i).xxx);
        xmlPut.rise();
    }
    xmlPut.rise();
    #endif

    //-------------------------------------------------------------------------
    // Label Information
    #if 0 // do Label
    xmlPut.descend("Label");
    xmlPut.putInt("Count", m_LabelCount);
    xmlPut.putInt("Offset", 0);         // Offset of the label data block in skin (raw uncompressed data)
    xmlPut.putInt("Size", 0);           // Size of the label data block in skin (raw uncompressed data)

    for(int i = 0; i < m_LabelCount; i++)
    {
        xmlPut.descend("Data");
        xmlPut.putString("Label", m_pLabelInfo->at(i).Filename);
        xmlPut.rise();
    }
    xmlPut.rise();

    //-------------------------------------------------------------------------
    // Label List Information
    xmlPut.descend("Label list");
    xmlPut.putInt("Count", m_LabelListCount);
    xmlPut.putInt("Offset", 0);         // Offset of the label list data block in skin (raw uncompressed data)
    xmlPut.putInt("Size", 0);           // Size of the label list data block in skin (raw uncompressed data)

    for(int i = 0; i < m_LabelListCount; i++)
    {
        xmlPut.putString("Label List", m_pLabelList);
        xmlPut.descend("Data");
        xmlPut.putString("Number Of Label", .......);
        for(Loop for the number of label in the list 
        {
            xmlPut.putString("Label ID", .......);
        }
        xmlPut.rise();
    }
    xmlPut.rise();
    #endif

    xmlPut.save(Path);
}

// ************************************************************************************************

QString SkinSave::GetFontFile(const QString& fontName)
{
    QStringList fontFiles;

    QList<QFontDatabase::WritingSystem> writingSystems = QFontDatabase::writingSystems(fontName);
    for (QFontDatabase::WritingSystem ws : writingSystems)
    {
        QStringList fonts = QFontDatabase::families(ws);
        for(const QString& f : fonts)
        {
            if(f == fontName)
            {
                fontFiles.append(f);
                return fontFiles.join(",");
            }
        }
    }

    return "";
}

// ************************************************************************************************

FontMetaData_t SkinSave::ReadFontMetadata(QString fontFile)
{
    FontMetaData_t MetaData = {0};
    FT_Library library;
    FT_Face face;

    // Initialize FreeType library
    if(FT_Init_FreeType(&library))
    {
        return {0};
    }

    QString PathAndFileName = getFontFilePath(fontFile);
    QByteArray utf8Bytes = PathAndFileName.toUtf8();
    const char* charPointer = utf8Bytes.constData();

    // Load the font file
    if(FT_New_Face(library, charPointer, 0, &face))
    {
        FT_Done_FreeType(library);
        return {0};
    }

    // Extract and display manufacturer and designer metadata
    FT_UInt nameCount = FT_Get_Sfnt_Name_Count(face);

    for(FT_UInt i = 0; i < nameCount; ++i)
    {
        FT_SfntName sfntName;

        if(FT_Get_Sfnt_Name(face, i, &sfntName) == 0)
        {
            const char* utf8Data = reinterpret_cast<const char*>(sfntName.string);
            QString nameValue = QString::fromUtf8(utf8Data, sfntName.string_len);

            switch (sfntName.name_id) {
            case 8: // Manufacturer
                MetaData.Manufacturer = nameValue;
                break;
            case 9: // Designer
                MetaData.Designer = nameValue;
                break;
            default:
                break;
            }
        }
    }

    QFileInfo fileInfo(PathAndFileName);
    MetaData.FileName = fileInfo.fileName();

    // Clean up
    FT_Done_Face(face);
    FT_Done_FreeType(library);

    return MetaData;
}

// ************************************************************************************************

QString SkinSave::getFontFilePath(const QString& fontFamily)
{
    // Set the font directory (Windows example)
    QString fontDirPath = "C:/Windows/Fonts/";

    QDir fontDir(fontDirPath);
    QStringList fontFiles = fontDir.entryList(QStringList() << "*.ttf" << "*.otf", QDir::Files);

    FT_Library library;
    if (FT_Init_FreeType(&library))
    {
        qDebug() << "Failed to initialize FreeType library.";
        return QString();
    }

    foreach (const QString& fileName, fontFiles)
    {
        QString filePath = fontDirPath + fileName;

        FT_Face face;
        if (FT_New_Face(library, filePath.toUtf8().constData(), 0, &face) == 0)
        {
            // Match the font family
            QString faceFamilyName = QString::fromUtf8(face->family_name);
            if (faceFamilyName.compare(fontFamily, Qt::CaseInsensitive) == 0)
            {
                FT_Done_Face(face);
                FT_Done_FreeType(library);
                return filePath; // Return the matching file path
            }
            FT_Done_Face(face);
        }
    }

    FT_Done_FreeType(library);
    return QString(); // No match found
}

// ************************************************************************************************

void SkinSave::ChangeEndianness(int Offset)
{
    // Applied change in endianess to raw data
    if(m_pSkinConfig->Endianess == LITTLE_ENDIAN)
    {
        uint32_t EndIndex = m_pImageInfo->at(Offset).RawIndex + m_pImageInfo->at(Offset).DataSize;

        if(m_pImageInfo->at(Offset).PixelFormat == QImage::Format_RGB16)
        {
            for(uint32_t Index = m_pImageInfo->at(Offset).RawIndex; Index < EndIndex; Index += 2)
            {
                ChangeEndianAt_uint16(m_pRawImageData, Index);
            }
        }
        else  // Any 32 bits format
        {
            for(uint32_t Index = m_pImageInfo->at(Offset).RawIndex; Index < EndIndex; Index += 4)
            {
                ChangeEndianAt_uint32(m_pRawImageData, Index);
            }
        }
    }
}

// ************************************************************************************************

void SkinSave::CreateBinary(void)
{
//    SystemState_e    State;
    uint32_t         NextBlock = 0;
    uint16_t         BlockTp;
    SkinBlockType_e  BlockType;
    size_t           ReadCount = 0;
    size_t           TotalToLoad;
    QFile            FileSource;
    QFile            FileDestination;
    QFileInfo        FileSourceInfo(m_SkinPathAndFileName);
    QFileInfo        FileDestinationInfo(m_SkinPathAndFileName);

    FileSource.setFileName(FileSourceInfo.baseName() + ".skn");
    FileSource.open(QIODevice::ReadOnly);
    FileDestination.setFileName(FileSourceInfo.baseName() + ".bin");
    FileDestination.open(QIODevice::WriteOnly);

    TotalToLoad = FileSource.size();

    do
    {
        FileSource.seek(qint64(NextBlock));
        ReadCount += Get_uint32_t(&FileSource, &NextBlock);
        ReadCount += Get_uint16_t(&FileSource, &BlockTp);
        BlockType = SkinBlockType_e(BlockTp);

        switch(BlockType)
        {
            case SkinBlockType_e::SKIN_IMAGE:
            {
          //      m_pCompressionMethod = (uint8_t*)GRAFX_DECOMPRESS_METHOD_ADDRESS;
          //      memset(m_pCompressionMethod, 0x00, sizeof(uint8_t) * DBASE_MAX_SKIN_IMAGE_QTY);
          //      m_pDataSize          = (uint32_t*)GRAFX_DATA_SIZE_ADDRESS;
          //      memset(m_pDataSize, 0x00, sizeof(uint32_t) * DBASE_MAX_SKIN_IMAGE_QTY);

           //     if((State = Get_uint16_t(&m_ItemCount)) == SYS_READY)
           //     {
             //       if((State = GetImageInfo()) == SYS_READY)
               //     {
             //           State = DeCompressAllImage();
            //        }
           //     }

            }
            break;

            case int(SkinBlockType_e::SKIN_FONT):
            {
           //     m_pCompressionMethod = (uint8_t*)GRAFX_DECOMPRESS_METHOD_ADDRESS;
           //     memset(m_pCompressionMethod, 0x00, sizeof(uint8_t) * DBASE_MAX_SKIN_FONT_QTY * 256);

          //      if((State = Get_uint16_t(&m_ItemCount)) == SYS_READY)
            //    {
              //      if((State = GetFontInfo()) == SYS_READY)
                //    {
                  //      State = DeCompressAllFont();
                    //}
               // }
            }
            break;
        }
    }
    while(NextBlock != 0);

    FileSource.close();
    FileDestination.close();

}

// code in Digini to generate the data in ram.. so i will use this to build the binary to load into flash of project

#if 0

#if defined(KIT_735IG)
  #define GRAFX_RAW_INPUT_DATA_ADDRESS                  0x700BF400      // 128K             
  #define GRAFX_DECODE_ARRAY_ADDRESS                    0x700DF400      // 224K
  #define GRAFX_APPEND_ARRAY_ADDRESS                    0x70117400      // 224K
  #define GRAFX_PREFIX_ARRAY_ADDRESS                    0x7014F400      // 224K
  #define GRAFX_DECOMPRESS_METHOD_ADDRESS               0x70187400      // 8K                   // this is where data is decompressed
  #define GRAFX_DATA_SIZE_ADDRESS                       0x70189400      // 8K                   // to find out what it is
#endif




SystemState_e SKIN_myClassTask::Load(void)
{
  .  SystemState_e    State;
  .  uint32_t         NextBlock = 0;
  .  uint16_t         BlockTp;
  .  SkinBlockType_e  BlockType;

  .  m_ReadCount = 0;

  .  // Register work area for logical drive
  .  do
  .  {
  .      m_FResult = f_mount(m_pFS, m_pDrive, 1);
  .      nOS_Sleep(10);
  .  }
  .  while(m_FResult != FR_OK);

  .  // Open source file on the internal drive (SPI Flash)
  .  m_FResult = f_open(m_pFile, m_Path, FA_OPEN_EXISTING | FA_READ);
  .  if((m_FResult = f_stat(m_Path, m_pFileInfo)) != FR_OK) return SYS_FAIL;
  .  m_TotalToLoad = f_size(m_pFile);

  .  do
  .  {
  .      if((m_FResult = f_lseek(m_pFile, NextBlock)) != FR_OK) return SYS_FAIL; // Jump to next block (first is zero)

  .      if((State = Get_uint32_t(&NextBlock)) != SYS_READY) return State;       // Get now the next block
  .      if((State = Get_uint16_t(&BlockTp))   != SYS_READY) return State;       // Get the Block type
  .      BlockType = SkinBlockType_e(BlockTp);

  .      switch(BlockType)
  .      {
  .      case SKIN_IMAGE:
  .      {
            m_pCompressionMethod = (uint8_t*)GRAFX_DECOMPRESS_METHOD_ADDRESS;
            memset(m_pCompressionMethod, 0x00, sizeof(uint8_t) * DBASE_MAX_SKIN_IMAGE_QTY);
            m_pDataSize          = (uint32_t*)GRAFX_DATA_SIZE_ADDRESS;
            memset(m_pDataSize, 0x00, sizeof(uint32_t) * DBASE_MAX_SKIN_IMAGE_QTY);

            if((State = Get_uint16_t(&m_ItemCount)) == SYS_READY)
            {
                if((State = GetImageInfo()) == SYS_READY)
                {
                    State = DeCompressAllImage();
                }
            }

  .          break;
  .      }
  .
  .      case SKIN_FONT:
  .      {
            m_pCompressionMethod = (uint8_t*)GRAFX_DECOMPRESS_METHOD_ADDRESS;
            memset(m_pCompressionMethod, 0x00, sizeof(uint8_t) * DBASE_MAX_SKIN_FONT_QTY * 256);

            if((State = Get_uint16_t(&m_ItemCount)) == SYS_READY)
            {
                if((State = GetFontInfo()) == SYS_READY)
                {
                    State = DeCompressAllFont();
                }
            }

  .          break;
  .      }
  .      }
  .      //nOS_Yield();
  .      nOS_Sleep(1);
  .  }
  .  while((NextBlock != 0) && (State == SYS_READY));

  .  f_close(m_pFile);

  .  // Unregister work area prior to discard it
  .  f_mount(nullptr, m_pDrive, 0);

  .  return State;
}
#endif

// ************************************************************************************************

void SkinSave::GetImageInfo(void)
{
    uint32_t      Dummy;
    ImageInfo_t   ImageInfo;
/*
    // Read all image information
    for(uint16_t i = 0; (i < m_ItemCount); i++)
    {
        Get_uint32_t(&Dummy);      // dummy read ID
        Get_uint32_t(&m_pDataSize[i]);
        Get_uint8_t((uint8_t*)&ImageInfo.PixelFormat);
        Get_uint16_t(&ImageInfo.Size.Width);
        Get_uint16_t(&ImageInfo.Size.Height);
        Get_uint8_t(&m_pCompressionMethod[i]);
        Get_uint32_t((uint32_t*)&ImageInfo.pPointer);      // use memory address pointer as temporary storage for in file index
        
        // write data to file at the right offset ---->   DB_Central.Set(&ImageInfo, GFX_IMAGE_INFO, i + (NUMBER_OF_STATIC_IMAGE + 1), 0);
    }
*/
}

// ************************************************************************************************

size_t SkinSave::Get_uint8_t(QFile* pFile, uint8_t* pValue)
{
    return size_t(pFile->read((char*)pValue, qint64(sizeof(uint8_t))));
}

// ************************************************************************************************

size_t SkinSave::Get_uint16_t(QFile* pFile, uint16_t* pValue)
{
    uint32_t ReadCount;

    ReadCount = size_t(pFile->read((char*)pValue, qint64(sizeof(uint16_t))));

    if(m_pSkinConfig->Endianess == BIG_ENDIAN)
    {
        uint16_t Value;

        Value  = (*pValue >> 8) & 0x00FF;
        Value |= (*pValue << 8) & 0xFF00;
        *pValue = Value;
    }

    return ReadCount;
}

// ************************************************************************************************

size_t SkinSave::Get_uint32_t(QFile* pFile, uint32_t* pValue)
{
    uint32_t ReadCount;

    ReadCount = size_t(pFile->read((char*)pValue, qint64(sizeof(uint32_t))));

    if(m_pSkinConfig->Endianess == BIG_ENDIAN)
    {
        uint32_t Value;

        Value  = (*pValue >> 24) & 0x000000FF;
        Value |= (*pValue >> 8)  & 0x0000FF00;
        Value |= (*pValue << 8)  & 0x00FF0000;
        Value |= (*pValue << 24) & 0xFF000000;
        *pValue = Value;
    }

    return ReadCount;
}

// ************************************************************************************************
