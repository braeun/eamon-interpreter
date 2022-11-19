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

#include "value.h"
#include <cmath>
#include <sstream>
#include <algorithm>
#include <stdexcept>



Value::Value()
{
  type = INVALID;
  i = 0;
  d = 0;
  s = "";
}

Value::Value(int32_t v)
{
  type = INT32;
  i = v;
}

Value::Value(double v)
{
  type = DOUBLE;
  d = v;
}

Value::Value(const std::string& v)
{
  type = STRING;
  i = 0;
  s = v;
}

bool Value::isValid() const
{
  return type != INVALID;
}

bool Value::isNumeric() const
{
  return type != STRING;
}

int32_t Value::getInt() const
{
  switch (type)
  {
    case INT32:
      return i;
    case DOUBLE:
      return static_cast<int32_t>(round(d));
    case STRING:
      return !s.empty() ? std::stoi(s) : 0;
  }
  return 0;
}

double Value::getDouble() const
{
  switch (type)
  {
    case INT32:
      return i;
    case DOUBLE:
      return d;
    case STRING:
      return !s.empty() ? std::stoi(s) : 0;
  }
  return 0;
}

std::string Value::getString() const
{
  std::ostringstream os;
  switch (type)
  {
    case INT32:
      os << i;
      return os.str();
    case DOUBLE:
      os << d;
      return os.str();
    case STRING:
      return s;
      break;
  }
  return s;
}

void Value::set(int32_t v)
{
  type = INT32;
  i = v;
}

void Value::set(double v)
{
  type = DOUBLE;
  d = v;
}

void Value::set(const std::string& v)
{
  type = STRING;
  s = v;
}

void Value::negate()
{
  switch (type)
  {
    case INT32:
      i *= -1;
      break;
    case DOUBLE:
      d *= -1;
      break;
    case STRING:
      throw std::runtime_error("illegal string operation");
      break;
  }
}

void Value::dec()
{
  switch (type)
  {
    case INT32:
      i--;
      break;
    case DOUBLE:
      d--;
      break;
    case STRING:
      throw std::runtime_error("illegal string operation");
      break;
  }
}

void Value::inc()
{
  switch (type)
  {
    case INT32:
      i++;
      break;
    case DOUBLE:
      d++;
      break;
    case STRING:
      throw std::runtime_error("illegal string operation");
      break;
  }
}

void Value::opnot()
{
  switch (type)
  {
    case INT32:
      i = !i;
      break;
    case DOUBLE:
      throw std::runtime_error("logical not not ddefined for double values");
      break;
    case STRING:
      throw std::runtime_error("illegal string operation");
      break;
  }
}

void Value::clear()
{
  i = 0;
  d = 0;
  s = "";
}

nlohmann::json Value::toJson() const
{
  nlohmann::json j;
  j["type"] = static_cast<int>(type);
  switch (type)
  {
    case INT32:
      j["i"] = i;
      break;
    case DOUBLE:
      j["d"] = d;
      break;
    case STRING:
      j["s"] = s;
      break;
  }
  return j;
}

Value& Value::operator+=(const Value &v)
{
  ValueType r = getResultType(v.type);
  switch (r)
  {
    case INT32:
      i = getInt() + v.getInt();
      break;
    case DOUBLE:
      d = getDouble() + v.getDouble();
      break;
    case STRING:
      set(getString()+v.getString());
      break;
  }
  type = r;
  return *this;
}

Value& Value::operator-=(const Value &v)
{
  ValueType r = getResultType(v.type);
  switch (r)
  {
    case INT32:
      i = getInt() - v.getInt();
      break;
    case DOUBLE:
      d = getDouble() - v.getDouble();
      break;
    case STRING:
      throw std::runtime_error("illegal string operation");
      break;
  }
  type = r;
  return *this;
}

Value& Value::operator*=(const Value &v)
{
  ValueType r = getResultType(v.type);
  switch (r)
  {
    case INT32:
      i = getInt() * v.getInt();
      break;
    case DOUBLE:
      d = getDouble() * v.getDouble();
      break;
    case STRING:
      throw std::runtime_error("illegal string operation");
      break;
  }
  type = r;
  return *this;
}

Value& Value::operator/=(const Value &v)
{
  ValueType r = getResultType(v.type);
  switch (r)
  {
    case INT32:
      i = getInt() / v.getInt();
      break;
    case DOUBLE:
      d = getDouble() / v.getDouble();
      break;
    case STRING:
      throw std::runtime_error("illegal string operation");
      break;
  }
  type = r;
  return *this;
}

Value& Value::operator%=(const Value &v)
{
  ValueType r = getResultType(v.type);
  switch (r)
  {
    case INT32:
      i = getInt() % v.getInt();
      break;
    case DOUBLE:
      throw std::runtime_error("illegal modulo not defined for double values");
      break;
    case STRING:
      throw std::runtime_error("illegal string operation");
      break;
  }
  type = r;
  return *this;
}

Value& Value::operator|=(const Value &v)
{
  ValueType r = getResultType(v.type);
  switch (r)
  {
    case INT32:
      i = getInt() | v.getInt();
      break;
    case DOUBLE:
      throw std::runtime_error("bitwise or not defined for double values");
      break;
    case STRING:
      throw std::runtime_error("illegal string operation");
      break;
  }
  type = r;
  return *this;
}

Value& Value::operator&=(const Value &v)
{
  ValueType r = getResultType(v.type);
  switch (r)
  {
    case INT32:
      i = getInt() & v.getInt();
      break;
    case DOUBLE:
      throw std::runtime_error("bitwise and not defined for double values");
      break;
    case STRING:
      throw std::runtime_error("illegal string operation");
      break;
  }
  type = r;
  return *this;
}




Value::ValueType Value::getResultType(ValueType t)
{
  if (type == ValueType::INVALID) return t;
  if (t == ValueType::INVALID) return type;
  int32_t r = std::max(static_cast<int32_t>(type),static_cast<int32_t>(t));
  return static_cast<ValueType>(r);
}


std::vector<Value> Value::values(const std::vector<int32_t>& list)
{
  std::vector<Value> v;
  for (int32_t i : list) v.push_back(i);
  return v;
}

std::vector<Value> Value::values(const std::vector<double>& list)
{
  std::vector<Value> v;
  for (double d : list) v.push_back(d);
  return v;
}

std::vector<Value> Value::values(const std::vector<std::string>& list)
{
  std::vector<Value> v;
  for (std::string s : list) v.push_back(s);
  return v;
}

Value Value::zero(Type type)
{
  Type t = type.getScalarType();
  if (t == Type::int32Type) return Value(0);
  if (t == Type::doubleType) return Value(0.0);
  if (t == Type::stringType) return Value("");
  return Value();
}

Value Value::fromJson(const nlohmann::json &j)
{
  ValueType t = static_cast<ValueType>(j.at("type").get<int>());
  switch (t)
  {
    case INT32:
      return Value(j.at("i").get<int32_t>());
    case DOUBLE:
      return Value(j.at("d").get<double>());
    case STRING:
      return Value(j.at("s").get<std::string>());
  }
  return Value();
}





TypedValue::TypedValue():
  type(Type::undefinedType),
  value()
{
}

TypedValue::TypedValue(int32_t v):
  type(Type::int32Type),
  value(v)
{
}

TypedValue::TypedValue(double v):
  type(Type::doubleType),
  value(v)
{
}

TypedValue::TypedValue(const std::string& v):
  type(Type::stringType),
  value(v)
{
}

const Type& TypedValue::getType() const
{
  return type;
}

const Value& TypedValue::getValue() const
{
  return value;
}



