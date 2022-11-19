/********************************************************************************
 *                                                                              *
 * EamonInterpreter - compiler                                                  *
 *                                                                              *
 * modified: 2022-11-15                                                         *
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

#include "compiler.h"
#include "library.h"
#include "assembler.h"
#include "executable.h"
#include <cctype>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

#define START_INTERNAL_LABEL_COUNTER 0x10000

const char* Compiler::readIndexVarName = "__readIndex%";

Compiler::Compiler():
  scanner(nullptr),
  parser(nullptr),
  currentLine(-1),
  printCount(0),
  internalLabelCounter(START_INTERNAL_LABEL_COUNTER),
  onGoLabel(0),
  onGoIndex(0),
  dataCounter(0)
{
  assembler = new Assembler(data);
}

Compiler::~Compiler()
{
  if (scanner) delete scanner;
  scanner = nullptr;
  if (parser) delete parser;
  parser = nullptr;
}

Executable* Compiler::compile(const std::string& filename)
{
  std::ifstream in_file(filename);
  if(!in_file.good())
  {
     exit(EXIT_FAILURE);
  }
  return compile_helper(in_file);
}

Executable* Compiler::compile(const std::vector<std::string>& src)
{
  std::string s = "";
  for (const std::string& line : src) s += line + "\n";
  std::istringstream stream(s);
  if(!stream.good() && stream.eof())
  {
   return nullptr;
  }
  //else
  return compile_helper(stream);
}

Executable* Compiler::compile(std::istream& src)
{
  return compile_helper(src);
}

const Errors& Compiler::getErrors() const
{
  return errors;
}

void Compiler::reset()
{
  errors.clear();
}


void Compiler::createLabel(int lineno, const yy::Parser::location_type &l)
{
  if (lineno < START_INTERNAL_LABEL_COUNTER)
  {
    COp line(ASM_LINE);
    line.setParameter(static_cast<int32_t>(lineno));
    code->push_back(line);
  }
  auto res = labels.insert(lineno);
  if (!res.second)
  {
    if (lineno >= START_INTERNAL_LABEL_COUNTER)
      throw yy::Parser::syntax_error(l,"Duplicate internal label.");
    else
    {
      std::ostringstream os;
      os << "Duplicate line number: " << lineno;
      throw yy::Parser::syntax_error(l,os.str());
    }
  }
  COp cop(OP_NOP);
  cop.setLabel(lineno);
  code->push_back(cop);
}

void Compiler::createDimVar(std::string name, int ndim, const yy::Parser::location_type &l)
{
//  std::cout << name << " " << std::endl;
  name = normalizeVar(name);// + "_";
  const Variable* v = data.globalVariables.findVariable(name);
  if (!v)
  {
    Variable nv(name,getType(name).getArrayType());
    data.globalVariables.addVariable(nv);
  }
  v = data.globalVariables.findVariable(name+"dim1");
  if (!v)
  {
    Variable vdim1(name+"dim1",Type::int32Type);
    data.globalVariables.addVariable(vdim1);
  }
  if (ndim == 2)
  {
    v = data.globalVariables.findVariable(name+"dim2");
    if (!v)
    {
      Variable vdim2(name+"dim2",Type::int32Type);
      data.globalVariables.addVariable(vdim2);
    }
  }
  /* the value in the dim statement gives the last valid index:
   * add 1 to get # of elements */
  if (ndim == 2)
  {
    createPush(1);
    code->push_back(COp(OP_ARIADD));
    code->push_back(COp(OP_DUP,Type::int32Type));
    const Variable* v = data.globalVariables.findVariable(name+"dim2");
    COp cop(OP_STO,v->getType());
    cop.setParameter(static_cast<int32_t>(v->getAddress()));
    code->push_back(cop);
    code->push_back(COp(OP_SWAP,Type::int32Type));
  }
  createPush(1);
  code->push_back(COp(OP_ARIADD));
  code->push_back(COp(OP_DUP,Type::int32Type));
  v = data.globalVariables.findVariable(name+"dim1");
  COp cop(OP_STO,v->getType());
  cop.setParameter(static_cast<int32_t>(v->getAddress()));
  code->push_back(cop);
  if (ndim == 2)
  {
    code->push_back(COp(OP_ARIMUL));
  }
  v = data.globalVariables.findVariable(name);
  cop = COp(OP_RSZ,v->getType().getScalarType());
  cop.setParameter(static_cast<int32_t>(v->getAddress()));
  code->push_back(cop);
}

//void Compiler::createDimVar(std::string name, int dim1, int dim2, Type type, const yy::Parser::location_type &l)
//{
//  name = normalizeVar(name);
//  const Variable* v = data.globalVariables.findVariable(name+"_");
//  if (v != nullptr)
//  {
////    errors.addError(l.end.line,"Cannot change dimension on variable '"+name+"' because it is already used.");
//    throw yy::Parser::syntax_error(l,"Cannot change dimension on variable '"+name+"' because it is already used.");
//    return;
//  }
//  Variable nv(name+"_",type);
//  std::vector<uint32_t> dims{static_cast<uint32_t>(dim1+1),static_cast<uint32_t>(dim2+1)};
//  nv.setDims(dims);
//  data.globalVariables.addVariable(nv);
//}


void Compiler::createEnd()
{
  COp cop(OP_END);
  code->push_back(cop);
}

void Compiler::createGoto(int lineno)
{
  COp cop(OP_JUMP);
  cop.setParameter(lineno);
  code->push_back(cop);
}

void Compiler::createOnErrorGoto(int lineno)
{
  COp cop(OP_ERRHDL);
  cop.setParameter(lineno);
  code->push_back(cop);
}

void Compiler::createGosub(int lineno)
{
  COp cop(OP_JSR);
  cop.setParameter(lineno);
  code->push_back(cop);
}

void Compiler::createReturn()
{
  COp cop(OP_RET);
  code->push_back(cop);
}

void Compiler::createPop()
{
  COp cop(OP_POP);
  code->push_back(cop);
}

void Compiler::createPush(int value)
{
  COp cop(OP_PUSH,Type::int32Type);
  cop.setParameter(value);
  code->push_back(cop);
  typeStack->push_back(cop.getParameterType());
}

void Compiler::createPush(double value)
{
  COp cop(OP_PUSH,Type::doubleType);
  cop.setParameter(value);
  code->push_back(cop);
  typeStack->push_back(cop.getParameterType());
}

void Compiler::createPush(const std::string& s)
{
  uint32_t index = data.constants.addConstant(s);
  COp cop(OP_PUSH,Type::stringType);
  cop.setParameter(static_cast<int32_t>(index));
  code->push_back(cop);
  typeStack->push_back(Type::stringType);
}

void Compiler::createOperator(const std::string& op, const yy::Parser::location_type &l)
{
  if (typeStack->size() < 2)
  {
    throw yy::Parser::syntax_error(l,"Stack underflow in arithmetic/logical operation");
  }
  Type t2 = typeStack->back();
  typeStack->pop_back();
  Type t1 = typeStack->back();
  typeStack->pop_back();
  Type t = Type::resultType(t1,t2);
  COp cop;
  if (op == "+")
    code->push_back(COp(OP_ARIADD));
  else if (op == "-")
    code->push_back(COp(OP_ARISUB));
  else if (op == "*")
    code->push_back(COp(OP_ARIMUL));
  else if (op == "/")
    code->push_back(COp(OP_ARIDIV));
  else if (op == "=")
  {
    code->push_back(COp(OP_ARIEQ));
    t = Type::int32Type;
  }
  else if (op == "<>")
  {
    code->push_back(COp(OP_ARINE));
    t = Type::int32Type;
  }
  else if (op == ">")
  {
    code->push_back(COp(OP_ARIGT));
    t = Type::int32Type;
  }
  else if (op == "<")
  {
    code->push_back(COp(OP_ARILT));
    t = Type::int32Type;
  }
  else if (op == ">=")
  {
    code->push_back(COp(OP_ARIGE));
    t = Type::int32Type;
  }
  else if (op == "<=")
  {
    code->push_back(COp(OP_ARILE));
    t = Type::int32Type;
  }
  else if (op == "&&")
  {
    code->push_back(COp(OP_AND));
    t = Type::int32Type;
  }
  else if (op == "||")
  {
    code->push_back(COp(OP_OR));
    t = Type::int32Type;
  }
  typeStack->push_back(t);
}

void Compiler::createNegate()
{
  if (typeStack->back().isNumericType())
  {
    COp cop(OP_NEG);
    code->push_back(cop);
  }
  else
  {
//    errors.addError(t.getLine(),"Unary minus defined only for numbers.");
  }
}

void Compiler::createNot()
{
  if (typeStack->back().isNumericType())
  {
    if (typeStack->back() == Type::doubleType)
    {
      code->push_back(COp(OP_CAST,Type::int32Type));
    }
    COp cop(OP_ARINOT);
    code->push_back(cop);
  }
  else
  {
//    errors.addError(t.getLine(),"Unary minus defined only for numbers.");
  }
}

void Compiler::createArrayOffset(std::string name, int32_t ndim, const yy::Parser::location_type &l)
{
  name = normalizeVar(name);
  std::string var = name;
//  /* we have to distinguish array type variables from scalar variables of same name */
//  if (type.isArrayType()) var += "_";
  const Variable* v = nullptr;
//  if (userFunction) v = userFunction->f.localvars.findVariable(var);
  if (v == nullptr) v = data.globalVariables.findVariable(var);
  if (v == nullptr)
  {
    Variable nv(var,getType(var).getArrayType());
    std::vector<uint32_t> dims{static_cast<uint32_t>(11)};
    if (ndim > 1) dims.push_back(11);
//    nv.setDims(dims);
    data.globalVariables.addVariable(nv);
    Variable vdim1(var+"dim1",Type::int32Type);
    data.globalVariables.addVariable(vdim1);
    int32_t size = 11;
    if (ndim == 2)
    {
      Variable vdim2(var+"dim2",Type::int32Type);
      data.globalVariables.addVariable(vdim2);
      size *= 11;
    }
    v = data.globalVariables.findVariable(var);
    COp cop(OP_PUSH,Type::int32Type);
    cop.setParameter(size);
    initcode.push_back(cop);
    cop = COp(OP_RSZ,v->getType().getScalarType());
    cop.setParameter(static_cast<int32_t>(v->getAddress()));
    initcode.push_back(cop);
  }
  else if (!v->getType().isArrayType())
  {
//    parser->error(yy::Parser::syntax_error(l,"Variable '"+name+"' is already used as a scalar variable"));
    throw yy::Parser::syntax_error(l,"Variable '"+name+"' is already used as a scalar variable");
    return;
  }
  if (ndim == 2)
  {
    v = data.globalVariables.findVariable(var+"dim1"); /* column major memory */
    COp cop(OP_RCL,v->getType());
    cop.setParameter(static_cast<int32_t>(v->getAddress()));
    code->push_back(cop);
    typeStack->push_back(v->getType());
    createOperator("*",l);
    createOperator("+",l);
  }
  Type t1 = typeStack->back();
  if (typeStack->back() != Type::int32Type)
  {
    code->push_back(COp(OP_CAST,Type::int32Type));
    typeStack->back() = Type::int32Type;
  }
}

void Compiler::createInputArrayOffset(int32_t ndim, const yy::Parser::location_type &l)
{
  InputData& id = inputData.back();
  id.type = id.type.getArrayType();
  createArrayOffset(id.var,ndim,l);
}

void Compiler::store(std::string var, const yy::Parser::location_type &l, bool swap)
{
  const Variable* v = findAndCreateVar(var);
  store(v,l,swap);
}

void Compiler::store(const Variable *v, const yy::Parser::location_type &l, bool swap)
{
  if (v->getType().isArrayType())
  {
    if (typeStack->size() < 2)
    {
      throw yy::Parser::syntax_error(l,"Stack underflow in store: missing array index?");
    }
    Type t = typeStack->back();
    if (swap) /* data on top of index */
    {
      code->push_back(COp(OP_SWAP,t)); /* bring index to top */
      typeStack->pop_back();
      typeStack->pop_back();
    }
    else /* index already on top */
    {
      typeStack->pop_back();
      t = typeStack->back(); /* type of actual data to store */
      typeStack->pop_back();
    }
    COp cop(OP_STOI,v->getType().getScalarType());
    cop.setParameter(static_cast<int32_t>(v->getAddress()));
    code->push_back(cop);
  }
  else
  {
    if (typeStack->empty())
    {
      throw yy::Parser::syntax_error(l,"Stack underflow in store");
    }
    Type t = typeStack->back();
    typeStack->pop_back();
    COp cop(OP_STO,v->getType());
    cop.setParameter(static_cast<int32_t>(v->getAddress()));
    code->push_back(cop);
  }
}

void Compiler::recall(std::string var, const yy::Parser::location_type &l)
{
  const Variable* v = findAndCreateVar(var);
  recall(v,l);
}

void Compiler::recall(const Variable *v, const yy::Parser::location_type &l)
{
  if (v->getType().isArrayType())
  {
    if (typeStack->empty())
    {
      throw yy::Parser::syntax_error(l,"Stack underflow in recall: missing array index?");
    }
    typeStack->pop_back();
    COp cop(OP_RCLI,v->getType().getScalarType());
    cop.setParameter(static_cast<int32_t>(v->getAddress()));
    code->push_back(cop);
    typeStack->push_back(v->getType().getScalarType());
  }
  else
  {
    COp cop(OP_RCL,v->getType());
    cop.setParameter(static_cast<int32_t>(v->getAddress()));
    code->push_back(cop);
    typeStack->push_back(v->getType());
  }
}

void Compiler::callPrint(bool nl)
{
  Type t = typeStack->back();
  typeStack->pop_back();
  COp cop(OP_PUSH,Type::int32Type);
  cop.setParameter(static_cast<int32_t>(t.toInt()));
  code->push_back(cop);
  cop = COp(OP_PUSH,Type::int32Type);
  cop.setParameter(1);
  code->push_back(cop);
  t = callFunction("print",yy::Parser::location_type());
}

void Compiler::callPrintTab()
{
  typeStack->pop_back();
  callFunction("tab",yy::Parser::location_type());
}

void Compiler::callPrintF(bool nl)
{
  if (nl)
  {
    COp cop;
    Type t = typeStack->back();
    typeStack->pop_back();
    cop = COp(OP_PUSH,Type::int32Type);
    cop.setParameter(static_cast<int32_t>(t.toInt()));
    code->push_back(cop);
    printCount++;
    cop = COp(OP_PUSH,Type::int32Type);
    cop.setParameter(printCount);
    code->push_back(cop);
    t = callFunction("printf",yy::Parser::location_type());
    printCount = 0;
  }
  else
  {
    Type t = typeStack->back();
    typeStack->pop_back();
    COp cop(OP_PUSH,Type::int32Type);
    cop.setParameter(static_cast<int32_t>(t.toInt()));
    code->push_back(cop);
    printCount++;
  }
}

void Compiler::startInput()
{
//  createPush("? ");
//  callPrint(false);
  inputData.clear();
}

void Compiler::addInput(std::string var, Type type)
{
  var = normalizeVar(var);
  InputData id;
  id.var = var;
  id.type = type;
  inputData.push_back(id);
  inputData.back().uplevelCode = code;
  inputData.back().uplevelTypeStack = typeStack;
  code = &inputData.back().code;
  typeStack = &inputData.back().typeStack;
}

void Compiler::addedInput()
{
  code = inputData.back().uplevelCode;
  typeStack = inputData.back().uplevelTypeStack;
}

void Compiler::endInput()
{
  for (const InputData& id : inputData)
  {
    COp cop(OP_PUSH,Type::int32Type);
    cop.setParameter(static_cast<int32_t>(id.type.getScalarType().toInt()));
    code->push_back(cop);
  }
  COp cop = COp(OP_PUSH,Type::int32Type);
  cop.setParameter(static_cast<int32_t>(inputData.size()));
  code->push_back(cop);
  Type t = callFunction("input",yy::Parser::location_type());
  code->push_back(COp(OP_POP,t)); /* discard return value */
  std::reverse(inputData.begin(),inputData.end());
  for (const InputData& id : inputData)
  {
    typeStack->push_back(id.type.getScalarType());
    if (id.type.isArrayType())
    {
      code->insert(code->end(),id.code.begin(),id.code.end());
      typeStack->push_back(id.typeStack.back());
    }
    store(id.var,yy::Parser::location_type(),false);
  }
  inputData.clear();
}

void Compiler::startGet()
{
  inputData.clear();
}

void Compiler::endGet()
{
  const InputData& id = inputData.front();
  COp cop(OP_PUSH,Type::int32Type);
  cop.setParameter(static_cast<int32_t>(id.type.getScalarType().toInt()));
  code->push_back(cop);
  Type t = callFunction("get",yy::Parser::location_type());
  typeStack->push_back(id.type.getScalarType());
  if (id.type.isArrayType())
  {
    code->insert(code->end(),id.code.begin(),id.code.end());
    typeStack->push_back(id.typeStack.back());
  }
  store(id.var,yy::Parser::location_type(),false);
  inputData.clear();
}

void Compiler::startFor(std::string var, const yy::Parser::location_type &l)
{
  var = normalizeVar(var);
  Compiler::ForLoopData fd;
  fd.var = var;
  fd.label = ++internalLabelCounter;
  forLoop.push_back(fd);
}

void Compiler::compareFor(const yy::Parser::location_type &l)
{
}

void Compiler::stepFor(const yy::Parser::location_type &l)
{
  const Variable* v = findAndCreateVar(forLoop.back().var);
  forLoop.pop_back();
  COp op(OP_FOR);
  op.setParameter(static_cast<int32_t>(v->getAddress()));
  code->push_back(op);
}

void Compiler::endFor(std::string v, const yy::Parser::location_type &l)
{
  COp op(OP_NEXT);
  code->push_back(op);
}

void Compiler::startIf(const yy::Parser::location_type &l)
{
  IfData id;
  id.falseLabel = ++internalLabelCounter;
  id.endLabel = ++internalLabelCounter;
  if (typeStack->back() != Type::int32Type)
  {
    Type t = typeStack->back();
    typeStack->pop_back();
    code->push_back(COp(OP_CAST,Type::int32Type));
    typeStack->push_back(Type::int32Type);
  }
  COp cop(OP_JZ);
  cop.setParameter(id.falseLabel);
  code->push_back(cop);
  ifData.push_back(id);
}

void Compiler::elseIf(const yy::Parser::location_type &l)
{
  IfData& id = ifData.back();
  /* true part must jump over else part */
  COp cop(OP_JUMP);
  cop.setParameter(id.endLabel);
  code->push_back(cop);
  /* entry for the else part */
  createLabel(id.falseLabel,l);
  id.falseLabel = 0; /* mark false label as used */
}

void Compiler::endIf(const yy::Parser::location_type &l)
{
  IfData id = ifData.back();
  createLabel(id.falseLabel==0?id.endLabel:id.falseLabel,l);
  ifData.pop_back();
}

bool Compiler::isIf()
{
  return !ifData.empty();
}

void Compiler::startOnGoto(const yy::Parser::location_type &l)
{
  onGoLabel = ++internalLabelCounter;
  onGoIndex = 1;
  /* cast to int for calculated goto/gosub */
  Type t = typeStack->back();
  typeStack->pop_back();
  code->push_back(COp(OP_CAST,Type::int32Type));
  typeStack->push_back(Type::int32Type);
}

void Compiler::addOnGoto(int lineno, const yy::Parser::location_type &l)
{
  Type t = typeStack->back();
  code->push_back(COp(OP_DUP,t));
  typeStack->push_back(t);
  createPush(onGoIndex++);
  createOperator("=",yy::Parser::location_type());
  int32_t lbl = ++internalLabelCounter;
  COp cop(OP_JZ); /* not equal -> jump to next check */
  cop.setParameter(lbl);
  code->push_back(cop);
  code->push_back(COp(OP_POP,t)); /* remove original jump index */
  createGoto(lineno); /* jump to label */
  createLabel(lbl,l);


}

void Compiler::addOnGosub(int lineno, const yy::Parser::location_type &l)
{
  Type t = typeStack->back();
  code->push_back(COp(OP_DUP,t));
  typeStack->push_back(t);
  createPush(onGoIndex++);
  createOperator("=",yy::Parser::location_type());
  int32_t lbl = ++internalLabelCounter;
  COp cop(OP_JZ); /* not equal -> jump to next check */
  cop.setParameter(lbl);
  code->push_back(cop);
  code->push_back(COp(OP_POP,t)); /* remove original jump index */
  createGosub(lineno); /* jump subroutine */
  createGoto(onGoLabel); /* return from subroutine -> jump to eng of on...gosub */
  createLabel(lbl,l);
}

void Compiler::endOnGoto(const yy::Parser::location_type &l)
{
  Type t = typeStack->back();
  code->push_back(COp(OP_POP,t)); /* remove original jump index */
  typeStack->pop_back();               /* also from the type stack */
  createLabel(onGoLabel,l);
}

void Compiler::restore()
{
  createPush(0);
  const Variable* readIndexVar = data.globalVariables.findVariable(readIndexVarName);
  store(readIndexVar,yy::Parser::location_type());
}

void Compiler::createData(double v)
{
  std::ostringstream s;
  s << "DATA_" << dataCounter;
  data.constants.addConstant(Constant(s.str(),v));
  dataCounter++;
}

void Compiler::createData(const std::string &v)
{
  std::ostringstream s;
  s << "DATA_" << dataCounter;
  data.constants.addConstant(Constant(s.str(),v));
  dataCounter++;
}

void Compiler::read(std::string var, Type type)
{
  var = normalizeVar(var);
  /* push the current read index */
  const Variable* readIndexVar = data.globalVariables.findVariable(readIndexVarName);
  recall(readIndexVar,yy::Parser::location_type());
  typeStack->pop_back();
  /* push the requested variable type */
  COp cop(OP_PUSH,Type::int32Type);
  cop.setParameter(static_cast<int32_t>(type.getScalarType().toInt()));
  code->push_back(cop);
  /* read the data leavin only the data on the stack */
  callFunction("read",yy::Parser::location_type());
  /* store the data in the variable */
  typeStack->push_back(type.getScalarType());
  store(var,yy::Parser::location_type());
  /* increment the readIndexVar */
  readIndexVar = data.globalVariables.findVariable(readIndexVarName);
  cop = COp(OP_INC,readIndexVar->getType());
  cop.setParameter(static_cast<int32_t>(readIndexVar->getAddress()));
  code->push_back(cop);
}

void Compiler::callFunction(const std::string &name, int npar, const yy::Parser::location_type &l)
{
  if (typeStack->size() < npar)
  {
    throw yy::Parser::syntax_error(l,"Not enough parameters in call to function '"+name+"'");
  }
  for (int32_t i=0;i<npar;i++)
  {
    typeStack->pop_back();
  }
  Type t = callFunction(name,l);
  if (t != Type::undefinedType) typeStack->push_back(t);
}

void Compiler::startUserFunction(const std::string& name, std::string& var, const yy::Parser::location_type &l)
{
  userFunction = std::make_unique<UserFunction>();
  userFunction->name = name;
  userFunction->f.var = normalizeVar(var);
  userFunction->f.name = name;
  userFunction->f.label = ++internalLabelCounter;
  userFunction->f.rettype = Type::doubleType;
  userFunction->uplevelCode = code;
  userFunction->uplevelTypeStack = typeStack;
  code = userFunction->f.code.getCodePtr();
  typeStack = &userFunction->typeStack;
  /* create label */
  createLabel(userFunction->f.label,l);
  /* put argument on top of stack */
  COp cop(OP_SWAP);
  code->push_back(cop);
  /* store argument in variable */
  const Variable* v = findAndCreateVar(userFunction->f.var);
  cop = COp(OP_STO,v->getType());
  cop.setParameter(static_cast<int32_t>(v->getAddress()));
  code->push_back(cop);
}

void Compiler::endUserFunction(const yy::Parser::location_type &l)
{
  /* put return address on top of stack */
  COp cop(OP_SWAP);
  code->push_back(cop);
  /* return from function */
  createReturn();
  code = userFunction->uplevelCode;
  typeStack = userFunction->uplevelTypeStack;
  if (!data.functions.addFunction(userFunction->f))
  {
    throw yy::Parser::syntax_error(l,"Redefinition of user function not allowed!");
  }
  userFunction.reset();
}

void Compiler::addError(const yy::Parser::location_type &l, const std::string &err_message)
{
  errors.addError(l.begin.line,err_message);
}





void Compiler::checkLine(const yy::Parser::location_type &l)
{
  if (l.begin.line != currentLine)
  {
    currentLine = l.begin.line;
    COp line(ASM_LINE);
    line.setParameter(currentLine);
    code->push_back(line);
  }
}

Executable* Compiler::compile_helper(std::istream& stream)
{
   if (scanner) delete(scanner);
   try
   {
      scanner = new Scanner(&stream);
   }
   catch( std::bad_alloc &ba )
   {
      std::cerr << "Failed to allocate scanner: (" << ba.what() << "), exiting!!\n";
      return nullptr;
   }
   if (parser) delete(parser);
   try
   {
      parser = new yy::Parser((*scanner),(*this));
   }
   catch( std::bad_alloc &ba )
   {
      std::cerr << "Failed to allocate parser: (" << ba.what() << "), exiting!!\n";
      return nullptr;
   }
   currentLine = -1;
   printCount = 0;
   internalLabelCounter = START_INTERNAL_LABEL_COUNTER;
   onGoLabel = 0;
   onGoIndex = 0;
   dataCounter = 0;
   labels.clear();
   forLoop.clear();
   ifData.clear();
   inputData.clear();
   userFunction.reset();
   data.clear();
//   data.noDebug = true;
   code = data.codeblock.getCodePtr();
   initcode.clear();
   toplevelTypeStack.clear();
   typeStack = &toplevelTypeStack;

   Variable nv1(readIndexVarName,getType(readIndexVarName));
   data.globalVariables.addVariable(nv1);

   restore();
   const int accept = 0;
   if (parser->parse() != accept)
   {
      return nullptr;
   }
   /* clear all scalar variables (arrays are cleared on resize) */
   for (uint32_t i=0;i<data.globalVariables.size();i++)
   {
     const auto& v = data.globalVariables[i];
     if (!v.getType().isArrayType())
     {
       COp cop(OP_CLR,v.getType().getScalarType());
       cop.setParameter(static_cast<int32_t>(v.getAddress()));
       initcode.push_back(cop);
     }
   }
   code->insert(code->begin(),initcode.begin(),initcode.end());
//   if (!forLoop.empty())
//   {
//     errors.addError(-1,"Missing NEXT statement(s) - unterminated FOR loop(s)");
//     return nullptr;
//   }
   Executable* exe = assembler->assemble();
   errors.add(assembler->getErrors());
   return exe;
}

const Variable* Compiler::findAndCreateVar(std::string var, bool normalize)
{
//  std::cout << var << " " << std::endl;
  if (normalize) var = normalizeVar(var);
//  /* we have to distinguish array type variables from scalar variables of same name */
//  if (type.isArrayType()) var += "_";
  if (userFunction && var == userFunction->f.var)
  {
    var = "__" + userFunction->f.name + "_" + var;
  }
  const Variable* v = data.globalVariables.findVariable(var);
  if (v == nullptr)
  {
    Variable nv(var,getType(var));
    data.globalVariables.addVariable(nv);
    v = data.globalVariables.findVariable(var);
  }
  return v;
}

Type Compiler::callFunction(const std::string &fn, const yy::Parser::location_type &l)
{
  Function* f = data.functions.findFunction(fn);
  if (f)
  {
    createGosub(f->label);
//    COp cop(OP_FN);
//    int32_t index = f->vtableIndex;
//    cop.setParameter(index);
//    code->push_back(cop);
    return f->rettype;
  }
  else
  {
    const LibraryFunction& func = Library::findFunction(fn);
    if (func.name.empty())
    {
      throw yy::Parser::syntax_error(l,"Undefined function '"+fn+"'");
      return Type::undefinedType;
    }
    COp cop(OP_CALL);
    int32_t index = func.id;
    cop.setParameter(index);
    code->push_back(cop);
    return func.rettype;
  }
}

std::string Compiler::normalizeVar(std::string var)
{
  if (var.length() >=2 && var[0] == '_' && var[1] == '_') return var;
  char c = '\0';
  size_t n = var.find('%');
  if (n != std::string::npos)
  {
    c = '%';
    var = var.substr(0,n);
  }
  else
  {
    n = var.find('$');
    if (n != std::string::npos)
    {
      c = '$';
      var = var.substr(0,n);
    }
  }
  if (var.length() > 2) var = var.substr(0,2);
  if (c) var += c;
  return var;
}

Type Compiler::getType(const std::string &var)
{
  size_t n = var.find('%');
  if (n != std::string::npos) return Type::int32Type;
  n = var.find('$');
  if (n != std::string::npos) return Type::stringType;
  return Type::doubleType;
}
