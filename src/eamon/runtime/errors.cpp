/********************************************************************************
 *                                                                              *
 * EamonInterpreter - errors                                                    *
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

#include "errors.h"
#include <sstream>



Message::Message():
  line(0),
  level(INFO),
  msg("")
{
}

Message::Message(Level level, int32_t line, const std::string& msg):
  line(line),
  level(level),
  msg(msg)
{
}

std::string Message::str() const
{
  std::ostringstream s;
  switch (level)
  {
    case INFO:
      s << "INFO : ";
      break;
    case WARNING:
      s << "WARN : ";
      break;
    case ERROR:
      s << "ERROR: ";
      break;
  }
  s << "line " << line << " - " << msg;
  return s.str();
}





Errors::Errors()
{
}

void Errors::add(const Errors& err)
{
  messages.insert(messages.end(),err.messages.begin(),err.messages.end());
}

void Errors::addError(int line, std::string txt)
{
  std::ostringstream s;
  messages.push_back(Message(Message::ERROR,line,txt));
}

void Errors::addError(int line, std::string txt, const std::string& arg1)
{
  size_t n = txt.find("$1");
  if (n != std::string::npos)
    txt.replace(n,2,arg1);
  else
    txt += ": " + arg1;
  addError(line,txt);
}

void Errors::addError(int line, std::string txt, const std::string& arg1, const std::string& arg2)
{
  size_t n = txt.find("$1");
  if (n != std::string::npos)
    txt.replace(n,2,arg1);
  else
    txt += ": " + arg1;
  n = txt.find("$2");
  if (n != std::string::npos)
    txt.replace(n,2,arg2);
  else
    txt += ": " + arg2;
  addError(line,txt);
}

void Errors::addWarning(int line, std::string txt)
{
  std::ostringstream s;
  messages.push_back(Message(Message::WARNING,line,txt));
}

void Errors::addWarning(int line, std::string txt, const std::string& arg1)
{
  size_t n = txt.find("$1");
  if (n != std::string::npos)
    txt.replace(n,2,arg1);
  else
    txt += ": " + arg1;
  addWarning(line,txt);
}

void Errors::addWarning(int line, std::string txt, const std::string& arg1, const std::string& arg2)
{
  size_t n = txt.find("$1");
  if (n != std::string::npos)
    txt.replace(n,2,arg1);
  else
    txt += ": " + arg1;
  n = txt.find("$2");
  if (n != std::string::npos)
    txt.replace(n,2,arg2);
  else
    txt += ": " + arg2;
  addWarning(line,txt);
}

const std::vector<Message>& Errors::getMessages() const
{
  return messages;
}

std::vector<Message> Errors::getErrors() const
{
  std::vector<Message> list;
  for (const Message& msg : messages)
  {
    if (msg.level == Message::ERROR)list.push_back(msg);
  }
  return list;
}

std::vector<Message> Errors::getWarnings() const
{
  std::vector<Message> list;
  for (const Message& msg : messages)
  {
    if (msg.level == Message::WARNING)list.push_back(msg);
  }
  return list;
}

void Errors::clear()
{
  messages.clear();
}


