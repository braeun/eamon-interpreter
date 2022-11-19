/********************************************************************************
 *                                                                              *
 * EamonInterpreter - application main window                                   *
 *                                                                              *
 * modified: 2022-11-19                                                         *
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "eamons.h"
#include "runtime/outputstream.h"
#include "runtime/inputstream.h"
#include "runtime/vm.h"
#include "runtime/disassembler.h"
#include <memory>
#include <QDir>
#include <QMainWindow>
#include <QThread>

class Compiler;
class ErrorMessagesDialog;

namespace Ui {
class MainWindow;
}

class EditorWindow;
class VMThread;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent=nullptr);
  ~MainWindow();

protected:
  virtual void closeEvent(QCloseEvent* event) override;
  virtual void keyPressEvent(QKeyEvent *event) override;

private slots:
  void vmFinished();

  void handleNewText(const QString& txt);

  void runFile(QString filename);

  void on_actionQuit_triggered();

  void on_actionPreferences_triggered();

  void on_actionReset_triggered();

  void on_actionRun_triggered();

  void on_actionShow_Player_s_Manual_triggered();

  void on_actionDisplay_Characters_triggered();

  void on_actionExtract_Disk_triggered();

  void on_actionCharacter_File_Maintenance_triggered();

  void on_actionAbout_triggered();

  void on_actionAbout_Qt_triggered();

  void on_actionResurrect_Character_triggered();

  void on_actionInsert_Game_Disk_triggered();

  void on_actionRun_Utility_triggered();

  void on_actionView_Remove_Character_triggered();

  void on_actionExport_Characters_triggered();

  void on_actionImport_Characters_triggered();

  void on_actionScreenshot_triggered();

  void on_actionRun_Disk_triggered();

  void on_actionEditor_triggered();

  void on_actionInsert_Master_Disk_triggered();

private:
  void setupGames();
  void copyMainHall(const std::string& gameDisk);
  void run(QString file="hello");
  void hiresPageLoaded();

  Ui::MainWindow *ui;
  std::unique_ptr<Eamons> eamons;
  QString hallPath;
  std::shared_ptr<OutputStream> os;
  std::shared_ptr<InputStream> is;
  QString filename;
  std::shared_ptr<Compiler> compiler;
  std::shared_ptr<VM> vm;
  Disassembler disassembler;
  std::shared_ptr<Executable> executable;
  VMThread* vmthread;
  QDir currentDisk;
  ErrorMessagesDialog* errorDlg;
  EditorWindow* editor;
};

#endif // MAINWINDOW_H
