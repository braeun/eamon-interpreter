/********************************************************************************
 *                                                                              *
 * EamonInterpreter - object address                                            *
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

#ifndef ADDRESS_H
#define ADDRESS_H

#include <stdint.h>

class Address
{
public:

  static uint32_t createGlobalAddress(uint32_t a);

  static uint32_t createConstantAddress(uint32_t a);

  static uint32_t getAddress(uint32_t a);

  static bool isGlobalAddress(uint32_t a);

  static bool isConstantAddress(uint32_t a);

private:
  Address();
};



#endif // ADDRESS_H
