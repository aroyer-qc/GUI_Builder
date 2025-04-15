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

#include <QDir>
#include "AddingAudio.h"
#include "ui_AddingAudio.h"
#include "Utility.h"

// ************************************************************************************************

AddingAudio::AddingAudio(QString Path, QSize Size, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::AddingAudio)
{
    this->setModal(true);
    ui->setupUi(this);
    setSizeGripEnabled(false);
    m_CurrentDir.setPath(Path);
    m_PreviousDir.setPath(Path);
    m_pPlayer = new QMediaPlayer;
    m_pAudioOutput = new QAudioOutput;
    m_pPlayer->setAudioOutput(m_pAudioOutput);
    
    ui->ComboBoxDirectory->blockSignals(true);
    ui->ComboBoxDirectory->setCurrentText(Path);
    UpdateComboBox(ui->ComboBoxDirectory);
    ui->ComboBoxDirectory->blockSignals(false);

    ui->comboBoxAudioMode->blockSignals(true);
    ui->comboBoxAudioMode->setCurrentIndex(0); /// ???  set to correct enum
    ui->comboBoxAudioMode->blockSignals(false);

    ui->pushButtonPlay->setStyleSheet("background-color: lightgray; border-color: darkgray;");
    ui->pushButtonStop->setStyleSheet("background-color: lightgray; border-color: darkgray;");
    ui->pushButtonPause->hide();

    ui->comboBoxSamplingRate->setCurrentIndex(0); /// ???  set to correct enum

    this->connect(ui->pushButtonAdd, SIGNAL(Clicked()),
                  this, SLOT(on_pushButtonAdd_clicked()));

    this->connect(ui->pushButtonClose, SIGNAL(Clicked()),
                  this, SLOT(on_pushButtonClose_clicked()));

    this->connect(ui->pushButtonClose, SIGNAL(Clicked()),
                  this, SLOT(on_pushButtonClose_clicked()));

    // Connecting the signal to the slot
    this->connect(m_pPlayer, SIGNAL(positionChanged(qint64)),
                  this, SLOT(on_PositionChanged(qint64)));


    this->connect(&m_Decoder, &QAudioDecoder::bufferReady, this, &AddingAudio::on_ProcessBufferReady);

     this->setAttribute(Qt::WA_DeleteOnClose);

    ResetLoadGUI();
    Find();
}

// ************************************************************************************************

AddingAudio::~AddingAudio()
{
    m_pPlayer->stop();
    delete m_pPlayer;
    delete m_pAudioOutput;

    delete ui;
}

// ************************************************************************************************
// *
// *  Slot
// *
// ************************************************************************************************
void AddingAudio::on_pushButtonAdd_clicked()
{
    AudioSelected();
}

// ************************************************************************************************

void AddingAudio::on_pushButtonClose_clicked()
{

    if(m_CurrentDir != m_PreviousDir)
    {
        emit SaveConfig(m_CurrentDir.absolutePath());
    }

    emit CloseAddAudio();
}

// ************************************************************************************************

void AddingAudio::on_ButtonBrowse_clicked()
{
    QString directory = QFileDialog::getExistingDirectory(this, tr("Find Audio Files"), m_CurrentDir.absolutePath());

    if (!directory.isEmpty())
    {
        if(ui->ComboBoxDirectory->findText(directory) == -1)
        {
            ui->ComboBoxDirectory->addItem(directory);
        }
        ui->ComboBoxDirectory->setCurrentIndex(ui->ComboBoxDirectory->findText(directory));
    }
}

// ************************************************************************************************

void AddingAudio::on_ComboBoxDirectory_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    m_CurrentDir = QDir(ui->ComboBoxDirectory->currentText());
    Find();
}

// ************************************************************************************************

void AddingAudio::on_TableFilesFound_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    Q_UNUSED(currentColumn);
    Q_UNUSED(previousRow);
    Q_UNUSED(previousColumn);

    if(currentRow >= 0)
    {
        m_pPlayer->stop();
        ui->pushButtonPlay->setStyleSheet("background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1,  stop: 0 #6B82AC, stop: 0.49 #566D97, stop: 0.5 #445B85, stop: 1 #566D97);border-color: #778EB8;");
        ui->pushButtonPlay->show();
        ui->pushButtonPause->hide();
        ui->pushButtonStop->setStyleSheet("background-color: lightgray; border-color: darkgray;");
        LoadingAudio(currentRow, AUTO_FORMAT);
    }
}

// ************************************************************************************************

void AddingAudio::on_TableFilesFound_cellClicked(int row, int column)
{
    Q_UNUSED(column);
    LoadingAudio(row, AUTO_FORMAT);
}

// ************************************************************************************************

void AddingAudio::on_TableFilesFound_cellActivated(int row, int column)
{
    // Double click on a line will automatically select this file
    Q_UNUSED(row);
    Q_UNUSED(column);
    AudioSelected();
}

// ************************************************************************************************

void AddingAudio::on_ProcessBufferReady()           // Slot to handle bufferReady signal
{
    QAudioBuffer buffer = m_Decoder.read();
    if (buffer.isValid())
    {
        m_LoadingAudioInfo.DataSize = 0;
        m_LoadingAudioInfo.SampleRate = buffer.format().sampleRate();
        m_LoadingAudioInfo.ChannelCount = buffer.format().channelCount();
        m_LoadingAudioInfo.BytesPerSample = buffer.format().bytesPerSample();
        m_LoadingAudioInfo.DurationInSecond = m_pPlayer->duration() / 1000;
        m_LoadingAudioInfo.SampleFormat = buffer.format().sampleFormat();

        // Additional processing if required...
    }
}

// ************************************************************************************************
// *
// *  Function
// *
// ************************************************************************************************

void AddingAudio::Find()
{
    QStringList files;
    QStringList filters;

    ResetLoadGUI();

    filters << "*.mp3" << "*.wav" << "*.aac" << "*.ogg" << "*.flac" << "*.aaif";
    m_CurrentDir.setNameFilters(filters);

    files = m_CurrentDir.entryList(filters, QDir::Files | QDir::NoSymLinks);

    m_FileFound = files.size();

    ui->TableFilesFound->blockSignals(true);
    for(int i = 0; i < m_FileFound; ++i)
    {
        QFile file(m_CurrentDir.absoluteFilePath(files[i]));
        qint64 size = QFileInfo(file).size();

        QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
        fileNameItem->setFlags(fileNameItem->flags() ^ Qt::ItemIsEditable);
        QTableWidgetItem *sizeItem = new QTableWidgetItem(PrintFileSize(size));
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        sizeItem->setFlags(sizeItem->flags() ^ Qt::ItemIsEditable);

        int row = ui->TableFilesFound->rowCount();
        ui->TableFilesFound->insertRow(row);
        ui->TableFilesFound->setItem(row, 0, fileNameItem);
        ui->TableFilesFound->setItem(row, 1, sizeItem);
    }
    ui->TableFilesFound->blockSignals(false);

    if(m_FileFound)
    {
        ui->LabelFilesFound->setText(tr("%1 file(s) found").arg(m_FileFound));
    }
    else
    {
        ui->LabelFilesFound->setText(tr("0 file found"));
    }

    setPlayTime(0);
}


// ************************************************************************************************

void AddingAudio::AudioSelected()
{

    //m_Player->play(); need to be in button play
    emit AddAudio(m_LoadingAudioInfo);
}

// ************************************************************************************************

void AddingAudio::ResetLoadGUI()
{
    ui->pushButtonAdd->setDisabled(true);
    ui->LabelFilename->setText("");
    ui->LabelDataSize->setText("");
    ui->LabelSamplingRate->setText("");
    ui->LabelDuration->setText("");
    ui->LabelSize->setText("");
    //ui->ClearTable(ui->TableFilesFound);
}

// ************************************************************************************************

void AddingAudio::ReloadingAudio()
{
    int row = ui->TableFilesFound->currentRow();
    if(row >= 0)
    {
        //LoadingAudio(row, RELOAD_AUDIO);
    }
}

// ************************************************************************************************

void AddingAudio::LoadingAudio(int row, eResizer Resizer)
{
    QTableWidgetItem *item;

    ui->pushButtonAdd->setEnabled(true);
    item = ui->TableFilesFound->item(row, 0);
    m_LoadingAudioInfo.Filename = item->text();
    m_LoadingAudioInfo.PathAndFilename = m_CurrentDir.absoluteFilePath(m_LoadingAudioInfo.Filename);

    // Set the audio source and start decoding
    m_Decoder.setSource(QUrl::fromLocalFile(m_LoadingAudioInfo.PathAndFilename));
    m_Decoder.start();  // Starts decoding

    m_pPlayer->setSource(QUrl::fromLocalFile(m_LoadingAudioInfo.PathAndFilename));
    m_pAudioOutput->setVolume(50);

    // Synchronous waiting for decoding
    QEventLoop loop;
    this->connect(&m_Decoder, &QAudioDecoder::bufferReady, &loop, &QEventLoop::quit);
    loop.exec();

    // Update UI with audio information
    ui->LabelFilename->setText(m_LoadingAudioInfo.Filename);
    ui->LabelSamplingRate->setText(QString("%1 Hz").arg(m_LoadingAudioInfo.SampleRate));
    ui->LabelDuration->setText(QTime(0, 0).addSecs(m_LoadingAudioInfo.DurationInSecond).toString("HH:mm:ss"));
}

// ************************************************************************************************

void AddingAudio::setPlayTime(qint64 Time)
{
    uint32_t Hours;
    uint32_t Minutes;
    uint32_t Seconds;
    QString TimeStr;
    QChar TempChar;
    int Count = 0;

    Seconds = Time % 60;
    Minutes = (Time / 60) % 60;
    Hours   = Time / 3600;

    TimeStr = QString("%3:%2:%1").arg(Seconds, 2, 10, QLatin1Char('0'))
                                 .arg(Minutes, 2, 10, QLatin1Char('0'))
                                 .arg(Hours, 3, 10, QLatin1Char('0'));

    TempChar = TimeStr.at(0);
    if(TempChar == '0')
    {
        TempChar = ' ';
        Count++;
    }
    ui->LabelAudioPlayTimeHour100->setText(TempChar);

    TempChar = TimeStr.at(1);
    if(TempChar == '0')
    {
        TempChar = ' ';
        Count++;
    }
    ui->LabelAudioPlayTimeHour10->setText(TempChar);

    TempChar = TimeStr.at(2);
    if(TempChar == '0')
    {
        TempChar = ' ';
        Count++;
    }
    ui->LabelAudioPlayTimeHour1->setText(TempChar);

    if(Count == 3)
    {
        ui->LabelStaticAudioPlayDot1->hide();
    }
    else
    {
        ui->LabelStaticAudioPlayDot1->show();
    }

    TempChar = TimeStr.at(4);
    if(TempChar == '0')
    {
        TempChar = ' ';
        Count++;
    }
    ui->LabelAudioPlayTimeMinute10->setText(TempChar);

    ui->LabelAudioPlayTimeMinute1->setText(TimeStr.at(5));
    ui->LabelAudioPlayTimeSecond10->setText(TimeStr.at(7));
    ui->LabelAudioPlayTimeSecond1->setText(TimeStr.at(8));
}

// ************************************************************************************************

void AddingAudio::on_pushButtonPlay_clicked()
{
    m_pPlayer->play();
    ui->pushButtonPlay->hide();
    ui->pushButtonPause->show();
    ui->pushButtonStop->setStyleSheet("background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1,  stop: 0 #6B82AC, stop: 0.49 #566D97, stop: 0.5 #445B85, stop: 1 #566D97);border-color: #778EB8;");
}

// ************************************************************************************************

void AddingAudio::on_pushButtonStop_clicked()
{
    m_pPlayer->stop();
    ui->pushButtonPlay->show();
    ui->pushButtonPause->hide();
    ui->pushButtonStop->setStyleSheet("background-color: lightgray;border-color: darkgray;");
    setPlayTime(0);
}


void AddingAudio::on_pushButtonPause_clicked()
{
    m_pPlayer->pause();
    ui->pushButtonPause->hide();
    ui->pushButtonPlay->show();
}

void AddingAudio::on_PositionChanged(qint64 position)
{
    static qint64 LastValue = 0;

    position /= 1000;

    if(position != LastValue)
    {
        LastValue = position;
        setPlayTime(position);
    }
}



//ui->pushButtonPlay->setStyleSheet("background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1,  stop: 0 #6B82AC, stop: 0.49 #566D97, stop: 0.5 #445B85, stop: 1 #566D97);border-color: #778EB8;");
//ui->pushButtonPause->hide();
//ui->pushButtonStop->setStyleSheet("lightgray;border-color: darkgray;")
 //m_pAudioOutput->setVolume(50);
// background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1,  stop: 0 #6B82AC, stop: 0.49 #566D97, stop: 0.5 #445B85, stop: 1 #566D97);
//border-color: #778EB8;


//# Set playback position (e.g., to 5000 milliseconds or 5 seconds)
//media_player.setPosition(5000);

//elapsed_time = media_player.position();
//printf("Elapsed time: {elapsed_time} ms");

