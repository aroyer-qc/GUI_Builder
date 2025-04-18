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

#ifndef __SKIN_SAVE_H__
#define __SKIN_SAVE_H__

#include "mainwindow.h"
#include <QThread>
#include <QTimer>
#include <ft2build.h>
#include FT_FREETYPE_H


//                  Skin file description
//
//                   ______________________________________________
//  Image block     |                                              |
//                  | Next block of data                (uint32_t) |
//                  |______________________________________________|
//                  |                                              |
//                  | Block type 0x0000                 (uint16_t) |
//                  |______________________________________________|
//                  |                                              |
//                  | Image Count                       (uint16_t) |
//                  |______________________________________________|__
//                  |                                              |  |
//                  | Structure image information                  |  |
//                  |                                              |  |
//                  |   0-3   Image ID                  (uint32_t) |  |
//                  |   4-7   Datasize                  (uint32_t) |  |___
//                  |   8     Pixel format              (uint8_t)  |  |   |
//                  |   9-10  Width                     (uint16_t) |  |   |
//                  |   11-12 Height                    (uint16_t) |  |   |
//                  |   13    Compression               (uint8_t)  |  |   |
//                  |   14-17 Offset in data            (uint32_t) |  |  /_\
//                  |______________________________________________|__|   |
//                  |                                              |      |
//                  |   ... Repeat last structure                  |\_____|
//                  |       According to image count               |/
//                  |______________________________________________|__
//                  |                                              |  |
//                  |   Image compressed raw data                  |  |___
//                  |______________________________________________|__|   |
//                  |                                              |     /_\
//                  |   ... Repeat compressed data                 |\_____|
//                  |       According to image count               |/
//                  |______________________________________________|
//
//                   ______________________________________________
//  Font block      |                                              |
//                  | Next block of data                (uint32_t) |
//                  |______________________________________________|
//                  |                                              |
//                  | Block type 0x0001                 (uint16_t) |
//                  |______________________________________________|
//                  |                                              |
//                  | Font Count                        (uint16_t) |
//                  |______________________________________________|__
//                  |                                              |  |
//                  |   0   Height                      (uint8_t)  |  |___
//                  |   1   Interline                   (uint8_t)  |  |   |
//                  |______________________________________________|__|   |
//                  |                                              |     /_\
//                  |   ... Repeat last structure                  |\_____|
//                  |       According to font                      |/
//                  |______________________________________________|______________
//                  |                                              |              |
//                  |   0-3 Font ID                     (uint32_t) |              |
//                  |   4   Character count in font     (uint8_t)  |              |
//                  |______________________________________________|__            |
//                  |                                              |  |           |
//                  | Structure font information                   |  |           |
//                  |                                              |  |           |
//                  |   0     Char number               (uint8_t)  |  |           |
//                  |   1-2   Datasize                  (uint16_t) |  |           |
//                  |   3     Left Bearing              (int8_t)   |  |___        |____
//                  |   4     right Bearing             (int8_t)   |  |   |       |    |
//                  |   5     Width Pixel Zone          (uint8_t)  |  |   |       |    |
//                  |   6     Height Pixel Zone         (uint8_t)  |  |   |       |    |
//                  |   7     Width                     (uint8_t)  |  |   |       |    |
//                  |   8     Offset Y                  (uint8_t)  |  |  /_\      |   /_\
//                  |   9     Compression               (uint8_t)  |  |   |       |    |
//                  |   10-13 Offset in data            (uint32_t) |  |   |       |    |
//                  |______________________________________________|__|   |       |    |
//                  |                                              |      |       |    |
//                  |   ... Repeat last structure                  |\_____|       |    |
//                  |       According to char count in this font   |/             |    |
//                  |______________________________________________|______________|    |
//                  |                                              |                   |
//                  |   ... Repeat last section                    |\__________________|
//                  |       According to font count                |/
//                  |______________________________________________|__
//                  |                                              |  |
//                  |   Image compressed raw data                  |  |___
//                  |______________________________________________|__|   |
//                  |                                              |     /_\
//                  |   ... Repeat compressed data                 |\_____|
//                  |       According to total character count for |/
//                  |       each font                              |
//                  |______________________________________________|
//
//
//                  Binary file description
//
//                   ______________________________________________  -----> 0x00000000
//                  |                                              |________________________________
//                  | Image Struct information pointer  (uint32_t) |                               |
//                  |______________________________________________| -----> 0x00000004             |
//                  |                                              |                               | 
//                  | Image count into Struct           (uint16_t) |                               |
//                  |______________________________________________| -----> 0x00000006             |
//                  |                                              |____________________________   |
//                  | Font Struct information pointer   (uint32_t) |                            |  |
//                  |______________________________________________| -----> 0x0000000A          |  |
//                  |                                              |                            |  |
//                  | Font count into Struct            (uint16_t) |                            |  |
//                  |______________________________________________| -----> 0x0000000C          |  |
//                  |                                              |__________________________  |  |
//                  | Audio Struct information pointer  (uint32_t) |                         |  |  |
//                  |______________________________________________| -----> 0x00000010       |  |  |
//                  |                                              |                         |  |  |
//                  | Audio count into Struct           (uint16_t) |                         |  |  |
//                  |______________________________________________| -----> 0x00000012       |  |  |
//                  |                                              |_______________________  |  |  |
//                  | Label Struct information pointer  (uint32_t) |                      |  |  |  |
//                  |______________________________________________| -----> 0x00000016    |  |  |  |
//                  |                                              |                      |  |  |  |
//                  | Label count into Struct           (uint16_t) |                      |  |  |  |
//                  |______________________________________________| -----> 0x00000018    |  |  |  |
//                  |                                              |___________________   |  |  |  |
//                  | Reserved for Future used Struct  and count   |                   |  |  |  |  |
//                  |______________________________________________| -----> 0x0000003C |  |  |  |  |
//                  |                                              |                   |  |  |  |  |
//                  | Raw data for all struct type                 |                   |  |  |  |  |
//                  |______________________________________________|                   |  |  |  |  |
//                  |                                              | /_________________| _| _| _| _|
//                  | Image Struct information                     | \                 |  |  |  |
//                  |______________________________________________|                   |  |  |  |
//                  |                                              | /_________________| _| _| _|
//                  | Font Struct information                      | \                 |  |  |
//                  |______________________________________________|                   |  |  |
//                  |                                              | /_________________| _| _|
//                  | Audio Struct information  (TODO)             | \                 |  |
//                  |______________________________________________|                   |  |
//                  |                                              | /_________________| _|
//                  | Label Struct information  (TODO)             | \                 |
//                  |______________________________________________|                   |
//                  |                                              | /_________________|
//                  | ...                                          \ \
//                  |______________________________________________| 

#define RAW_DATA_OFFSET             0x0000003C


class SkinSave : public QThread
{
    Q_OBJECT

    public:
        explicit        SkinSave                (QString SkinPathAndFileName, QObject* parent = 0);
                        ~SkinSave               ();

    signals:
        void            SaveProgress            (QString Status, int Value);
        void            SaveDone                ();

    protected:
        void            run                     ();

    private:

        void            SaveFontInfo            (QVector<uint8_t>* pFileRawData);
        void            SaveImageInfo           (QVector<uint8_t>* pFileRawData);
        void            SaveAudioInfo           (QVector<uint8_t>* pFileRawData);
      //void            SaveLabelInfo           (QVector<uint8_t>* pFileRawData);
      //void            SaveLabelListInfo       (QVector<uint8_t>* pFileRawData);
        void            SaveAllImage            (QVector<uint8_t>* pFileRawData);
        void            SaveAllFont             (QVector<uint8_t>* pFileRawData);
        void            SaveAllAudio            (QVector<uint8_t>* pFileRawData);
      //void            SaveAllLabel            (QVector<uint8_t>* pFileRawData);
      //void            SaveAllLabelList        (QVector<uint8_t>* pFileRawData);
        
        void            SaveEachCharFont        (QVector<uint8_t>* pFileRawData, uint8_t Char);

        void            CreateXML               (QString Path);
        void            ExtractFontInfo         (QVector<uint8_t>* pFileRawData, uint8_t Char);
        QString         GetFontFile             (const QString& fontName);
        sFontMetaData   ReadFontMetadata        (QString fontFile);
        QString         getFontFilePath         (const QString& fontFamily);
        void            ChangeEndianness        (int Offset);

        int*                        m_pSkinType;
        QSize*                      m_pSkinSize;
        eEndianess*                 m_pEndian;
        QString                     m_SkinPathAndFileName;
        int                         m_PreviousBlockOfData;
        int                         m_ThisBlockOfData;

        // Image
        QVector<sImageInfo>*        m_pImageInfo;
        QVector<uint8_t>*           m_pRawImageData;
        int                         m_ImageCount;
        uint32_t                    m_OffsetImageHeader;

        // Audio
        QVector<sAudioInfo>*        m_pAudioInfo;
        QVector<uint8_t>*           m_pRawAudioData;
        int                         m_AudioCount;

        // Font
        QVector<QFont>*             m_pFontInfo;
        QVector<uint32_t>           m_FontSamplingSize;                         // get the size in bytes for the font sampling
        QVector<uint8_t>*           m_pFontSamplingInfo;
        QVector<uint32_t>           m_OffsetFontHeader;
        QVector<uint32_t>           m_OffsetFontHeight;
        int                         m_FontCount;
        uint32_t                    m_TotalCharCount;
        uint32_t                    m_InFontCharCount;
        uint32_t                    m_OffsetFontCharCountHeader;
        uint8_t                     m_MaxX_FixedFont;
        QVector<uint8_t>            m_MinX;
        QVector<uint8_t>            m_MaxX;
        QVector<uint8_t>            m_MinY;
        QVector<uint8_t>            m_MaxY;
        QVector<uint8_t>            m_Width;
        QVector<int8_t>             m_LeftBearing;
        QVector<int8_t>             m_RightBearing;
        const QFont*                m_pFont;
        QFontMetrics*               m_pFontMetric;

    #if 0
        // Label
        QVector<sLabelInfo>*        m_pLabelInfo;
        QVector<uint8_t>*           m_pRawLabelData;
        int                         m_LabelCount;

        // Label
        QVector<sLabelListInfo>*    m_pLabelListInfo;
        QVector<uint8_t>*           m_pRawLabelListData;
        int                         m_LabelListCount;
    #endif
};

#endif // SKIN_SAVE_H
