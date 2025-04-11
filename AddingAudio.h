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

#ifndef __ADDING_AUDIO_H__
#define __ADDING_AUDIO_H__

#include <QDialog>
#include <QtWidgets>
#include "typedef.h"
#include "qmediaplayer.h"
#include <QAudioOutput>
#include <QAudioDecoder>

namespace Ui {
class AddingAudio;
}

class AddingAudio : public QDialog
{
        Q_OBJECT

    public:
        explicit AddingAudio(QString Path, QSize Size, QWidget *parent = 0);
        ~AddingAudio();

    signals:

        void AddAudio(sLoadingAudioInfo LoadingInfo);                // Signal for Audio file
        void CloseAddAudio();                                   // signal to Main app to close this form

    private slots:

        void on_ButtonBrowse_clicked();
        void on_pushButtonAdd_clicked();
        void on_pushButtonClose_clicked();
        void on_pushButtonPlay_clicked();
        void on_ComboBoxDirectory_currentIndexChanged(int index);
        void on_TableFilesFound_cellClicked(int row, int column);
        void on_TableFilesFound_cellActivated(int row, int column);
        void on_TableFilesFound_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
        void on_ProcessBufferReady();
        void on_pushButtonStop_clicked();
        void on_pushButtonPause_clicked();
        void on_PositionChanged(qint64 position);

    private:

        void Find();
        void AudioSelected();
        void ResetLoadGUI();
        void ReloadingAudio();
        void LoadingAudio(int Row, eResizer Resizer);
        void setPlayTime(qint64 Time);


        Ui::AddingAudio*        ui;
        QDir                    m_currentDir;
        int                     m_FileFound;
        size_t                  m_TotalCount;
        
        sLoadingAudioInfo       m_LoadingAudioInfo;
        QAudioDecoder           m_Decoder;
        QMediaPlayer*           m_pPlayer;
        QAudioOutput*           m_pAudioOutput;
};

#endif // ADDING_AUDIO_H
