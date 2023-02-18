/********************************************************************************
 *                                                                              *
 * EamonInterpreter - VM library                                                *
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

#include "library.h"
#include "memory.h"
#include "stack.h"
#include "symbol.h"
#include "outputstream.h"
#include "inputstream.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <filesystem>

#define F_PRINT 0
#define F_INPUT 1
#define F_READ  2
#define F_SIN 3
#define F_COS 4
#define F_TAN 5
#define F_ASIN 6
#define F_ACOS 7
#define F_ATAN 8
#define F_ATAN2 9
#define F_SQRT 10
#define F_EXP 11
#define F_LOG 12
#define F_LOG10 13
#define F_LOG2 14
#define F_ABS 15
#define F_TAB 16
#define F_SIGN 17
#define F_RND 18
#define F_INT 19
#define F_PRINTF 20
#define F_LEFT 21
#define F_MID 22
#define F_MID1 23
#define F_RIGHT 24
#define F_LEN 25
#define F_ASC 26
#define F_CHR 27
#define F_VAL 28
#define F_STR 29
#define F_POW 30
#define F_PEEK 31
#define F_POKE 32
#define F_GET 33
#define F_INVERSE 34
#define F_NORMAL 35
#define F_VTAB 36
#define F_HTAB 37
#define F_SPC 38
#define F_HOME 39
#define F_FLASH 40
#define F_TEXT 41
#define F_FRE 42

int32_t VariableArgument::getInt() const
{
  if (type == Type::int32Type) return i32;
  if (type == Type::doubleType) return static_cast<int32_t>(d);
  return  0;
}

int64_t VariableArgument::getI64() const
{
  if (type == Type::int32Type) return i32;
  if (type == Type::doubleType) return static_cast<int64_t>(d);
  return  0;
}

double VariableArgument::getDouble() const
{
  if (type == Type::int32Type) return i32;
  if (type == Type::doubleType) return d;
  return  0;
}





LibraryFunction::LibraryFunction():
  id(0),
  name(""),
  args(""),
  rettype(Type())
{
}

LibraryFunction::LibraryFunction(const LibraryFunction& f):
  id(f.id),
  name(f.name),
  args(f.args),
  rettype(f.rettype)
{
}

LibraryFunction::LibraryFunction(uint16_t id, const std::string& name, const std::string& args, Type rettype):
  id(id),
  name(name),
  args(args),
  rettype(rettype)
{
}



const uint16_t Library::ID = 0xFFFF;

std::vector<LibraryFunction> Library::definitions;

LibraryFunction Library::UNDEFINED(0,"","",Type());

Library::Library(std::shared_ptr<InputStream>& sin, std::shared_ptr<OutputStream>& sout):
  lastRnd(0),
  cmdMode(false),
  chain(""),
  currentHiresPage(0),
  os(sout),
  is(sin)
{
  init();
}

Library::~Library()
{
}

uint16_t Library::getId() const
{
  return ID;
}

bool Library::isTerminateRequested() const
{
  return terminate;
}


void Library::requestTerminate(bool flag)
{
  terminate = flag;
}

const LibraryFunction& Library::findFunction(const std::string& name)
{
  for (const LibraryFunction &f : definitions)
  {
    if (f.name == name) return f;
  }
  return UNDEFINED;
}

const std::vector<LibraryFunction>& Library::getFunctions()
{
  return definitions;
}

void Library::execute(uint16_t id, Memory& mem, Stack& stack, const ConstantData* data)
{
  switch (id)
  {
    case F_PRINT:
      print(stack,mem);
      break;
    case F_INPUT:
      input(stack);
      break;
    case F_READ:
      read(stack,data);
      break;
    case F_SIN:
      stack.push(sin(stack.pop().getDouble()));
      break;
    case F_COS:
      stack.push(cos(stack.pop().getDouble()));
      break;
    case F_TAN:
      stack.push(tan(stack.pop().getDouble()));
      break;
    case F_ASIN:
      stack.push(asin(stack.pop().getDouble()));
      break;
    case F_ACOS:
      stack.push(acos(stack.pop().getDouble()));
      break;
    case F_ATAN:
      stack.push(atan(stack.pop().getDouble()));
      break;
    case F_ATAN2:
      {
        double x = stack.pop().getDouble();
        double y = stack.pop().getDouble();
        stack.push(atan2(y,x));
      }
      break;
    case F_SQRT:
      stack.push(sqrt(stack.pop().getDouble()));
      break;
    case F_EXP:
      stack.push(exp(stack.pop().getDouble()));
      break;
    case F_LOG:
      stack.push(log(stack.pop().getDouble()));
      break;
    case F_LOG10:
      stack.push(log10(stack.pop().getDouble()));
      break;
    case F_LOG2:
      stack.push(log2(stack.pop().getDouble()));
      break;
    case F_ABS:
      stack.push(fabs(stack.pop().getDouble()));
      break;
    case F_TAB:
      if (os) os->gotoColumn(round(stack.pop().getDouble()));
      break;
    case F_SIGN:
      {
        double v = stack.pop().getDouble();
        stack.push(v<0?-1:(v>0?1:0));
      }
      break;
    case F_RND:
      {
        int32_t v = stack.pop().getInt();
        if (v < 0)
        {
          srand(static_cast<uint32_t>(abs(v)));
        }
        else if (v > 0)
        {
          lastRnd = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
        }
        stack.push(lastRnd);
      }
      break;
    case F_INT:
      stack.push(static_cast<int32_t>(floor(stack.pop().getDouble())));
      break;
    case F_PRINTF:
      printf(stack);
      break;
    case F_LEFT:
      left(stack);
      break;
    case F_MID:
      mid(stack);
      break;
    case F_MID1:
      mid1(stack);
      break;
    case F_RIGHT:
      right(stack);
      break;
    case F_LEN:
      stack.push(static_cast<int32_t>(stack.pop().getString().length()));
      break;
    case F_ASC:
      stack.push(static_cast<double>(stack.pop().getString()[0]));
      break;
    case F_CHR:
      chr(stack);
      break;
    case F_VAL:
      stack.push(atof(stack.pop().getString().c_str()));
      break;
    case F_STR:
      str(stack);
      break;
    case F_POW:
      {
        double y = stack.pop().getDouble();
        double x = stack.pop().getDouble();
        stack.push(pow(x,y));
      }
      break;
    case F_PEEK:
      peek(stack);
      break;
    case F_POKE:
      poke(stack);
      break;
    case F_GET:
      get(stack);
      break;
    case F_INVERSE:
      printInverse();
      break;
    case F_NORMAL:
      printNormal();
      break;
    case F_VTAB:
      vtab(stack);
      break;
    case F_HTAB:
      htab(stack);
      break;
    case F_SPC:
      stack.push(std::string(stack.pop().getInt(),' '));
      break;
    case F_HOME:
      home();
      break;
    case F_FLASH:
      /* TODO: flashing is not supported: treat is as inverse */
      printInverse();
      break;
    case F_TEXT:
      text();
      break;
    case F_FRE:
      stack.pop();
      stack.push(0xFFFF);
      break;
  }
}

void Library::reset()
{
  requestTerminate(false);
  chain = "";
  cmdMode = false;
  doscmd.clear();
  files.clear();
  inputfile = nullptr;
  outputfile = nullptr;
}

void Library::setDisk(const std::string &path)
{
  disk = path;
}

const std::string& Library::getChainedFile()
{
  return chain;
}

const std::vector<uint8_t>& Library::getHiresPage() const
{
  if (currentHiresPage <= 1) return hiresPage1;
  return hiresPage2;
}







void Library::left(Stack& stack) const
{
  int32_t l = stack.pop().getInt();
  if (l <= 0 || l > 255) throw std::runtime_error("ILLEGAL QUANTITIY");
  std::string s = stack.pop().getString();
  stack.push(s.substr(0,l));
}

void Library::mid(Stack& stack) const
{
  int32_t l = stack.pop().getDouble();
  if (l <= 0 || l > 255) throw std::runtime_error("ILLEGAL QUANTITIY");
  int32_t p = stack.pop().getInt();
  if (p <= 0 || p > 255) throw std::runtime_error("ILLEGAL QUANTITIY");
  p--;  /* counting starts with 1 in BASIC */
  std::string s = stack.pop().getString();
  if (p < s.length())
    stack.push(s.substr(p,l));
  else
    stack.push(std::string(""));
}

void Library::mid1(Stack& stack) const
{
  int32_t p = stack.pop().getInt();
  if (p <= 0 || p > 255) throw std::runtime_error("ILLEGAL QUANTITIY");
  p--;  /* counting starts with 1 in BASIC */
  std::string s = stack.pop().getString();
  if (p < s.length())
    stack.push(s.substr(p));
  else
    stack.push(std::string(""));
}

void Library::right(Stack& stack) const
{
  int32_t l = stack.pop().getInt();
  if (l <= 0 || l > 255) throw std::runtime_error("ILLEGAL QUANTITIY");
  std::string s = stack.pop().getString();
  if (l == 0)
    stack.push(std::string(""));
  else if (l > s.length())
    stack.push(s);
  else
    stack.push(s.substr(s.length()-l));
}

void Library::chr(Stack &stack) const
{
  int32_t a = stack.pop().getInt();
  char txt[2] = {static_cast<char>(a), 0};
  stack.push(std::string(txt));
}

void Library::str(Stack &stack) const
{
  std::ostringstream os;
  os << stack.pop().getDouble();
  stack.push(os.str());
}

void Library::print(Stack& stack, Memory& mem)
{
  std::ostringstream s;
  print(&s,stack,mem);
  if (!s.str().empty())
  {
    if (outputfile)
    {
      outputfile->write(s.str());
    }
    else if (os)
    {
      os->write(s.str());
      os->flush();
    }
  }
}

void Library::print(std::ostream* printstream, Stack& stack, Memory& mem)
{
  int32_t narg = stack.pop().getInt();
  if (narg == 0)
  {
    stack.push(0);
    return;
  }
  std::vector<VariableArgument> args;
  for (int32_t i=0;i<narg;i++)
  {
    VariableArgument a;
    a.type = Type::fromInt(static_cast<uint32_t>(stack.pop().getInt()));
    if (a.type == Type::int32Type)
      a.i32 = stack.pop().getInt();
    else if (a.type == Type::doubleType)
      a.d = stack.pop().getDouble();
    else if (a.type == Type::stringType)
      a.s = stack.pop().getString();
    args.push_back(a);
  }
  std::reverse(args.begin(),args.end());
  if (!cmdMode)
  {
    const VariableArgument& a = args.front();
    if (a.type == Type::stringType && a.s[0] == 4)
    {
      cmdMode = true;
      doscmd.clear();
    }
    else
      print(printstream,args);
  }
  else
  {
    for (const VariableArgument& a : args)
    {
      if (a.type == Type::stringType && a.s[0] == '\n')
      {
        cmdMode = false;
        printExecute(mem);
      }
      else if (a.type == Type::stringType && a.s[0] == 4)
      {
      }
      else
      {
        std::ostringstream os;
        if (a.type == Type::int32Type)
          os << a.i32;
        else if (a.type == Type::doubleType)
          os << a.d;
        else if (a.type == Type::stringType)
          os << a.s;
        doscmd += os.str();
      }
    }
  }
}

void Library::print(std::ostream *printstream, const std::vector<VariableArgument> &args)
{
  for (const VariableArgument& a : args)
  {
    if (a.type == Type::int32Type)
      *printstream << a.i32;
    else if (a.type == Type::doubleType)
      *printstream << a.d;
    else if (a.type == Type::stringType)
      *printstream << a.s;
  }
  printstream->flush();
}

void Library::printInverse()
{
  os->inverse();
}

void Library::printNormal()
{
  os->normal();
}

void Library::printExecute(Memory& mem)
{
  if (doscmd.empty())
  {
    inputfile = nullptr;
    outputfile = nullptr;
    return;
  }
  std::string cmd = doscmd;
  std::transform(cmd.begin(),cmd.end(),cmd.begin(),::tolower);
  if (cmd.substr(0,3) == "run")
    dosRun(cmd.substr(4));
  else if (cmd.substr(0,4) == "open")
    dosOpen(trim(cmd.substr(4)));
  else if (cmd.substr(0,5) == "close")
    dosClose(trim(cmd.substr(5)));
  else if (cmd.substr(0,4) == "read")
    dosRead(trim(cmd.substr(4)));
  else if (cmd.substr(0,5) == "write")
    dosWrite(trim(cmd.substr(5)));
  else if (cmd.substr(0,6) == "delete")
    dosDelete(trim(cmd.substr(6)));
  else if (cmd.substr(0,6) == "verify")
    dosVerify(trim(cmd.substr(6)));
  else if (cmd.substr(0,5) == "bload")
    dosBLoad(trim(cmd.substr(5)),mem);
  else if (cmd.substr(0,5) == "bsave")
    dosBSave(trim(cmd.substr(5)),mem);
}

void Library::dosRun(std::string file)
{
  FileSpec spec = DiskFile::getFileSpec(file);
  chain = spec.name;
  requestTerminate(true);
}

void Library::dosOpen(std::string file)
{
  FileSpec spec = DiskFile::getFileSpec(file);
  file = disk + "/" + spec.name;
  if (spec.recordlength == 0)
  {
    dosClose(file);
    DiskFile f(file);
    files.push_back(f);
  }
  else
  {
    dosClose(file);
    DiskFile f(file,spec.recordlength);
    files.push_back(f);
  }
}

void Library::dosClose(std::string file)
{
  if (file.empty()) /* close all files */
  {
    files.clear();
    inputfile = nullptr;
    outputfile = nullptr;
  }
  else
  {
    if (file[0] != '/')
    {
      FileSpec spec = DiskFile::getFileSpec(file);
      file = disk + "/" + spec.name;
    }
    for (size_t i=0;i<files.size();i++)
    {
      if (files[i].getFilename() == file)
      {
        if (inputfile == &files[i]) inputfile = nullptr;
        if (outputfile == &files[i]) outputfile = nullptr;
        files.erase(files.begin()+i);
        break;
      }
    }
  }
}

void Library::dosRead(std::string file)
{
  FileSpec spec = DiskFile::getFileSpec(file);
  file = disk + "/" + spec.name;
  for (auto& f : files)
  {
    if (f.getFilename() == file)
    {
      if (f.isRandomAccess()) f.setIndex(spec.record,true);
      inputfile = &f;
      if (outputfile == inputfile) outputfile = nullptr;
      break;
    }
  }
}

void Library::dosWrite(std::string file)
{
  FileSpec spec = DiskFile::getFileSpec(file);
  file = disk + "/" + spec.name;
  for (auto& f : files)
  {
    if (f.getFilename() == file)
    {
      if (f.isRandomAccess()) f.setIndex(spec.record,false);
      f.erase();
      outputfile = &f;
      if (outputfile == inputfile) inputfile = nullptr;
      break;
    }
  }
}

void Library::dosDelete(std::string file)
{
  FileSpec spec = DiskFile::getFileSpec(file);
  file = disk + "/" + spec.name;
  dosClose(file);
  std::filesystem::remove(file);
}

void Library::dosVerify(std::string file)
{
  FileSpec spec = DiskFile::getFileSpec(file);
  file = disk + "/" + spec.name;
  DiskFile::verify(file);
}

void Library::dosBLoad(std::string file, Memory& mem)
{
  FileSpec spec = DiskFile::getFileSpec(file);
  file = disk + "/" + spec.name;
  if (spec.address > 0)
  {
    if (spec.address == 0x2000)
    {
      hiresPage1 = DiskFile::readBinaryFile(file);
      os->notifyHiresLoaded();
    }
    else if (spec.address == 0x4000)
    {
      hiresPage2 = DiskFile::readBinaryFile(file);
      os->notifyHiresLoaded();
    }
  }
  else
  {
    uint32_t addr = 0;
    std::ifstream s(file);
    if (s.is_open() && !s.fail())
    {
      s.read(reinterpret_cast<char*>(&addr),sizeof(addr));
      s.close();
    }
    if (addr == 0x69)
      restoreMemory(file,mem);
  }
}

void Library::dosBSave(std::string file, Memory& mem)
{
  FileSpec spec = DiskFile::getFileSpec(file);
  file = disk + "/" + spec.name;
  if (spec.address == 0x69)
  {
    saveMemory(file,mem);
  }
}

void Library::printf(Stack& stack) const
{
  std::ostringstream s;
  printf(&s,stack);
  if (os)
  {
    os->write(s.str());
    os->flush();
  }
}

void Library::printf(std::ostream* printstream, Stack& stack) const
{
  int narg = stack.pop().getInt();
  if (narg == 0)
  {
    stack.push(0);
    return;
  }
  std::vector<VariableArgument> args;
  for (int32_t i=0;i<narg;i++)
  {
    VariableArgument a;
    a.type = Type::fromInt(static_cast<uint32_t>(stack.pop().getInt()));
    if (a.type == Type::int32Type)
      a.i32 = stack.pop().getInt();
    else if (a.type == Type::doubleType)
      a.d = stack.pop().getDouble();
    else if (a.type == Type::stringType)
      a.s = stack.pop().getString();
    args.push_back(a);
  }
  VariableArgument a = args.back();
  args.pop_back();
  if (a.type != Type::stringType)
  {
    stack.push(0);
    return;
  }
  const char* c = a.s.c_str();
  while (*c != '\0')
  {
    if (*c == '!' || *c == '&' || *c == '\\')
    {
      c = printfString(printstream,c,&args);
    }
    else if (*c == '#')
    {
      c = printfNumber(printstream,c,&args);
    }
    else
    {
      *printstream << *c++;
    }
  }
  if (!args.empty()) *printstream << "\n";
  printstream->flush();
}

const char* Library::printfString(std::ostream* printstream, const char* c, std::vector<VariableArgument>* args) const
{
  std::string s = args->back().s;
  args->pop_back();
  if (*c == '!')
  {
    if (s.empty())
      *printstream << "?";
    else
      *printstream << s;
    c++;
  }
  else if (*c == '&')
  {
    *printstream << s;
    c++;
  }
  else if (*c == '\\')
  {
    c++;
    uint32_t n = 1;
    while (*c != '\0' && *c != '\\')
    {
      n++;
      c++;
    }
    n++;
    if (s.length() > n)
    {
      *printstream << s.substr(0,n);
    }
    else
    {
      *printstream << s;
      n -= s.length();
      while (n > 0)
      {
        *printstream << " ";
        n--;
      }
    }
    c++;
  }
  return c;
}

const char* Library::printfNumber(std::ostream* printstream, const char* c, std::vector<VariableArgument>* args) const
{
  double v = args->back().getDouble();
  args->pop_back();
  bool e = false;
  int n = 0;
  int p = 0;
  while (*c == '#')
  {
    n++;
    c++;
  }
  if (*c == '.')
  {
    c++;
    while (*c == '#')
    {
      n++; p++;
      c++;
    }
  }
  const char* tmp = c;
  if (*c == '^')
  {
    int32_t i = 0;
    while (*c == '^' && i < 4)
    {
      i++;
      c++;
    }
    if (i == 4)
    {
      e = true;
      n += 4;
    }
    else
    {
      c = tmp;
    }
  }
  std::ostringstream s;
//  if (fmt.leftalign) s << std::left;
  s.width(n);
  if (p == 0 && !e)
  {
    s << static_cast<int32_t>(v);
  }
  else if (!e)
  {
    s << std::fixed;
    s.precision(p);
    s << v;
  }
  else
  {
    s << std::scientific;
    if (p > 0) s.precision(p);
    s << v;
  }
  std::string str = s.str();
//  if (!fmt.leftalign && fmt.fill != ' ') std::replace(str.begin(),str.end(),' ',fmt.fill);
  *printstream << str;
  return c;
}

void Library::input(Stack &stack) const
{
  bool prompt = stack.pop().getInt() != 0;
  int32_t narg = stack.pop().getInt();
  if (narg == 0)
  {
    stack.push(0);
    return;
  }
  std::vector<Type> types;
  for (int32_t i=0;i<narg;i++)
  {
    types.push_back(Type::fromInt(static_cast<uint32_t>(stack.pop().getInt())));
  }
  std::reverse(types.begin(),types.end());
  if (!is && !inputfile)
  {
    for (const Type& t : types)
    {
      if (t == Type::int32Type)
        stack.push(static_cast<int32_t>(0));
      else if (t == Type::doubleType)
        stack.push(0.0);
      else if (t == Type::stringType)
        stack.push(std::string(""));
    }
  }
  else
  {
    std::vector<std::string> fields;
    while (fields.size() < types.size())
    {
      std::string line;
      if (inputfile)
        line = inputfile->read();
      else
      {
        if (os)
        {
          os->write(fields.empty()?(prompt?"":"?"):"??");
          os->flush();
        }
        line = is->readLine();
      }
      auto f = split(line,',',false);
      fields.insert(fields.end(),f.begin(),f.end());
    }
    size_t index = 0;
    for (const Type& t : types)
    {
      if (t == Type::int32Type)
      {
        int32_t v = std::stoi(fields[index]);
        stack.push(v);
      }
      else if (t == Type::doubleType)
      {
        double v = std::stod(fields[index]);
        stack.push(v);
      }
      else if (t == Type::stringType)
      {
        std::string t = fields[index];
        if (!t.empty() && t[0] == '"') t = t.substr(1);
        if (!t.empty() && t[t.length()-1] == '"') t = t.substr(0,t.length()-1);
        stack.push(t);
      }
      index++;
    }
  }
  stack.push(narg);
}

void Library::get(Stack &stack) const
{
  Type type = Type::fromInt(static_cast<uint32_t>(stack.pop().getInt()));
  if (!is)
  {
    if (type == Type::int32Type)
      stack.push(static_cast<int32_t>(0));
    if (type == Type::doubleType)
      stack.push(0.0);
    if (type == Type::stringType)
      stack.push(std::string(""));
  }
  else
  {
    char c = is->readChar();
    if (type == Type::int32Type)
    {
      int32_t v = c - '0';
      stack.push(v);
    }
    if (type == Type::doubleType)
    {
      int32_t v = c - '0';
      stack.push(v);
    }
    if (type == Type::stringType)
    {
      std::string t(1,c);
      stack.push(t);
    }
  }
}

void Library::read(Stack &stack, const ConstantData *data) const
{
  Type t = Type::fromInt(static_cast<uint32_t>(stack.pop().getInt()));
  int32_t addr = stack.pop().getInt();
  std::ostringstream s;
  s << "DATA_" << addr;
  const Symbol* sym = data->findConstant(s.str());
  if (sym)
    stack.push(data->getConstant(sym->getAddress()));
  else
    stack.push(0);
}

void Library::peek(Stack &stack) const
{
  int16_t addr = static_cast<int16_t>(stack.pop().getInt());
  switch (addr)
  {
    case 36:
      stack.push(os->getCursorColumn());
      break;
    case 37:
      stack.push(os->getCursorRow());
      break;
    case 78:  /* used for seeding rnd() */
      stack.push(static_cast<int32_t>((time(nullptr)&0xFF00)>>8));
      break;
    case 79:
      stack.push(static_cast<int32_t>(time(nullptr)&0xFF));
      break;
    case 105: /* 105 + 106: start of variable space -> map to magic number 0x69 */
      stack.push(0x69);
      break;
    case 106:
      stack.push(0);
      break;
    case 111: /* 111 + 112: start of string storage -> map to magic number 0x69 */
      stack.push(0x69);
      break;
    case 112:
      stack.push(0);
      break;
    case 222:
      /* onerr code */
      stack.push(DiskFile::getErrorCode());
      break;
    case -16384:
      {
        if (is) stack.push(static_cast<int32_t>(is->getLastKey())+128);
      }
      break;
    default:
      stack.push(0);
      break;
  }
}

void Library::poke(Stack &stack)
{
  uint8_t value = static_cast<uint8_t>(stack.pop().getInt());
  uint16_t addr = static_cast<uint16_t>(stack.pop().getInt());
  switch (addr)
  {
    case 0xC050:
      /* switch to graphical display */
      os->setScreenMode(OutputStream::Graphics);
      break;
    case 0xC052:
      /* full screen */
      break;
    case 0xC054:
      /* display page 1 */
      currentHiresPage = 1;
      os->notifyHiresLoaded();
      break;
    case 0xC055:
      /* display page 2 */
      currentHiresPage = 2;
      os->notifyHiresLoaded();
      break;
    case 0xC057:
      /* HIRES */
      break;
  }
}

void Library::vtab(Stack &stack) const
{
  int32_t row = static_cast<int32_t>(stack.pop().getDouble()) - 1;
  os->gotoRow(row);
}

void Library::htab(Stack &stack) const
{
  int32_t col = static_cast<int32_t>(stack.pop().getDouble()) - 1;
  os->gotoColumn(col);
}

void Library::home() const
{
  os->home();
}

void Library::text() const
{
  os->setScreenMode(OutputStream::Text);
}

void Library::saveMemory(const std::string &filename, const Memory &mem)
{
  nlohmann::json j = mem.save();
  std::ostringstream s;
  s << j;
  std::string data = s.str();
  std::ofstream os(filename);
  if (!os.fail())
  {
    uint32_t addr = 0x69;
    os.write(reinterpret_cast<const char*>(&addr),sizeof(addr));
    uint32_t len = data.length() +1;
    os.write(reinterpret_cast<const char*>(&len),sizeof(len));
    os.write(data.c_str(),len);
    os.close();
  }
}

void Library::restoreMemory(const std::string &filename, Memory &mem)
{
  uint32_t addr;
  std::ifstream s(filename);
  if (s.is_open() && !s.fail())
  {
    s.read(reinterpret_cast<char*>(&addr),sizeof(addr));
    uint32_t len;
    s.read(reinterpret_cast<char*>(&len),sizeof(len));
    char txt[len];
    s.read(txt,len);
    s.close();
    std::istringstream is(txt);
    nlohmann::json j = nlohmann::json::parse(is);
    mem.restore(j);
  }
}





std::string Library::trim(std::string s)
{
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
      return !std::isspace(ch);
  }));
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
      return !std::isspace(ch);
  }).base(), s.end());
  return s;
}

std::vector<std::string> Library::split(const std::string& s, char delim, bool skipempty)
{
  std::vector<std::string> list;
#if 1
  size_t start = 0;
  size_t index = 0;
  bool quote = false;
  while (index < s.length())
  {
    if (s[index] == delim && !quote)
    {
      std::string field = s.substr(start,index-start);
      if (!(field.empty() && skipempty)) list.push_back(field);
      start = index + 1;
      index = start;
    }
    else
    {
      if (s[index] == '\"')
      {
        quote = !quote;
      }
      index++;
    }
  }
  if (start <= index)
  {
    std::string field = s.substr(start,index-start);
    if (!(field.empty() && skipempty)) list.push_back(field);
  }
#else
  std::istringstream iss(s);
  std::string item;
  while (std::getline(iss,item,delim))
  {
    if (!(item.empty() && skipempty)) list.push_back(item);
  }
#endif
  return list;
}

void Library::init()
{
  if (definitions.empty())
  {
    definitions.push_back(LibraryFunction(F_PRINT,"print","",Type::undefinedType));
    definitions.push_back(LibraryFunction(F_INPUT,"input","",Type::int32Type));
    definitions.push_back(LibraryFunction(F_READ,"read","",Type::int32Type));
    definitions.push_back(LibraryFunction(F_SIN,"sin","d",Type::doubleType));
    definitions.push_back(LibraryFunction(F_COS,"cos","d",Type::doubleType));
    definitions.push_back(LibraryFunction(F_TAN,"tan","d",Type::doubleType));
    definitions.push_back(LibraryFunction(F_ASIN,"asin","d",Type::doubleType));
    definitions.push_back(LibraryFunction(F_ACOS,"acos","d",Type::doubleType));
    definitions.push_back(LibraryFunction(F_ATAN,"atan","d",Type::doubleType));
    definitions.push_back(LibraryFunction(F_ATAN2,"atan2","d,d",Type::doubleType));
    definitions.push_back(LibraryFunction(F_SQRT,"sqrt","d",Type::doubleType));
    definitions.push_back(LibraryFunction(F_EXP,"exp","d",Type::doubleType));
    definitions.push_back(LibraryFunction(F_LOG,"log","d",Type::doubleType));
    definitions.push_back(LibraryFunction(F_LOG10,"log10","d",Type::doubleType));
    definitions.push_back(LibraryFunction(F_LOG2,"log2","d",Type::doubleType));
    definitions.push_back(LibraryFunction(F_ABS,"abs","d",Type::doubleType));
    definitions.push_back(LibraryFunction(F_TAB,"tab","d",Type::doubleType));
    definitions.push_back(LibraryFunction(F_SIGN,"sgn","d",Type::doubleType));
    definitions.push_back(LibraryFunction(F_RND,"rnd","d",Type::doubleType));
    definitions.push_back(LibraryFunction(F_INT,"int","d",Type::int32Type));
    definitions.push_back(LibraryFunction(F_PRINTF,"printf","",Type::undefinedType));
    definitions.push_back(LibraryFunction(F_LEFT,"left$","t,d",Type::stringType));
    definitions.push_back(LibraryFunction(F_MID,"mid$","t,d,d",Type::stringType));
    definitions.push_back(LibraryFunction(F_MID1,"mid1$","t,d",Type::stringType));
    definitions.push_back(LibraryFunction(F_RIGHT,"right$","t,d",Type::stringType));
    definitions.push_back(LibraryFunction(F_LEN,"len","t",Type::int32Type));
    definitions.push_back(LibraryFunction(F_ASC,"asc","t",Type::int32Type));
    definitions.push_back(LibraryFunction(F_CHR,"chr$","d",Type::stringType));
    definitions.push_back(LibraryFunction(F_VAL,"val","t",Type::doubleType));
    definitions.push_back(LibraryFunction(F_STR,"str$","d",Type::stringType));
    definitions.push_back(LibraryFunction(F_POW,"pow","d,d",Type::doubleType));
    definitions.push_back(LibraryFunction(F_PEEK,"peek","d",Type::doubleType));
    definitions.push_back(LibraryFunction(F_POKE,"poke","d,d",Type::undefinedType));
    definitions.push_back(LibraryFunction(F_GET,"get","",Type::int32Type));
    definitions.push_back(LibraryFunction(F_INVERSE,"inverse","",Type::undefinedType));
    definitions.push_back(LibraryFunction(F_NORMAL,"normal","",Type::undefinedType));
    definitions.push_back(LibraryFunction(F_VTAB,"vtab","",Type::undefinedType));
    definitions.push_back(LibraryFunction(F_HTAB,"htab","",Type::undefinedType));
    definitions.push_back(LibraryFunction(F_SPC,"spc","d",Type::stringType));
    definitions.push_back(LibraryFunction(F_HOME,"home","",Type::undefinedType));
    definitions.push_back(LibraryFunction(F_FLASH,"flash","",Type::undefinedType));
    definitions.push_back(LibraryFunction(F_TEXT,"text","",Type::undefinedType));
    definitions.push_back(LibraryFunction(F_FRE,"fre","d",Type::int32Type));
  }
}
