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

#include "inputstream.h"
#include <ctype.h>
#include <QKeyEvent>

InputStream::InputStream(QObject *parent) : QObject(parent),
  lastKey(-1),
  state(Idle)
{
}

InputStream::State InputStream::handleKey(QKeyEvent *event)
{
  State s = state;
  if (!event->text().isEmpty())
  {
    lastKey = toupper(event->text().toStdString()[0]);
  }
  if (state == ReadLine)
  {
    bool handled = false;
    if (event->modifiers().testFlag(Qt::ControlModifier))
    {
      switch (event->key())
      {
         case Qt::Key_G:
          text = lastentry;
          buffer = "";
          condition.wakeAll();
          state = Idle;
          handled = true;
          break;
      }
    }
    if (!handled)
    {
      if (event->key() == Qt::Key_Return)
      {
        text = buffer;
        lastentry = text;
        buffer = "";
        condition.wakeAll();
        state = Idle;
      }
      else if (event->key() == Qt::Key_Backspace)
      {
        if (!buffer.empty()) buffer = buffer.substr(0,buffer.size()-1);
      }
      else if (!event->text().isEmpty())
      {
        buffer += toupper(event->text().toStdString()[0]);
      }
    }
  }
  else if (state == ReadChar)
  {
    if (!event->text().isEmpty())
    {
      ch = toupper(event->text().toStdString()[0]);
      condition.wakeAll();
      state = Idle;
    }
  }
  return s;
}

bool InputStream::echoInput() const
{
  return state == ReadLine;
}

bool InputStream::canBackspace() const
{
  return state == ReadLine && !buffer.empty();
}

std::string InputStream::getLastEntry() const
{
  return lastentry;
}

char InputStream::getLastKey() const
{
  return lastKey;
}

std::string InputStream::readLine()
{
  mutex.lock();
  state = ReadLine;
  condition.wait(&mutex);
  std::string t = text;
  mutex.unlock();
  return t;
}

char InputStream::readChar()
{
  mutex.lock();
  state = ReadChar;
  condition.wait(&mutex);
  char c = ch;
  mutex.unlock();
  return c;
}
