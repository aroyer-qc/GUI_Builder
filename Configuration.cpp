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

// ************************************************************************************************
// *
// *  Slot
// *
// ************************************************************************************************

void MainWindow::on_checkBoxLoadable_checkStateChanged(int state)
{
    if(state == Qt::Checked)
    {
        ui->checkBoxBinary->setChecked(false);
        m_SkinType = SKIN_TYPE_LOADABLE;
    }
    else if(state == Qt::Unchecked)
    {
        ui->checkBoxBinary->setChecked(true);
        m_SkinType = SKIN_TYPE_BINARY;
    }
}

// ************************************************************************************************

void MainWindow::on_checkBoxBinary_checkStateChanged(int state)
{
    if(state == Qt::Checked)
    {
        ui->checkBoxLoadable->setChecked(false);
        m_SkinType = SKIN_TYPE_BINARY;
    }
    else if(state == Qt::Unchecked)
    {
        ui->checkBoxLoadable->setChecked(true);
        m_SkinType = SKIN_TYPE_LOADABLE;
    }
}

// ************************************************************************************************

void MainWindow::on_checkBoxLittleEndian_checkStateChanged(int state)
{
    if(state == Qt::Checked)
    {
        ui->checkBoxBigEndian->setChecked(false);
        m_Endian = LITTLE_ENDIAN;
    }
    else if(state == Qt::Unchecked)
    {
        ui->checkBoxBigEndian->setChecked(true);
        m_Endian = BIG_ENDIAN;
    }
}

// ************************************************************************************************

void MainWindow::on_checkBoxBigEndian_checkStateChanged(int state)
{
    if(state == Qt::Checked)
    {
        ui->checkBoxLittleEndian->setChecked(false);
        m_Endian = BIG_ENDIAN;
    }
    else if(state == Qt::Unchecked)
    {
        ui->checkBoxLittleEndian->setChecked(true);
        m_Endian = LITTLE_ENDIAN;
    }
}

// ************************************************************************************************

void MainWindow::on_radioButton_84x48_clicked()
{
    ResetAllRadioButton();
    ui->radioButton_84x48->setChecked(true);
    m_ConfigDisplaySize = {84, 48};
    UpdateLineEdit();
}

// ************************************************************************************************

void MainWindow::on_radioButton_96x64_clicked()
{
    ResetAllRadioButton();
    ui->radioButton_96x64->setChecked(true);
    m_ConfigDisplaySize = {96, 64};
    UpdateLineEdit();
}

// ************************************************************************************************

void MainWindow::on_radioButton_128x128_clicked()
{
    ResetAllRadioButton();
    ui->radioButton_128x128->setChecked(true);
    m_ConfigDisplaySize = {128, 128};
    UpdateLineEdit();
}

// ************************************************************************************************

void MainWindow::on_radioButton_240x320_clicked()
{
    ResetAllRadioButton();
    ui->radioButton_240x320->setChecked(true);
    m_ConfigDisplaySize = {240, 320};
    UpdateLineEdit();
}

// ************************************************************************************************

void MainWindow::on_radioButton_320x200_clicked()
{
    ResetAllRadioButton();
    ui->radioButton_320x200->setChecked(true);
    m_ConfigDisplaySize = {320, 200};
    UpdateLineEdit();
}

// ************************************************************************************************

void MainWindow::on_radioButton_320x240_clicked()
{
    ResetAllRadioButton();
    ui->radioButton_320x240->setChecked(true);
    m_ConfigDisplaySize = {320, 240};
    UpdateLineEdit();
}

// ************************************************************************************************

void MainWindow::on_radioButton_480x272_clicked()
{
    ResetAllRadioButton();
    ui->radioButton_480x272->setChecked(true);
    m_ConfigDisplaySize = {480, 272};
    UpdateLineEdit();
}

// ************************************************************************************************

void MainWindow::on_radioButton_640x480_clicked()
{
    ResetAllRadioButton();
    ui->radioButton_640x480->setChecked(true);
    m_ConfigDisplaySize = {640, 480};
    UpdateLineEdit();
}

// ************************************************************************************************

void MainWindow::on_radioButton_800x480_clicked()
{
    ResetAllRadioButton();
    ui->radioButton_800x480->setChecked(true);
    m_ConfigDisplaySize = {800, 480};
    UpdateLineEdit();
}

// ************************************************************************************************

void MainWindow::on_radioButton_800x600_clicked()
{
    ResetAllRadioButton();
    ui->radioButton_800x600->setChecked(true);
    m_ConfigDisplaySize = {800, 600};
    UpdateLineEdit();
}

// ************************************************************************************************

void MainWindow::on_radioButton_CustomSize_clicked()
{
    ResetAllRadioButton();
    ui->radioButton_CustomSize->setChecked(true);
    ui->lineEdit_CustomWidth->setEnabled(true);
    ui->lineEdit_CustomHeight->setEnabled(true);
}

// ************************************************************************************************
// *
// *  Function
// *
// ************************************************************************************************

void MainWindow::InitConfigurator()
{
    m_Endian = BIG_ENDIAN;
    m_SkinType = SKIN_TYPE_BINARY;

    ui->checkBoxBinary->setChecked(true);
    ui->checkBoxLoadable->setChecked(false);
    ui->checkBoxBigEndian->setChecked(true);
    ui->checkBoxLittleEndian->setChecked(false);
    
    this->connect(ui->checkBoxBinary, &QCheckBox::stateChanged, this, &MainWindow::on_checkBoxBinary_checkStateChanged);
    this->connect(ui->checkBoxLoadable, &QCheckBox::stateChanged, this, &MainWindow::on_checkBoxLoadable_checkStateChanged);
    this->connect(ui->checkBoxBigEndian, &QCheckBox::stateChanged, this, &MainWindow::on_checkBoxBigEndian_checkStateChanged);
    this->connect(ui->checkBoxLittleEndian, &QCheckBox::stateChanged, this, &MainWindow::on_checkBoxLittleEndian_checkStateChanged);

    ui->lineEdit_CustomWidth->setValidator(new QIntValidator(0, 2048, this));
    ui->lineEdit_CustomHeight->setValidator(new QIntValidator(0, 2048, this));
    m_ConfigDisplaySize = m_DisplaySize;
    CheckButton();
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
    ui->lineEdit_CustomWidth->setText(QString("%1").arg(m_ConfigDisplaySize.width()));
    ui->lineEdit_CustomHeight->setText(QString("%1").arg(m_ConfigDisplaySize.height()));
}

// ************************************************************************************************

void MainWindow::CheckButton()
{
    if     ((m_ConfigDisplaySize.width() == 84)  && (m_ConfigDisplaySize.height() == 48))  on_radioButton_84x48_clicked();
    else if((m_ConfigDisplaySize.width() == 96)  && (m_ConfigDisplaySize.height() == 64))  on_radioButton_96x64_clicked();
    else if((m_ConfigDisplaySize.width() == 128) && (m_ConfigDisplaySize.height() == 128)) on_radioButton_128x128_clicked();
    else if((m_ConfigDisplaySize.width() == 240) && (m_ConfigDisplaySize.height() == 320)) on_radioButton_240x320_clicked();
    else if((m_ConfigDisplaySize.width() == 320) && (m_ConfigDisplaySize.height() == 200)) on_radioButton_320x200_clicked();
    else if((m_ConfigDisplaySize.width() == 320) && (m_ConfigDisplaySize.height() == 240)) on_radioButton_320x240_clicked();
    else if((m_ConfigDisplaySize.width() == 480) && (m_ConfigDisplaySize.height() == 272)) on_radioButton_480x272_clicked();
    else if((m_ConfigDisplaySize.width() == 640) && (m_ConfigDisplaySize.height() == 480)) on_radioButton_640x480_clicked();
    else if((m_ConfigDisplaySize.width() == 800) && (m_ConfigDisplaySize.height() == 480)) on_radioButton_800x480_clicked();
    else if((m_ConfigDisplaySize.width() == 800) && (m_ConfigDisplaySize.height() == 600)) on_radioButton_800x600_clicked();
    else                                                                                   on_radioButton_CustomSize_clicked();
}

// ************************************************************************************************

void MainWindow::SetSizeDisplay(QSize Size)
{
    QRect   MainRect;
    QRect   TabRect;
    QRect   ViewRect;
    int     AddVertical;
    int     AddHorizontal;

    if(m_ConfigDisplaySize != m_DisplaySize)                       // don't do anything if nothing has change
    {
        SaveSkinAndClearData();
        m_DisplaySize = m_ConfigDisplaySize;
        SaveDisplaySizeToXML(m_ConfigDisplaySize);
    }

    // All dynamic resize will follow setting from original designer value of 'graphicsViewImage'
    MainRect         = m_MainRect;
    TabRect          = m_TabRect;
    ViewRect         = m_ViewRect;

    // X pos of GraphicsView realigned
    if(m_DisplaySize.width() > MINIMUM_PREVIEW_WIDTH)
    {
        AddHorizontal = (m_DisplaySize.width() - MINIMUM_PREVIEW_WIDTH);
        MainRect.setWidth(MainRect.width() + AddHorizontal);
        TabRect.setWidth(TabRect.width()   + AddHorizontal);
        ViewRect.setWidth(ViewRect.width() + AddHorizontal);
    }
    else
    {
        ViewRect.moveLeft(ViewRect.x() + ((MINIMUM_PREVIEW_WIDTH / 2) - (m_DisplaySize.width() / 2)));
        ViewRect.setWidth((ViewRect.width() - MINIMUM_PREVIEW_WIDTH) +  m_DisplaySize.width());
    }

    // Y pos of GraphicsView realigned
    if(m_DisplaySize.height() > MINIMUM_PREVIEW_HEIGHT)
    {
        AddVertical = (m_DisplaySize.height() - MINIMUM_PREVIEW_HEIGHT);
        MainRect.setHeight(MainRect.height() + AddVertical);
        TabRect.setHeight(TabRect.height()   + AddVertical);
        ViewRect.setHeight(ViewRect.height() + AddVertical);
    }
    else
    {
        ViewRect.moveTop(ViewRect.y() + ((MINIMUM_PREVIEW_HEIGHT / 2) - (m_DisplaySize.height() / 2)));
        ViewRect.setHeight((ViewRect.height() - MINIMUM_PREVIEW_HEIGHT) +  m_DisplaySize.height());
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
    QSize OffsetWidget = GetSizeFromGraphicsView(ui->graphicsViewImage, m_DisplaySize);
    AdjustTabImage(OffsetWidget, ViewRect);
    AdjustTabConverter(OffsetWidget, ViewRect);
    this->move(SelectScreenAndCenter(this->frameGeometry().size()));
    this->show();
}

