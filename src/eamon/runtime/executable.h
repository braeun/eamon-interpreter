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

#ifndef EXECUTABLE_H
#define EXECUTABLE_H

#include "constant.h"
#include "memory.h"
#include "symbol.h"
#include "value.h"
#include <stdint.h>
#include <string>
#include <vector>




class Assembler;

class Executable: public ConstantData
{
public:
  Executable();
  ~Executable();

  /**
   * @brief Returns a pointer to the start of the code area.
   * @return pointer to the code
   */
  const uint32_t* getCode();

  /**
   * @brief Returns the length of the code in number of uint32_t values.
   * @return length of the code
   */
  uint32_t getCodeLength();

  /**
   * @brief Returns a constant at the given constant address plus index.
   * @param addr the address of the constant
   * @param index the index of the value in the constant.
   * @return value
   */
  virtual Value getConstant(uint32_t addr, int32_t index=0) const override;

  /**
   * @brief Returns a all values belonging to the constant at the given address.
   * @param addr the address of the constant
   * @return list of values
   */
  virtual std::vector<Value> getConstantArray(uint32_t addr) const override;

  /**
   * @brief Return a pointer to the symbol structure for a constant of the given name.
   * If the constant is not found, a nullptr will be returned.
   * @param name name of the symbol
   * @return pointer to the symbol or nullptr
   */
  virtual const Symbol* findConstant(const std::string& name) const override;

  /**
   * @brief Returns the pointer to the vtable.
   * The vtable maps the index of the function (index in the table) to the
   * address in the code (value at the index).
   * @return pointer to the vtable
   */
  const int32_t* getVTable();

  /**
   * @brief Return a pointer to the symbol structure for a symbol of the given name.
   * If the symbol of the given type is not found, a nullptr will be returned.
   * @param name name of the symbol
   * @param type type of the symbol
   * @return pointer to the symbol or nullptr
   */
  const Symbol* findSymbol(const std::string& name, Symbol::SymbolType type) const;

  std::vector<Symbol> getSymbolTable(Symbol::SymbolType type) const;

  /**
   * @brief Saves the executable data to file.
   * @param filename the filename
   * @return true on success
   */
  bool save(const std::string& filename);

  /**
   * @brief Saves the executable data to the output stream
   * @param os the output stream
   * @return true on success
   */
  bool save(std::ostream& os);

  /**
   * @brief Saves the executable data to a memory buffer.
   * This method will allocate the buffer. It is the responsibility of the
   * caller to free the buffer - with a call to free() - when no longer used.
   * @param buffer pointer to the pointer of the new buffer
   * @param size the buffer size
   * @return  true on success
   */
  bool save(char** buffer, uint32_t* size);

  /**
   * @brief Loads the executable data from file.
   * @param filename the filename
   * @return true on success
   */
  bool load(const std::string& filename);

  /**
   * @brief Loads the executable data from the input stream.
   * @param is the input stream
   * @return true on success
   */
  bool load(std::istream& is);

  /**
   * @brief Loads the executable data from a buffer.
   * THe passed buffer is copied into a new internal buffer. The caller may
   * delete the buffer after the method returns.
   * @param buffer the buffer with the data
   * @param size the buffer size
   * @return true on success
   */
  bool load(char* buffer, uint32_t size);

protected:
  friend class Assembler;

  Executable(uint32_t codelength, uint32_t textlength, uint32_t vtablelength, uint32_t fsymlength, uint32_t csymlength, uint32_t vsymlength);

  void setCodeSegment(const uint32_t* code);

  void setTextSegment(const char* text);

  void setVTable(std::vector<int32_t> table);

  void setFunctionSymbolTable(std::vector<Symbol> table);

  void setConstantSymbolTable(std::vector<Symbol> table);

  void setVariableSymbolTable(std::vector<Symbol> table);

private:
  void setupTables();
  void buildConstantValueTable();


  char* buffer; /* buffer containing everything as one chunk */
  uint32_t buffersize;
  uint32_t* code;
  uint32_t codelength;
  char* text;
  uint32_t textlength;
  int32_t* vtable;
  uint32_t vtablelength;
  Symbol* globalVarSymbolTable;
  uint32_t globalVarSymbolTableLength;
  Symbol* functionSymbolTable;
  uint32_t functionSymbolTableLength;
  Symbol* constantSymbolTable;
  uint32_t constantSymbolTableLength;
  std::vector<std::vector<Value>> constantValues;
};



#endif // EXECUTABLE_H
