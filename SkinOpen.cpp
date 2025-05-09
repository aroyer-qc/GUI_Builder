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

#include "ui_mainwindow.h"
#include "SkinOpen.h"
#include "compression.h"
#include "Utility.h"
#include "qxmlputget.h"

// ************************************************************************************************

void MainWindow::Open(QString File)
{
    m_SkinName     = File;
    m_IsSkinSaveAs = false;

    //Decompressing shouls change according to type of skin

    m_pProgress = new Progress("Open skin file", "Loading", "Decompressing", m_SkinName);
    m_pSkinOpen = new SkinOpen(m_SkinName, this);
    connect(m_pSkinOpen, SIGNAL(OpenProgress(QString, int)), m_pProgress, SLOT(on_UpdateProgress(QString, int)));
    connect(m_pSkinOpen, SIGNAL(OpenDone()),                 this,        SLOT(on_OpenDone()));
    m_pSkinOpen->start();
}

// ************************************************************************************************

void MainWindow::on_OpenDone()
{
    int Count;

    disconnect(m_pSkinOpen, SIGNAL(OpenProgress(QString, int)), m_pProgress, SLOT(on_UpdateProgress(QString, int)));
    disconnect(m_pSkinOpen, SIGNAL(OpenDone()),                 this,        SLOT(on_OpenDone()));
    m_pSkinOpen->exit();

    delete m_pProgress;
    delete m_pSkinOpen;

    //----------------------------------
    // Add each found image in the table
    Count = m_ImageInfo.count();
    for(int i = 0; i < Count; i++)
    {
        ID_Code ID(m_ImageInfo[i].ID);

        ui->TableImage->blockSignals(true);
        InsertNewRowInTableImage(i,
                                 m_ImageInfo[i].Filename,
                                 "IM - Image",
                                 ID.getCodeText(),
                                 tr("%1 kB").arg(int((m_ImageInfo[i].DataSize + 1023) / 1024)));
        ui->TableImage->blockSignals(false);
    }

    m_SkinSize += m_RawImage.size();

    //---------------------------------
    // Add each found font in the table
    m_IsAllFontValide = true;
    Count = m_Font.count();
    for(int i = 0; i < Count; i++)
    {
        ui->TableFont->blockSignals(true);
        InsertNewRowInTableFont(i, m_Font[i].family(), QString("%1").arg(m_Font[i].pointSize()), " ");
        SetTableProperties(i);
        ui->TableFont->blockSignals(false);
    }

    //----------------------------------
    // Add each found audio in the table
    /*
    m_IsAllFontValide = true;
    Count = m_Audio.count();

    for(int i = 0; i < Count; i++)
    {
        ui->TableFont->blockSignals(true);
        InsertNewRowInTableFont(i,
                                m_Audio[i].Filename,
        SetTableProperties(i);
        ui->TableAudio->blockSignals(false);
    }

    And also add label!

    */

    setSkinHasUnsavedData(false);
    UpdateStatusBar();
}

// ************************************************************************************************
//
//  Class SkinOpen ( QThread )
//
// ************************************************************************************************

SkinOpen::SkinOpen(QString SkinPathAndFileName, QObject* parent) : QThread(parent)
{
    m_SkinPathAndFileName = SkinPathAndFileName;

    // Image
    m_pRawImageData       = ((MainWindow*)parent)->getRawImageDataPtr();
    m_pImageInfo          = ((MainWindow*)parent)->getImageInfoPtr();

    // Audio
    m_pRawAudioData       = ((MainWindow*)parent)->getRawAudioDataPtr();
    m_pAudioInfo          = ((MainWindow*)parent)->getAudioInfoPtr();

    // Font
    m_pFontSamplingInfo   = ((MainWindow*)parent)->getFontSamplingInfoPtr();
    m_pFontInfo           = ((MainWindow*)parent)->getFontInfoPtr();

    // Label and Label List
    //m_pLabelInfo        = ((MainWindow*)parent)->getLabelInfoPtr();
    //m_pLabelList        = ((MainWindow*)parent)->getLabelListInfoPtr();

    // Endian
    m_pEndian             = ((MainWindow*)parent)->getEndianPtr();
    
    // Skin Type
    m_pSkinType            = ((MainWindow*)parent)->getSkinTypePtr();
}

// ************************************************************************************************

SkinOpen::~SkinOpen()
{
}

// ************************************************************************************************

void SkinOpen::run(void)
{
    QVector<uint8_t>    CompxData;
    uint8_t             Data;
    QFile               File;
    QFileInfo           FileInfo(m_SkinPathAndFileName);
  
    emit OpenProgress("", 0);

    ReadXML(FileInfo.absolutePath() + "/" + FileInfo.baseName() + ".skn");
    File.setFileName(FileInfo.baseName() + ((m_pSkinType == SKIN_TYPE_LOADABLE) ? ".lsk" : ".bin"));
    File.open(QIODevice::ReadOnly);
    File.seek(0);
    int Size = File.size();
    m_NextBlockOfData = 0;

    // Read Raw Data
    for(int i = 0; i < Size; i++)
    {
        File.getChar((char*)&Data);
        CompxData.append(Data);
    }

    if(m_pSkinType == SKIN_TYPE_LOADABLE)             // Open loading skin file and fill data
    {
        // Process data
        OpenImageInfo(&CompxData);                    // Open image info structure
        DeCompressAllImage(&CompxData);               // Read all Image and Decompress according to method found in file
        // OpenAudioInfo(&CompxData);                 // Open audio info structure
        // DeCompressAllAudio(&CompxData);            // Read all Audio and Decompress according to method found in file
        // OpenLabelInfo(&CompxData);                 // Open label info structure
        // DeCompressAllLabel(&CompxData);            // Read all Label and Decompress according to method found in file
        // OpenLabelListInfo(&CompxData);             // Open label list info structure
        // DeCompressAllLabelList(&CompxData);        // Read all Label and Decompress according to method found in file
        // We don't get the data for the font, because it will be too heavy to rebuild font data from data file, we use info from skn file to rebuild the data file when it's time to save.
    }
    else // m_pSkinType == SKIN_TYPE_BINARY           Open binary skin file and fill data
    {

    }

    // Close file and exit
    File.close();
    emit OpenProgress("", 100);
    emit OpenDone();
    exec();
}

// ************************************************************************************************

void SkinOpen::DeCompressAllImage(QVector<uint8_t>* pCompxData)
{
    int      Progress;
    uint32_t OffsetInCompressData;
    uint8_t  CompressionMethod;

    // Scan all image and decompress all image in memory
    for(uint16_t Count = 0; Count < m_ImageCount; Count++)
    {
        // Get offset for this image data
        OffsetInCompressData = GetAt_uint32(pCompxData, (Count * IMAGE_HEADER_SIZE) + 14 + m_OffsetImageHeader);

        // Get compression method for this image
        CompressionMethod = pCompxData->at((Count * IMAGE_HEADER_SIZE) + 13 + m_OffsetImageHeader);

        // New Image data will start at this offset for this image in decompressed data
        (*m_pImageInfo)[Count].RawIndex = m_pRawImageData->size();

        DeCompress(m_pRawImageData,
                   pCompxData,
                   m_pImageInfo->at(Count).CompressDataSize,
                   OffsetInCompressData,
                   CompressionMethod);

        // Applied change in endianess to raw data
        if(*m_pEndian == BIG_ENDIAN)
        {
            uint32_t EndIndex = m_pImageInfo->at(Count).RawIndex + m_pImageInfo->at(Count).DataSize;

            if(m_pImageInfo->at(Count).PixelFormat == QImage::Format_RGB16)
            {
                for(uint32_t Index = m_pImageInfo->at(Count).RawIndex; Index < EndIndex; Index += 2)
                {
                    ChangeEndianAt_uint16(m_pRawImageData, Index);
                }
            }
            else
            {
                for(uint32_t Index = m_pImageInfo->at(Count).RawIndex; Index < EndIndex; Index += 4)
                {
                    ChangeEndianAt_uint32(m_pRawImageData, Index);
                }
            }
        }

        Progress = (Count * 98) / m_ImageCount;
        emit OpenProgress("", Progress);
    }
}

// ************************************************************************************************

void SkinOpen::OpenImageInfo(QVector<uint8_t>* pCompxData)
{
    sImageInfo       ImageInfo;
    GFX_ePixelFormat PixelFormat;

    // Get Offset for image count and keep acopy of it it
    m_OffsetImageCount = m_NextBlockOfData + 6;
    m_ImageCount = (int)GetAt_uint16(pCompxData, m_OffsetImageCount);

    // Get Offset for first image header and keep a copy of it
    m_OffsetImageHeader = m_NextBlockOfData + 8;

    // Get offset for the next block of raw data
    m_NextBlockOfData = (int)GetAt_uint32(pCompxData, m_NextBlockOfData);

    for(uint16_t i = 0; i < m_ImageCount; i++)
    {
        ImageInfo.ID               = (uint32_t)GetAt_uint32(pCompxData, (i * IMAGE_HEADER_SIZE) + 0 + m_OffsetImageHeader);
        ImageInfo.CompressDataSize = (size_t)GetAt_uint32(pCompxData, (i * IMAGE_HEADER_SIZE) + 4 + m_OffsetImageHeader);

        PixelFormat                = (GFX_ePixelFormat)pCompxData->at((i * IMAGE_HEADER_SIZE) + 8 + m_OffsetImageHeader);
        if(PixelFormat == ARGB8888) ImageInfo.PixelFormat = QImage::Format_ARGB32;
        if(PixelFormat == RGB565)   ImageInfo.PixelFormat = QImage::Format_RGB16;
       //  To do must add mono mode for Font !

        ImageInfo.Size.setWidth((int)GetAt_uint16(pCompxData, (i * IMAGE_HEADER_SIZE) + 9 + m_OffsetImageHeader));
        ImageInfo.Size.setHeight((int)GetAt_uint16(pCompxData, (i * IMAGE_HEADER_SIZE) + 11 + m_OffsetImageHeader));
        ImageInfo.DataSize        = ImageInfo.Size.width() * ImageInfo.Size.height() * BytesPerPixel[ImageInfo.PixelFormat];

        m_pImageInfo->append(ImageInfo);
    }
}

// ************************************************************************************************

void SkinOpen::ReadXML(QString Path)
{
    sImageInfo  ImageInfo;
    QXmlGet     xmlGet;

    xmlGet.load(Path);

    if(xmlGet.findNext("Project Type"))
    {
        *m_pSkinType = (xmlGet.getString() == "Loadable Skin") ? SKIN_TYPE_LOADABLE : SKIN_TYPE_BINARY;
    }
    else
    {
        *m_pSkinType = SKIN_TYPE_BINARY;
    }

    if(xmlGet.findNext("Endian"))
    {
        xmlGet.descend();
        xmlGet.findNext("State");
        *m_pEndian = (xmlGet.getString() == "Little") ? LITTLE_ENDIAN : BIG_ENDIAN;
        xmlGet.rise();
    }
    else
    {
        *m_pEndian = BIG_ENDIAN;
    }

    if(xmlGet.findNext("Image"))
    {
        xmlGet.descend();

        if(xmlGet.findNext("Count"))
        {
            m_ImageCount = xmlGet.getInt();
        }
        
        for(int i = 0; i < m_ImageCount; i++)
        {
            ImageInfo = m_pImageInfo->at(i);

            if(xmlGet.findNext("Data"))
            {
                xmlGet.descend();

                if(xmlGet.findNext("File"))
                {
                    ImageInfo.Filename = xmlGet.getString();
                }

                if(xmlGet.findNext("OffSet"))
                {
                    (*m_pImageInfo)[i].RawIndex = QImage::Format(xmlGet.getInt());
                }

                if(xmlGet.findNext("Size"))
                {
                    (*m_pImageInfo)[i].DataSize = size_t(xmlGet.getInt());
                }

                if(xmlGet.findNext("Format"))
                {
                    (*m_pImageInfo)[i].PixelFormat = QImage::Format(xmlGet.getInt());
                }

                if(xmlGet.findNext("Width"))
                {
                    (*m_pImageInfo)[i].DataSize = xmlGet.getInt();
                }
                
                xmlGet.rise();
            }

            m_pImageInfo->replace(i, ImageInfo);
        }
        xmlGet.rise();
    }

    if(xmlGet.findNext("Font"))
    {
        // Font Information
        xmlGet.descend();

        if(xmlGet.findNext("Count"))
        {
            m_ImageCount = xmlGet.getInt();
        }

        while(xmlGet.findNext("Family"))
        {
            m_pFontInfo->append(xmlGet.getFont());
            xmlGet.findNext("Option");
            m_pFontSamplingInfo->append(uint8_t(xmlGet.getInt()));
        }

        xmlGet.rise();
    }

    // Audio Information
    // TODO

    // Label Information
    // TODO

    // LabelList Information
    // TODO
}

// ************************************************************************************************

