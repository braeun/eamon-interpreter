/********************************************************************************
 *                                                                              *
 * EamonInterpreter - VM output stream                                          *
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

#ifndef OUTPUTSTREAM_H
#define OUTPUTSTREAM_H

#include <QObject>

class Screen;

class OutputStream : public QObject
{
  Q_OBJECT
public:

  enum ScreenMode { Text, Graphics };

  explicit OutputStream(Screen* screen, QObject *parent=nullptr);
  ~OutputStream();

  void write(const std::string &s);

  void gotoColumn(int c);

  void gotoRow(int r);

  void home();

  void inverse();

  void normal();

  void setScreenMode(ScreenMode m);

  void notifyHiresLoaded();

  void flush();

  int getCursorColumn() const;

  int getCursorRow() const;

signals:
  void newText(const QString& s);
  void moveToColumn(int c);
  void moveToRow(int c);
  void moveHome();
  void printInverse();
  void printNormal();
  void changeScreenMode(int m);
  void hiresLoaded();

private:
  void newScreenMode(int m);

  Screen* screen;

};

#endif // OUTPUTSTREAMEMITTER_H
