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

#include "symbol.h"
#include <string.h>



Symbol::Symbol():
  addr(0),
  type(Type::undefinedType.toInt()),
  symboltype(CONSTANT)
{
  name[0] = '\0';
}

Symbol::Symbol(const std::string& n, uint32_t a, Type t, SymbolType st):
  addr(a),
  type(t.toInt()),
  symboltype(st)
{
  strncpy(name,n.c_str(),MAX_IDENTIFIER_LENGTH);
  name[MAX_IDENTIFIER_LENGTH] = '\0';
}

std::string Symbol::getName() const
{
  return name;
}

uint32_t Symbol::getAddress() const
{
  return addr;
}

Type Symbol::getType() const
{
  return Type::fromInt(type);
}

Symbol::SymbolType Symbol::getSymbolType() const
{
  return symboltype;
}

