/********************************************************************************
 *                                                                              *
 * EamonInterpreter - source code scanner                                       *
 *                                                                              *
 * modified: 2022-11-17                                                         *
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

#ifndef SCANNER_H
#define SCANNER_H

/*
 * Definition of the lexer class
 */
#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "parser.h"
#include "location.hh"

class Scanner: public yyFlexLexer
{
public:
  Scanner(std::istream* in);
  virtual ~Scanner();

  //get rid of override virtual function warning
  using FlexLexer::yylex;

  virtual int yylex(yy::Parser::semantic_type* const lval, yy::Parser::location_type* location);

  void setIfOp(bool flag);

  void setDataOp(bool flag);

private:
  char *replace_escapes(char *string);		/* replace \n,\a, etc. */
  int count_backslashes(char *text); /* count number of backslashes up to position in string */

  /* yyval ptr */
  yy::Parser::semantic_type* yylval = nullptr;
  /* location ptr */
  yy::Parser::location_type *loc    = nullptr;
  /* true if an if op is in progress */
  bool if_op;
  /* true if a data declaration is in progress */
  bool data_op;
};

#endif // SCANNER_H
