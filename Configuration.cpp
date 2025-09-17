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

#include "mainwindow.h"
#include "ui_mainwindow.h"

// TODO configurator: custom size is not working!!


// ************************************************************************************************
// *
// *  Slot
// *
// ************************************************************************************************

void MainWindow::on_checkBoxBinary_checkStateChanged(Qt::CheckState state)
{
    if(state == Qt::Checked)
    {
        ui->LabelStaticMemoryLocation->setStyleSheet("QLabel { color: black; }");
        ui->MemoryOffset->setEnabled(true);
        ui->LabelStaticFontMAX->setEnabled(true);
        ui->LabelStaticFONT_MAX->setEnabled(true);
        ui->LabelStaticImageMAX->setEnabled(true);
        ui->LabelStaticIMAGE_MAX->setEnabled(true);
        ui->spinBoxFont->setEnabled(true);
        ui->spinBoxImage->setEnabled(true);

    }
    else if(state == Qt::Unchecked)
    {
        ui->LabelStaticMemoryLocation->setStyleSheet("QLabel { color: gray; }");
        ui->MemoryOffset->setEnabled(false);
        ui->LabelStaticFontMAX->setEnabled(false);
        ui->LabelStaticFONT_MAX->setEnabled(false);
        ui->LabelStaticImageMAX->setEnabled(false);
        ui->LabelStaticIMAGE_MAX->setEnabled(false);
        ui->spinBoxFont->setEnabled(false);
        ui->spinBoxImage->setEnabled(false);
    }

    // Not available Yet, so always gray out
    ui->LabelStaticAudioMAX->setEnabled(false);
    ui->LabelStaticAUDIO_MAX->setEnabled(false);
    ui->LabelStaticLabelMAX->setEnabled(false);
    ui->LabelStaticLABEL_MAX->setEnabled(false);
    ui->spinBoxAudio->setEnabled(false);
    ui->spinBoxLabel->setEnabled(false);


    SetButtonState(true);
}

// ************************************************************************************************

void MainWindow::on_checkBoxLittleEndian_checkStateChanged(Qt::CheckState state)
{
    if(state == Qt::Checked)
    {
        ui->checkBoxBigEndian->setChecked(false);
    }
    else if(state == Qt::Unchecked)
    {
        ui->checkBoxBigEndian->setChecked(true);
    }

    SetButtonState(true);
}

// ************************************************************************************************

void MainWindow::on_checkBoxBigEndian_checkStateChanged(Qt::CheckState state)
{
    if(state == Qt::Checked)
    {
        ui->checkBoxLittleEndian->setChecked(false);
    }
    else if(state == Qt::Unchecked)
    {
        ui->checkBoxLittleEndian->setChecked(true);
    }

    SetButtonState(true);
}

// ************************************************************************************************

void MainWindow::on_radioButton_84x48_clicked()
{
    ResetAllRadioButton();
    ui->radioButton_84x48->setChecked(true);
    m_TempDisplaySize = {84, 48};
    UpdateLineEdit();
    SetButtonState(true);
}

// ************************************************************************************************

void MainWindow::on_radioButton_96x64_clicked()
{
    ResetAllRadioButton();
    ui->radioButton_96x64->setChecked(true);
    m_TempDisplaySize = {96, 64};
    UpdateLineEdit();
    SetButtonState(true);
}

// ************************************************************************************************

void MainWindow::on_radioButton_128x128_clicked()
{
    ResetAllRadioButton();
    ui->radioButton_128x128->setChecked(true);
    m_TempDisplaySize = {128, 128};
    UpdateLineEdit();
    SetButtonState(true);
}

// ************************************************************************************************

void MainWindow::on_radioButton_240x320_clicked()
{
    ResetAllRadioButton();
    ui->radioButton_240x320->setChecked(true);
    m_TempDisplaySize = {240, 320};
    UpdateLineEdit();
    SetButtonState(true);
}

// ************************************************************************************************

void MainWindow::on_radioButton_320x200_clicked()
{
    ResetAllRadioButton();
    ui->radioButton_320x200->setChecked(true);
    m_TempDisplaySize = {320, 200};
    UpdateLineEdit();
    SetButtonState(true);
}

// ************************************************************************************************

void MainWindow::on_radioButton_320x240_clicked()
{
    ResetAllRadioButton();
    ui->radioButton_320x240->setChecked(true);
    m_TempDisplaySize = {320, 240};
    UpdateLineEdit();
    SetButtonState(true);
}

// ************************************************************************************************

void MainWindow::on_radioButton_480x272_clicked()
{
    ResetAllRadioButton();
    ui->radioButton_480x272->setChecked(true);
    m_TempDisplaySize = {480, 272};
    UpdateLineEdit();
    SetButtonState(true);
}

// ************************************************************************************************

void MainWindow::on_radioButton_640x480_clicked()
{
    ResetAllRadioButton();
    ui->radioButton_640x480->setChecked(true);
    m_TempDisplaySize = {640, 480};
    UpdateLineEdit();
    SetButtonState(true);
}

// ************************************************************************************************

void MainWindow::on_radioButton_800x480_clicked()
{
    ResetAllRadioButton();
    ui->radioButton_800x480->setChecked(true);
    m_TempDisplaySize = {800, 480};
    UpdateLineEdit();
    SetButtonState(true);
}

// ************************************************************************************************

void MainWindow::on_radioButton_800x600_clicked()
{
    ResetAllRadioButton();
    ui->radioButton_800x600->setChecked(true);
    m_TempDisplaySize = {800, 600};
    UpdateLineEdit();
    SetButtonState(true);
}

// ************************************************************************************************

void MainWindow::on_radioButton_CustomSize_clicked()
{
    ResetAllRadioButton();
    ui->radioButton_CustomSize->setChecked(true);
    ui->lineEdit_CustomWidth->setEnabled(true);
    ui->lineEdit_CustomHeight->setEnabled(true);
    SetButtonState(true);
}

// ************************************************************************************************

void MainWindow::on_SkinFileName_textEdited()
{
    SetButtonState(true);
}

// ************************************************************************************************

void MainWindow::on_MemoryOffset_textEdited()
{
    SetButtonState(true);
}

// ************************************************************************************************

void MainWindow::on_ButtonApplyConfig_clicked()
{
    m_SkinName = ui->SkinFileName->text();
    m_SkinConfig.UseBinary = ui->checkBoxBinary->isChecked();
    m_SkinConfig.Endianess = (ui->checkBoxBigEndian->isChecked() == true) ? BIG_ENDIAN : LITTLE_ENDIAN;
    m_SkinConfig.MemoryOffset = ui->MemoryOffset->text().toUInt(nullptr, 16);

    if(ui->radioButton_CustomSize->isChecked() == true)
    {
        m_SkinConfig.DisplaySize.setWidth(ui->lineEdit_CustomWidth->text().toUInt());
        m_SkinConfig.DisplaySize.setHeight(ui->lineEdit_CustomHeight->text().toUInt());
    }
    if(m_SkinConfig.DisplaySize != m_TempDisplaySize)                       // don't do anything if nothing has change
    {
        SaveSkinAndClearData();
        SetSizeDisplay();
        m_SkinConfig.DisplaySize = m_TempDisplaySize;
    }

    SetButtonState(false);
    UpdateStatusBar();
    SetConfigToXML();
}

// ************************************************************************************************

void MainWindow::on_ButtonCancelConfig_clicked()
{
    UpdateConfigurator();
    SetButtonState(false);
}

// ************************************************************************************************
// *
// *  Function
// *
// ************************************************************************************************

void MainWindow::InitConfigurator()
{
    m_SkinConfig.Endianess = BIG_ENDIAN;
    m_SkinConfig.UseBinary = true;
    m_SkinConfig.DisplaySize = {480, 272};

    ui->checkBoxBigEndian->setChecked(true);
    ui->checkBoxLittleEndian->setChecked(false);

    ui->checkBoxBinary->setChecked(true);
    ui->LabelStaticMemoryLocation->setStyleSheet("QLabel { color: black; }");
    ui->MemoryOffset->setEnabled(true);

    this->connect(ui->checkBoxBinary, &QCheckBox::checkStateChanged, this, &MainWindow::on_checkBoxBinary_checkStateChanged);
    this->connect(ui->checkBoxBigEndian, &QCheckBox::checkStateChanged, this, &MainWindow::on_checkBoxBigEndian_checkStateChanged);
    this->connect(ui->checkBoxLittleEndian, &QCheckBox::checkStateChanged, this, &MainWindow::on_checkBoxLittleEndian_checkStateChanged);
    this->connect(ui->ButtonApplyConfig, &QPushButton::clicked, this, &MainWindow::on_ButtonApplyConfig_clicked);
    this->connect(ui->ButtonCancelConfig, &QPushButton::clicked, this, &MainWindow::on_ButtonCancelConfig_clicked);
    this->connect(ui->SkinFileName, &QLineEdit::textEdited, this, &MainWindow::on_SkinFileName_textEdited);
    this->connect(ui->MemoryOffset, &QLineEdit::textEdited, this, &MainWindow::on_MemoryOffset_textEdited);
    this->connect(ui->Tip_BinaryMode, &QPushButton::clicked, this, &MainWindow::showBinaryHelpTip);

    ui->lineEdit_CustomWidth->setValidator(new QIntValidator(0, 2048, this));
    ui->lineEdit_CustomHeight->setValidator(new QIntValidator(0, 2048, this));
    ConfigCheckButton();

    SetButtonState(false);
}

// ************************************************************************************************

void MainWindow::UpdateConfigurator()
{
    ui->SkinFileName->setText(m_SkinName);
    on_checkBoxBinary_checkStateChanged((m_SkinConfig.UseBinary == true) ? Qt::Checked : Qt::Unchecked);        // Use slot to update widget (dirty)
    on_checkBoxLittleEndian_checkStateChanged((m_SkinConfig.Endianess == LITTLE_ENDIAN) ? Qt::Checked : Qt::Unchecked);
    on_checkBoxBigEndian_checkStateChanged((m_SkinConfig.Endianess == BIG_ENDIAN) ? Qt::Checked : Qt::Unchecked);
    ui->MemoryOffset->setText(QString::asprintf("0x%08X", m_SkinConfig.MemoryOffset));
    m_TempDisplaySize = m_SkinConfig.DisplaySize;
    UpdateLineEdit();
    ConfigCheckButton();
    SetButtonState(false);
}

// ************************************************************************************************

void MainWindow::SetButtonState(bool Active)
{
    ui->ButtonApplyConfig->setEnabled(Active);
    ui->ButtonCancelConfig->setEnabled(Active);
}

// ************************************************************************************************

void MainWindow::ResetAllRadioButton()
{
    ui->radioButton_84x48->setChecked(false);
    ui->radioButton_96x64->setChecked(false);
    ui->radioButton_128x128->setChecked(false);
    ui->radioButton_240x320->setChecked(false);
    ui->radioButton_320x200->setChecked(false);
    ui->radioButton_320x240->setChecked(false);
    ui->radioButton_480x272->setChecked(false);
    ui->radioButton_640x480->setChecked(false);
    ui->radioButton_800x480->setChecked(false);
    ui->radioButton_800x600->setChecked(false);
    ui->radioButton_CustomSize->setChecked(false);
    ui->lineEdit_CustomWidth->setEnabled(false);
    ui->lineEdit_CustomHeight->setEnabled(false);
}

// ************************************************************************************************

void MainWindow::UpdateLineEdit()
{
    ui->lineEdit_CustomWidth->setText(QString("%1").arg(m_TempDisplaySize.width()));
    ui->lineEdit_CustomHeight->setText(QString("%1").arg(m_TempDisplaySize.height()));
}

// ************************************************************************************************

void MainWindow::ConfigCheckButton()
{
    if     ((m_SkinConfig.DisplaySize.width() == 84)  && (m_SkinConfig.DisplaySize.height() == 48))  on_radioButton_84x48_clicked();
    else if((m_SkinConfig.DisplaySize.width() == 96)  && (m_SkinConfig.DisplaySize.height() == 64))  on_radioButton_96x64_clicked();
    else if((m_SkinConfig.DisplaySize.width() == 128) && (m_SkinConfig.DisplaySize.height() == 128)) on_radioButton_128x128_clicked();
    else if((m_SkinConfig.DisplaySize.width() == 240) && (m_SkinConfig.DisplaySize.height() == 320)) on_radioButton_240x320_clicked();
    else if((m_SkinConfig.DisplaySize.width() == 320) && (m_SkinConfig.DisplaySize.height() == 200)) on_radioButton_320x200_clicked();
    else if((m_SkinConfig.DisplaySize.width() == 320) && (m_SkinConfig.DisplaySize.height() == 240)) on_radioButton_320x240_clicked();
    else if((m_SkinConfig.DisplaySize.width() == 480) && (m_SkinConfig.DisplaySize.height() == 272)) on_radioButton_480x272_clicked();
    else if((m_SkinConfig.DisplaySize.width() == 640) && (m_SkinConfig.DisplaySize.height() == 480)) on_radioButton_640x480_clicked();
    else if((m_SkinConfig.DisplaySize.width() == 800) && (m_SkinConfig.DisplaySize.height() == 480)) on_radioButton_800x480_clicked();
    else if((m_SkinConfig.DisplaySize.width() == 800) && (m_SkinConfig.DisplaySize.height() == 600)) on_radioButton_800x600_clicked();
    else                                                                                             on_radioButton_CustomSize_clicked();
}

// ************************************************************************************************

void MainWindow::SetSizeDisplay()
{
    QRect   MainRect;
    QRect   TabRect;
    QRect   ViewRect;
    int     AddVertical;
    int     AddHorizontal;

    // All dynamic resize will follow setting from original designer value of 'graphicsViewImage'
    MainRect         = m_MainRect;
    TabRect          = m_TabRect;
    ViewRect         = m_ViewRect;

    // X pos of GraphicsView realigned
    if(m_SkinConfig.DisplaySize.width() > MINIMUM_PREVIEW_WIDTH)
    {
        AddHorizontal = (m_SkinConfig.DisplaySize.width() - MINIMUM_PREVIEW_WIDTH);
        MainRect.setWidth(MainRect.width() + AddHorizontal);
        TabRect.setWidth(TabRect.width()   + AddHorizontal);
        ViewRect.setWidth(ViewRect.width() + AddHorizontal);
    }
    else
    {
        ViewRect.moveLeft(ViewRect.x() + ((MINIMUM_PREVIEW_WIDTH / 2) - (m_SkinConfig.DisplaySize.width() / 2)));
        ViewRect.setWidth((ViewRect.width() - MINIMUM_PREVIEW_WIDTH) +  m_SkinConfig.DisplaySize.width());
    }

    // Y pos of GraphicsView realigned
    if(m_SkinConfig.DisplaySize.height() > MINIMUM_PREVIEW_HEIGHT)
    {
        AddVertical = (m_SkinConfig.DisplaySize.height() - MINIMUM_PREVIEW_HEIGHT);
        MainRect.setHeight(MainRect.height() + AddVertical);
        TabRect.setHeight(TabRect.height()   + AddVertical);
        ViewRect.setHeight(ViewRect.height() + AddVertical);
    }
    else
    {
        ViewRect.moveTop(ViewRect.y() + ((MINIMUM_PREVIEW_HEIGHT / 2) - (m_SkinConfig.DisplaySize.height() / 2)));
        ViewRect.setHeight((ViewRect.height() - MINIMUM_PREVIEW_HEIGHT) +  m_SkinConfig.DisplaySize.height());
    }

    // Set Size of the main window, QTabWidget and QGraphicsView
    this->hide();
    this->setMinimumSize(0, 0);
    this->setMaximumSize(2048, 2048);
    this->setGeometry(MainRect);
    this->setMinimumSize(MainRect.size().width(), MainRect.size().height());
    this->setMaximumSize(MainRect.size().width(), MainRect.size().height());
    ui->TabFunctionSelect->setGeometry(TabRect);

    // Resize tab
    QSize OffsetWidget = GetSizeFromGraphicsView(ui->graphicsViewImage, m_SkinConfig.DisplaySize);
    AdjustTabImage(OffsetWidget, ViewRect);
    AdjustTabConverter(OffsetWidget, ViewRect);
    this->move(SelectScreenAndCenter(this->frameGeometry().size()));
    this->show();
}

// ************************************************************************************************

void MainWindow::showBinaryHelpTip()
{
    QMessageBox msgBox;
    msgBox.setOption(QMessageBox::Option::DontUseNativeDialog);
    msgBox.setStyleSheet("QLabel { color: black; font-weight: bold; }" + ButtonStyle);
    msgBox.setText("All the parameter In the 'Binary Section' must match the config you are using into your Digini project.");
    msgBox.exec();
}

// ************************************************************************************************
