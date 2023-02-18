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

#ifndef VM_H
#define VM_H

#include "executable.h"
#include "memory.h"
#include "stack.h"
#include "type.h"
#include "library.h"
#include <map>
#include <ostream>
#include <memory>



class InputStream;
class OutputStream;

class VM
{
public:
  VM(std::shared_ptr<InputStream>& sin, std::shared_ptr<OutputStream>& sout);

  /**
   * @brief Clears the stack.
   */
  void clearStack();

  /**
   * @brief Loads the executable.
   *
   * This method loads the xecutable and resets the code pointer.
   * It also setsup the global memory. It does not clear the stack.
   * @param x the executable to load
   */
  void load(std::shared_ptr<Executable> x);

  /**
   * @brief Get whether an executable is loaded.
   * @return true if an executable is loaded
   */
  bool isExecutableLoaded() const;

  /**
   * @brief Runs apreviously loaded executable.
   */
  void run();

  /**
   * @brief Convenience function to load and run an executable
   * @param x the executable to load and run
   */
  void run(std::shared_ptr<Executable> x);

  /**
   * @brief Pause the execution loop.
   */
  void pause();

  /**
   * @brief Check if the virtual machine is only paused.
   * @return
   */
  bool isPaused() const;

  /**
   * @brief Resume the execution loop
   */
  void resume();

  Value getValue(Symbol::SymbolType type, const std::string& name, int32_t index=0);

//  void push(const Value& value);

//  void push(const std::vector<Value>& values);

//  Value pop();

//  /**
//   * @brief Pops an array of values from the stack.
//   *
//   * The elements in the returned vector are in the correct order of the array.
//   * @return array of values
//   */
//  std::vector<Value> popArray();

  void setSlowdown(uint32_t microseconds);

  uint32_t getSlowdown() const;

  void setDisk(const std::string& d);

  const std::vector<uint8_t>& getHiresPage() const;

  const std::string& getChainedFile();

private:
  void setupGlobal(uint32_t numSize);
  uint32_t getVariableAddress(const std::string& name);
  void pushArray(uint32_t addr, Type type);
  void loop(int depth=0);
  void opPush(uint32_t op);
  void opPop();
  void opAri(uint32_t op);
  void opCmp(uint32_t op);
  void opBit(uint32_t op);
  void opLogic(uint32_t op);
  void opNot(uint32_t op);
  void opNeg(uint32_t op);
  void opClr(uint32_t op);
  void opRsz(uint32_t op);
  void opStore(uint32_t op, bool indexed);
  void opStoreG(int32_t addr, Type t, bool indexed);
  void opStoreArrayG(int32_t addr, Type t);
  void opRecall(uint32_t op, bool indexed);
  void opRecallG(int32_t addr, Type t1, bool indexed);
  void opRecallC(uint32_t addr, Type t1, bool indexed);
  void opDec();
  void opInc();
  void opCall();
  void opDup();
  void opSwap();
  void opCast(uint32_t op);

  std::shared_ptr<Executable> executable;
  Stack stack;
  const uint32_t* cptr;
  uint32_t currentLine;
  bool requestPause;
  Memory mem;
  std::unique_ptr<Library> library;
  std::shared_ptr<OutputStream> os;
  std::shared_ptr<InputStream> is;
  uint32_t errorHandler;
  uint32_t slowdown; //!< number of microseconds to sleep after each instruction
};



#endif // VM_H
