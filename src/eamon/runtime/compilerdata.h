/********************************************************************************
 *                                                                              *
 * EamonInterpreter - common data for compiler and assembler                    *
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

#ifndef COMPILERDATA_H
#define COMPILERDATA_H

#include "constant.h"
#include "op.h"
#include "function.h"
#include "variable.h"
#include <memory>


class Library;

class CompilerData
{
public:
  CompilerData();

  void clear();

//  const Constant* findConstant(const std::string& name) const;

  /* List of declared constants */
  Constants constants;
  /* List of declared functions */
  FunctionList functions;
  /* List of global variables */
  VariableList globalVariables;
  /* main code block */
  CodeBlock codeblock;
  /* counter for internally generated Labels */
  int32_t labelCounter;

  /* remove debug information */
  bool noDebug;

};

#endif // COMPILERDATA_H
