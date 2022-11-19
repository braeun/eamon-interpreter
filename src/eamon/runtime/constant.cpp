/********************************************************************************
 *                                                                              *
 * EamonInterpreter - constants definitions                                     *
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

#include "constant.h"
#include "address.h"
#include <stdlib.h>
#include <sstream>
#include <iostream>

using namespace std;

uint32_t Constant::tmpcounter = 0;

Constant::Constant(void):
  name(""),
  type(Type::undefinedType),
  addr(0)
{
}

Constant::~Constant()
{
}

/** Ctor for a string constant with name @a n and value @a v */
Constant::Constant(const std::string &n, const std::string &v):
  name(n),
  type(Type::stringType)
{
  a.push_back(TypedValue(v));
}

/** Ctor for a string constant with value @a v */
Constant::Constant(const std::string &v):
  type(Type::stringType)
{
  std::ostringstream os;
  os << "$" << tmpcounter++;
  name = os.str();
  a.push_back(TypedValue(v));
}

/** Ctor for a numeric constant with name @a n and value @a v */
Constant::Constant(const std::string &n, int32_t v):
  name(n),
  type(Type::int32Type)
{
  a.push_back(TypedValue(v));
}

/** Ctor for a numeric constant with name @a n and value @a v */
Constant::Constant(const std::string &n, double v):
  name(n),
  type(Type::doubleType)
{
  a.push_back(TypedValue(v));
}

/** Ctor for a numeric array constant with name @a n and value @a v */
Constant::Constant(const std::string &n, const std::vector<TypedValue> &v, Type type):
  name(n),
  type(type)
{
  a = v;
}

Constant::Constant(const std::vector<TypedValue> &v, Type type):
  type(type)
{
  std::ostringstream os;
  os << "$" << tmpcounter++;
  name = os.str();
  a = v;
}

std::string Constant::getName(void) const
{
  return name;
}

Type Constant::getType() const
{
  return type;
}

uint32_t Constant::getAddress() const
{
  return Address::createConstantAddress(addr);
}

void Constant::setAddress(uint32_t a)
{
  addr = a;
}


std::string Constant::getValueString(uint32_t index) const
{
  return a[index].getValue().getString();
}

int32_t Constant::getValueI32(uint32_t index) const
{
  return a[index].getValue().getInt();
}

double Constant::getValueDouble(uint32_t index) const
{
  return a[index].getValue().getDouble();
}

const std::vector<TypedValue>& Constant::getArray() const
{
  return a;
}





Constants::Constants()
{
}

void Constants::clear()
{
  constants.clear();
}

uint32_t Constants::addConstant(const Constant &c)
{
  for (const Constant& co : constants)
  {
    if (co.getName() == c.getName()) return CONSTANT_REDECLARATION;
  }
  if (c.getType() == Type::stringType && c.getName()[0] == '$')
  {
    for (const Constant& co : constants)
    {
      if (co.getValueString() == c.getValueString()) return co.getAddress();
    }
  }
  uint32_t addr = static_cast<uint32_t>(constants.size());
  constants.push_back(c);
  constants.back().setAddress(addr);
  return addr;
}

const Constant* Constants::findConstant(const std::string& n) const
{
  for (const Constant& c : constants)
  {
    if (c.getName() == n) return &c;
  }
  return nullptr;
}

std::vector<Constant>& Constants::getConstants()
{
  return constants;
}

