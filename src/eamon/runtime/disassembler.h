/********************************************************************************
 *                                                                              *
 * EamonInterpreter - disassembler                                              *
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

#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include "executable.h"
#include "type.h"
#include <ostream>
#include <memory>
#include <map>
#include <vector>



class Disassembler
{
public:
  Disassembler();
  Disassembler(std::ostream& s);

  void setOutputStream(std::ostream& s);

  void disassemble(Executable* x);

private:
  void printType(uint32_t op);
  const uint32_t* printPar(Executable* executable, uint32_t op, const uint32_t* cptr);
  const uint32_t* printAddr(uint32_t op, const uint32_t* cptr);
  void printType(Type type);
  const uint32_t* printEntryData(uint32_t op, const uint32_t* cptr);
  const uint32_t* printLibraryCallData(uint32_t op, const uint32_t* cptr);
  std::string resolveFunction(uint16_t func);

  std::ostream* os;

};



#endif // DISASSEMBLER_H
