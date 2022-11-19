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

#ifndef VMTHREAD_H
#define VMTHREAD_H

#include <QThread>


class Executable;
class VM;

class VMThread : public QThread
{
  Q_OBJECT
public:
  VMThread(std::shared_ptr<VM>& vm, QObject* parent=nullptr);

  const QString& getError() const;

  void run(std::shared_ptr<Executable> x);

  void runDirect(std::shared_ptr<Executable> x);

  void pause();

  void resume();

protected:
  virtual void run() override;

private:
  QString error;
  std::shared_ptr<VM> vm;
};

#endif // VMTHREAD_H
