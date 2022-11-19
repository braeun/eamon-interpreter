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

#ifndef TYPE_H
#define TYPE_H

#include <stdint.h>
#include <string>

class Type {
public:
  Type();

  std::string getTypeString() const;

  std::string getVerboseTypeString() const;

  Type getArrayType() const;

  Type getScalarType() const;

  bool isNumericType() const;

  bool isArrayType() const;

  bool isAssignable(const Type& t) const;

  bool isLoosePrecisionInCast(const Type& resulttype) const;

  int operator==(const Type& t) const;

  int operator!=(const Type& t) const;

  uint32_t toInt() const;

  static Type fromInt(uint32_t id);

  static Type fromTypeString(std::string t);

  static Type fromTypeString(char c);

  static Type resultType(const Type& t1, const Type& t2);

  static Type undefinedType;
  static Type voidType;
  static Type int32Type;
  static Type doubleType;
  static Type stringType;
  static Type aint32Type;
  static Type adoubleType;
  static Type astringType;

private:
  Type(int t);

  uint32_t id;
};

#endif // TYPE_H
