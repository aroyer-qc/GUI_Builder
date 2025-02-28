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
#include "SkinSave.h"

// ************************************************************************************************
// *
// *  Slot
// *
// ************************************************************************************************

void MainWindow::on_actionNew_Project_triggered()
{
    SaveSkinAndClearData();
}

// ************************************************************************************************

void MainWindow::on_actionOpen_Skin_triggered()
{
    if(SaveSkinAndClearData() == false)
    {
        QFileDialog fileDialog(this, tr("Open file"), m_SkinDir.absolutePath(), tr("Skin file (*.skn)"));
        fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);
        fileDialog.resize(800, 600);
        fileDialog.setStyleSheet("QPushButton {color: #DFD;background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6B82AC, stop: 0.49 #566D97, stop: 0.5 #445B85, stop: 1 #566D97); border-width: 3px; border-color: #778EB8; border-style: solid; border-radius: 7; padding: 3px; font: bold \"Ubuntu\"; font-size: 12px; padding-left: 5px; padding-right: 5px; }");

        QListView *listView = fileDialog.findChild<QListView*>("listView");
        if(listView)
        {
            listView->setMinimumSize(400, 300); // Adjust the minimum size of the list view pane
        }

        QTreeView *treeView = fileDialog.findChild<QTreeView*>("treeView");
        if(treeView)
        {
            treeView->setMinimumSize(400, 300); // Adjust the minimum size of the tree view pane
        }

        QList<QPushButton*> buttons = fileDialog.findChildren<QPushButton*>();
        for(QPushButton *button : buttons)
        {
            button->setMinimumWidth(150); // Set the minimum width of the buttons
        }

        if (fileDialog.exec() == QDialog::Accepted)
        {
            QString File = fileDialog.selectedFiles().first();
            if (!File.isEmpty())
            {
                Open(File);
            }
        }
    }
    else
    {
        m_IsNeedCompleteFileOpen = true;            // Will trigger file open at the end saving
    }
}

// ************************************************************************************************

void MainWindow::on_actionSave_Skin_triggered()
{
    if(m_SkinSize != 0)
    {
        if(m_IsSkinHasUnsavedData == true)
        {
            if(m_IsSkinSaveAs == true)
            {
                on_actionSave_Skin_As_triggered();
            }
            else
            {
                Save();
            }
        }
        else if(m_IsWarningDisplayed == true)
        {
            QMessageBox msgBox;
            msgBox.setText("<font size=\"5\">" + tr("File already up to date</font>"));
            msgBox.setIconPixmap(QPixmap(":/graphic/Warning64.png"));
            msgBox.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint);
            msgBox.exec();
        }
    }
    else if(m_IsWarningDisplayed == true)
    {
        QMessageBox msgBox;
        msgBox.setText("<font size=\"5\">" + tr("Nothing to save</font>"));
        msgBox.setIconPixmap(QPixmap(":/graphic/Warning64.png"));
        msgBox.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint);
        msgBox.exec();
    }
}

// ************************************************************************************************

void MainWindow::on_actionSave_Skin_As_triggered()
{
    QFileDialog fileDialog(this, tr("Save file as"), m_SkinDir.absolutePath() + "/untitled", tr("Skin file (*.skn)"));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);
    fileDialog.setStyleSheet("QPushButton {color: #DFD;background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6B82AC, stop: 0.49 #566D97, stop: 0.5 #445B85, stop: 1 #566D97); border-width: 3px; border-color: #778EB8; border-style: solid; border-radius: 7; padding: 3px; font: bold \"Ubuntu\"; font-size: 12px; padding-left: 5px; padding-right: 5px; }");

    // Adjust the size of the QFileDialog window
    fileDialog.resize(800, 600);  // Set width to 800 and height to 600
    fileDialog.setLabelText(QFileDialog::Accept, "Save Loadable Skin");


    QListView *listView = fileDialog.findChild<QListView*>("listView");
    if(listView)
    {
        listView->setMinimumSize(400, 300); // Adjust the minimum size of the list view pane
    }

    QTreeView *treeView = fileDialog.findChild<QTreeView*>("treeView");
    if(treeView)
    {
        treeView->setMinimumSize(400, 300); // Adjust the minimum size of the tree view pane
    }

    // Set the horizontal size of the buttons within the dialog
    QList<QPushButton*> buttons = fileDialog.findChildren<QPushButton*>();
    for(QPushButton *button : buttons)
    {
        button->setMinimumWidth(150); // Set the minimum width of the buttons to 150
    }

    // Create a custom button for saving the skin
    QPushButton *saveSkinButton = new QPushButton("Save Binary Skin", &fileDialog);
    saveSkinButton->setToolTip("This set the project type and will save data skin file in binary skin format to be loaded into internal flash memory of the project.");

    // Locate the button box within the file dialog
    QDialogButtonBox *buttonBox = fileDialog.findChild<QDialogButtonBox*>();
    if (buttonBox)
    {
        // Change the label of the "Save" button
        QAbstractButton *saveButton = buttonBox->button(QDialogButtonBox::Save);
        if(saveButton)
        {
            saveButton->setText("Save Loadable Skin");
            saveButton->setToolTip("This set the project type and will save data skin file in loadable skin format for medium like SD Card, USB Key, Etc...");
        }

        // Add the custom button before the "Cancel" button
        buttonBox->addButton(saveSkinButton, QDialogButtonBox::ActionRole);

        // Move the button to the desired position
        QAbstractButton *cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
        QHBoxLayout *layout = qobject_cast<QHBoxLayout*>(buttonBox->layout());
        if (layout && cancelButton)
        {
            layout->removeWidget(saveSkinButton);
            int cancelIndex = layout->indexOf(cancelButton);
            layout->insertWidget(cancelIndex, saveSkinButton);
        }

        // Connect the custom button to a custom slot for saving the skin
        connect(saveSkinButton, &QPushButton::clicked, this, &MainWindow::on_SaveBinaryButton_clicked);
        // Connect the custom button to QDialog::accept slot
        connect(saveSkinButton, &QPushButton::clicked, &fileDialog, &QDialog::accept);
    }

    if (fileDialog.exec() == QDialog::Accepted)
    {
        QString File = fileDialog.selectedFiles().first();

        if (!File.isEmpty())
        {
            m_SkinName = File;
            m_IsSkinSaveAs = false;
            m_IsSkinHasUnsavedData = true;  // To trigger data to save
            on_actionSave_Skin_triggered();
            m_IsSkinHasUnsavedData = false;
        }
    }
    else
    {
        m_IsNeedCompleteFileOpen = true;  // Will trigger file open at the end saving
    }

    delete saveSkinButton;
}

void MainWindow::on_SaveBinaryButton_clicked()
{
    m_SkinType = SKIN_TYPE_BINARY;
}

// ************************************************************************************************
// *
// *  Function
// *
// ************************************************************************************************

bool MainWindow::SaveSkinAndClearData()
{
    bool FileSaveInProgress = false;

    if(m_IsSkinHasUnsavedData == true)
    {
        QMessageBox msgBox;
        msgBox.setText("<font size=\"5\">" + tr("Current skin not saved<br>Do you want to save?</font>"));
        msgBox.setIconPixmap(QPixmap(":/graphic/question_mark64.png"));
        msgBox.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);

        if(msgBox.exec() == QMessageBox::Yes)
        {
            m_IsWarningDisplayed = false;
            on_actionSave_Skin_triggered();  // todo how to decide wich method to use!!!????
            m_IsWarningDisplayed = true;
            FileSaveInProgress = true;
            m_IsNeedToClearDataAfterSave = true;
        }
        else
        {
            ClearAllData();
            ResetAllSkinTab();
        }
    }
    else
    {
        ClearAllData();
        ResetAllSkinTab();
    }

    return FileSaveInProgress;
}

// ************************************************************************************************

void MainWindow::setSkinHasUnsavedData(bool State)
{
    m_IsSkinHasUnsavedData = State;
    if(State)   this->setWindowTitle("* Skin Builder");
    else        this->setWindowTitle("Skin Builder");
}
