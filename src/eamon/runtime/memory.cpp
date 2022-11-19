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

#include "memory.h"
#include <fstream>


ConstantData::ConstantData()
{
}

ConstantData::~ConstantData()
{
}






Chunk::Chunk()
{
  values.push_back(0);
}

nlohmann::json Chunk::toJson() const
{
  nlohmann::json j;
  nlohmann::json jv;
  for (const auto& v : values)
  {
    jv.push_back(v.toJson());
  }
  j["values"] = jv;
  return j;
}

Chunk Chunk::fromJson(const nlohmann::json &j)
{
  Chunk c;
  c.values.clear();
  for (const nlohmann::json& tmp : j.at("values"))
  {
    c.values.push_back(Value::fromJson(tmp));
  }
  return c;
}




Memory::Memory()
{
}

void Memory::reset(uint32_t size)
{
  mem.clear();
  mem.resize(size);
}

int32_t Memory::getInt(uint32_t addr)
{
  return mem[addr].values[0].getInt();
}

double Memory::getDouble(uint32_t addr)
{
  return mem[addr].values[0].getDouble();
}

std::string Memory::getString(uint32_t addr)
{
  return mem[addr].values[0].getString();
}

const Value& Memory::getValue(uint32_t addr, int32_t offset) const
{
  return mem[addr].values[offset];
}

void Memory::store(int32_t v, uint32_t addr, int32_t offset)
{
  mem[addr].values[offset].set(v);
}

void Memory::store(double v, uint32_t addr, int32_t offset)
{
  mem[addr].values[offset].set(v);
}

void Memory::store(const std::string& v, uint32_t addr, int32_t offset)
{
  mem[addr].values[offset].set(v);
}

void Memory::store(const Value& v, uint32_t addr, int32_t offset)
{
  mem[addr].values[offset] = v;
}

void Memory::clr(const Value &zero, uint32_t addr)
{
  for (auto& v : mem[addr].values)
  {
    v = zero;
  }
}

void Memory::dec(uint32_t addr)
{
  mem[addr].values[0].dec();
}

void Memory::inc(uint32_t addr)
{
  mem[addr].values[0].inc();
}

void Memory::resize(uint32_t addr, uint32_t size)
{
  Value v = mem[addr].values[0];
  v.clear();
  mem[addr].values.resize(size);
  for (uint32_t i=0;i<size;i++) mem[addr].values[i] = v;
}

nlohmann::json Memory::save() const
{
  nlohmann::json j;
  nlohmann::json jc;
  for (const Chunk& c : mem)
  {
    jc.push_back(c.toJson());
  }
  j["mem"] = jc;
  return j;
}

void Memory::restore(const nlohmann::json& j)
{
  mem.clear();
  for (const nlohmann::json& tmp : j.at("mem"))
  {
    mem.push_back(Chunk::fromJson(tmp));
  }
}

