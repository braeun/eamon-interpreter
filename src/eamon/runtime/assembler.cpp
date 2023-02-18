/********************************************************************************
 *                                                                              *
 * EamonInterpreter - assembler                                                 *
 *                                                                              *
 * modified: 2023-02-18                                                         *
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

#include "assembler.h"
#include "executable.h"
#include "op.h"
#include "compilerdata.h"
#include "address.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>

#define SIZE_INCREMENT 1024



Assembler::Assembler(CompilerData& data):
  data(data),
  code(nullptr),
  text(nullptr)
{
}

Assembler::~Assembler()
{
  if (text) free(text);
  if (code) free(code);
}

const Errors& Assembler::getErrors() const
{
  return errors;
}


Executable* Assembler::assemble()
{
  reset();
  storeConstants();
  *cptr++ = OP_ENTRY;
  *cptr++ = data.globalVariables.getNumericBlockSize();
//  *cptr++ = data.globalVariables.getStringBlockSize();
  assembleBlock(data.codeblock);
  int index = 0;
  for (const Function& func : data.functions.getFunctions())
  {
//    int32_t fp = static_cast<int32_t>(cptr - code);
//    functionAddr.push_back(fp);
//    *cptr++ = OP_ENTRY;
//    *cptr++ = func.localvars.getNumericBlockSize();
//    *cptr++ = func.localvars.getStringBlockSize();
    assembleBlock(func.code);
    index++;
  }
  codeSize = static_cast<uint32_t>(cptr - code);

  resolveLabels();

  std::vector<Symbol> vsymtable;
  for (uint32_t i=0;i<data.globalVariables.size();i++)
  {
    const Variable& v = data.globalVariables[i];
    checkIdentifierLength(v.getName());
    vsymtable.push_back(Symbol(v.getName(),v.getAddress(),v.getType(),Symbol::VARIABLE));
//    vsymtable.back().setDims(v.getDims());
  }
  std::vector<Symbol> fsymtable;
  for (uint32_t i=0;i<data.functions.size();i++)
  {
    const Function& f = data.functions.getFunctions()[i];
    checkIdentifierLength(f.name);
//    fsymtable.push_back(Symbol(f.name,static_cast<uint32_t>(f.vtableIndex),f.rettype,Symbol::FUNCTION));
    fsymtable.push_back(Symbol(f.name,static_cast<uint32_t>(0),f.rettype,Symbol::FUNCTION));
  }
  std::vector<Symbol> csymtable;
  for (uint32_t i=0;i<data.constants.getConstants().size();i++)
  {
    const Constant& c = data.constants.getConstants()[i];
    checkIdentifierLength(c.getName());
    csymtable.push_back(Symbol(c.getName(),c.getAddress(),c.getType(),Symbol::CONSTANT));
//    csymtable.back().setDims(c.getDims());
  }

  uint32_t codelength = static_cast<uint32_t>(cptr-code) * sizeof(uint32_t);
  uint32_t textlength = static_cast<uint32_t>(ctext-text);
  uint32_t vtablelength = static_cast<uint32_t>(functionAddr.size()*sizeof(int32_t));
  uint32_t fsymlength = static_cast<uint32_t>(fsymtable.size()) * sizeof(Symbol);
  uint32_t csymlength = static_cast<uint32_t>(csymtable.size()) * sizeof(Symbol);
  uint32_t vsymlength = static_cast<uint32_t>(vsymtable.size()) * sizeof(Symbol);

  Executable* x = new Executable(codelength,textlength,vtablelength,fsymlength,csymlength,vsymlength);
  x->setCodeSegment(code);
  x->setTextSegment(text);
  x->setVTable(functionAddr);
  x->setFunctionSymbolTable(fsymtable);
  x->setConstantSymbolTable(csymtable);
  x->setVariableSymbolTable(vsymtable);

  return x;
}



void Assembler::reset()
{
  if (code) free(code);
  code = reinterpret_cast<uint32_t*>(malloc(SIZE_INCREMENT*sizeof(uint32_t)));
  maxSize = SIZE_INCREMENT;
  if (text) free(text);
  text = reinterpret_cast<char*>(malloc(SIZE_INCREMENT));
  textMaxSize = SIZE_INCREMENT;
  cptr = code;
  ctext = text;
  functionAddr.clear();
  labelAddr.clear();
}

void Assembler::reallocateCode()
{
  int64_t l = cptr - code;
  uint32_t s = maxSize + SIZE_INCREMENT;
  uint32_t* m = reinterpret_cast<uint32_t*>(realloc(code,s*sizeof(uint32_t)));
  if (m != nullptr)
  {
    code = m;
    maxSize = s;
    cptr = code + l;
  }
}

void Assembler::reallocateText(uint32_t l)
{
  int64_t p = ctext - text;
  uint32_t s = textMaxSize + SIZE_INCREMENT + l;
  char* m = reinterpret_cast<char*>(realloc(text,s));
  if (m != nullptr)
  {
    text = m;
    textMaxSize = s;
    ctext = text + p;
  }
}

uint32_t Assembler::storeConstants()
{
  ctext = text;
  uint32_t a = 0;
  for (Constant& c : data.constants.getConstants())
  {
    uint32_t addr = static_cast<uint32_t>(ctext - text);
    if (addr + 12 >= textMaxSize) reallocateText();
    addr = storeArray(c.getArray(),c.getType());
    c.setAddress(a);
    a++;
#if 0
//    std::cout << c.getName() << " 0x" << std::hex << c.getAddress() << std::dec  << " @ " << addr << std::endl;
    if (c.getType() == Type::int32Type)
    {
      *(reinterpret_cast<uint32_t*>(ctext)) = 1;
      ctext += sizeof(uint32_t);
      *(reinterpret_cast<int32_t*>(ctext)) = c.getValueI32();
      c.setAddress(addr);
      ctext += sizeof(int32_t);
    }
    else if (c.getType() == Type::int64Type)
    {
      *(reinterpret_cast<uint32_t*>(ctext)) = 1;
      ctext += sizeof(uint32_t);
      *(reinterpret_cast<int64_t*>(ctext)) = c.getValueI64();
      c.setAddress(addr);
      ctext += sizeof(int64_t);
    }
    else if (c.getType() == Type::doubleType)
    {
      *(reinterpret_cast<uint32_t*>(ctext)) = 1;
      ctext += sizeof(uint32_t);
      *(reinterpret_cast<double*>(ctext)) = c.getValueDouble();
      c.setAddress(addr);
      ctext += sizeof(double);
    }
    else if (c.getType() == Type::stringType)
    {
      addr = storeText(c.getValueString());
      c.setAddress(addr);
    }
    else if (c.getType().isArrayType())
    {
      addr = storeArray(c.getArray(),c.getType());
      c.setAddress(addr);
    }
#endif
  }
  return static_cast<uint32_t>(ctext-text);
}

uint32_t Assembler::storeArray(const std::vector<TypedValue>& values, Type type)
{
  uint32_t size = sizeof(int32_t) + static_cast<uint32_t>(values.size()) * sizeof(int32_t);
  for (const TypedValue& v : values)
  {
    if (v.getType() == Type::stringType)
    {
      uint64_t l = v.getValue().getString().length() + 1;
      /* align to 4 byte boundary */
//      l = ((l + sizeof(int32_t) - 1) / sizeof(int32_t)) * sizeof(int32_t);
      size += l;
    }
    else if (v.getType() == Type::int32Type)
      size += sizeof(int32_t);
    else if (v.getType() == Type::doubleType)
      size += sizeof(double);
  }
  uint32_t p = static_cast<uint32_t>(ctext - text);
  if (p + size + 5 > textMaxSize) reallocateText(size);
  *(reinterpret_cast<uint32_t*>(ctext)) = static_cast<uint32_t>(values.size());
  ctext += sizeof(uint32_t);
  for (const TypedValue& v : values)
  {
    *(reinterpret_cast<uint32_t*>(ctext)) = static_cast<uint32_t>(v.getType().toInt());
    ctext += sizeof(uint32_t);
    if (v.getType() == Type::stringType)
    {
      strcpy(ctext,v.getValue().getString().c_str());
      uint64_t l = v.getValue().getString().length() + 1;
      /* align to 4 byte boundary */
//      l = ((l + sizeof(int32_t) - 1) / sizeof(int32_t)) * sizeof(int32_t);
      ctext += l;
    }
    else if (v.getType() == Type::int32Type)
    {
      *reinterpret_cast<int32_t*>(ctext) = v.getValue().getInt();
      ctext += sizeof(int32_t);
    }
    else if (v.getType() == Type::doubleType)
    {
      *reinterpret_cast<double*>(ctext) = v.getValue().getDouble();
      ctext += sizeof(double);
    }
  }
  return p;
}

void Assembler::assembleBlock(const CodeBlock &block)
{
  assembleBlock(*block.getCodePtr());
}

void Assembler::assembleBlock(const Code& ops)
{
  for (const COp& op : ops)
  {
    if (cptr-code > maxSize - 5) reallocateCode();
    if (op.getLabel() > 0) labelAddr[op.getLabel()] = cptr - code;
    if (op.getMnemonic() == OP_NOP)
      continue;
    else if (op.getMnemonic() == ASM_LINE)
    {
      if (data.noDebug) continue;
      *cptr++ = op.getOpCode();
      *cptr++ = static_cast<uint32_t>(op.getParameterInt32());
    }
    else
    {
      *cptr++ = op.getOpCode();
      if (op.getParameterType() != Type::undefinedType)
      {
        if (op.getParameterType() == Type::int32Type)
        {
          *cptr++ = static_cast<uint32_t>(op.getParameterInt32());
        }
        else if (op.getParameterType() == Type::doubleType)
        {
          *(reinterpret_cast<double*>(cptr)) = op.getParameterDouble();
          cptr += 2;
        }
//          case stringType:
//            *cptr++ = storeText(op.getParameterString());
//            break;
//          case aint32Type:
//          case aint64Type:
//          case adoubleType:
//          case astringType:
//            *cptr++ = storeArray(op.getParameterArray(),op.getParameterType());
//            break;
      }
    }
  }
}

void Assembler::resolveLabels()
{
  uint32_t* cptr = code;
  while (cptr - code < codeSize)
  {
    uint32_t op = *cptr++;
    switch (COp::getMnemonic(op))
    {
      case OP_ENTRY:
        cptr += 1;
        break;
      case OP_PUSH:
        if (COp::getType(op).isArrayType() || COp::getType(op) == Type::stringType)
        {
          *cptr = data.constants.getConstants()[Address::getAddress(*cptr)].getAddress();
        }
        cptr++;
        break;
      case OP_STO:
      case OP_STOI:
      case OP_RCL:
      case OP_INC:
      case OP_DEC:
      case OP_CALL:
      case OP_RSZ:
      case OP_CLR:
      case OP_RCLI:
        if (Address::isConstantAddress(*cptr)) /* special case of index array constant access */
        {
          *cptr = data.constants.getConstants()[Address::getAddress(*cptr)].getAddress();
        }
        cptr++;
        break;
      case OP_JSR:
      case OP_JZ:
      case OP_JNZ:
      case OP_JUMP:
      case OP_ERRHDL:
        *cptr = labelAddr[*cptr];
        cptr++;
        break;
      case ASM_LINE:
        cptr++;
        break;
      default:
        break;
    }
  }
}

void Assembler::checkIdentifierLength(std::string name)
{
  if (name.length() > MAX_IDENTIFIER_LENGTH)
  {
    std::ostringstream s;
    s << "identifier length exceeds max. length (" << name.length() << " > " << MAX_IDENTIFIER_LENGTH << "): " << name;
    errors.addWarning(-1,s.str());
  }
}

