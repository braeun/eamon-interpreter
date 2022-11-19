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

#ifndef ERRORS_H
#define ERRORS_H

#include <stdint.h>
#include <vector>
#include <string>



class Message
{
public:
  enum Level { INFO, WARNING, ERROR };

  Message();
  Message(Level level, int32_t line, const std::string& msg);

  std::string str() const;

  int line;
  Level level;
  std::string msg;
};

class Errors
{
public:
  Errors();

  void add(const Errors& err);

  void addError(int line, std::string txt);

  void addError(int line, std::string txt, const std::string& arg1);

  void addError(int line, std::string txt, const std::string& arg1, const std::string& arg2);

  void addWarning(int line, std::string txt);

  void addWarning(int line, std::string txt, const std::string& arg1);

  void addWarning(int line, std::string txt, const std::string& arg1, const std::string& arg2);

  const std::vector<Message>& getMessages() const;

  std::vector<Message> getErrors() const;

  std::vector<Message> getWarnings() const;

  void clear();

private:
  /* List of messages */
  std::vector<Message> messages;

};


#endif // ERRORS_H
