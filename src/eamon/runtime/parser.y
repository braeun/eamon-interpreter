%skeleton "lalr1.cc"
%require  "3.7"
%debug
%defines
/**
 * bison 3.3.2 change
 * %define parser_class_name to this, updated
 * should work for previous bison versions as
 * well. -jcb 24 Jan 2020
 */
%define api.parser.class {Parser}

%define api.value.type variant

%define parse.error verbose

%code requires{

  class Compiler;
  class Scanner;
  class Symbol;

      // The following definitions is missing when %locations isn't used
      # ifndef YY_NULLPTR
      #  if defined __cplusplus && 201103L <= __cplusplus
      #   define YY_NULLPTR nullptr
      #  else
      #   define YY_NULLPTR 0
      #  endif
      # endif
}

%parse-param { Scanner  &scanner  }
%parse-param { Compiler  &compiler  }

%code{
#include "runtime/compiler.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <ctype.h>

#undef yylex
#define yylex scanner.yylex
}

%type   <double>      const

%token                END    0     "end of file"
%token  <double>      NUMBER
%token  <int>         INTEGER
%token  <int>         LINENO
%token  <std::string> SYMBOL
%token  <std::string> STRINGSYMBOL
%token  <std::string> STRING
%token  <std::string> USERFUNCTION

%token OR AND NOT NEQ GEQ LEQ LTN GTN EQU UNARYMINUS

%left OR
%left AND
%left NOT
%left NEQ
%left GEQ
%left LEQ
%left LTN
%left GTN
%left EQU
%left   '+' '-'
%left   '*' '/'
%left   UNARYMINUS
%right  POW     /* exponentiation */

%token SEP

%token DATA DATAEND DEF DIM ELSE FOR FN GET GO GOSUB GOTO IF IMPLICITENDIF INPUT LABEL LET
%token ON ONERR NEXT POP PRINT READ RESTORE RETURN STEP THEN TO USING

%token HOME TEXT HTAB INVERSE NORMAL TAB VTAB SPC FLASH

%token ABS ACOS ASIN ATAN COS EXP INT LOG LOG10 LOG2
%token POW RND SGN SQRT SIN TAN

%token ASC CHR LEFT LEN MID RIGHT STR VAL

%token PEEK POKE CALL FRE

%token LIST


%start statement_list

%locations

%%

statement_list: statement
  | statement_list SEP statement
  ;

assignment: number_assignment
  | LET number_assignment
  | string_assignment
  | LET string_assignment
  ;

number_assignment:
  SYMBOL EQU expr { compiler.store($1,@1); }
  | SYMBOL '(' expr ')' { compiler.createArrayOffset($1,1,@1); }
    EQU expr { compiler.store($1,@1); }
  | SYMBOL '(' expr ',' expr ')' { compiler.createArrayOffset($1,2,@1); }
    EQU expr { compiler.store($1,@1); }
  ;

string_assignment:
    STRINGSYMBOL EQU stringexpr { compiler.store($1,@1); }
    | STRINGSYMBOL '(' expr ')' { compiler.createArrayOffset($1,1,@1); }
      EQU stringexpr { compiler.store($1,@1); }
    | STRINGSYMBOL '(' expr ',' expr ')' { compiler.createArrayOffset($1,2,@1); }
      EQU stringexpr { compiler.store($1,@1); }
  ;

statement: /* empty */
      | assignment
      | LABEL LINENO { compiler.createLabel($2,@2); }
      | END { compiler.createEnd(); }
      | GOTO INTEGER { compiler.createGoto($2); }
      | GO TO INTEGER { compiler.createGoto($3); }
      | ONERR GOTO INTEGER { compiler.createOnErrorGoto($3); }
      | ON expr GOTO { compiler.startOnGoto(@1); }
          gotolist {compiler.endOnGoto(@1); }
      | ON expr GO TO { compiler.startOnGoto(@1); }
          gotolist {compiler.endOnGoto(@1); }
      | GOSUB INTEGER { compiler.createGosub($2); }
//      | GO SUB INTEGER { compiler.createGosub($3); }
      | ON expr GOSUB { compiler.startOnGoto(@1); }
          gosublist {compiler.endOnGoto(@1); }
//      | ON expr GO SUB { compiler.startOnGoto(@1); }
//          gosublist {compiler.endOnGoto(@1); }
      | POP { compiler.createPop(); }
      | RETURN { compiler.createReturn(); }
      | PRINT printlist ';'
      | PRINT printlist { compiler.createPush("\n"); compiler.callPrint(false); }
      | PRINT USING stringexpr ';' { compiler.callPrintF(false); }
          printflist { compiler.createPush("\n"); compiler.callPrintF(true); }
      | INPUT prompt { compiler.startInput(); }
          inputlist { compiler.endInput(); }
      | GET { compiler.startGet(); }
          getinput { compiler.endGet(); }
      | for_loop
      | NEXT next_symbol
      | shortif_statement
      | if_statement
      | RESTORE { compiler.restore(); }
      | DATA { scanner.setDataOp(true); }
          datalist
          DATAEND { scanner.setDataOp(false); }
      | READ readlist
      | DIM dimlist
      | DEF USERFUNCTION '(' SYMBOL ')' EQU { compiler.startUserFunction($2,$4,@1); }
          expr { compiler.endUserFunction(@1); }
      | DEF FN SYMBOL '(' SYMBOL ')' EQU { compiler.startUserFunction($3,$5,@1); }
          expr { compiler.endUserFunction(@1); }
      | HOME { compiler.callFunction("home",0,@1); }
      | TEXT { compiler.callFunction("text",0,@1); }
      | POKE  expr ',' expr { compiler.callFunction("poke",2,@1); }
      | CALL expr
      | INVERSE { compiler.callFunction("inverse",0,@1); }
      | NORMAL { compiler.callFunction("normal",0,@1); }
      | FLASH { compiler.callFunction("flash",0,@1); }
      | VTAB  expr { compiler.callFunction("vtab",1,@1); }
      | HTAB  expr { compiler.callFunction("htab",1,@1); }
      | LIST /* listing of source code not supported */
      | LIST expr /* listing of source code not supported */
      | LIST expr ',' expr /* listing of source code not supported */
      ;

function: ABS '(' expr ')' { compiler.callFunction("abs",1,@1); }
  | ACOS '(' expr ')' { compiler.callFunction("acos",1,@1); }
  | ASIN '(' expr ')' { compiler.callFunction("asin",1,@1); }
  | ATAN '(' expr ')' { compiler.callFunction("atan",1,@1); }
  | ATAN '(' expr ',' expr ')' { compiler.callFunction("atan2",2,@1); }
  | COS '(' expr ')' { compiler.callFunction("cos",1,@1); }
  | EXP '(' expr ')' { compiler.callFunction("exp",1,@1); }
  | INT '(' expr ')' { compiler.callFunction("int",1,@1); }
  | LOG '(' expr ')' { compiler.callFunction("log",1,@1); }
  | LOG10 '(' expr ')' { compiler.callFunction("log10",1,@1); }
  | LOG2 '(' expr ')' { compiler.callFunction("log2",1,@1); }
  | RND '(' expr ')' { compiler.callFunction("rnd",1,@1); }
  | SGN '(' expr ')' { compiler.callFunction("sgn",1,@1); }
  | SQRT '(' expr ')' { compiler.callFunction("sqrt",1,@1); }
  | SIN '(' expr ')' { compiler.callFunction("sin",1,@1); }
  | TAN '(' expr ')' { compiler.callFunction("tan",1,@1); }
  | USERFUNCTION '(' expr ')' { compiler.callFunction($1,1,@1); }
  | FN SYMBOL '(' expr ')' { compiler.callFunction($2,1,@1); }
  | ASC '(' stringexpr ')' { compiler.callFunction("asc",1,@1); }
  | LEN '(' stringexpr ')' { compiler.callFunction("len",1,@1); }
  | VAL '(' stringexpr ')' { compiler.callFunction("val",1,@1); }
  | PEEK '(' expr ')' { compiler.callFunction("peek",1,@1); }
  | SPC '(' expr ')' { compiler.callFunction("spc",1,@1); }
  | FRE '(' expr ')' { compiler.callFunction("fre",1,@1); }
  ;

expr:     NUMBER { compiler.createPush($1); }
        | INTEGER { compiler.createPush($1); }
        | SYMBOL { compiler.recall($1,@1); }
        | expr '+' expr { compiler.createOperator("+",@1); }
        | expr '-' expr { compiler.createOperator("-",@1); }
        | expr '*' expr { compiler.createOperator("*",@1); }
        | expr '/' expr { compiler.createOperator("/",@1); }
        | expr POW expr { compiler.callFunction("pow",2,@1); }
        | expr EQU expr { compiler.createOperator("=",@1); }
        | expr NEQ expr { compiler.createOperator("<>",@1); }
        | expr LTN expr { compiler.createOperator("<",@1); }
        | expr LEQ expr { compiler.createOperator("<=",@1); }
        | expr GTN expr { compiler.createOperator(">",@1); }
        | expr GEQ expr { compiler.createOperator(">=",@1); }
        | expr AND expr { compiler.createOperator("&&",@1); }
        | expr OR expr { compiler.createOperator("||",@1); }
        | NOT expr { compiler.createNot(); }
        | stringexpr EQU stringexpr { compiler.createOperator("=",@1); }
        | stringexpr NEQ stringexpr { compiler.createOperator("<>",@1); }
        | stringexpr LTN stringexpr { compiler.createOperator("<",@1); }
        | stringexpr LEQ stringexpr { compiler.createOperator("<=",@1); }
        | stringexpr GTN stringexpr { compiler.createOperator(">",@1); }
        | stringexpr GEQ stringexpr { compiler.createOperator(">=",@1); }
        | function
        | array
        | '(' expr ')'
        | '-' expr  %prec UNARYMINUS  { compiler.createNegate(); }
        ;

stringfunction: CHR '(' expr ')' { compiler.callFunction("chr$",1,@1); }
          | LEFT '(' stringexpr ',' expr ')' { compiler.callFunction("left$",2,@1); }
          | MID '(' stringexpr ',' expr ')' { compiler.callFunction("mid1$",2,@1); }
          | MID '(' stringexpr ',' expr ',' expr ')' { compiler.callFunction("mid$",3,@1); }
          | RIGHT '(' stringexpr ',' expr ')' { compiler.callFunction("right$",2,@1); }
          | STR '(' expr ')' { compiler.callFunction("str$",1,@1); }
          ;

stringexpr: STRING { compiler.createPush($1); }
  | STRINGSYMBOL { compiler.recall($1,@1); }
  | stringarray
  | stringexpr '+' stringexpr { compiler.createOperator("+",@1); }
  | stringfunction
  | '(' stringexpr ')'
  ;

array:
  SYMBOL '(' expr ')' { compiler.createArrayOffset($1,1,@1); compiler.recall($1,@1); }
  | SYMBOL '(' expr ',' expr ')' { compiler.createArrayOffset($1,2,@1); compiler.recall($1,@1); }
  ;

stringarray:
  STRINGSYMBOL '(' expr ')' { compiler.createArrayOffset($1,1,@1); compiler.recall($1,@1); }
  | STRINGSYMBOL '(' expr ',' expr ')' { compiler.createArrayOffset($1,2,@1); compiler.recall($1,@1); }
  ;

dim:
      SYMBOL '(' expr ')' { compiler.createDimVar($1,1,@1); }
      | SYMBOL '(' expr ',' expr ')' { compiler.createDimVar($1,2,@1); }
      | STRINGSYMBOL '(' expr ')' { compiler.createDimVar($1,1,@1); }
      | STRINGSYMBOL '(' expr ',' expr ')' { compiler.createDimVar($1,2,@1); }
      ;

dimlist:
      dim
      | dimlist ',' dim
      ;

printlist:  /* possible empty */
      | expr { compiler.callPrint(false); }
      | printlist ',' expr { compiler.createPush(" "); compiler.callPrint(false); compiler.callPrint(false); }
      | printlist ';' expr { compiler.callPrint(false); }
      | stringexpr { compiler.callPrint(false); }
      | printlist ',' stringexpr { compiler.createPush(" "); compiler.callPrint(false); compiler.callPrint(false); }
      | printlist ';' stringexpr { compiler.callPrint(false); }
      | printlist stringexpr { compiler.callPrint(false); }
      | printtab
      | printlist ',' printtab
      | printlist ';' printtab
  ;

printflist:  /* possible empty */
      | expr { compiler.callPrintF(false); }
      | printflist ',' expr { compiler.callPrintF(false); }
      | printflist ';' expr { compiler.callPrintF(false); }
      | stringexpr { compiler.callPrintF(false); }
      | printflist ',' stringexpr { compiler.callPrintF(false); }
      | printflist ';' stringexpr { compiler.callPrintF(false); }
  ;

printtab:
      TAB '(' expr ')' { compiler.callPrintTab(); }
      ;

input:
      SYMBOL { compiler.addInput($1,Type::doubleType); }
      arrayindex { compiler.addedInput(); }
      | STRINGSYMBOL { compiler.addInput($1,Type::stringType); }
      arrayindex { compiler.addedInput(); }
      ;

getinput:
      STRINGSYMBOL { compiler.addInput($1,Type::stringType); }
      arrayindex { compiler.addedInput(); }
      ;

arrayindex: /* may be empty */
      | '(' expr ')' { compiler.createInputArrayOffset(1,@1); }
      | '(' expr ',' expr ')' { compiler.createInputArrayOffset(2,@1); }
      ;

inputlist:
      input
      | inputlist ',' input
      | inputlist ';' input
      ;

prompt: /* possible empty { compiler.createPush("?"); compiler.callPrint(false); } */
      | STRING ',' { compiler.createPush($1); compiler.callPrint(false); }
      | STRING ';' { compiler.createPush($1); compiler.callPrint(false); }
      ;

read:  SYMBOL { compiler.read($1,Type::doubleType); }
  | SYMBOL '(' expr ')' { compiler.createArrayOffset($1,1,@1); compiler.read($1,Type::adoubleType); }
  | SYMBOL '(' expr ',' expr ')' { compiler.createArrayOffset($1,2,@1); compiler.read($1,Type::adoubleType); }
  | STRINGSYMBOL { compiler.read($1,Type::stringType); }
  | STRINGSYMBOL '(' expr ')' { compiler.createArrayOffset($1,1,@1); compiler.read($1,Type::astringType); }
  | STRINGSYMBOL '(' expr ',' expr ')' { compiler.createArrayOffset($1,2,@1); compiler.read($1,Type::astringType); }
  ;

readlist:
  read
  | readlist ',' read
  ;

gotolist: INTEGER { compiler.addOnGoto($1,@1); }
  | gotolist ',' INTEGER { compiler.addOnGoto($3,@3); }
  ;

gosublist: INTEGER { compiler.addOnGosub($1,@1); }
  | gosublist ',' INTEGER { compiler.addOnGosub($3,@3); }
  ;

for_loop: FOR SYMBOL EQU expr { compiler.store($2,@2); compiler.startFor($2,@2); }
  TO expr { compiler.compareFor(@2); }
  step_part { compiler.stepFor(@2); }
  ;

step_part: { compiler.createPush(1); } /* can be omitted */
  | STEP expr
  ;

next_symbol:  { compiler.endFor(""); } /* can be omitted */
  | SYMBOL { compiler.endFor($1,@1); }
  | next_symbol ',' SYMBOL { compiler.endFor($3,@3); }
  ;

if_statement: IF expr THEN { scanner.setIfOp(true); compiler.startIf(@1); }
  statement_list
  else_part
  IMPLICITENDIF { compiler.endIf(@1); scanner.setIfOp(compiler.isIf()); }
  ;

else_part: /* can be omitted */
  | ELSE INTEGER { compiler.elseIf(@2); compiler.createGoto($2); }
  | ELSE { compiler.elseIf(@1); } statement_list
  ;

shortif_statement:
  IF expr THEN INTEGER { scanner.setIfOp(true); compiler.startIf(@1); compiler.createGoto($4); }
  else_part
  IMPLICITENDIF { compiler.endIf(@1); scanner.setIfOp(compiler.isIf()); }
  ;

const: NUMBER {$$=$1;}
  | '+' NUMBER {$$=$2;}
  | '-' NUMBER {$$=-$2;}
  | INTEGER {$$=$1;}
  | '+' INTEGER {$$=$2;}
  | '-' INTEGER {$$=-$2;}
  ;

datalist: STRING { compiler.createData($1); }
  | SYMBOL { compiler.createData($1); }
  | const { compiler.createData($1); }
  | datalist ','  STRING { compiler.createData($3); }
  | datalist ',' const { compiler.createData($3); }
  | datalist ',' SYMBOL { compiler.createData($3); }
  ;



%%
/* end of grammar */

void yy::Parser::error(const yy::Parser::location_type &l, const std::string &err_message )
{
  compiler.addError(l,err_message);
//  std::cerr << "Error: " << err_message << " at " << l << "\n";
}

