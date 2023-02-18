/********************************************************************************
 *                                                                              *
 * EamonInterpreter - variable object                                           *
 *                                                                              *
 * modified: 2023-02-17                                                         *
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

#include "variable.h"
#include "address.h"
#include <stdlib.h>
#include <algorithm>

Variable::Variable():
  name(""),
  type(Type::undefinedType),
  addr(0)
{
}

Variable::Variable(const std::string& name, Type type, uint32_t addr):
  name(name),
  type(type),
  addr(addr)
{
}

Variable::~Variable()
{
}

std::string Variable::getName() const
{
  return name;
}

Type Variable::getType() const
{
  return type;
}

uint32_t Variable::getAddress() const
{
  return Address::createGlobalAddress(addr);
}

void Variable::setAddress(uint32_t a)
{
  addr = a;
}











VariableList::VariableList():
  addr(0)
{
}

void VariableList::clear()
{
  list.clear();
  addr = 0;
}

uint32_t VariableList::size() const
{
  return static_cast<uint32_t>(list.size());
}

void VariableList::addVariable(Variable& v)
{
  v.setAddress(addr);
  addr++;
  list.push_back(v);
}

Variable VariableList::findVariable(const std::string& name)
{
  for (const Variable &v : list)
  {
    if (v.getName() == name) return v;
  }
  return Variable();
}

uint32_t VariableList::getNumericBlockSize() const
{
  return addr;
}

void VariableList::reverse()
{
  std::reverse(list.begin(),list.end());
}

Variable VariableList::operator[](uint32_t index) const
{
  return list[index];
}
