/********************************************************************************
 *                                                                              *
 * EamonInterpreter - variable memory                                           *
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

#ifndef MEMORY_H
#define MEMORY_H

#include "value.h"
#include <vector>


class Symbol;

/**
 * @brief The ConstantData class provides an interface to retrieve constants during runtime.
 */
class ConstantData
{
public:
  ConstantData();
  virtual ~ConstantData();

  /**
   * @brief Returns a constant at the given constant address plus index.
   * @param addr the address of the constant
   * @param index the index of the value in the constant.
   * @return value
   */
  virtual Value getConstant(uint32_t addr, int32_t index=0) const = 0;

  /**
   * @brief Returns all values belonging to the constant at the given address.
   * @param addr the address of the constant
   * @return list of values
   */
  virtual std::vector<Value> getConstantArray(uint32_t addr) const = 0;

  /**
   * @brief Return a pointer to the symbol structure for a constant of the given name.
   * If the constant is not found, a nullptr will be returned.
   * @param name name of the symbol
   * @return pointer to the symbol or nullptr
   */
  virtual const Symbol* findConstant(const std::string& name) const = 0;

};

/**
 * @brief A chunk of memory contains one or more values.
 *
 * In general, a chunk of memory maps to a variable in the higher level
 * language.
 */
class Chunk
{
public:
  Chunk();

  nlohmann::json toJson() const;

  static Chunk fromJson(const nlohmann::json& j);

  std::vector<Value> values;
};

class Memory
{
public:
  Memory();

  void reset(uint32_t size);

  int32_t getInt(uint32_t addr);

  double getDouble(uint32_t addr);

  std::string getString(uint32_t addr);

  const Value& getValue(uint32_t addr, int32_t offset) const;

  void store(int32_t v, uint32_t addr, int32_t offset);

  void store(double v, uint32_t addr, int32_t offset);

  void store(const std::string& v, uint32_t addr, int32_t offset);

  void store(const Value& v, uint32_t addr, int32_t offset);

  void clr(const Value& zero, uint32_t addr);

  void dec(uint32_t addr);

  void inc(uint32_t addr);

  void resize(uint32_t addr, uint32_t size);

  nlohmann::json save() const;

  void restore(const nlohmann::json& j);

private:
  std::vector<Chunk> mem;
};




#endif // MEMORY_H
