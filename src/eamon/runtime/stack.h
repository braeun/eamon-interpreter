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

#ifndef STACK_H
#define STACK_H

#include "value.h"
#include <stdint.h>
#include <string>
#include <vector>




class Stack
{
public:
  Stack();

  void clear();

  void push(const Value& v);

  /**
   * @brief Pops a value from the stack.
   * @return value
   * @throws out_of_range if stack is empty
   */
  Value pop();

  /**
   * @brief Swaps the two top entries in the numeric stack.
   */
  void swap();

private:
  std::vector<Value> stack;
};





#endif // STACK_H
