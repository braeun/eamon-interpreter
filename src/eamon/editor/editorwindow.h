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

#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <QMainWindow>

namespace Ui {
class EditorWindow;
}

class ErrorMessagesDialog;

class EditorWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit EditorWindow(QWidget *parent = nullptr);
  ~EditorWindow();

  void updateSettings();

signals:
  void runFile(QString s);

private slots:
  void on_actionOpen_triggered();

  void on_actionSave_triggered();

  void on_actionSave_As_triggered();

  void on_actionParse_File_triggered();

  void on_actionRun_File_triggered();

  void on_lineNoField_returnPressed();

private:
  Ui::EditorWindow *ui;
  ErrorMessagesDialog* errorDlg;
  QString filename;
};

#endif // EDITORWINDOW_H
