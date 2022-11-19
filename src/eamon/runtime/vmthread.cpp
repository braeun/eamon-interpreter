/********************************************************************************
 *                                                                              *
 * EamonInterpreter - execution thread for the virtual machine                  *
 *                                                                              *
 * modified: 2022-11-17                                                         *
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

#include "vmthread.h"
#include "vm.h"
#include <QDebug>

VMThread::VMThread(std::shared_ptr<VM>& vm, QObject* parent):QThread(parent),
  vm(vm)
{
}

const QString& VMThread::getError() const
{
  return error;
}

void VMThread::run(std::shared_ptr<Executable> x)
{
  error = "";
  vm->load(x);
  start();
}

void VMThread::runDirect(std::shared_ptr<Executable> x)
{
  error = "";
  vm->load(x);
  vm->run();
}

void VMThread::run()
{
  try
  {
    vm->run();
  }
  catch (std::exception& ex)
  {
    error = ex.what();
  }
}

void VMThread::pause()
{
  qDebug() << "pause";
  vm->pause();
}

void VMThread::resume()
{
  start();
}
