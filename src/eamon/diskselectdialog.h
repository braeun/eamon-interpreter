/********************************************************************************
 *                                                                              *
 * EamonInterpreter - select game disk dialog                                   *
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

#ifndef DISKSELECTDIALOG_H
#define DISKSELECTDIALOG_H

#include <QDialog>

namespace Ui {
class DiskSelectDialog;
}

class DiskSelectDialog : public QDialog
{
  Q_OBJECT

public:
  explicit DiskSelectDialog(const std::vector<std::string>& games, QWidget *parent = nullptr);
  ~DiskSelectDialog();

  QString getDisk() const;

private:
  Ui::DiskSelectDialog *ui;
};

#endif // DISKSELECTDIALOG_H
