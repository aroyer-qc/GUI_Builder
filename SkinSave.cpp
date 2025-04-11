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

    // Skin Type
    m_pSkinType           = ((MainWindow*)parent)->getSkinTypePtr();            // SKIN_TYPE_BINARY or SKIN_TYPE_LOADABLE
    m_pSkinSize           = ((MainWindow*)parent)->getSkinSizePtr();            // QSize  Width (X)  and Height (Y)

    // Image
    m_pRawImageData       = ((MainWindow*)parent)->getRawImageDataPtr();        // Get pointer on Image data
    m_pImageInfo          = ((MainWindow*)parent)->getImageInfoPtr();           // Get pointer on Image structure information

    // Audio
    m_pRawAudioData       = ((MainWindow*)parent)->getRawAudioDataPtr();        // Get pointer on Audio data
    m_pAudioInfo          = ((MainWindow*)parent)->getAudioInfoPtr();           // Get pointer on Audio structure information

    // Font
    m_pFontSamplingInfo   = ((MainWindow*)parent)->getFontSamplingInfoPtr();    // Get Sampling information of font
    m_pFontInfo           = ((MainWindow*)parent)->getFontInfoPtr();

    //Endian
    m_pEndian              = ((MainWindow*)parent)->getEndianPtr();             // Get pointer on Endianess of the skin
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

    if(m_pSkinType == SKIN_TYPE_LOADABLE)
    {
        File.setFileName(FileInfo.baseName() + ".lsk");
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
    }
    else // SKIN_TYPE_BINARY (order of operation are different. raw data must be save before info)
    {
        File.setFileName(FileInfo.baseName() + ".bin");
        File.open(QIODevice::WriteOnly);

        // Reserve space in binary data for the next block of data
        AppendReservedSpace(&FileRawData, RAW_DATA_OFFSET);                    // Reserved space information header of binary data.
        
        // Only All components count can be save on header at this point.
        Replace_uint16(&FileRawData, BINARY_OFFSET_IMG_COUNT, m_ImageCount);
        Replace_uint16(&FileRawData, BINARY_OFFSET_FNT_COUNT, m_FontCount);
        Replace_uint16(&FileRawData, BINARY_OFFSET_AUD_COUNT, m_AudioCount);
     // Replace_uint16(&FileRawData, BINARY_OFFSET_LBL_COUNT, m_LabelCount);
     // Replace_uint16(&FileRawData, BINARY_OFFSET_LST_COUNT, m_LabelListCount);
        
        SaveAllImage(&FileRawData);
        SaveAllFont(&FileRawData);
        SaveAllAudio(&FileRawData);
        //SaveAllLabel(&FileRawData);
        //SaveAllLabelList(&FileRawData);
        
        // Get pointer for all struct and update the beginning of the file with the pointer
        
        SaveImageInfo(&FileRawData);                            // Save image info structure
        SaveFontInfo(&FileRawData);                             // Save font info structure
        SaveAudioInfo(&FileRawData);                            // Save audio info structure
      //SaveLabelInfo(&FileRawData);                            // Save label info structure
      //SaveLabelListInfo(&FileRawData);                        // Save label list info structure
    }

    // Save all data to skin file
    File.write((const char*)FileRawData.data(), FileRawData.size());

    // Close file and exit
    File.close();
    CreateXML(FileInfo.absolutePath() + "/" + FileInfo.baseName() + ".skn");
    emit SaveProgress("", 100);

    emit SaveDone();
    exec();
}

// ************************************************************************************************

void SkinSave::SaveImageInfo(QVector<uint8_t>* pFileRawData)            // TODO modify to handle the Binary type
{
    GFX_ePixelFormat PixelFormat;

    if(m_ThisBlockOfData != 0)
    {
        Replace_uint32(pFileRawData, m_PreviousBlockOfData, pFileRawData->size());
    }

    // Reserve space in compressed data for the next block of data
    Append_uint32(pFileRawData, (uint32_t)0x00000000);

    // Put the type of data block (here Image)
    Append_uint16(pFileRawData, (uint16_t)0x0000);

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

    Append_uint32(pFileRawData, (uint32_t)0x00000000);        // Reserve space in compressed data for the next block of data
    Append_uint16(pFileRawData, (uint16_t)0x0001);            // Put type of data block (here Font)
    Append_uint16(pFileRawData, (uint16_t)m_FontCount);       // Put font count in compressed data

    m_TotalCharCount = 0;                                   // Reset value of the character count

    for(int Count = 0; Count < m_FontCount; Count++)
    {
        m_OffsetFontHeight.append(pFileRawData->size());      // Kept offset so we can modifed then later
        pFileRawData->append(0x00);                           // Reserve space for height for this font
        pFileRawData->append(0x00);                           // Reserve space for interline for this font
    }

    for(int Count = 0; Count < m_FontCount; Count++)
    {
        m_InFontCharCount = 0;

        m_pFont       = &m_pFontInfo->at(Count);
        m_pFontMetric = new QFontMetrics(*m_pFont);

        // Put font count in compressed data
        Append_uint32(pFileRawData, Count);                   // For now we use count number as ID

        // Reserve space in compressed data for character count
        m_OffsetFontCharCountHeader = pFileRawData->size();   // Keep offset for later
        pFileRawData->append(0x00);                           // Value rewritten when count is knowned

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

        if(m_pSkinType == SKIN_TYPE_LOADABLE)
        {
            Size = pFileRawData->size();
            Replace_uint32(pFileRawData, (Count * IMAGE_HEADER_SIZE) + 14 + m_OffsetImageHeader, Size);     // Write offset for this image data
        }
        
        ChangeEndianness(Count);                            // Change Endianness of raw data before save in needed

        if(m_pSkinType == SKIN_TYPE_LOADABLE)
        {
            // Write data for this image
            CompressionMethod = Compress(pFileRawData,
                                m_pRawImageData,
                                m_pImageInfo->at(Count).DataSize,
                                m_pImageInfo->at(Count).RawIndex);
        }
        else // (m_pSkinType == SKIN_TYPE_BINARY)
        {
            std::copy(m_pRawImageData->begin() + m_pImageInfo->at(Count).RawIndex, 
                      m_pRawImageData->begin() + m_pImageInfo->at(Count).RawIndex + m_pImageInfo->at(Count).DataSize, 
                      std::back_inserter(*pFileRawData));
        }

        ChangeEndianness(Count);                            // Replace raw data into original state for more editing if it was needed
        
        if(m_pSkinType == SKIN_TYPE_LOADABLE)
        {
            // Write compression method for this image
            pFileRawData->replace((Count * IMAGE_HEADER_SIZE) + 13 + m_OffsetImageHeader, CompressionMethod);

            Size = pFileRawData->size() - Size;

            // Write raw compress datasize for this image
            Replace_uint32(pFileRawData, (Count * IMAGE_HEADER_SIZE) + 4 + m_OffsetImageHeader, uint32_t(Size));
        }
        else // (m_pSkinType == SKIN_TYPE_BINARY)
        {
            // TODO if there is something to do
        }

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
            
            if(m_pSkinType == SKIN_TYPE_LOADABLE)
            {
                InputData.append(Data);                                                 // Put data into temporary buffer for compression if true
            }
            else // (m_pSkinType == SKIN_TYPE_BINARY)
            {
                pFileRawData->append(Data);                                             // Put data directly into file data for binary if false
            }
            
            Count++;
        }
    }

    if(Count)
    {
        if(m_pSkinType == SKIN_TYPE_LOADABLE)
        {
            Size = pFileRawData->size();
            CompressionMethod = Compress(pFileRawData, &InputData, Count, 0);
            Size = pFileRawData->size() - Size;
        }
        else
        {
            Size = Count;                                                               // For binary, there is no compression, so Size is the count
        }
        
        
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

    xmlPut.putString("Type", (*m_pSkinType == SKIN_TYPE_LOADABLE) ? "Loadable Skin" : "Binary Skin");
    xmlPut.putInt("Width", m_pSkinSize->width());
    xmlPut.putInt("Height", m_pSkinSize->height());
    xmlPut.putString("Endian", (*m_pEndian == LITTLE_ENDIAN) ? "Little" : "Big");
    xmlPut.putInt("Size", 0);           // Size of the data in skin (raw uncompressed data)

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
        sFontMetaData MetaData = ReadFontMetadata(GetFontFile(m_pFontInfo->at(i).family()));
        xmlPut.putString("Filename",MetaData.FileName);
        xmlPut.putString("Manufacturer", MetaData.Manufacturer);
        xmlPut.putString("Designer", MetaData.Designer);
        xmlPut.rise();
    }

    #if 0 // do Audio
    // Audio Information
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

    #if 0 // do Label
    // Label Information
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

sFontMetaData SkinSave::ReadFontMetadata(QString fontFile)
{
    sFontMetaData MetaData = {0};
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
    if(*m_pEndian == LITTLE_ENDIAN)
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
