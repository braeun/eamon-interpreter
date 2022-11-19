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

#ifndef CONSTANT_H
#define CONSTANT_H

#include "value.h"
#include <vector>
#include <string>

#define CONSTANT_REDECLARATION 0xFFFFFFFF

class Assembler;

class Constant {
public:
  Constant(void);
  virtual ~Constant();

  /** Ctor for a string constant with name @a n and value @a v */
  Constant(const std::string &n, const std::string &v);

  /** Ctor for a string constant with tmp name and value @a v */
  Constant(const std::string &v);

  /** Ctor for a numeric constant with name @a n and value @a v */
  Constant(const std::string &n, int32_t v);

  /** Ctor for a numeric constant with name @a n and value @a v */
  Constant(const std::string &n, double v);

  /** Ctor for an array constant with name @a n and value @a v */
  Constant(const std::string &n, const std::vector<TypedValue> &v, Type type);

  /** Ctor for an array constant with value @a v */
  Constant(const std::vector<TypedValue> &v, Type type);

  /** Return the name of the constant */
  std::string getName(void) const;

  Type getType() const;

  virtual uint32_t getAddress() const;

  virtual void setAddress(uint32_t a);

  /** Return the value of the constant as a string */
  std::string getValueString(uint32_t index=0) const;

  /** Return the value of the constant as a number */
  int32_t getValueI32(uint32_t index=0) const;

  /** Return the value of the constant as a number */
  double getValueDouble(uint32_t index=0) const;

  const std::vector<TypedValue>& getArray() const;

private:
  /* Name of constant */
  std::string name;
  Type type;
  uint32_t addr;
  /* value(s) */
  std::vector<TypedValue> a;

  static uint32_t tmpcounter;
};


class Constants {
public:
  Constants();

  void clear();

  uint32_t addConstant(const Constant& c);

  const Constant* findConstant(const std::string& n) const;

protected:
  friend class Assembler;

  std::vector<Constant>& getConstants();

private:
  std::vector<Constant> constants;
};


#endif
