/********************************************************************************
 *                                                                              *
 * EamonInterpreter - value in the VM                                           *
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

#ifndef VALUE_H
#define VALUE_H

#include "../nlohmann/json.h"
#include "type.h"
#include <stdint.h>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>



class Value
{
public:
  Value();
  Value(int32_t v);
  Value(double v);
  Value(const std::string& v);

  bool isValid() const;

  bool isNumeric() const;

  int32_t getInt() const;

  double getDouble() const;

  std::string getString() const;

  void set(int32_t v);

  void set(double v);

  void set(const std::string& v);

  void negate();

  void dec();

  void inc();

  void opnot();

  void clear();

  nlohmann::json toJson() const;

  Value& operator+=(const Value& v);

  Value& operator-=(const Value& v);

  Value& operator*=(const Value& v);

  Value& operator/=(const Value& v);

  Value& operator%=(const Value& v);

  Value& operator|=(const Value& v);

  Value& operator&=(const Value& v);

  static std::vector<Value> values(const std::vector<int32_t>& list);

  static std::vector<Value> values(const std::vector<double>& list);

  static std::vector<Value> values(const std::vector<std::string>& list);

  static Value zero(Type type);

  static Value fromJson(const nlohmann::json& j);

private:
  enum ValueType { INVALID, INT32, DOUBLE, STRING };

  /*
   * Get the resulting type of an operation between this value's type
   * and the other value's type t
   */
  ValueType getResultType(ValueType t);

  ValueType type;
  union {
    int32_t i;
    double d;
  };
  std::string s;
};

inline Value operator+(Value lhs, const Value& rhs)
{
  lhs += rhs;
  return lhs;
}

inline Value operator-(Value lhs, const Value& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline Value operator*(Value lhs, const Value& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline Value operator/(Value lhs, const Value& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline Value operator%(Value lhs, const Value& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline Value operator|(Value lhs, const Value& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline Value operator&(Value lhs, const Value& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline bool operator<(const Value& lhs, const Value& rhs)
{
  if (lhs.isNumeric() && rhs.isNumeric()) return lhs.getDouble() < rhs.getDouble();
  return lhs.getString() < rhs.getString();
}

inline bool operator<=(const Value& lhs, const Value& rhs)
{
  if (lhs.isNumeric() && rhs.isNumeric()) return lhs.getDouble() <= rhs.getDouble();
  return lhs.getString() <= rhs.getString();
}

inline bool operator==(const Value& lhs, const Value& rhs)
{
  if (lhs.isNumeric() && rhs.isNumeric()) return lhs.getDouble() == rhs.getDouble();
  return lhs.getString() == rhs.getString();
}

inline bool operator!=(const Value& lhs, const Value& rhs)
{
  if (lhs.isNumeric() && rhs.isNumeric()) return lhs.getDouble() != rhs.getDouble();
  return lhs.getString() != rhs.getString();
}

inline bool operator>(const Value& lhs, const Value& rhs)
{
  if (lhs.isNumeric() && rhs.isNumeric()) return lhs.getDouble() > rhs.getDouble();
  return lhs.getString() > rhs.getString();
}

inline bool operator>=(const Value& lhs, const Value& rhs)
{
  if (lhs.isNumeric() && rhs.isNumeric()) return lhs.getDouble() >= rhs.getDouble();
  return lhs.getString() >= rhs.getString();
}

inline bool operator&&(const Value& lhs, const Value& rhs)
{
  if (lhs.isNumeric() && rhs.isNumeric()) return lhs.getInt() && rhs.getInt();
  throw std::runtime_error("illegal string operation");
}

inline bool operator||(const Value& lhs, const Value& rhs)
{
  if (lhs.isNumeric() && rhs.isNumeric()) return lhs.getInt() || rhs.getInt();
  throw std::runtime_error("illegal string operation");
}










class TypedValue {
public:
  TypedValue();
  TypedValue(int32_t v);
  TypedValue(int64_t v);
  TypedValue(double v);
  TypedValue(const std::string& v);

  const Type& getType() const;

  const Value& getValue() const;

private:
  Type type;
  Value value;
};



#endif // VALUE_H
