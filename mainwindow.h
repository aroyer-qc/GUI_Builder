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

#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <QMainWindow>
#include <QDir>
#include <QtWidgets>
#include <QMediaPlayer>
#include "AddingImage.h"
#include "AddingAudio.h"
#include "progress.h"
#include "ComboBoxDelegate.h"
#include "SpinBoxDelegate.h"
#include "qxmlputget.h"

const int BytesPerPixel[19] =
    {
        0,  // 0
        1,  // 1
        1,  // 2
        1,  // 3
        4,  // 4
        4,  // 5
        4,  // 6
        2,  // 7
        3,  // 8
        3,  // 9
        3,  // 10
        2,  // 11
        3,  // 12
        3,  // 13
        2,  // 14
        2,  // 15
        4,  // 16
        4,  // 17
        4,  // 18
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

        // Image
        QVector<uint8_t>*       getRawImageDataPtr()                    { return &m_RawImage; }
        QVector<sImageInfo>*    getImageInfoPtr()                       { return &m_ImageInfo; }
        
        // Audio
        QVector<uint8_t>*       getRawAudioDataPtr()                    { return &m_RawAudio; }
        QVector<sAudioInfo>*    getAudioInfoPtr()                       { return &m_AudioInfo; }
        
        // Font
        QVector<QFont>*         getFontInfoPtr()                        { return &m_Font; }
        QVector<uint8_t>*       getFontSamplingInfoPtr()                { return &m_SamplingFont; }

        // Non specific global function
        eEndianess*             getEndianPtr()                          { return &m_Endian; }
        int*                    getSkinTypePtr()                        { return &m_SkinType; }
        QSize*                  getSkinSizePtr()                        { return &m_DisplaySize; }
        static QScreen*         getPrimaryScreen()                      { return m_pPrimary; }
        static void             setPrimaryScreen(QScreen* pScreen)      { m_pPrimary = pScreen; }

    signals:

    public slots:


        // Public slot for file operation
        void on_OpenDone();
        void on_SaveDone();

        // Public slot for Image
        void AddImage(sLoadingImageInfo LoadingInfo);
        void CloseAddImage();

        // Public slot for Audio
        void AddAudio(sLoadingAudioInfo LoadingInfo);
        void CloseAddAudio();

    private slots:

        // Slot for Image tab
        void on_ButtonDownImage_clicked();
        void on_ButtonUpImage_clicked();
        void on_ButtonAddImage_clicked();
        void on_ButtonRemoveImage_clicked();
        void on_TableImage_cellClicked(int row, int column);
        void on_TableImage_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
        void on_TableImage_cellDoubleClicked(int row, int column);
        void on_TableImage_itemChanged(QTableWidgetItem *item);
        void on_ImageCheckerBoardSlider_sliderMoved(int position);

        // Slot for Font tab
        void on_ButtonAddFont_clicked();
        void on_ButtonRemoveFont_clicked();
        void on_ButtonUpFont_clicked();
        void on_ButtonDownFont_clicked();
        void on_TableFont_cellActivated(int row, int column);
        void on_TableFont_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
        void on_TableFont_cellClicked(int row, int column);
        void on_TableFont_cellDoubleClicked(int row, int column);
        void on_TableFont_itemChanged(QTableWidgetItem *item);
        void on_checkBoxAlphaFont_clicked(bool checked);
        void on_checkBoxNumericFont_clicked(bool checked);
        void on_checkBoxSymbolFont_clicked(bool checked);
        void on_checkBoxExtraSymbolFont_clicked(bool checked);
        void on_checkBoxLatinFont_clicked(bool checked);
        void on_checkBoxBold_clicked(bool checked);
        void on_checkBoxItalic_clicked(bool checked);
        void on_checkBoxFixed_clicked(bool checked);

        // Slot for Audio tab
        void on_ButtonUpAudio_clicked();
        void on_ButtonDownAudio_clicked();
        void on_ButtonAddAudio_clicked();
        void on_ButtonRemoveAudio_clicked();
        void on_TableAudio_cellClicked(int row, int column);
        void on_TableAudio_cellDoubleClicked(int row, int column);
        void on_TableAudio_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
        void on_TableAudio_itemChanged(QTableWidgetItem *item);
        
        // Slot for Converter
        void on_ButtonBrowse_clicked();
        void on_ButtonConvert_clicked();
        void on_TableFilesFound_cellActivated(int row, int column);
        void on_TableFilesFound_cellClicked(int row, int column);
        void on_TableFilesFound_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
        void on_ComboBoxDirectory_currentIndexChanged(int index);
        void on_horizontalScrollBarConverter_valueChanged(int value);
        void on_verticalScrollBarConverter_valueChanged(int value);

        // Slot for Configurator tab
        void on_checkBoxLoadable_checkStateChanged(int state);
        void on_checkBoxBinary_checkStateChanged(int state);
        void on_checkBoxBigEndian_checkStateChanged(int state);
        void on_checkBoxLittleEndian_checkStateChanged(int state);
        void on_radioButton_84x48_clicked();
        void on_radioButton_96x64_clicked();
        void on_radioButton_128x128_clicked();
        void on_radioButton_240x320_clicked();
        void on_radioButton_320x200_clicked();
        void on_radioButton_320x240_clicked();
        void on_radioButton_640x480_clicked();
        void on_radioButton_480x272_clicked();
        void on_radioButton_800x480_clicked();
        void on_radioButton_800x600_clicked();
        void on_radioButton_CustomSize_clicked();
        
        // Menu action
        void on_actionNew_Project_triggered();
        void on_actionOpen_Skin_triggered();
        void on_actionSave_Skin_triggered();
        void on_actionSave_Skin_As_triggered();
        void on_SaveBinaryButton_clicked();
        void on_comboBoxResize_currentIndexChanged(int index);
        void on_comboBoxPixelFormat_currentIndexChanged(int index);
        void on_actionExit_triggered();
        void on_CheckerBoardSlider_sliderMoved(int position);
        void on_TabFunctionSelect_tabBarClicked(int index);

        void on_SetNewPathImage(QString Path);
        void on_SetNewPathAudio(QString Path);

    private:

        void     ClearAllData            ();
        void     ResetAllSkinTab         ();
        void     UpdateStatusBar         ();
        void     UpdateWidget            ();
        void     closeEvent              (QCloseEvent *bar);

        // Config in XML File
        void     GetConfigFromXML        ();
        void     SetConfigToXML          ();

        // Function for file operation
        void     Open                    (QString File);
        void     Save                    ();
        bool     SaveSkinAndClearData    ();
        void     setSkinHasUnsavedData   (bool state);

        // Function for Image
        void     InitImage               ();
        void     ResetImageGUI           ();
        void     UpdateImageGUI          (int row);
        void     ClearSceneImage         ();
        void     AdjustTabImage          (QSize Offset, QRect ViewRect);
        void     InsertNewRowInTableImage(int row, QString File, QString CodeType, QString CodeID, QString Size);

        // Function for Font
        void     InitFont                ();
        void     ResetFontGUI            ();
        void     UpdateFontGUI           (int row);
        void     ClearSceneFont          ();
        void     LoadFont                (int row);
        void     SetTableProperties      (int row);
        void     InsertNewRowInTableFont (int row, QString Name, QString Size, QString Properties);
        void     CheckBoxActivation      (bool state);
        void     DisplayExample          (int row);
        void     checkValidFont          ();

        // Function for Audio
        void     InitAudio               ();
        void     ResetAudioGUI           ();
        void     UpdateAudioGUI          (int row);
        void     AdjustTabAudio          (QSize Offset, QRect ViewRect);
        void     InsertNewRowInTableAudio(int row, QString File, QString SamplingRate, QString Duration, QString Size);

        // Function for Converter
        void     InitConverter           ();
        void     ResetConverterGUI       ();
        void     ClearSceneConverter     ();
        void     Extract                 (QTextStream* pStream, int Index);
        void     Find                    ();
        void     AdjustTabConverter      (QSize Offset, QRect ViewRect);
        void     ClearScrollBarValue     ();
        void     ReloadImageConverter    ();
        void     LoadImageConverter      (int row, eResizer Resizer);
        void     BinToFile               (QTextStream* pStream, QString pFileName);

        // Function for configurator
        void     InitConfigurator        ();
        void     ResetAllRadioButton     ();
        void     UpdateLineEdit          ();
        void     CheckButton             ();
        void     SetSizeDisplay          (QSize Size);
        

        Ui::MainWindow*         ui;
        int                     m_SkinType;
        eEndianess              m_Endian;
        uint32_t                m_MemoryOffset;
        static QScreen*         m_pPrimary;

        QMediaPlayer*           m_Player;

        // Layout info
        QSize                   m_DisplaySize;
        QLabel*                 m_pStatusLabel;
        QRect                   m_ViewRect;
        QRect                   m_TabRect;
        QRect                   m_MainRect;
        QString                 m_SkinName;
        int                     m_SkinSize;
        QString                 m_SpecialNote;

        QVector<uint8_t>*       m_pInUseCode;

        // Data of the Skin
        //QVector<wchar_t>        m_RawLabel;                     // Raw label data
        //QVector<uint32_t>       m_Widget;                       // remove all widget
        //QVector<uint32_t>       m_WidgetIndex;                  // Quick index to recover Widget
        //sLoadingInfo            m_LoadingInfo;

        // File operation
        QDir                    m_CurrentDir;
        QDir                    m_SkinDir;
        AddingImage*            m_pLoadImage;
        AddingAudio*            m_pLoadAudio;
        QThread*                m_pSkinSave;
        QThread*                m_pSkinOpen;
        bool                    m_IsSkinSaveAs;                 // Save As or Save
        bool                    m_IsSkinHasUnsavedData;         // is there any unsaved data
        bool                    m_IsWarningDisplayed;
        bool                    m_IsNeedCompleteFileOpen;
        bool                    m_IsNeedToClearDataAfterSave;
        bool                    m_IsNeedToCloseAfterSave;
        int                     m_ProgressSave;
        int                     m_ProgressOpen;
        Progress*               m_pProgress;

        // Variable for Image Tab
        QDir                    m_ImageDir;
        QVector<sImageInfo>     m_ImageInfo;                    // Array of image structure information
        QVector<uint8_t>        m_RawImage;                     // Raw image data
        QGraphicsScene          m_SceneImage;
        ComboBoxDelegate*       m_pImageComboBoxDelegate;
        SpinBoxDelegate*        m_pImageSpinBoxDelegate;

        // Variable for Font Tab
        QGraphicsScene          m_SceneExample;
        QGraphicsScene          m_SceneFont;
        QVector<QFont>          m_Font;
        QVector<uint8_t>        m_SamplingFont;
        ComboBoxDelegate*       m_pFontComboBoxDelegate;
        bool                    m_IsAllFontValide;

        // Variable for Audio Tab
        QDir                    m_AudioDir;
        QVector<sAudioInfo>     m_AudioInfo;                    // Array of audio structure information
        QVector<uint8_t>        m_RawAudio;                     // Raw audio data
        ComboBoxDelegate*       m_pAudioComboBoxDelegate;
        SpinBoxDelegate*        m_pAudioSpinBoxDelegate;

        // Variable for Label Tab
      //QVector<sLabelInfo>     m_LabelInfo;                    // Array of label structure information
      //QVector<uint8_t>        m_RawLabel;                     // Raw label data


        // Variable for Converter Tab
        QGraphicsScene          m_SceneConverter;
        QGraphicsPixmapItem*    m_pPixmapItem;
        QImage::Format          m_PixelFormatConverter;
        int                     m_verticalScrollRange;
        int                     m_horizontalScrollRange;
        int                     m_FileFound;
        QImage*                 m_pImage;
        QImage*                 m_pProcessedImage;
        QSize                   m_Scale;
        size_t                  m_TotalCount;
        size_t                  m_FileSize;

        // Variable for Configurator Tab
        QSize                   m_ConfigDisplaySize;

};

#endif // MAINWINDOW_H
