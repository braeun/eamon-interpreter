/********************************************************************************
 *                                                                              *
 * EamonInterpreter - VM input stream                                           *
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

#ifndef INPUTSTREAM_H
#define INPUTSTREAM_H

#include <QObject>
#include <QMutex>
#include <QWaitCondition>

class QKeyEvent;

class InputStream : public QObject
{
  Q_OBJECT
public:
  enum State { Idle, ReadLine, ReadChar };

  explicit InputStream(QObject *parent = nullptr);

  State handleKey(QKeyEvent* event);

  bool echoInput() const;

  bool canBackspace() const;

  std::string getLastEntry() const;

  char getLastKey() const;

  std::string readLine();

  char readChar();

signals:

private:
  std::string text;
  std::string buffer;
  std::string lastentry;
  char ch;
  char lastKey;
  QMutex mutex;
  QWaitCondition condition;
  State state;

};

#endif // SHELLINPUTSTREAM_H
