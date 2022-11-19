/********************************************************************************
 *                                                                              *
 * EamonInterpreter - VM output stream                                          *
 *                                                                              *
 * modified: 2022-11-16                                                         *
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

#include "outputstream.h"
#include <iostream>

OutputStream::OutputStream(QObject *parent) : QObject(parent)
{
}

OutputStream::~OutputStream()
{
}

void OutputStream::write(const std::string &s)
{
//  std::cout << s;
  emit newText(QString::fromStdString(s));
}

void OutputStream::gotoColumn(int c)
{
  emit moveToColumn(c);
}

void OutputStream::gotoRow(int r)
{
  emit moveToRow(r);
}

void OutputStream::home()
{
  emit moveHome();
}

void OutputStream::inverse()
{
  emit printInverse();
}

void OutputStream::normal()
{
  emit printNormal();
}

void OutputStream::setScreenMode(ScreenMode m)
{
  emit changeScreenMode(m);
}

void OutputStream::notifyHiresLoaded()
{
  emit hiresLoaded();
}

void OutputStream::flush()
{
}

