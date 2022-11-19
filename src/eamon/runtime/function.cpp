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

#include "function.h"
#include "library.h"


Function::Function()
{
}

FunctionList::FunctionList()
{
}

void FunctionList::clear()
{
  list.clear();
}

uint32_t FunctionList::size() const
{
  return static_cast<uint32_t>(list.size());
}

bool FunctionList::addFunction(Function& v)
{
  for (Function &func : list)
  {
//    if (func.name == v.name && func.argtype == v.argtype)
    if (func.name == v.name) // function name must be unique
    {
      return false;
    }
  }
//  v.vtableIndex = static_cast<int32_t>(list.size());
  list.push_back(v);
  return true;
}

const std::vector<Function>& FunctionList::getFunctions() const
{
  return list;
}


Function* FunctionList::findFunction(const std::string& name)
{
  for (Function &func : list)
  {
    if (func.name == name) return &func;
  }
  return nullptr;
}





