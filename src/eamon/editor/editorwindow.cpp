/********************************************************************************
 *                                                                              *
 * EamonInterpreter - editor window                                             *
 *                                                                              *
 * modified: 2022-11-15                                                         *
 *                                                                              *
 ********************************************************************************
 * Copyright (C) Harald Braeuning                                               *
 ********************************************************************************
 * This file is part of EamonInterpreter.                                       *
 * EamonInterpreter is free software: you can redistribute it and/or modify it  *
 * under the terms of the GNU General Public License as published by the Free   *
 * Software Foundation, either version 3 of the License, or (at your option)    *
 * any later version.                                                           *
 * EamonInterpreter is distributed in the hope that it will be useful, but      *
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY   *
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                                *
 * You should have received a copy of the GNU General Public License along with *
 * EamonInterpreter. If not, see <https://www.gnu.org/licenses/>.               *
 ********************************************************************************/

#include "editorwindow.h"
#include "ui_editorwindow.h"
#include "../defines.h"
#include "../errormessagesdialog.h"
#include "../runtime/disassembler.h"
#include "../runtime/compiler.h"
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QTextStream>

EditorWindow::EditorWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::EditorWindow)
{
  ui->setupUi(this);
  errorDlg = new ErrorMessagesDialog(this);
}

EditorWindow::~EditorWindow()
{
  delete ui;
}

void EditorWindow::updateSettings()
{
  ui->editorWidget->updateSettings();
}


void EditorWindow::on_actionOpen_triggered()
{
  QSettings settings;
  QString path = settings.value(SETTING_PATH_DISK,SETTING_VALUE_PATH_DISK).toString();
  QString fn = QFileDialog::getOpenFileName(this,QApplication::applicationDisplayName(),path);
  if (!fn.isNull())
  {
    QFileInfo info(fn);
    settings.setValue(SETTING_PATH_DISK,info.absolutePath());
    QFile file(fn);
    if (file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
      QTextStream s(&file);
      ui->editorWidget->setText(s.readAll());
      filename = fn;
      file.close();
//      ui->disassemblerField->clear();
    }
    else
    {
      QMessageBox::critical(this,QApplication::applicationDisplayName(),"Failed to open script file!");
    }
  }
}

void EditorWindow::on_actionSave_triggered()
{
  if (filename.isEmpty())
  {
    on_actionSave_As_triggered();
  }
  else
  {
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
      QTextStream s(&file);
      QString code = ui->editorWidget->getText();
      s << code;
      file.close();
    }
    else
    {
      QMessageBox::critical(this,QApplication::applicationDisplayName(),"Failed to save script file!");
    }
  }

}

void EditorWindow::on_actionSave_As_triggered()
{
  QSettings settings;
  QString path = settings.value(SETTING_PATH_DISK,SETTING_VALUE_PATH_DISK).toString();
  QString fn = QFileDialog::getSaveFileName(this,QApplication::applicationDisplayName(),path);
  if (!fn.isNull())
  {
    QFileInfo info(fn);
    settings.setValue(SETTING_PATH_DISK,info.absolutePath());
    filename = fn;
    on_actionSave_triggered();
  }
}

void EditorWindow::on_actionParse_File_triggered()
{
  std::istringstream stream(ui->editorWidget->getText().toStdString());
  Compiler compiler;
  Executable* executable = compiler.compile(stream);
  if (!compiler.getErrors().getMessages().empty())
  {
    errorDlg->setErrors(compiler.getErrors());
    errorDlg->show();
  }
  if (!executable)
  {
    QMessageBox::critical(this,QApplication::applicationDisplayName(),"Failed to parse file!");
  }
  else
  {
    std::ostringstream s;
    Disassembler disassembler(s);
    disassembler.disassemble(executable);
    QString str = QString::fromStdString(s.str());
    ui->disassemblerBrowser->setPlainText(str);
    auto symbols = executable->getSymbolTable(Symbol::VARIABLE);
    ui->symbolTableWidget->clearContents();
    ui->symbolTableWidget->setRowCount(symbols.size());
    int row = 0;
    for (Symbol s : symbols)
    {
      ui->symbolTableWidget->setItem(row,0,new QTableWidgetItem(QString::fromStdString(s.getName())));
      ui->symbolTableWidget->setItem(row,1,new QTableWidgetItem(QString::number(s.getAddress())));
      row++;
    }
    delete executable;
  }
}


void EditorWindow::on_actionRun_File_triggered()
{
  on_actionSave_triggered();
  emit runFile(filename);
}


void EditorWindow::on_lineNoField_returnPressed()
{
  QTextCursor crsr = ui->disassemblerBrowser->textCursor();
  crsr.movePosition(QTextCursor::Start);
  ui->disassemblerBrowser->setTextCursor(crsr);
  if (!ui->disassemblerBrowser->find(".LINE",QTextDocument::FindWholeWords))
  {
    QMessageBox::information(this,QApplication::applicationDisplayName(),"No line information available.");
  }
  bool flag = false;
//  if (ui->disassemblerBrowser->find(ui->lineNoField->text(),QTextDocument::FindWholeWords))
//  {
//    int pos = ui->disassemblerBrowser->textCursor().position();
//    ui->disassemblerBrowser->find(".LINE",QTextDocument::FindBackward|QTextDocument::FindWholeWords);
//    int dp = pos - ui->disassemblerBrowser->textCursor().position();
//    qDebug() << pos << ui->disassemblerBrowser->textCursor().position() << dp;
//  }
  flag = ui->disassemblerBrowser->find(".LINE "+ui->lineNoField->text(),QTextDocument::FindWholeWords);
  if (!flag)
  {
    QTextCursor crsr = ui->disassemblerBrowser->textCursor();
    crsr.movePosition(QTextCursor::Start);
    ui->disassemblerBrowser->setTextCursor(crsr);
    QMessageBox::information(this,QApplication::applicationDisplayName(),"Line number not found.");
  }
}
