/********************************************************************************
 *                                                                              *
 * EamonInterpreter - about dialog                                              *
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

#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::AboutDialog)
{
  ui->setupUi(this);
  ui->versionLabel->setText("Version "+QApplication::applicationVersion());
  ui->releaseLabel->setText(QString("Release ")+__DATE__);
}

AboutDialog::~AboutDialog()
{
  delete ui;
}
