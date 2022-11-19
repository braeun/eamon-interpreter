/********************************************************************************
 *                                                                              *
 * EamonInterpreter - syntax highlighter for BASIC                              *
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

#include "syntaxhighlighter.h"
#include <QDebug>

SyntaxHighlighter::SyntaxHighlighter(QObject *parent): QSyntaxHighlighter(parent)
{
  init();
}

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent): QSyntaxHighlighter(parent)
{
  init();
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
  for (const HighlightingRule &rule : qAsConst(highlightingRules))
  {
    QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
    while (matchIterator.hasNext())
    {
      QRegularExpressionMatch match = matchIterator.next();
      setFormat(match.capturedStart(), match.capturedLength(), rule.format);
    }
  }
//  setCurrentBlockState(0);

//  int startIndex = 0;
//  if (previousBlockState() != 1) startIndex = text.indexOf(commentStartExpression);

//  while (startIndex >= 0)
//  {
//    QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
//    int endIndex = match.capturedStart();
//    int commentLength = 0;
//    if (endIndex == -1)
//    {
//      setCurrentBlockState(1);
//      commentLength = text.length() - startIndex;
//    }
//    else
//    {
//      commentLength = endIndex - startIndex + match.capturedLength();
//    }
//    setFormat(startIndex, commentLength, multiLineCommentFormat);
//    startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
//  }
}


void SyntaxHighlighter::init()
{
  HighlightingRule rule;

  keywordFormat.setForeground(Qt::darkBlue);
  keywordFormat.setFontWeight(QFont::Bold);
  const QString keywordPatterns[] = {
//    QStringLiteral("\\bTEXT\\b"),
//    QStringLiteral("\\bHOME\\b"),
//    QStringLiteral("\\bPRINT\\b"),
//    QStringLiteral("\\bdo\\b"),
//    QStringLiteral("\\bdouble\\b"),
//    QStringLiteral("\\belse\\b"),
//    QStringLiteral("\\bextern\\b"),
//    QStringLiteral("\\bfor\\b"),
//    QStringLiteral("\\bif\\b"),
//    QStringLiteral("\\bint\\b"),
//    QStringLiteral("\\blong\\b"),
//    QStringLiteral("\\breturn\\b"),
//    QStringLiteral("\\bstring\\b"),
//    QStringLiteral("\\bstruct\\b"),
//    QStringLiteral("\\bwhile\\b")
  };

  for (const QString &pattern : keywordPatterns)
  {
    rule.pattern = QRegularExpression(pattern);
    rule.format = keywordFormat;
    highlightingRules.append(rule);
  }

//  classFormat.setFontWeight(QFont::Bold);
//  classFormat.setForeground(Qt::darkMagenta);
//  rule.pattern = QRegularExpression(QStringLiteral("\\bQ[A-Za-z]+\\b"));
//  rule.format = classFormat;
//  highlightingRules.append(rule);

  functionFormat.setFontItalic(true);
//  functionFormat.setForeground(Qt::blue);
  rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_$]+(?=\\()"));
  rule.format = functionFormat;
  highlightingRules.append(rule);

//  quotationFormat.setForeground(Qt::darkGreen);
//  rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
  rule.pattern = QRegularExpression(QStringLiteral("\"(\\.|[^\"])*\""));
  rule.format = quotationFormat;
  highlightingRules.append(rule);

  singleLineCommentFormat.setForeground(Qt::red);
  rule.pattern = QRegularExpression(QStringLiteral("REM[^\n]*"));
  rule.format = singleLineCommentFormat;
  highlightingRules.append(rule);

  multiLineCommentFormat.setForeground(Qt::red);
  commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
  commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));
}

