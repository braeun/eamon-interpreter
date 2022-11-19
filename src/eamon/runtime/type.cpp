/********************************************************************************
 *                                                                              *
 * EamonInterpreter - type class                                                *
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

#include "type.h"

#define TYPE_UNDEFINED    0
#define TYPE_VOID         1
#define TYPE_INT32        2
#define TYPE_DOUBLE       4
#define TYPE_STRING       5
#define TYPE_ARRAY_INT32  7
#define TYPE_ARRAY_DOUBLE 9
#define TYPE_ARRAY_STRING 10



Type Type::undefinedType(TYPE_UNDEFINED);
Type Type::voidType(TYPE_VOID);
Type Type::int32Type(TYPE_INT32);
Type Type::doubleType(TYPE_DOUBLE);
Type Type::stringType(TYPE_STRING);
Type Type::aint32Type(TYPE_ARRAY_INT32);
Type Type::adoubleType(TYPE_ARRAY_DOUBLE);
Type Type::astringType(TYPE_ARRAY_STRING);

Type::Type():
  id(TYPE_UNDEFINED)
{
}

Type::Type(int t):
  id(t)
{
}

std::string Type::getTypeString() const
{
  switch (id)
  {
    case TYPE_INT32:
      return "i";
    case TYPE_DOUBLE:
      return "d";
    case TYPE_STRING:
      return "t";
    case TYPE_ARRAY_INT32:
      return "I";
    case TYPE_ARRAY_DOUBLE:
      return "D";
    case TYPE_ARRAY_STRING:
      return "T";
    default:
      return "?";
  }
}

std::string Type::getVerboseTypeString() const
{
  switch (id)
  {
    case TYPE_INT32:
      return "int";
    case TYPE_DOUBLE:
      return "double";
    case TYPE_STRING:
      return "string";
    case TYPE_ARRAY_INT32:
      return "int[]";
    case TYPE_ARRAY_DOUBLE:
      return "double[]";
    case TYPE_ARRAY_STRING:
      return "string[]";
    case TYPE_VOID:
      return "void";
    default:
      return "?";
  }
}

Type Type::getArrayType() const
{
  switch (id)
  {
    case TYPE_INT32:
      return aint32Type;
    case TYPE_DOUBLE:
      return adoubleType;
    case TYPE_STRING:
      return astringType;
  }
  return *this;
}

Type Type::getScalarType() const
{
  switch (id)
  {
    case TYPE_ARRAY_INT32:
      return int32Type;
    case TYPE_ARRAY_DOUBLE:
      return doubleType;
    case TYPE_ARRAY_STRING:
      return stringType;
  }
  return *this;
}


bool Type::isNumericType() const
{
  if (id == TYPE_UNDEFINED) return false;
  if (id == TYPE_VOID) return false;
  if (id == TYPE_STRING || id == TYPE_ARRAY_STRING) return false;
  return true;
}

bool Type::isArrayType() const
{
  switch (id)
  {
    case TYPE_ARRAY_INT32:
    case TYPE_ARRAY_DOUBLE:
    case TYPE_ARRAY_STRING:
      return true;
  }
  return false;
}

bool Type::isAssignable(const Type& t) const
{
  if (id == TYPE_UNDEFINED || t.id == TYPE_UNDEFINED) return false;
  if (isArrayType() && !t.isArrayType()) return false;
  if (!isArrayType() && t.isArrayType()) return false;
  if (isNumericType() && t.isNumericType()) return true;
  if (!isNumericType() && !t.isNumericType()) return true;
  return false;
};

bool Type::isLoosePrecisionInCast(const Type& t) const
{
  if (isNumericType() && t.isNumericType()) return toInt() > t.toInt();
  return false;
}


int Type::operator==(const Type &t) const
{
  return id == t.id;
}

int Type::operator!=(const Type &t) const
{
  return id != t.id;
}

uint32_t Type::toInt() const
{
  return id;
}



Type Type::fromTypeString(std::string t)
{
  if (t.empty()) return undefinedType;
  return fromTypeString(t[0]);
}

Type Type::fromTypeString(char c)
{
  switch (c)
  {
    case 'i':
      return int32Type;
    case 'd':
      return doubleType;
    case 't':
      return stringType;
    case 'I':
      return int32Type;
    case 'D':
      return doubleType;
    case 'T':
      return stringType;
  }
  return undefinedType;
}



Type Type::fromInt(uint32_t id)
{
  return Type(id);
}


Type Type::resultType(const Type& t1, const Type& t2)
{
  /* if the two types are not assignable, there is no result type */
  if (!t1.isAssignable(t2)) return undefinedType;
  if (t1.isNumericType())
  {
    return Type(std::max(t1.id,t2.id));
  }
  return t1;
}



