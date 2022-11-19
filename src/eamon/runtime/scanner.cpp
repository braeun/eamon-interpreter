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

#include "scanner.h"
#include <cstring>

Scanner::Scanner(std::istream* in):yyFlexLexer(in),
  if_op(false),
  data_op(false)
{
}

Scanner::~Scanner()
{
}

void Scanner::setIfOp(bool flag)
{
  if_op = flag;
}

void Scanner::setDataOp(bool flag)
{
  data_op = flag;
}


char *Scanner::replace_escapes(char *string)		/* replace \n,\a, etc. */
{
  char *from, *to;
  const char *p;
  int val;
  static const char *hexdigits = "0123456789abcdef";

  from = to = string;
  while (*from)
  {
    if (*from == '\\')
    {
      from++;
      switch (*from)
      {
        case 'n':
          *to = '\n';
          break;
        case 't':
          *to = '\t';
          break;
        case 'v':
          *to = '\v';
          break;
        case 'b':
          *to = '\b';
          break;
        case 'r':
          *to = '\r';
          break;
        case 'f':
          *to = '\f';
          break;
        case 'a':
          *to = '\a';
          break;
        case '\\':
          *to = '\\';
          break;
        case '\?':
          *to = '\?';
          break;
        case '\'':
          *to = '\'';
          break;
        case '\"':
          *to = '\"';
          break;
        case 'x':
          val = 0;
          if ((p = strchr(hexdigits, tolower(*(from + 1)))) && p - hexdigits < 16)
          {
            from++;
            val = p - hexdigits;
            if ((p = strchr(hexdigits, tolower(*(from + 1)))) && p - hexdigits < 16)
            {
              from++;
              val *= 16;
              val += p - hexdigits;
            }
          }
          *to = (char)val;
          break;
        default:
          *to = '\\';
          to++;
          *to = *from;
      }
    }
    else
    {
      *to = *from;
    }
    from++;
    to++;
  }
  *to = '\0';
  return string;
}


int Scanner::count_backslashes(char *text) /* count number of backslashes up to position in string */
{
  int cnt = 0;
  while(*text == '\\')
  {
    text--;
    cnt++;
  }
  return cnt;
}
