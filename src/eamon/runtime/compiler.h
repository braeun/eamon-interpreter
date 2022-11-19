/********************************************************************************
 *                                                                              *
 * EamonInterpreter - compiler                                                  *
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

#ifndef COMPILER_H
#define COMPILER_H

#include "compilerdata.h"
#include "errors.h"
#include "function.h"
#include "scanner.h"
#include "parser.h"
#include "type.h"
#include <string>
#include <cstddef>
#include <istream>
#include <memory>
#include <set>

class Assembler;
class Executable;

/**
 * @brief The Compiler class
 *
 * This class creates and controls the flex/bison generated scanner and parser.
 */
class Compiler
{
public:
  Compiler();
  ~Compiler();

  /**
   * @brief Parse from a file
   * @param filename - valid string with input file
   * @return the executable
   */
  Executable* compile(const std::string& filename);

  /**
   * @brief Compile a script.
   * @param src the script source as a vector of text lines
   * @return the executable
   */
  Executable* compile(const std::vector<std::string>& src);

  /**
   * @brief Parse from a c++ input stream
   * @param is - std::istream&, valid input stream
   * @return the executable
   */
  Executable* compile(std::istream& src);

  /**
   * @brief Get all compiler errors and warnings.
   * @return the compiler errors and warnings
   */
  const Errors& getErrors() const;

  void reset();

  void createLabel(int lineno, const yy::Parser::location_type &l);

  void createDimVar(std::string var, int ndim, const yy::Parser::location_type &l);

//  void createDimVar(std::string var, int dim1, int dim2, Type type, const yy::Parser::location_type &l);

  void createEnd();

  void createGoto(int lineno);

  void createOnErrorGoto(int lineno);

  void createGosub(int lineno);

  void createReturn();

  void createPop();

  void createPush(int value);

  void createPush(double value);

  void createPush(const std::string& s);

  void createOperator(const std::string& op, const yy::Parser::location_type &l);

  void createNegate();

  void createNot();

  void createArrayOffset(std::string var, int32_t ndim, const yy::Parser::location_type &l);

  void createInputArrayOffset(int32_t ndim, const yy::Parser::location_type &l);

  /**
   * @brief store
   * @param var
   * @param type
   * @param l
   * @param swap false if index to array  variable is already on top of stack
   */
  void store(std::string var, const yy::Parser::location_type &l, bool swap=true);

  void recall(std::string var, const yy::Parser::location_type &l);

  void callPrint(bool nl);

  void callPrintF(bool nl);

  void callPrintTab();

  void startInput();

  void addInput(std::string var, Type type);

  void addedInput();

  void endInput();

  void startGet();

  void endGet();

  void startFor(std::string v, const yy::Parser::location_type &l);

  void compareFor(const yy::Parser::location_type &l);

  void stepFor(const yy::Parser::location_type &l);

  void endFor(std::string v, const yy::Parser::location_type &l=yy::Parser::location_type());

  void startIf(const yy::Parser::location_type &l);

  void elseIf(const yy::Parser::location_type &l);

  void endIf(const yy::Parser::location_type &l);

  bool isIf();

  void startOnGoto(const yy::Parser::location_type &l);

  void addOnGoto(int lineno, const yy::Parser::location_type &l);

  void addOnGosub(int lineno, const yy::Parser::location_type &l);

  void endOnGoto(const yy::Parser::location_type &l);

  void restore();

  void createData(double v);

  void createData(const std::string& v);

  void read(std::string var, Type type);

  void callFunction(const std::string& name, int npar, const yy::Parser::location_type &l);

  void startUserFunction(const std::string& name, std::string& var, const yy::Parser::location_type &l);

  void endUserFunction(const yy::Parser::location_type &l);

  void addError(const yy::Parser::location_type &l, const std::string &err_message);

private:
  struct ForLoopData
  {
    std::string var;
//    std::string compareVar;
//    Code* uplevelCode;
//    std::vector<Type>* uplevelTypeStack;
//    Code compareCode;
//    std::vector<Type> compareTypeStack;
//    Code stepCode;
//    std::vector<Type> stepTypeStack;
    int32_t label;
  };
  struct IfData
  {
    int32_t falseLabel;
    int32_t endLabel;
  };
  struct InputData
  {
    std::string var;
    Type type;
    Code* uplevelCode;
    std::vector<Type>* uplevelTypeStack;
    /* store the code to create array index */
    Code code;
    std::vector<Type> typeStack;
  };
  struct UserFunction
  {
    std::string name;
    Function f;
    Code* uplevelCode;
    std::vector<Type>* uplevelTypeStack;
    std::vector<Type> typeStack;
  };

  void checkLine(const yy::Parser::location_type &l);
  Executable* compile_helper(std::istream &stream);
  const Variable* findAndCreateVar(std::string name, bool normalize=true);
  Type callFunction(const std::string& fn, const yy::Parser::location_type &l);
  std::string normalizeVar(std::string var);
  void store(const Variable* var, const yy::Parser::location_type &l, bool swap=true);
  void recall(const Variable* var, const yy::Parser::location_type &l);
  Type getType(const std::string& var);


  CompilerData data;
  /* List of errors */
  Errors errors;
  Scanner* scanner;
  yy::Parser* parser;
  int32_t currentLine;
  Assembler* assembler;
  Code* code;
  Code initcode;
  std::vector<Type>* typeStack;
  std::vector<Type> toplevelTypeStack;
  std::vector<ForLoopData> forLoop;
  std::vector<IfData> ifData;
  std::vector<InputData> inputData;
  std::unique_ptr<UserFunction> userFunction;
  std::set<int32_t> labels;
  int32_t printCount;
  int32_t internalLabelCounter;
  int32_t onGoLabel;
  int32_t onGoIndex;
  int32_t dataCounter;

  static const char* readIndexVarName;
};


#endif // COMPILER_H
