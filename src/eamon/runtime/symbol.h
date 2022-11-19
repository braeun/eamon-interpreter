/********************************************************************************
 *                                                                              *
 * EamonInterpreter - VM symbol                                                 *
 *                                                                              *
 * modified: 2022-11-17                                                         *
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

#ifndef SYMBOL_H
#define SYMBOL_H

#include "type.h"
#include <stdint.h>
#include <vector>
#include <string>

#define MAX_IDENTIFIER_LENGTH 31



class Symbol {
public:

  enum SymbolType { CONSTANT, VARIABLE, FUNCTION };

  Symbol();
  Symbol(const std::string& name, uint32_t addr, Type t, SymbolType st);

  std::string getName() const;

  uint32_t getAddress() const;

  Type getType() const;

  SymbolType getSymbolType() const;

private:
  char name[MAX_IDENTIFIER_LENGTH+1];
  uint32_t addr;
  uint32_t type;
  SymbolType symboltype;
};



#endif // SYMBOL_H
