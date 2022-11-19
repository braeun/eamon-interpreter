/********************************************************************************
 *                                                                              *
 * EamonInterpreter - dialog for error messages                                 *
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

#include "errormessagesdialog.h"
#include "ui_errormessagesdialog.h"

ErrorMessagesDialog::ErrorMessagesDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ErrorMessagesDialog)
{
  ui->setupUi(this);
}

ErrorMessagesDialog::~ErrorMessagesDialog()
{
  delete ui;
}

void ErrorMessagesDialog::setErrors(const Errors &errors)
{
  ui->msgField->clear();
  for (const Message& s : errors.getMessages())
  {
    ui->msgField->append(QString::fromStdString(s.str()));
  }
}
