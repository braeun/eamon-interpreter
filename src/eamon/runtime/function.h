/********************************************************************************
 *                                                                              *
 * EamonInterpreter - function object                                           *
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

#ifndef FUNCTION_H
#define FUNCTION_H

#include "type.h"
#include "variable.h"
#include "op.h"
#include <string>

class Function {
public:
  Function();

  /* name of function */
  std::string name = "";
  /* name of argument variable */
  std::string var = "";
  /* return type of function */
  Type rettype = Type::undefinedType;
  /* index of code block */
  CodeBlock code;
  /* label to enter function */
  int32_t label;
};

class FunctionList {
public:
  FunctionList();

  void clear();

  uint32_t size() const;

  bool addFunction(Function& v);

  const std::vector<Function>& getFunctions() const;

  Function* findFunction(const std::string& name);

private:
  std::vector<Function> list;
};


#endif // FUNCTION_H
