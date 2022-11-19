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

#ifndef OUTPUTSTREAM_H
#define OUTPUTSTREAM_H

#include <QObject>

class OutputStream : public QObject
{
  Q_OBJECT
public:

  enum ScreenMode { Text, Graphics };

  explicit OutputStream(QObject *parent=nullptr);
  virtual ~OutputStream();

  virtual void write(const std::string &s);

  virtual void gotoColumn(int c);

  virtual void gotoRow(int r);

  virtual void home();

  virtual void inverse();

  virtual void normal();

  virtual void setScreenMode(ScreenMode m);

  virtual void notifyHiresLoaded();

  virtual void flush();

signals:
  void newText(const QString& s);
  void moveToColumn(int c);
  void moveToRow(int c);
  void moveHome();
  void printInverse();
  void printNormal();
  void changeScreenMode(int m);
  void hiresLoaded();

};

#endif // OUTPUTSTREAMEMITTER_H
