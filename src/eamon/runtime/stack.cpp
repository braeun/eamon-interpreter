/********************************************************************************
 *                                                                              *
 * EamonInterpreter - VM stack                                                  *
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

#include "stack.h"
#include <stdexcept>



const int32_t initial_stacksize = 1000;

Stack::Stack()
{
  stack.reserve(initial_stacksize);
}

void Stack::clear()
{
  stack.clear();
}

void Stack::push(const Value& v)
{
  stack.push_back(v);
}

Value Stack::pop()
{
  if (stack.empty()) throw std::out_of_range("Stack underflow!");
  Value v = stack.back();
  stack.pop_back();
  return v;
}

void Stack::swap()
{
  if (stack.size() < 2) throw std::out_of_range("Stack underflow!");
  Value t1 = stack.back();
  stack.pop_back();
  Value t2 = stack.back();
  stack.pop_back();
  stack.push_back(t1);
  stack.push_back(t2);
}


