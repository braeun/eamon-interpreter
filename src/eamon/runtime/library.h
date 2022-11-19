/********************************************************************************
 *                                                                              *
 * EamonInterpreter - VM library                                                *
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

#ifndef SYSTEMLIBRARY_H
#define SYSTEMLIBRARY_H

#include "diskfile.h"
#include "type.h"
#include "value.h"
#include <string>
#include <memory>
#include <vector>

class Memory;
class Stack;
class ConstantData;
class InputStream;
class OutputStream;


class VariableArgument {
public:
  Type type = Type::undefinedType;
  int32_t i32 = 0;
  double d = 0;
  std::string s = "";

  int32_t getInt() const;

  int64_t getI64() const;

  double getDouble() const;

};

class LibraryFunction {
public:
  LibraryFunction();
  LibraryFunction(const LibraryFunction& f);
  LibraryFunction(uint16_t id, const std::string& name, const std::string& args, Type rettype);

  const uint16_t id;
  const std::string name;
  const std::string args;
  const Type rettype;
};

class Library
{
public:
  Library(std::shared_ptr<InputStream>& sin, std::shared_ptr<OutputStream>& sout);
  virtual ~Library();

  virtual uint16_t getId() const;

  static const LibraryFunction& findFunction(const std::string& name);

  static const std::vector<LibraryFunction>& getFunctions();

  virtual void execute(uint16_t id, Memory& mem, Stack& stack, const ConstantData* data);

  void reset();

  void setDisk(const std::string& path);

  const std::string& getChainedFile();

  const std::vector<uint8_t>& getHiresPage() const;

  static const uint16_t ID;

  virtual bool isTerminateRequested() const;

protected:
  virtual void requestTerminate(bool flag);

private:
  void left(Stack& stack) const;
  void mid(Stack& stack) const;
  void mid1(Stack& stack) const;
  void right(Stack& stack) const;
  void chr(Stack& stack) const;
  void str(Stack& stack) const;
  void print(Stack& stack, Memory& mem);
  void print(std::ostream* printstream, Stack& stack, Memory& mem);
  void print(std::ostream* printstream, const std::vector<VariableArgument>& args);
  void printInverse();
  void printNormal();
  void printExecute(Memory& mem);
  void dosRun(std::string file);
  void dosOpen(std::string file);
  void dosClose(std::string file);
  void dosRead(std::string file);
  void dosWrite(std::string file);
  void dosDelete(std::string file);
  void dosVerify(std::string file);
  void dosBLoad(std::string file, Memory& mem);
  void dosBSave(std::string file, Memory& mem);
  void printf(Stack& stack) const;
  void printf(std::ostream* printstream, Stack& stack) const;
  const char* printfString(std::ostream* printstream, const char* s, std::vector<VariableArgument>* args) const;
  const char* printfNumber(std::ostream* printstream, const char* s, std::vector<VariableArgument>* args) const;
  void input(Stack& stack) const;
  void read(Stack& stack, const ConstantData* data) const;
  void get(Stack& stack) const;
  void peek(Stack& stack) const;
  void poke(Stack& stack);
  void vtab(Stack& stack) const;
  void htab(Stack& stack) const;
  void home() const;
  void text() const;
  void saveMemory(const std::string& filename, const Memory& mem);
  void restoreMemory(const std::string& filename, Memory& mem);

  static std::string trim(std::string s);
  static std::vector<std::string> split(const std::string& s, char delim, bool skipempty);
  static void init();

  bool terminate;
  double lastRnd;
  bool cmdMode;
  std::string disk;
  std::string chain;
  std::string doscmd;
  std::vector<DiskFile> files;
  DiskFile* inputfile;
  DiskFile* outputfile;
  int currentHiresPage;
  std::vector<uint8_t> hiresPage1;
  std::vector<uint8_t> hiresPage2;
  std::shared_ptr<OutputStream> os;
  std::shared_ptr<InputStream> is;

  static LibraryFunction UNDEFINED;

  static std::vector<LibraryFunction> definitions;
};

#endif // SYSTEMLIBRARY_H
