/********************************************************************************
 *                                                                              *
 * EamonInterpreter - opcodes and code object                                   *
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

#include "op.h"



COp::COp(int32_t mnemonic, Type arg1):
  label(0),
  mnemonic(mnemonic),
  type(arg1),
  parType(Type::undefinedType)
{
}

COp::~COp(void)
{
}

uint32_t COp::getOpCode() const
{
  return static_cast<uint32_t>(type.toInt() << 8 | (mnemonic & 0xFF));
}

/** Return op code */
int32_t COp::getMnemonic() const
{
  return mnemonic;
}

/**
 * @brief Gets the label associated with this op
 * @return the label or 0 if no label is associated
 */
int32_t COp::getLabel() const
{
  return label;
}

/**
 * @brief Sets the label associated with the op
 * @param l the label
 */
void COp::setLabel(int32_t l)
{
  label = l;
}

Type COp::getType() const
{
  return type;
}

Type COp::getParameterType() const
{
  return parType;
}

void COp::setParameter(int32_t v)
{
  parType = Type::int32Type;
  i32 = v;
}

void COp::setParameter(double v)
{
  parType = Type::doubleType;
  d = v;
}

void COp::setParameter(const std::string& v)
{
  parType = Type::stringType;
  s = v;
}

void COp::setParameter(const std::vector<Value>& v, Type t)
{
  parType = t;
  a = v;
}


int32_t COp::getParameterInt32() const
{
  if (parType == Type::int32Type) return i32;
  if (parType == Type::doubleType) return static_cast<int32_t>(d);
  return 0;
}

double COp::getParameterDouble() const
{
  if (parType == Type::int32Type) return i32;
  if (parType == Type::doubleType) return d;
  return 0;
}

std::string COp::getParameterString() const
{
  return s;
}

const std::vector<Value>& COp::getParameterArray() const
{
  return a;
}


/** Return true if op codes are equal */
int COp::operator==(int32_t n)
{
  return mnemonic == n;
}

/** Return true if op codes are not equal */
int COp::operator!=(int32_t n)
{
  return mnemonic != n;
}

uint32_t COp::getMnemonic(uint32_t op)
{
  return op & 0xFF;
}

Type COp::getType(uint32_t op)
{
  return Type::fromInt((op >> 8) & 0xFF);
}










CodeBlock::~CodeBlock(void)
{
}

/** Return pointer to the code in the block */
Code* CodeBlock::getCodePtr(void)
{
  return &code;
}

/** Return pointer to the code in the block */
const Code* CodeBlock::getCodePtr(void) const
{
  return &code;
}

/** Return name of the code block */
std::string CodeBlock::getName(void) const
{
  return name;
}



