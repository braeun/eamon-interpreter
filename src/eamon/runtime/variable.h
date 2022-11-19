/********************************************************************************
 *                                                                              *
 * EamonInterpreter - variable object                                           *
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

#ifndef VARIABLE_H
#define VARIABLE_H

#include "type.h"
#include <vector>
#include <string>

class Variable {
public:

  Variable();
  Variable(const std::string& name, Type type, uint32_t addr=0);
  virtual ~Variable();

  std::string getName() const;

  Type getType() const;

  virtual uint32_t getAddress() const;

  virtual void setAddress(uint32_t a);

private:
  std::string name;
  Type type;
  uint32_t addr;
};




class VariableList {
public:
  VariableList();

  void clear();

  uint32_t size() const;

  void addVariable(Variable& v);

  const Variable* findVariable(const std::string& name);

  uint32_t getNumericBlockSize() const;

//  uint32_t getStringBlockSize() const;

  void reverse();

  const Variable& operator[](uint32_t index) const;

private:
  uint32_t addr;
  std::vector<Variable> list;
};


#endif
