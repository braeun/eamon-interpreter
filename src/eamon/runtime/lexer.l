%{
#include "parser.h"
/* C++ string header, for string ops below */
#include <string>

/* Implementation of yyFlexScanner */
#include "runtime/scanner.h"
#undef  YY_DECL
#define YY_DECL int Scanner::yylex( yy::Parser::semantic_type * const lval, yy::Parser::location_type *loc )

/* typedef to make the returns for the tokens shorter */
using token = yy::Parser::token;

/* define yyterminate as this instead of NULL */
#define yyterminate() return( token::END )

/* msvc2010 requires that we exclude this header file. */
#define YY_NO_UNISTD_H

/* update location on matching */
int yycolumn = 1;
//#define YY_USER_ACTION loc->step(); loc->columns(yyleng);
#define YY_USER_ACTION loc->begin.line=loc->end.line=yylineno; loc->begin.column=yycolumn; loc->end.column=yycolumn+yyleng-1;yycolumn+=yyleng;

%}

WS [ \t\f\r\v]
NAME ([a-z_][a-z0-9_]*\.[a-z_][a-z0-9_]*)|([a-z_][a-z0-9_]*)

%option default
%option yyclass="Scanner"
%option noyywrap
%option yylineno
%option c++

%x PRELNO
%x PASTLNO
%x PASTIMPORT
%x EVAL_DIGITS

%%
%{          /** Code executed at the beginning of yylex **/
            yylval = lval;
%}

{WS}+ {BEGIN(INITIAL);}     /* ignore whitespace */

^{WS}*/[0-9]+ { BEGIN(PRELNO);return token::LABEL; }
<PRELNO>[0-9]+ {
  BEGIN(PASTLNO);
  yylval->emplace<int>(atoi(yytext));
  return token::LINENO;
}
<PASTLNO>.* { yycolumn = 1; BEGIN(INITIAL); yyless(0); return token::SEP; }
<PASTLNO>\n { yycolumn = 1; BEGIN(INITIAL); return token::SEP; }

\n { yycolumn = 1; if (data_op) { yyless(0); return token::DATAEND; } if (if_op) { yyless(0); return token::IMPLICITENDIF; } return token::SEP; }
: { if (data_op) { yyless(0); return token::DATAEND; } return token::SEP; }

REM[-+*/:(),.; \t\f\r\v]+.* { return token::SEP; }  /* comments span 'til end of line */
REM\n { yycolumn = 1; if (data_op) { yyless(0); return token::DATAEND; } if (if_op) { yyless(0); return token::IMPLICITENDIF; } return token::SEP; }
REM { yymore(); }

"<>" return token::NEQ;
"< >" return token::NEQ;
"><" return token::NEQ;
"> <" return token::NEQ;
"#" return token::NEQ;
"<=" return token::LEQ;
"< =" return token::LEQ;
"=<" return token::LEQ;
"= <" return token::LEQ;
">=" return token::GEQ;
"> =" return token::GEQ;
"=>" return token::GEQ;
"= >" return token::GEQ;
"=" return token::EQU;
"<" return token::LTN;
">" return token::GTN;
"!" return token::NOT;
AND return token::AND;
NOT return token::NOT;
OR return token::OR;
"^" return token::POW;
"**" return token::POW;

[-+*/:(),.;] {return yytext[0];}

DATA return token::DATA;
DEF return token::DEF;
DIM return token::DIM;
ELSE return token::ELSE;
END return token::END;
FN return token::FN;
FOR return token::FOR;
GET { if (data_op) { yylval->emplace<std::string>(yytext); return token::STRING; } return token::GET; }
GO return token::GO;
GOSUB return token::GOSUB;
GOTO return token::GOTO;
IF return token::IF;
INPUT return token::INPUT;
LET return token::LET;
NEXT return token::NEXT;
ON return token::ON;
ONERR return token::ONERR;
POP return token::POP;
PRINT return token::PRINT;
READ { if (data_op) { yylval->emplace<std::string>(yytext); return token::STRING; } return token::READ; }
RESTORE return token::RESTORE;
RETURN return token::RETURN;
STEP return token::STEP;
STOP return token::END;
THEN return token::THEN;
TO return token::TO;
USING return token::USING;

HOME return token::HOME;
TEXT return token::TEXT;

ABS return token::ABS;
ACOS return token::ACOS;
ASIN return token::ASIN;
ATAN return token::ATAN;
ATN return token::ATAN;
COS return token::COS;
EXP return token::EXP;
INT return token::INT;
LOG return token::LOG;
LOG10 return token::LOG10;
LOG2 return token::LOG2;
RND return token::RND;
SGN return token::SGN;
SQR return token::SQRT;
SQRT return token::SQRT;
SIN return token::SIN;
TAN return token::TAN;

HTAB return token::HTAB;
INVERSE return token::INVERSE;
NORMAL return token::NORMAL;
TAB return token::TAB;
VTAB return token::VTAB;
SPC return token::SPC;
FLASH return token::FLASH;

ASC return token::ASC;
CHR\$ return token::CHR;
LEFT\$ return token::LEFT;
LEN return token::LEN;
MID\$ return token::MID;
RIGHT\$ return token::RIGHT;
STR\$ return token::STR;
VAL return token::VAL;

PEEK return token::PEEK;
POKE return token::POKE;
CALL return token::CALL;
FRE return token::FRE;

LIST return token::LIST;

<INITIAL>[0-9]+ {
    yylval->emplace<int>(atoi(yytext)); /* sscanf(yytext, "%d", &yylval->ival); */ return token::INTEGER; }

<INITIAL>(([0-9]+|([0-9]*\.[0-9]*))([eE][-+]?[0-9]+)?) {
    yylval->emplace<double>(atof(yytext)); /* sscanf(yytext, "%lf", &yylval->dval); */ return token::NUMBER; }

FN{NAME} { /* user defined function name */
    yylval->emplace<std::string>(yytext+2); return token::USERFUNCTION; }


{NAME} { /* no reserved word -> check for symbol */
    yylval->emplace<std::string>(yytext); return token::SYMBOL; }

{NAME}\% { /* no reserved word -> check for symbol */
    yylval->emplace<std::string>(yytext); return token::SYMBOL; }

{NAME}\$ { /* string variable */
    yylval->emplace<std::string>(yytext); return token::STRINGSYMBOL; }

\"[^\"\n\r]*(\"|\n|\r) {
  if (yyleng<2 || yytext[yyleng-1]=='\n' || yytext[yyleng-1]=='\r')
  { /* unterminated string has reached end of line, report qualified error in bison */
// TODO: handle eol as string termination instead of closing quotes
      yyless(1);
//      yylval->emplace<std::string>("");
//      return token::STRING;
      char* tmp = strdup(yytext+1);
      if (strlen(tmp) > 0) *(tmp+strlen(tmp)-1) = '\0';
      replace_escapes(tmp);
      yylval->emplace<std::string>(tmp);
      free(tmp);
      return token::STRING;
  }
  else if (yytext[yyleng-1]=='\"' && count_backslashes(yytext+yyleng-2)%2==1)
  { /* final quote was escaped, so put all text back and read more */
    yyless(yyleng-1);
    yymore();
  }
  else
  { /* properly quoted string; remove quotes and return it */
    char* tmp = strdup(yytext+1);
    *(tmp+strlen(tmp)-1) = '\0';
    replace_escapes(tmp);
    yylval->emplace<std::string>(tmp);
    free(tmp);
    return token::STRING;
  }
}

%%

