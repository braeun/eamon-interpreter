/********************************************************************************
 *                                                                              *
 * EamonInterpreter - application main window                                   *
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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include "defines.h"
#include "diskselectdialog.h"
#include "errormessagesdialog.h"
#include "optionsdialog.h"
#include "disk/extractionutils.h"
#include "editor/editorwindow.h"
#include "runtime/compiler.h"
#include "runtime/library.h"
#include "runtime/vmthread.h"
#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QFont>
#include <QFontMetrics>
#include <QSyntaxHighlighter>
#include <sstream>
#include <iostream>
#include <filesystem>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  filename(""),
  executable(nullptr),
  editor(nullptr)
{
  ui->setupUi(this);
  QSettings settings;
  compiler = std::make_shared<Compiler>();
  os = std::make_shared<OutputStream>();
  connect(os.get(),&OutputStream::newText,this,&MainWindow::handleNewText,Qt::QueuedConnection);
  connect(os.get(),&OutputStream::moveToColumn,ui->screenWidget,&Screen::moveToColumn,Qt::QueuedConnection);
  connect(os.get(),&OutputStream::moveToRow,ui->screenWidget,&Screen::moveToRow,Qt::QueuedConnection);
  connect(os.get(),&OutputStream::moveHome,ui->screenWidget,[=](){ui->screenWidget->clear();},Qt::QueuedConnection);
  connect(os.get(),&OutputStream::printInverse,ui->screenWidget,&Screen::inverse,Qt::QueuedConnection);
  connect(os.get(),&OutputStream::printNormal,ui->screenWidget,&Screen::normal,Qt::QueuedConnection);
  connect(os.get(),&OutputStream::hiresLoaded,this,&MainWindow::hiresPageLoaded,Qt::QueuedConnection);
  connect(os.get(),&OutputStream::changeScreenMode,this,&MainWindow::setScreenMode,Qt::QueuedConnection);
  is = std::make_shared<InputStream>();
  vm = std::make_shared<VM>(is,os);
  vmthread = new VMThread(vm);
  connect(vmthread,&QThread::finished,this,&MainWindow::vmFinished);
  errorDlg = new ErrorMessagesDialog(this);
  ui->screenWidget->setFocus();
  ui->screenWidget->setFocusPolicy(Qt::FocusPolicy::ClickFocus);

  setupGames();
  if (settings.value(SETTING_AUTOSTART,SETTING_VALUE_AUTOSTART).toBool())
  {
    QMetaObject::invokeMethod(this,&MainWindow::on_actionRun_triggered,Qt::ConnectionType::QueuedConnection);
  }
}

MainWindow::~MainWindow()
{
  delete ui;
}



void MainWindow::closeEvent(QCloseEvent *event)
{
  vmthread->pause();
  vmthread->wait(QDeadlineTimer(500));
  vmthread->terminate();
  event->accept();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
//  qDebug() << "keyPressEvent: " << event;
  if (event->modifiers().testFlag(Qt::ControlModifier))
  {
    switch (event->key())
    {
      case Qt::Key_C:
//        copy();
        break;
      case Qt::Key_F:
//        on_actionSearch_in_Dict_triggered();
        break;
      case Qt::Key_V:
//        paste();
        break;
    }
  }
  if (event->modifiers().testFlag(Qt::AltModifier))
  {
  }
  {
    if (is->echoInput())
    {
      ui->screenWidget->setPunctuation(false);
      if (event->key() == Qt::Key_Backspace && is->canBackspace())
        ui->screenWidget->backspace();
      else if (event->modifiers().testFlag(Qt::ControlModifier) && event->key() == Qt::Key_G)
        ui->screenWidget->print(QString::fromStdString(is->getLastEntry()+"\n"));
      else if (event->key() == Qt::Key_Return)
        ui->screenWidget->print("\n");
      else
        ui->screenWidget->print(event->text().toUpper());
    }
    ui->screenWidget->setPunctuation(true);
    is->handleKey(event);
//    if (state == ShellInputStream::Idle) ui->screenWidget->setPunctuation(true);
  }
  event->accept();
}



void MainWindow::setupGames()
{
  QSettings settings;
  QString path = settings.value(SETTING_PATH_GAMES,SETTING_VALUE_PATH_GAMES).toString();
  eamons = std::make_unique<Eamons>(path.toStdString());
  if (eamons->getMainHall().empty())
  {
    std::string gameDisk = (path+"/"+Eamons::main_hall_name).toStdString();
    try
    {
      copyMainHall(gameDisk);
    }
    catch (std::filesystem::filesystem_error& ex)
    {
      QMessageBox::critical(this,QApplication::applicationDisplayName(),QString("Failed to create main hall:\n")+ex.what());
    }
  }
  eamons = std::make_unique<Eamons>(path.toStdString());
}

void MainWindow::copyMainHall(const std::string& gameDisk)
{
  std::filesystem::create_directories(gameDisk);
  QDir srcdir(":/resources/main_hall");
  if (srcdir.exists())
  {
    QDir destdir(QString::fromStdString(gameDisk));
    for (const QFileInfo& info : srcdir.entryInfoList())
    {
      qDebug() << info.absoluteFilePath();
      QFile::copy(info.absoluteFilePath(),destdir.absoluteFilePath(info.fileName()));
    }
  }
  else if (!ExtractionUtils::extractDisk(":/resources/D3_001.DSK",QString::fromStdString(gameDisk)))
  {
    QMessageBox::critical(this,QApplication::applicationDisplayName(),"Failed to recreate main hall!");
  }
}

void MainWindow::run(QString file)
{
  std::cout << "run:" << file.toStdString() << std::endl;
  ui->stackedWidget->setCurrentWidget(ui->gamePage);
  QFileInfo f(currentDisk.absoluteFilePath(file));
  if (f.exists())
  {
    compiler->reset();
    executable = std::shared_ptr<Executable>(compiler->compile(f.absoluteFilePath().toStdString()));
    if (!compiler->getErrors().getMessages().empty())
    {
      errorDlg->setErrors(compiler->getErrors());
      errorDlg->show();
    }
    if (executable)
    {
      QSettings settings;
      vm->setSlowdown(settings.value(SETTING_VM_SLOWDOWN,SETTING_VALUE_VM_SLOWDOWN).toUInt());
      ui->screenWidget->setFocus();
      vm->setDisk(currentDisk.absolutePath().toStdString());
      vmthread->run(executable);
//      vmthread->runDirect(executable);
//      vmFinished();
    }
    else
    {
      QMessageBox::critical(this,QApplication::applicationDisplayName(),"Failed to execute script!");
    }
  }
  else
  {
    QMessageBox::critical(this,QApplication::applicationDisplayName(),"Failed to open script file!");
  }
}

void MainWindow::hiresPageLoaded()
{
  ui->screenWidget->setImage(vm->getHiresPage());
}

void MainWindow::setScreenMode(int m)
{
  switch (static_cast<OutputStream::ScreenMode>(m))
  {
    case OutputStream::Text:
      ui->screenWidget->setMode(Screen::Text);
      break;
    case OutputStream::Graphics:
      ui->screenWidget->setMode(Screen::Graphics);
      break;
  }
}



void MainWindow::vmFinished()
{
  if (!vmthread->getError().isEmpty())
  {
    QMessageBox::warning(this,QApplication::applicationDisplayName(),"VM terminated with error:\n"+vmthread->getError());
  }
  else
  {
    std::string chain = vm->getChainedFile();
    if (!chain.empty())
    {
      run(QString::fromStdString(chain));
    }
    else
    {
      QMessageBox::information(this,QApplication::applicationDisplayName(),"Finished!");
    }
  }
}

void MainWindow::handleNewText(const QString& txt)
{
  ui->screenWidget->print(txt);
}

void MainWindow::runFile(QString filename)
{
  QFileInfo info(filename);
  currentDisk = QDir(info.absolutePath());
  ui->currentDiskLabel->setText(currentDisk.dirName());
  ui->screenWidget->clear();
  run(filename);
}



void MainWindow::on_actionQuit_triggered()
{
  vmthread->pause();
  vmthread->wait(QDeadlineTimer(500));
  vmthread->terminate();
  QApplication::exit();
}

void MainWindow::on_actionPreferences_triggered()
{
  OptionsDialog d(this);
  if (d.exec())
  {
    d.commit();
    ui->screenWidget->updateSettings();
    if (editor) editor->updateSettings();
  }
}

void MainWindow::on_actionReset_triggered()
{
  if (QMessageBox::question(this,QApplication::applicationDisplayName(),"Really reset the main hall?") == QMessageBox::Yes)
  {
    const std::string& gameDisk = eamons->getMainHall();
    try
    {
      std::filesystem::remove_all(gameDisk);
      copyMainHall(gameDisk);
    }
    catch (std::filesystem::filesystem_error& ex)
    {
      QMessageBox::critical(this,QApplication::applicationDisplayName(),QString("Failed to recreate main hall:\n")+ex.what());
    }
  }
}

void MainWindow::on_actionRun_triggered()
{
  std::cout << "call on_actionRun_triggered()" << std::endl;
  std::cout.flush();
  if (eamons->getMainHall().empty())
  {
    QMessageBox::critical(this,QApplication::applicationDisplayName(),"Could not find disk with main hall!");
//    QSettings settings;
//    QString path = settings.value(SETTING_PATH_DISK,SETTING_VALUE_PATH_DISK).toString();
//    QString fn = QFileDialog::getExistingDirectory(this,QApplication::applicationDisplayName(),path);
//    if (!fn.isNull())
//    {
//      QFileInfo info(fn);
//      settings.setValue(SETTING_PATH_DISK,info.absolutePath());
//      currentDisk = QDir(fn);
//      ui->currentDiskLabel->setText(currentDisk.dirName());
//      ui->screenWidget->clear();
//      run();
//    }
  }
  else
  {
    currentDisk = QDir(QString::fromStdString(eamons->getMainHall()));
    ui->currentDiskLabel->setText(currentDisk.dirName());
  }
  ui->screenWidget->clear();
  run();
}

void MainWindow::on_actionShow_Player_s_Manual_triggered()
{
  if (eamons->getMainHall().empty())
  {
    QMessageBox::warning(this,QApplication::applicationDisplayName(),"Cannot display players manual!");
  }
  else
  {
    QDir disk(QString::fromStdString(eamons->getMainHall()));
    QFileInfo f(disk.absoluteFilePath(Eamons::players_manual));
    if (f.exists())
    {
      QFile file(f.absoluteFilePath());
      if (file.open(QFile::ReadOnly))
      {
        QByteArray data = file.readAll();
        ui->stackedWidget->setCurrentWidget(ui->textPage);
        ui->textBrowser->setPlainText(data);
      }
      else
      {
        QMessageBox::warning(this,QApplication::applicationDisplayName(),"Failed to read players manual!");
      }
    }
    else
    {
      QMessageBox::warning(this,QApplication::applicationDisplayName(),"No players manual found on main disk!");
    }
  }
}

void MainWindow::on_actionDisplay_Characters_triggered()
{
  if (eamons->getMainHall().empty())
  {
    QMessageBox::warning(this,QApplication::applicationDisplayName(),"Cannot display characters!");
  }
  else
  {
    currentDisk = QDir(QString::fromStdString(eamons->getMainHall()));
    ui->currentDiskLabel->setText(currentDisk.dirName());
    run(Eamons::display_characters_script);
  }
}

void MainWindow::on_actionExtract_Disk_triggered()
{
  QSettings settings;
  QString path = settings.value(SETTING_PATH_DISKIMG,SETTING_VALUE_PATH_DISKIMG).toString();
  QString filename = QFileDialog::getOpenFileName(this,QApplication::applicationDisplayName(),path,"Disk Images (*.dsk *.DSK);;All Files (*)");
  if (!filename.isEmpty())
  {
    QFileInfo info(filename);
    settings.setValue(SETTING_PATH_DISKIMG,info.absolutePath());
    QDir dir(QString::fromStdString(eamons->getPath()));
    QString dirname = dir.absoluteFilePath(info.baseName());
    QFileInfo dirinfo(dirname);
    if (dirinfo.exists())
    {
      auto ret = QMessageBox::question(this,QApplication::applicationDisplayName(),
                                       "It seems the disk has already been extrected!\nOverwrite the current files?");
      if (ret != QMessageBox::Yes) return;
      std::filesystem::remove_all(dirname.toStdString());
    }
    if (ExtractionUtils::extractDisk(filename,dirname))
    {
      QMessageBox::information(this,QApplication::applicationDisplayName(),"Extraction of disk image finished!");
    }
    else
    {
      QMessageBox::warning(this,QApplication::applicationDisplayName(),"Extraction of disk image failed!");
    }
    setupGames();
  }
}

void MainWindow::on_actionCharacter_File_Maintenance_triggered()
{
  if (eamons->getMainHall().empty())
  {
    QMessageBox::warning(this,QApplication::applicationDisplayName(),"Cannot start character file maintenance!");
  }
  else
  {
    currentDisk = QDir(QString::fromStdString(eamons->getMainHall()));
    ui->currentDiskLabel->setText(currentDisk.dirName());
    run(Eamons::character_file_maintenance_script);
  }
}

void MainWindow::on_actionAbout_triggered()
{
  AboutDialog().exec();
}

void MainWindow::on_actionAbout_Qt_triggered()
{
  QApplication::aboutQt();
}

void MainWindow::on_actionResurrect_Character_triggered()
{
  if (eamons->getMainHall().empty())
  {
    QMessageBox::warning(this,QApplication::applicationDisplayName(),"Cannot start character file maintenance!");
  }
  else
  {
    currentDisk = QDir(QString::fromStdString(eamons->getMainHall()));
    ui->currentDiskLabel->setText(currentDisk.dirName());
    run(Eamons::resurrect_character_script);
  }
}

void MainWindow::on_actionInsert_Game_Disk_triggered()
{
  DiskSelectDialog d(eamons->getGames(),this);
  if (d.exec())
  {
    QString game = d.getDisk();
    if (!game.isEmpty())
    {
      QString disk = QString::fromStdString(eamons->getGameInfo(game.toStdString()).disk);
      currentDisk = QDir(disk);
      vm->setDisk(currentDisk.absolutePath().toStdString());
      ui->currentDiskLabel->setText(currentDisk.dirName());
    }
  }
}

void MainWindow::on_actionRun_Utility_triggered()
{
  QSettings settings;
  QString path = settings.value(SETTING_PATH_DISK,SETTING_VALUE_PATH_DISK).toString();
  QString fn = QFileDialog::getOpenFileName(this,QApplication::applicationDisplayName(),path);
  if (!fn.isNull())
  {
    QFileInfo info(fn);
    settings.setValue(SETTING_PATH_DISK,info.absolutePath());
    currentDisk = QDir(info.absolutePath());
    ui->currentDiskLabel->setText(currentDisk.dirName());
    ui->screenWidget->clear();
    run(fn);
  }
}

void MainWindow::on_actionView_Remove_Character_triggered()
{
  if (eamons->getMainHall().empty())
  {
    QMessageBox::warning(this,QApplication::applicationDisplayName(),"Cannot start view / remove character script!");
  }
  else
  {
    currentDisk = QDir(QString::fromStdString(eamons->getMainHall()));
    ui->currentDiskLabel->setText(currentDisk.dirName());
    ui->screenWidget->clear();
    run(Eamons::view_remove_character_script);
  }

}

void MainWindow::on_actionExport_Characters_triggered()
{
  QSettings settings;
  QString path = settings.value(SETTING_PATH_EXPORT,SETTING_VALUE_PATH_EXPORT).toString();
  QString fn = QFileDialog::getSaveFileName(this,QApplication::applicationDisplayName(),path);
  if (!fn.isNull())
  {
    QFileInfo info(fn);
    settings.setValue(SETTING_PATH_EXPORT,info.absolutePath());
    eamons->exportCharacters(fn.toStdString());
  }
}

void MainWindow::on_actionImport_Characters_triggered()
{
  QSettings settings;
  QString path = settings.value(SETTING_PATH_EXPORT,SETTING_VALUE_PATH_EXPORT).toString();
  QString fn = QFileDialog::getOpenFileName(this,QApplication::applicationDisplayName(),path);
  if (!fn.isNull())
  {
    QFileInfo info(fn);
    settings.setValue(SETTING_PATH_EXPORT,info.absolutePath());
    eamons->importCharacters(fn.toStdString());
  }
}

void MainWindow::on_actionScreenshot_triggered()
{
  QPixmap screenshot = ui->centralwidget->grab(); //emulator->getScreen()->grab();
  QSettings settings;
  QString path = settings.value(SETTING_PATH_SCREENSHOT,SETTING_VALUE_PATH_SCREENSHOT).toString();
  QString fn = QFileDialog::getSaveFileName(this,QApplication::applicationDisplayName(),path,"Portable Network Graphics(*.png)");
  if (!fn.isNull())
  {
    QFileInfo info(fn);
    settings.setValue(SETTING_PATH_SCREENSHOT,info.absolutePath());
    if (!screenshot.save(fn))
    {
      QMessageBox::warning(this,QApplication::applicationDisplayName(),tr("Failed to save screenshot!"));
    }
  }

}

void MainWindow::on_actionRun_Disk_triggered()
{
  DiskSelectDialog d(eamons->getGames(),this);
  if (d.exec())
  {
    QString game = d.getDisk();
    if (!game.isEmpty())
    {
      const EamonInfo& eamon = eamons->getGameInfo(game.toStdString());
      currentDisk = QDir(QString::fromStdString(eamon.disk));
      vm->setDisk(currentDisk.absolutePath().toStdString());
      ui->currentDiskLabel->setText(currentDisk.dirName());
      run(QString::fromStdString(DiskFile::correctFilename(eamon.name)));
    }
  }
}

void MainWindow::on_actionEditor_triggered()
{
  if (!editor)
  {
    editor = new EditorWindow(this);
    connect(editor,&EditorWindow::runFile,this,&MainWindow::runFile);
  }
  editor->show();
  editor->raise();
}
