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

#include "address.h"

Address::Address()
{
}

#define ADDRESS_MASK 0x7FFFFFFF

#define MOD_MASK 0x80000000

#define GLOBAL_VAR 0x00000000

#define CONST_VAR 0x80000000

uint32_t Address::createGlobalAddress(uint32_t a)
{
  return (a & ADDRESS_MASK) | GLOBAL_VAR;
}

uint32_t Address::createConstantAddress(uint32_t a)
{
  return (a & ADDRESS_MASK) | CONST_VAR;
}

uint32_t Address::getAddress(uint32_t a)
{
  return a & ADDRESS_MASK;
}

bool Address::isGlobalAddress(uint32_t a)
{
  return (a & MOD_MASK) == GLOBAL_VAR;
}

bool Address::isConstantAddress(uint32_t a)
{
  return (a & MOD_MASK) == CONST_VAR;
}



