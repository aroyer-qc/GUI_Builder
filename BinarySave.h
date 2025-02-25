/*
   Copyright(c) 2025 Alain Royer.
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

#ifndef __BINARY_SAVE_H__
#define __BINARY_SAVE_H__

#include "mainwindow.h"
#include <QThread>
#include <QTimer>


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


class BinarySave : public QThread
{
    Q_OBJECT

    public:
        explicit    BinarySave                (QString BinaryPathAndFileName, QObject* parent = 0);
                    ~BinarySave               ();

    signals:
        void        SaveProgress            (QString Status, int Value);
        void        SaveDone                ();

    protected:
        void        run                     ();

    private:

        bool        SaveFontInfo            (QVector<uint8_t>* pCompxData);
        bool        SaveImageInfo           (QVector<uint8_t>* pCompxData);
        void        CreateXML               (QString Path);
        void        ExtractFontInfo         (QVector<uint8_t>* pCompxData, uint8_t Char);

        eEndianess*             m_pEndian;
        QVector<uint8_t>*       m_pRawData;
        QString                 m_BinaryPathAndFileName;
        int                     m_PreviousBlockOfData;
        int                     m_ThisBlockOfData;

        QVector<sImageInfo>*    m_pImageInfo;
        int                     m_ImageCount;
        uint32_t                m_OffsetImageHeader;

        QVector<QFont>*         m_pFontInfo;
        QVector<uint8_t>*       m_pFontSamplingInfo;
        QVector<uint32_t>       m_OffsetFontHeader;
        QVector<uint32_t>       m_OffsetFontHeight;
        int                     m_FontCount;
        uint32_t                m_TotalCharCount;
        uint32_t                m_InFontCharCount;
        uint32_t                m_OffsetFontCharCountHeader;
        uint8_t                 m_MaxX_FixedFont;
        QVector<uint8_t>        m_MinX;
        QVector<uint8_t>        m_MaxX;
        QVector<uint8_t>        m_MinY;
        QVector<uint8_t>        m_MaxY;
        QVector<uint8_t>        m_Width;
        QVector<int8_t>         m_LeftBearing;
        QVector<int8_t>         m_RightBearing;

        const QFont*            m_pFont;
        QFontMetrics*           m_pFontMetric;
};

#endif // BINARY_SAVE_H
