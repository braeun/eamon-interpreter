/********************************************************************************
 *                                                                              *
 * EamonInterpreter - simple code editor widget                                 *
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

#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>

class CodeEditor : public QPlainTextEdit
{
  Q_OBJECT

public:
  CodeEditor(QWidget *parent = nullptr);

  void lineNumberAreaPaintEvent(QPaintEvent *event);

  int lineNumberAreaWidth();

  virtual bool findString(QString s, bool reverse, bool casesens, bool words);

  virtual void replace(QString r);

  virtual void replaceFind(QString s, QString r, bool casesens, bool words);

  virtual void replaceAll(QString s, QString r, bool casesens, bool words);


protected:
  void resizeEvent(QResizeEvent *event) override;
  void keyPressEvent(QKeyEvent* event) override;

private slots:
  void updateLineNumberAreaWidth(int newBlockCount);
  void highlightCurrentLine();
  void updateLineNumberArea(const QRect &, int);

private:
  void indentNewLine();
  void indentSelection();
  void unindentSelection();

  QWidget *lineNumberArea;
  int32_t indentSize;
  QString indentString;
};

#endif // CODEEDITOR_H
