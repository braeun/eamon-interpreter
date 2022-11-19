/********************************************************************************
 *                                                                              *
 * EamonInterpreter - assembler                                                 *
 *                                                                              *
 * modified: 2022-11-15                                                         *
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

#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "op.h"
#include "variable.h"
#include "function.h"
#include "errors.h"
#include <map>



class CompilerData;
class Executable;

class Assembler
{
public:
  Assembler(CompilerData& data);
  ~Assembler();

  const Errors& getErrors() const;

  Executable* assemble();

private:
  void reset();
  void reallocateCode();
  void reallocateText(uint32_t l=0);
  uint32_t storeConstants();
//  uint32_t storeText(const std::string& s);
  uint32_t storeArray(const std::vector<TypedValue>& values, Type type);
  void assembleBlock(const CodeBlock& block);
  void assembleBlock(const Code& code);
  void resolveLabels();
  void checkIdentifierLength(std::string name);

  CompilerData& data;
  uint32_t* code;
  uint32_t* cptr;
  uint32_t codeSize;
  uint32_t maxSize;
  char* text;
  char* ctext;
  uint32_t textMaxSize;
  std::vector<int32_t> functionAddr;
  std::map<int32_t,uint32_t> labelAddr;
  /* List of assembler errors */
  Errors errors;
};




#endif // ASSEMBLER_H
