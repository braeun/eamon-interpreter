/********************************************************************************
 *                                                                              *
 * EamonInterpreter - executable                                                *
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

#include "executable.h"
#include "address.h"
#include <string.h>
#include <fstream>
#include <iostream>
#include <stdexcept>

static const char* ID = "HSCR";
static const char* ID_CODE = "CODE";
static const char* ID_TEXT = "TEXT";
static const char* ID_VTBL = "VTBL";
static const char* ID_FSYM = "FSYM";
static const char* ID_CSYM = "CSYM";
static const char* ID_VSYM = "VSYM";

Executable::Executable():
  buffer(nullptr),
  buffersize(0),
  code(nullptr),
  codelength(0),
  text(nullptr),
  textlength(0),
  globalVarSymbolTable(nullptr),
  globalVarSymbolTableLength(0),
  functionSymbolTable(nullptr),
  functionSymbolTableLength(0)
{
}

Executable::Executable(uint32_t codelength, uint32_t textlength, uint32_t vtablelength, uint32_t fsymlength, uint32_t csymlength, uint32_t vsymlength):
  codelength(codelength),
  textlength(textlength),
  vtablelength(vtablelength),
  globalVarSymbolTableLength(vsymlength),
  functionSymbolTableLength(fsymlength),
  constantSymbolTableLength(csymlength)
{
  uint32_t size = 3 * sizeof(uint32_t); /* header */
  size += 2 * sizeof(uint32_t) + codelength; /* code segment */
  size += 2 * sizeof(uint32_t) + textlength; /* text segment */
  size += 2 * sizeof(uint32_t) + vtablelength; /* vtable segment */
  size += 2 * sizeof(uint32_t) + fsymlength; /* function symbol table segment */
  size += 2 * sizeof(uint32_t) + csymlength; /* constant symbol table segment */
  size += 2 * sizeof(uint32_t) + vsymlength; /* variable symbol table segment */
  buffer = reinterpret_cast<char*>(malloc(size));
  buffersize = size;
  char *ptr = buffer;
  uint32_t* tmp = reinterpret_cast<uint32_t*>(ptr);
  *tmp++ = *(reinterpret_cast<const uint32_t*>(ID));
  *tmp++ = 1;
  *tmp++ = buffersize;
  ptr += 3 * sizeof(uint32_t);
  code = reinterpret_cast<uint32_t*>(ptr);
  *code++ = *(reinterpret_cast<const uint32_t*>(ID_CODE));
  *code++ = codelength;

  ptr += 2 * sizeof(uint32_t) + codelength;
  tmp = reinterpret_cast<uint32_t*>(ptr);
  *tmp++ = *(reinterpret_cast<const uint32_t*>(ID_TEXT));
  *tmp++ = textlength;
  text = ptr + 2 * sizeof(uint32_t);

  ptr += 2 * sizeof(uint32_t) + textlength;
  vtable = reinterpret_cast<int32_t*>(ptr);
  *vtable++ = *(reinterpret_cast<const int32_t*>(ID_VTBL));
  *vtable++ = static_cast<int32_t>(vtablelength);

  ptr += 2 * sizeof(uint32_t) + vtablelength;
  tmp = reinterpret_cast<uint32_t*>(ptr);
  *tmp++ = *(reinterpret_cast<const uint32_t*>(ID_FSYM));
  *tmp++ = functionSymbolTableLength;
  functionSymbolTable = reinterpret_cast<Symbol*>(tmp);

  ptr += 2 * sizeof(uint32_t) + functionSymbolTableLength;
  tmp = reinterpret_cast<uint32_t*>(ptr);
  *tmp++ = *(reinterpret_cast<const uint32_t*>(ID_CSYM));
  *tmp++ = constantSymbolTableLength;
  constantSymbolTable = reinterpret_cast<Symbol*>(tmp);

  ptr += 2 * sizeof(uint32_t) + constantSymbolTableLength;
  tmp = reinterpret_cast<uint32_t*>(ptr);
  *tmp++ = *(reinterpret_cast<const uint32_t*>(ID_VSYM));
  *tmp++ = globalVarSymbolTableLength;
  globalVarSymbolTable = reinterpret_cast<Symbol*>(tmp);
//  ptr += 2 * sizeof(uint32_t) + globalVarSymbolTableLength;
}


Executable::~Executable()
{
  if (buffer != nullptr) free(buffer);
}





const uint32_t* Executable::getCode()
{
  return code;
}

uint32_t Executable::getCodeLength()
{
  return codelength;
}

Value Executable::getConstant(uint32_t addr, int32_t index) const
{
  addr = Address::getAddress(addr);
  if (addr >= constantValues.size()) throw std::runtime_error("Illegal getConstant access");
  size_t size = constantValues[addr].size();
  if (index < 0 || index >= size)  throw std::runtime_error("Illegal getConstant access");
  return constantValues[addr][index];
}

std::vector<Value> Executable::getConstantArray(uint32_t addr) const
{
  std::vector<Value> list;
  addr = Address::getAddress(addr);
  if (addr >= constantValues.size()) throw std::runtime_error("Illegal getConstantArray access");
  return constantValues[addr];
}

const Symbol* Executable::findConstant(const std::string& name) const
{
  return findSymbol(name,Symbol::CONSTANT);
}

const int32_t* Executable::getVTable()
{
  return vtable;
}

const Symbol* Executable::findSymbol(const std::string& name, Symbol::SymbolType type) const
{
  const Symbol* s;
  uint32_t n = 0;
  switch (type)
  {
    case Symbol::FUNCTION:
      s = functionSymbolTable;
      n = functionSymbolTableLength / sizeof(Symbol);
      break;
    case Symbol::CONSTANT:
      s = constantSymbolTable;
      n = constantSymbolTableLength / sizeof(Symbol);
      break;
    case Symbol::VARIABLE:
      s = globalVarSymbolTable;
      n = globalVarSymbolTableLength / sizeof(Symbol);
      break;
  }
  while (n > 0)
  {
    if (strncmp(s->getName().c_str(),name.c_str(),MAX_IDENTIFIER_LENGTH) == 0) return s;
    s++;
    n--;
  }
  return nullptr;
}

std::vector<Symbol> Executable::getSymbolTable(Symbol::SymbolType type) const
{
  const Symbol* s;
  uint32_t n = 0;
  switch (type)
  {
    case Symbol::FUNCTION:
      s = functionSymbolTable;
      n = functionSymbolTableLength / sizeof(Symbol);
      break;
    case Symbol::CONSTANT:
      s = constantSymbolTable;
      n = constantSymbolTableLength / sizeof(Symbol);
      break;
    case Symbol::VARIABLE:
      s = globalVarSymbolTable;
      n = globalVarSymbolTableLength / sizeof(Symbol);
      break;
  }
  std::vector<Symbol> list;
  while (n > 0)
  {
    list.push_back(*s);
    s++;
    n--;
  }
  return list;
}





bool Executable::save(const std::string& filename)
{
  std::ofstream s;
  s.open(filename.c_str(),std::ios::out|std::ios::binary);
  if (!(s.is_open() && s.good())) return false;
  save(s);
//  annotations.write(s);
  s.close();
  return true;
}

bool Executable::save(std::ostream& os)
{
  os.write(buffer,buffersize);
  return true;
}

bool Executable::save(char **buf, uint32_t *size)
{
  char* tmp = reinterpret_cast<char*>(malloc(buffersize));
  memmove(tmp,buffer,buffersize);
  *size = buffersize;
  *buf = tmp;
  return true;
}

bool Executable::load(const std::string& filename)
{
  std::ifstream s;
  s.open(filename.c_str(),std::ios::in|std::ios::binary);
  if (!(s.is_open() && s.good())) return false;
  load(s);
  s.close();
  return true;
}


bool Executable::load(std::istream& s)
{
  uint32_t hdr[3];
  s.read(reinterpret_cast<char*>(&hdr),sizeof(hdr));
  // TODO: test for magic number  and version
  buffersize = hdr[2];
  if (buffer != nullptr) free(buffer);
  buffer = reinterpret_cast<char*>(malloc(buffersize));
  memcpy(buffer,hdr,sizeof(hdr));
  s.read(buffer+sizeof(hdr),buffersize-sizeof(hdr));
  setupTables();
  return true;
}

bool Executable::load(char *buf, uint32_t size)
{
  if (buffer != nullptr) free(buffer);
  buffer = reinterpret_cast<char*>(malloc(size));
  memcpy(buffer,buf,size);
  buffersize = size;
  setupTables();
  return true;
}


void Executable::setCodeSegment(const uint32_t* c)
{
  memcpy(code,c,codelength);
}

void Executable::setTextSegment(const char* t)
{
  memcpy(text,t,textlength);
  buildConstantValueTable();
}

void Executable::setVTable(std::vector<int32_t> table)
{
  memcpy(vtable,table.data(),vtablelength);
}

void Executable::setFunctionSymbolTable(std::vector<Symbol> table)
{
  memcpy(functionSymbolTable,table.data(),functionSymbolTableLength);
}

void Executable::setConstantSymbolTable(std::vector<Symbol> table)
{
  memcpy(constantSymbolTable,table.data(),constantSymbolTableLength);
}

void Executable::setVariableSymbolTable(std::vector<Symbol> table)
{
  memcpy(globalVarSymbolTable,table.data(),globalVarSymbolTableLength);
}






void Executable::setupTables()
{
  uint32_t hdr[3];
  char* ptr = buffer + sizeof(hdr);
  uint32_t* tmp;
  tmp = reinterpret_cast<uint32_t*>(ptr);
  tmp++; // TODO check for correct segment
  codelength = *tmp++;
  code = tmp;
  ptr += 2 * sizeof(uint32_t) + codelength;

  tmp = reinterpret_cast<uint32_t*>(ptr);
  tmp++; // TODO check for correct segment
  textlength = *tmp++;
  text = ptr + 2 * sizeof(uint32_t);
  ptr += 2 * sizeof(uint32_t) + textlength;

  tmp = reinterpret_cast<uint32_t*>(ptr);
  tmp++; // TODO check for correct segment
  vtablelength = *tmp++;
  vtable = reinterpret_cast<int32_t*>(tmp);
  ptr += 2 * sizeof(uint32_t) + vtablelength;

  tmp = reinterpret_cast<uint32_t*>(ptr);
  tmp++; // TODO check for correct segment
  functionSymbolTableLength = *tmp++;
  functionSymbolTable = reinterpret_cast<Symbol*>(tmp);
  ptr += 2 * sizeof(uint32_t) + functionSymbolTableLength;

  tmp = reinterpret_cast<uint32_t*>(ptr);
  tmp++; // TODO check for correct segment
  constantSymbolTableLength = *tmp++;
  constantSymbolTable = reinterpret_cast<Symbol*>(tmp);
  ptr += 2 * sizeof(uint32_t) + constantSymbolTableLength;

  tmp = reinterpret_cast<uint32_t*>(ptr);
  tmp++; // TODO check for correct segment
  globalVarSymbolTableLength = *tmp++;
  globalVarSymbolTable = reinterpret_cast<Symbol*>(tmp);
  ptr += 2 * sizeof(uint32_t) + globalVarSymbolTableLength;

  buildConstantValueTable();
}

void Executable::buildConstantValueTable()
{
  constantValues.clear();
  const char* p = text;
  while (p-text < textlength)
  {
    std::vector<Value> constant;
    int32_t n = *reinterpret_cast<const int32_t*>(p);
    p += sizeof(int32_t);
    while (n-- > 0)
    {
      uint32_t t = *reinterpret_cast<const uint32_t*>(p);
      p += sizeof(int32_t);
      Type type = Type::fromInt(t);
      if (type == Type::int32Type)
      {
        int32_t v = *reinterpret_cast<const int32_t*>(p);
        p += sizeof(int32_t);
        constant.push_back(Value(v));
      }
      else if (type == Type::doubleType)
      {
        double v = *reinterpret_cast<const double*>(p);
        p += sizeof(double);
        constant.push_back(Value(v));
      }
      else if (type == Type::stringType)
      {
        std::string v(p);
        while (*p != '\0') p++;
        p++;
        constant.push_back(Value(v));
      }
    }
    constantValues.push_back(constant);
  }
}



