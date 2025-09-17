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

#pragma once

#include <QImage>
#include <QAudioFormat>
#include <stdint.h>

#define MINIMUM_PREVIEW_WIDTH   480
#define MINIMUM_PREVIEW_HEIGHT  272

#define FORMAT_RGB565           0
#define FORMAT_ARGB8888         1
#define FORMAT_FILE_DATA        2

#define IMAGE_HEADER_SIZE       18

#define SIZE_OF_ID_CODE_SPACE   10000


#define SAMPLING_ALPHA          uint8_t(0x01)
#define SAMPLING_NUMERIC        uint8_t(0x02)
#define SAMPLING_SYMBOL         uint8_t(0x04)
#define SAMPLING_EXTRA_SYMBOL   uint8_t(0x08)
#define SAMPLING_LATIN          uint8_t(0x10)
#define SAMPLING_FIXED_NUMERIC  uint8_t(0x20)
#define SAMPLING_DEFAULT_FONT   uint8_t(0x27)


#define SAMPLING_BOX_X_SIZE     230
#define SAMPLING_BOX_Y_SIZE     200

#define BINARY_OFFSET_IMG_STRUCT_POINTER    0x00000000
#define BINARY_OFFSET_IMG_COUNT             0x00000004
#define BINARY_OFFSET_FNT_STRUCT_POINTER    0x00000006
#define BINARY_OFFSET_FNT_COUNT             0x0000000A
#define BINARY_OFFSET_AUD_STRUCT_POINTER    0x0000000C
#define BINARY_OFFSET_AUD_COUNT             0x00000010
#define BINARY_OFFSET_LBL_STRUCT_POINTER    0x00000012
#define BINARY_OFFSET_LBL_COUNT             0x00000016
#define BINARY_OFFSET_LST_STRUCT_POINTER    0x00000018
#define BINARY_OFFSET_LST_COUNT             0x0000001C

#define DEFAULT_CHECKER_VALUE               225

typedef QSize QOffset;

enum SkinBlockType_e
{
    SKIN_IMAGE,
    SKIN_FONT,
};

enum Endianess_e
{
    LITTLE_ENDIAN = 0,
    BIG_ENDIAN,
};

enum Resizer_e
{
    RELOAD_FORMAT = 0,
    AUTO_FORMAT,
};

enum Scale_e
{
    SCALE_NONE = 0,
    SCALE_FIT,
    SCALE_WIDTH,
    SCALE_HEIGHT,
};

enum GFX_PixelFormat_e
{
   ARGB8888,
   RGB888,
   RGB565,
   ARGB1555,
   ARGB4444,
   L8,
   AL44,
   AL88,
   L4,
   A8,
   A4,
   DUMMY_PIXEL_FORMAT,
};

enum Compression_e
{
    COMPRESSION_NONE,
    RLE_4,                                  // 4 bits per pixel  (font, or grayscale)
    RLE_8,                                  // 8 bits per pixel  (font, or grayscale)
    RLE_16,                                 // 16 bits per pixel (RGB565)
    RLE_32,                                 // 32 bits per pixel (ARGB8888)
    RLE_16_CLUT,                            // 16 bits per pixel (RGB565, with color table)
    RLE_32_CLUT,                            // 32 bits per pixel (ARGB8888, with color table);
    LZW_12,                                 // Good for all
    LZW_13,                                 // Good for all
    LZW_14,                                 // Good for all
    LZW_15,                                 // Good for all
    LZW_16,                                 // Good for all
    JPEG,                                   // If the file is already in JPEG and smaller than other method
};

enum WidgetType_e
{
    TYPE_BACKGROUND,
    TYPE_BUTTON,
    TYPE_FONT,
    TYPE_GLYPH,
    TYPE_ICON,
    TYPE_IMAGE,
    TYPE_AUDIO,
    TYPE_AUDIO_WAV,
    TYPE_AUDIO_MP3,
    TYPE_AUDIO_FLAC,
    TYPE_AUDIO_RAW,
    NUMBER_OF_WIDGET_TYPE,
};

#pragma pack (push)
#pragma pack (1)
struct ImageInfo_t
{
    uint32_t        ID;
    uint32_t        RawIndex;
    size_t          CompressDataSize;
    size_t          DataSize;
    QSize           Size;
    QImage::Format  PixelFormat;
    QString         Filename;
    QString         Description;
};

// not definitive in TODO list
struct AudioInfo_t
{
    uint32_t        ID;
    uint32_t        RawIndex;
    size_t          CompressDataSize;
    size_t          DataSize;
    time_t          Duration;
    QString         Filename;
    QString         Description;
};

// not definitive in TODO list
struct LabelInfo_t
{
    uint32_t        ID;
    uint32_t        RawIndex;
    size_t          CompressDataSize;
    size_t          DataSize;
    QString         Description;
};

// not definitive in TODO list
struct LabelListInfo_t
{
    uint32_t        ID;
    uint32_t        RawIndex;
    size_t          CompressDataSize;
    size_t          DataSize;
    QString         Description;
};
#pragma pack (pop)

// Struct for data in skin file.
struct SkinInfoData_t
{
        uint32_t        ID;
        uint32_t        DataSize;
        uint8_t         Compression;
#pragma pack (push)
    #pragma pack (1)
    union
    {
        struct
        {
            uint16_t        width;
            uint16_t        height;
            uint8_t         PixelFormat;
        } Image;
        struct
        {
            uint8_t         Code;
            uint8_t         Page;
        } Font;
        struct
        {
            uint8_t         LabelCount;
        };
    }u;
#pragma pack (pop)
};

struct LoadingImageInfo_t
{
    QString         PathAndFilename;
    QString         Filename;
    size_t          DataSize;
    QSize           Size;
    QPoint          Offset;
    Scale_e         ScaleType;
    QImage::Format  PixelFormat;
};

struct LoadingAudioInfo_t
{
    QString                     PathAndFilename;
    QString                     Filename;
    size_t                      DataSize;
    int                         SampleRate;
    int                         ChannelCount;
    int                         BytesPerSample;
    int                         DurationInSecond;
    QAudioFormat::SampleFormat  SampleFormat;
};

struct Encoding_t
{
    Compression_e Type;
    int           Size;
};

struct FontMetaData_t
{
    QString FileName;
    QString Manufacturer;
    QString Designer;
};

struct SkinConfig_t
{
    Endianess_e Endianess;
    QSize       DisplaySize;
    bool        UseBinary;
    uint32_t    MemoryOffset;
    int         ImageMax;    
    int         FontMax;
    int         AudioMax;    
    int         LabelMax; 
    int         LabelListMax; 
};

// FOR Binary file

/*
typedef struct BoxSize_s
{
    uint16_t       Width;
    uint16_t       Height;
} sBoxSize;

typedef struct DiginiImageInfo_s
{
    void*          pPointer;
    BoxSize_s      Size;
    PixelFormat_e  PixelFormat;

} sDiginiImageInfo;
*/
