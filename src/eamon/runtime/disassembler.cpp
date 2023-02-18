/********************************************************************************
 *                                                                              *
 * EamonInterpreter - disassembler                                              *
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

#include "disassembler.h"
#include "op.h"
#include "address.h"
#include "library.h"
#include <iostream>
#include <iomanip>



Disassembler::Disassembler():
  os(&std::cout)
{
}

Disassembler::Disassembler(std::ostream& os):
  os(&os)
{
}

void Disassembler::setOutputStream(std::ostream &s)
{
  os = &s;
}

void Disassembler::disassemble(Executable* executable)
{
  const uint32_t* cptr = executable->getCode();
  while (cptr-executable->getCode() < executable->getCodeLength() / sizeof(uint32_t))
  {
    *os << std::setw(4) << cptr-executable->getCode() << ": ";
    uint32_t op = *cptr++;
    switch (COp::getMnemonic(op))
    {
      case OP_NOP:
        *os << "nop";
        break;
      case OP_ENTRY:
        *os << "ENTRY";
        cptr = printEntryData(op,cptr);
        break;
      case OP_PUSH:
        *os << "push";
        cptr = printPar(executable,op,cptr);
        break;
      case OP_POP:
        *os << "pop";
//        printType(op);
        break;
      case OP_STO:
        *os << "sto";
        printType(op);
        cptr = printAddr(op,cptr);
        break;
      case OP_STOI:
        *os << "stoi";
        printType(op);
        cptr = printAddr(op,cptr);
        break;
      case OP_RCL:
        *os << "rcl";
        printType(op);
        cptr = printAddr(op,cptr);
        break;
      case OP_RCLI:
        *os << "rcli";
        printType(op);
        cptr = printAddr(op,cptr);
        break;
      case OP_DUP:
        *os << "dup";
        break;
      case OP_SWAP:
        *os << "swap";
        break;
      case OP_NEG:
        *os << "neg";
        break;
      case OP_ARIADD:
        *os << "add";
        break;
      case OP_ARISUB:
        *os << "sub";
        break;
      case OP_ARIMUL:
        *os << "mul";
        break;
      case OP_ARIDIV:
        *os << "div";
        break;
      case OP_ARIMOD:
        *os << "mod";
        break;
      case OP_CAST:
        *os << "cast";
        printType(op);
        break;
      case OP_INC:
        *os << "inc";
        cptr = printAddr(op,cptr);
        break;
      case OP_DEC:
        *os << "dec";
        cptr = printAddr(op,cptr);
        break;
      case OP_ARIEQ:
        *os << "eq";
        break;
      case OP_ARINE:
        *os << "ne";
        break;
      case OP_ARIGE:
        *os << "ge";
        break;
      case OP_ARILE:
        *os << "le";
        break;
      case OP_ARIGT:
        *os << "gt";
        break;
      case OP_ARILT:
        *os << "lt";
        break;
      case OP_ARIAND:
        *os << "bitand";
        break;
      case OP_ARIOR:
        *os << "bitor";
        break;
      case OP_ARINOT:
        *os << "not";
        break;
      case OP_AND:
        *os << "and";
        break;
      case OP_OR:
        *os << "or";
        break;
      case OP_JSR:
        *os << "jsr";
        cptr = printAddr(op,cptr);
        break;
      case OP_RET:
        *os << "ret";
        break;
      case OP_JZ:
        *os << "jz";
        cptr = printAddr(op,cptr);
        break;
      case OP_JNZ:
        *os << "jnz";
        cptr = printAddr(op,cptr);
        break;
      case OP_JUMP:
        *os << "jump";
        cptr = printAddr(op,cptr);
        break;
      case OP_CALL:
        *os << "call";
        cptr = printLibraryCallData(op,cptr);
        break;
      case OP_CLR:
        *os << "clr";
        printType(op);
        cptr = printAddr(op,cptr);
        break;
      case OP_RSZ:
        *os << "rsz";
        printType(op);
        cptr = printAddr(op,cptr);
        break;
      case OP_ERRHDL:
        *os << "errhdl";
        cptr = printAddr(op,cptr);
        break;
      case OP_END:
        *os << "end";
        break;
      case ASM_LINE:
        *os << ".LINE " << *cptr++;
        break;
      default:
        *os << "??? " << COp::getMnemonic(op);
        break;
    }
    *os << std::endl;
  }
}



void Disassembler::printType(uint32_t op)
{
  printType(COp::getType(op));
}

const uint32_t* Disassembler::printPar(Executable* executable, uint32_t op, const uint32_t* cptr)
{
  Type type = COp::getType(op);
  printType(type);
  if (type == Type::int32Type)
  {
    *os << " " << *cptr;
    cptr++;
  }
  else if (type == Type::doubleType)
  {
    *os << " " << *(reinterpret_cast<const double*>(cptr));
    cptr += 2;
  }
  else if (type == Type::stringType)
  {
    std::string s = executable->getConstant(Address::getAddress(*cptr)).getString();
    std::string::size_type i = s.find('\n');
    while (i != std::string::npos)
    {
      s.replace(i,1,"\\n");
      i = s.find('\n',i+2);
    }
    *os << " C@" << Address::getAddress(*cptr) << ": \"" << s << "\"";
    cptr++;
  }
  else if (type.isArrayType())
  {
    *os << " C@" << Address::getAddress(*cptr) << " (array)";
    cptr++;
  }
  return cptr;
}

const uint32_t* Disassembler::printAddr(uint32_t /*op*/, const uint32_t* cptr)
{
  uint32_t a = *cptr;
  *os << " ";
  if (Address::isConstantAddress(a))
    *os << "C";
  *os << "@" << Address::getAddress(a);
  cptr++;
  return cptr;
}

void Disassembler::printType(Type type)
{
  *os << "." << type.getTypeString();
}

const uint32_t* Disassembler::printEntryData(uint32_t /*op*/, const uint32_t *cptr)
{
  *os << " " << *cptr;
  cptr++;
  return cptr;
}

const uint32_t* Disassembler::printLibraryCallData(uint32_t /*op*/, const uint32_t* cptr)
{
  uint32_t id = *cptr++;
  uint16_t func = id & 0xFFFF;
  std::string name = resolveFunction(func);
  *os << " " << func;
  if (!name.empty()) *os << ": " << name;
  return cptr;
}

std::string Disassembler::resolveFunction(uint16_t func)
{
  for (const auto& f : Library::getFunctions())
  {
    if (f.id == func) return f.name;
  }
  return "";
}



