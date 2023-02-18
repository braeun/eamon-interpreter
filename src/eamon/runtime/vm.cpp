/********************************************************************************
 *                                                                              *
 * EamonInterpreter - virtual machine                                           *
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

#include "vm.h"
#include "op.h"
#include "address.h"
#include "inputstream.h"
#include "outputstream.h"
#include <string.h>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <algorithm>



VM::VM(std::shared_ptr<InputStream>& sin, std::shared_ptr<OutputStream>& sout):
  executable(nullptr),
  os(sout),
  is(sin),
  errorHandler(0),
  slowdown(0)
{
  library = std::make_unique<Library>(is,os);
}

void VM::clearStack()
{
  stack.clear();
}

void VM::load(std::shared_ptr<Executable> x)
{
  currentLine = 0;
  executable = x;
  if (executable)
  {
    cptr = executable->getCode();
    cptr++; /* skip OP_ENTRY */
    uint32_t numSize = *cptr++;
    setupGlobal(numSize);
  }
  else
  {
    cptr = nullptr;
  }
}

bool VM::isExecutableLoaded() const
{
  return executable.get() != nullptr;
}

void VM::run()
{
  if (isExecutableLoaded())
  {
    errorHandler = 0;
    cptr = executable->getCode();
    cptr += 2;
    stack.clear();
    loop();
  }
}

void VM::run(std::shared_ptr<Executable> x)
{
  load(x);
  run();
}

void VM::pause()
{
  requestPause = true;
}

bool VM::isPaused() const
{
  return (requestPause && cptr != nullptr && COp::getMnemonic(*cptr) != OP_END);
}

void VM::resume()
{
  if (cptr != nullptr && COp::getMnemonic(*cptr) != OP_END)
  {
    loop();
  }
}

Value VM::getValue(Symbol::SymbolType type, const std::string& name, int32_t index)
{
  if (!isExecutableLoaded()) return Value();
  if (type == Symbol::FUNCTION) return Value();
  const Symbol* s = executable->findSymbol(name,type);
  if (s == nullptr) return Value();
  uint32_t addr = Address::getAddress(s->getAddress());
  switch (type)
  {
    case Symbol::VARIABLE:
      return mem.getValue(addr,index);
    case Symbol::CONSTANT:
      return executable->getConstant(addr,index);
    default:
      return Value();
  }
}

void VM::setSlowdown(uint32_t microseconds)
{
  slowdown = microseconds;
}

uint32_t VM::getSlowdown() const
{
  return slowdown;
}

void VM::setDisk(const std::string &d)
{
  library->reset();
  library->setDisk(d);
}

const std::vector<uint8_t>& VM::getHiresPage() const
{
  return library->getHiresPage();
}

const std::string& VM::getChainedFile()
{
  return library->getChainedFile();
}






void VM::setupGlobal(uint32_t numSize)
{
  mem.reset(numSize);
}

uint32_t VM::getVariableAddress(const std::string& name)
{
  const Symbol* s = executable->findSymbol(name,Symbol::VARIABLE);
  if (s != nullptr) return s->getAddress();
  return 0xFFFFFFFF;
}

void VM::pushArray(uint32_t addr, Type /*type*/)
{
  std::vector<Value> values = executable->getConstantArray(addr);
  for (const Value& v : values) stack.push(v);
  stack.push(static_cast<int32_t>(values.size()));
}

void VM::loop(int depth)
{
  try
  {
    requestPause = false;
    while (!requestPause && cptr != nullptr)
    {
//      std::cout << static_cast<int64_t>(cptr-executable->getCode()) << " " << COp::getMnemonic(*cptr) << std::endl;
      uint32_t op = *cptr++;
      switch (COp::getMnemonic(op))
      {
        case OP_PUSH:
          opPush(op);
          break;
        case OP_POP:
          opPop();
          break;
        case OP_STO:
          opStore(op,false);
          break;
        case OP_RCL:
          opRecall(op,false);
          break;
        case OP_STOI:
          opStore(op,true);
          break;
        case OP_RCLI:
          opRecall(op,true);
          break;
        case OP_DUP:
          opDup();
          break;
        case OP_SWAP:
          opSwap();
          break;
        case OP_ARIADD:
        case OP_ARISUB:
        case OP_ARIMUL:
        case OP_ARIDIV:
        case OP_ARIMOD:
          opAri(op);
          break;
        case OP_CAST:
          opCast(op);
          break;
        case OP_NEG:
          opNeg(op);
          break;
        case OP_INC:
          opInc();
          break;
        case OP_DEC:
          opDec();
          break;
        case OP_ARIEQ:
        case OP_ARINE:
        case OP_ARIGE:
        case OP_ARILE:
        case OP_ARIGT:
        case OP_ARILT:
          opCmp(op);
          break;
        case OP_ARIAND:
        case OP_ARIOR:
          opBit(op);
          break;
        case OP_ARINOT:
          opNot(op);
          break;
        case OP_AND:
        case OP_OR:
          opLogic(op);
          break;
        case OP_JSR:
          stack.push(static_cast<int32_t>(cptr-executable->getCode()+1)); /* push address of next op */
          cptr = executable->getCode() + *cptr;     /* jump to address */
          break;
        case OP_RET:
          cptr = executable->getCode() + stack.pop().getInt(); /* get return address from stack */
          break;
        case OP_JZ:
          if (stack.pop().getInt() == 0)
            cptr = executable->getCode() + *cptr;
          else
            cptr++;
          break;
        case OP_JNZ:
          if (stack.pop().getInt() != 0)
            cptr = executable->getCode() + *cptr;
          else
            cptr++;
          break;
        case OP_JUMP:
          cptr = executable->getCode() + *cptr;
          break;
        case OP_CALL:
          opCall();
          break;
        case OP_CLR:
          opClr(op);
          break;
        case OP_RSZ:
          opRsz(op);
          break;
        case OP_ERRHDL:
          errorHandler = *cptr++;
          break;
        case OP_END:
          requestPause = true;
          break;
        case ASM_LINE:
          currentLine = *cptr++;
          break;
      }
      if (slowdown > 0) usleep(slowdown);
    }
  }
  catch (std::exception& ex)
  {
    if (errorHandler != 0 && depth < 1)
    {
       cptr = executable->getCode() + errorHandler;
       loop(depth+1);
    }
    else
    {
      std::ostringstream os;
      if (currentLine > 0)
        os << "Runtime exception in line " << currentLine << " (@" << (cptr-executable->getCode()) << ")";
      else
        os << "Runtime exception at " << (cptr-executable->getCode());
      os << ": " << ex.what();
      throw std::runtime_error(os.str());
    }
  }
}

void VM::opPush(uint32_t op)
{
  Type type = COp::getType(op);
  if (type == Type::int32Type)
  {
    int32_t v = static_cast<int32_t>(*cptr++);
    stack.push(v);
  }
  else if (type == Type::doubleType)
  {
    double v = *(reinterpret_cast<const double*>(cptr));
    cptr += 2;
    stack.push(v);
  }
  else if (type == Type::stringType)
  {
    stack.push(executable->getConstant(Address::getAddress(*cptr++)).getString());
  }
  else if (type.isArrayType())
  {
    pushArray(Address::getAddress(*cptr++),type);
  }
}

void VM::opPop()
{
  stack.pop();
}

void VM::opAri(uint32_t op)
{
  Value v2 = stack.pop();
  Value v1 = stack.pop();
  switch (COp::getMnemonic(op))
  {
    case OP_ARIADD:
      stack.push(v1 + v2);
      break;
    case OP_ARISUB:
      stack.push(v1 - v2);
      break;
    case OP_ARIMUL:
      stack.push(v1 * v2);
      break;
    case OP_ARIDIV:
      stack.push(v1 / v2);
      break;
    case OP_ARIMOD:
      stack.push(v1 % v2);
      break;
  }
}

void VM::opCmp(uint32_t op)
{
  Value v2 = stack.pop();
  Value v1 = stack.pop();
  switch (COp::getMnemonic(op))
  {
    case OP_ARIEQ:
      stack.push(v1 == v2 ? 1 : 0);
      break;
    case OP_ARINE:
      stack.push(v1 != v2 ? 1 : 0);
      break;
    case OP_ARIGE:
      stack.push(v1 >= v2 ? 1 : 0);
      break;
    case OP_ARILE:
      stack.push(v1 <= v2 ? 1 : 0);
      break;
    case OP_ARIGT:
      stack.push(v1 > v2 ? 1 : 0);
      break;
    case OP_ARILT:
      stack.push(v1 < v2 ? 1 : 0);
      break;
  }
}

void VM::opBit(uint32_t op)
{
  Value v2 = stack.pop();
  Value v1 = stack.pop();
  Value v;
  switch (COp::getMnemonic(op))
  {
    case OP_ARIAND:
      v = (v1 & v2);
      break;
    case OP_ARIOR:
      v = (v1 | v2);
      break;
  }
  stack.push(v);
}

void VM::opLogic(uint32_t op)
{
  Value v2 = stack.pop();
  Value v1 = stack.pop();
  bool v = false;
  switch (COp::getMnemonic(op))
  {
    case OP_AND:
      v = (v1 && v2);
      break;
    case OP_OR:
      v = (v1 || v2);
      break;
  }
  stack.push(static_cast<int32_t>(v));
}

void VM::opNot(uint32_t /*op*/)
{
  Value v = stack.pop();
  v.opnot();
  stack.push(v);
}

void VM::opNeg(uint32_t /*op*/)
{
  Value v = stack.pop();
  v.negate();
  stack.push(v);
}

void VM::opClr(uint32_t op)
{
  Type t = COp::getType(op);
  uint32_t a = *cptr++;
  int32_t addr = static_cast<int32_t>(Address::getAddress(a));
  if (Address::isGlobalAddress(a))
    mem.clr(Value::zero(t),addr);
}

void VM::opRsz(uint32_t op)
{
  Type t = COp::getType(op);
  uint32_t a = *cptr++;
  int32_t addr = static_cast<int32_t>(Address::getAddress(a));
  uint32_t size = stack.pop().getInt();
  if (Address::isGlobalAddress(a))
  {
    mem.resize(addr,size);
    mem.clr(Value::zero(t),addr);
  }
}

void VM::opStore(uint32_t op, bool indexed)
{
  Type t = COp::getType(op);
  uint32_t a = *cptr++;
  int32_t addr = static_cast<int32_t>(Address::getAddress(a));
  if (Address::isGlobalAddress(a))
    opStoreG(addr,t,indexed);
}

void VM::opStoreG(int32_t addr, Type t, bool indexed)
{
  int32_t offset = indexed ? stack.pop().getInt() : 0;
  /* Variables have a type. If we store in a variable, we must make sure the
   * value corresponds to the variable type */
  if (t == Type::int32Type)
    mem.store(stack.pop().getInt(),addr,offset);
  else if (t == Type::doubleType)
    mem.store(stack.pop().getDouble(),addr,offset);
  else if (t == Type::stringType)
    mem.store(stack.pop().getString(),addr,offset);
  else if (t.isArrayType())
    opStoreArrayG(addr+offset,t);
}

void VM::opStoreArrayG(int32_t addr, Type t)
{
  int32_t n = stack.pop().getInt();
  if (t == Type::aint32Type)
    for (int32_t i=n;i>0;i--) mem.store(stack.pop().getInt(),addr,i-1);
  else if (t == Type::adoubleType)
    for (int32_t i=n;i>0;i--) mem.store(stack.pop().getDouble(),addr,i-1);
  else if (t == Type::astringType)
    for (int32_t i=n;i>0;i--) mem.store(stack.pop().getString(),addr,i-1);
}

void VM::opRecall(uint32_t op, bool indexed)
{
  Type t1 = COp::getType(op);
  uint32_t a = *cptr++;
  int32_t addr = static_cast<int32_t>(Address::getAddress(a));
  if (Address::isGlobalAddress(a))
    opRecallG(addr,t1,indexed);
  else if (Address::isConstantAddress(a))
    opRecallC(static_cast<uint32_t>(addr),t1,indexed);
}

void VM::opRecallG(int32_t addr, Type t1, bool indexed)
{
  int32_t offset = indexed ? stack.pop().getInt() : 0;
  if (t1.isArrayType())
  {
    int32_t n = stack.pop().getInt();
    for (int32_t i=0;i<n;i++) stack.push(mem.getValue(addr+offset,i));
    stack.push(n);
  }
  else
  {
    stack.push(mem.getValue(addr,offset));
  }
}

void VM::opRecallC(uint32_t addr, Type /*t1*/, bool indexed)
{
  int32_t offset = indexed ? stack.pop().getInt() : 0;
  stack.push(executable->getConstant(addr,offset));
}


void VM::opDec()
{
  uint32_t a = *cptr++;
  uint32_t addr = static_cast<uint32_t>(Address::getAddress(a));
  if (Address::isConstantAddress(a)) return;
  mem.dec(addr);
}

void VM::opInc()
{
  uint32_t a = *cptr++;
  uint32_t addr = static_cast<uint32_t>(Address::getAddress(a));
  if (Address::isConstantAddress(a)) return;
  mem.inc(addr);
}


void VM::opCall()
{
  uint32_t id = *cptr++;
  uint32_t func = id & 0xFFFF;
  library->execute(static_cast<uint16_t>(func),mem,stack,executable.get());
  if (library->isTerminateRequested())
  {
    cptr = nullptr;
  }
}

void VM::opDup()
{
  Value v = stack.pop();
  stack.push(v);
  stack.push(v);
}

void VM::opSwap()
{
  stack.swap();
}

void VM::opCast(uint32_t op)
{
  Type t2 = COp::getType(op);
  if (t2 == Type::int32Type)
    stack.push(stack.pop().getInt());
  else if (t2 == Type::doubleType)
    stack.push(stack.pop().getDouble());
  else if (t2 == Type::stringType)
    stack.push(stack.pop().getString());
}

