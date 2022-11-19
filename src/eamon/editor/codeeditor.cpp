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

#include "codeeditor.h"
#include "linenumberarea.h"
#include <QApplication>
#include <QPainter>
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <QDebug>

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent),
  indentSize(2),
  indentString(QString(2,' '))
{
  lineNumberArea = new LineNumberArea(this);

  connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
  connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
  connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

  updateLineNumberAreaWidth(0);
  highlightCurrentLine();
  setTabStopDistance(fontMetrics().horizontalAdvance(indentString));
}

int CodeEditor::lineNumberAreaWidth()
{
  int digits = 1;
  int max = qMax(1, blockCount());
  while (max >= 10)
  {
    max /= 10;
    ++digits;
  }
  int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
  return space;
}

bool CodeEditor::findString(QString s, bool reverse, bool casesens, bool words)
{
  QTextDocument::FindFlags flag;
  if (reverse) flag |= QTextDocument::FindBackward;
  if (casesens) flag |= QTextDocument::FindCaseSensitively;
  if (words) flag |= QTextDocument::FindWholeWords;

  QTextCursor cursor = textCursor();
  // here , you save the cursor position
  QTextCursor cursorSaved = cursor;

  if (!find(s,flag))
  {
    //nothing is found | jump to start/end
    cursor.movePosition(reverse?QTextCursor::End:QTextCursor::Start);

    /* following line :
      - the cursor is set at the beginning/end of the document (if search is reverse or not)
      - in the next "find", if the word is found, now you will change the cursor position
    */
    setTextCursor(cursor);

    if (!find(s, flag))
    {
      // word not found : we set the cursor back to its initial position
      setTextCursor(cursorSaved);
      return false;
    }
  }
  return true;
}

void CodeEditor::replace(QString r)
{
  QTextCursor cursor = textCursor();
  if (cursor.hasSelection())
  {
    cursor.beginEditBlock();
    cursor.insertText(r);
    cursor.endEditBlock();
  }
}

void CodeEditor::replaceFind(QString s, QString r, bool casesens, bool words)
{
  replace(r);
  findString(s,false,casesens,words);
}

void CodeEditor::replaceAll(QString s, QString r, bool casesens, bool words)
{
  QTextDocument::FindFlags flag;
  if (casesens) flag |= QTextDocument::FindCaseSensitively;
  if (words) flag |= QTextDocument::FindWholeWords;

  QTextCursor cursor = textCursor();
  // here , you save the cursor position
  QTextCursor cursorSaved = cursor;

  cursor.beginEditBlock();
  cursor.movePosition(QTextCursor::Start);
  while (find(s,flag))
  {
    QTextCursor qc = textCursor();
    if (qc.hasSelection())
    {
      qc.insertText(r);
    }
  }
  cursor.endEditBlock();
  setTextCursor(cursorSaved);
}





void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
  setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
  if (dy)
    lineNumberArea->scroll(0, dy);
  else
    lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
  if (rect.contains(viewport()->rect())) updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
  QPlainTextEdit::resizeEvent(e);
  QRect cr = contentsRect();
  lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::keyPressEvent(QKeyEvent *event)
{
//  qDebug() << event;
  if (event->key() == Qt::Key_Return)
  {
    QPlainTextEdit::keyPressEvent(event);
    indentNewLine();
  }
  else if (event->key() == Qt::Key_Tab)
  {
    if (textCursor().hasSelection())
    {
      indentSelection();
      event->accept();
    }
    else
      QPlainTextEdit::keyPressEvent(event);
  }
  else if (event->key() == Qt::Key_Backtab)
  {
    if (textCursor().hasSelection())
    {
      unindentSelection();
    }
    event->accept();
  }
  else
  {
    QPlainTextEdit::keyPressEvent(event);
  }
}

void CodeEditor::highlightCurrentLine()
{
  QList<QTextEdit::ExtraSelection> extraSelections;

  if (!isReadOnly())
  {
    QTextEdit::ExtraSelection selection;
//    QColor lineColor = QColor(Qt::yellow).lighter(160);
    QColor lineColor = QApplication::palette().color(QPalette::Highlight);
    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);
  }
  setExtraSelections(extraSelections);
}


void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
  QPainter painter(lineNumberArea);
  painter.fillRect(event->rect(), Qt::lightGray);

  QTextBlock block = firstVisibleBlock();
  int blockNumber = block.blockNumber();
  int top = (int)blockBoundingGeometry(block).translated(contentOffset()).top();
  int bottom = top + (int)blockBoundingRect(block).height();
  while (block.isValid() && top <= event->rect().bottom())
  {
    if (block.isVisible() && bottom >= event->rect().top())
    {
      QString number = QString::number(blockNumber + 1);
      painter.setPen(Qt::black);
      painter.drawText(0,top,lineNumberArea->width(),fontMetrics().height(),Qt::AlignRight, number);
    }
    block = block.next();
    top = bottom;
    bottom = top + (int)blockBoundingRect(block).height();
    ++blockNumber;
  }
}

void CodeEditor::indentNewLine()
{
  QTextCursor cur = textCursor();
  cur.movePosition(QTextCursor::Up);
  cur.movePosition(QTextCursor::EndOfLine,QTextCursor::MoveMode::KeepAnchor);
  QString txt = cur.selectedText();
  int n = 0;
  for (int32_t i=0;i<txt.length();i++)
  {
    if (txt[i] == ' ')
      n++;
    else if (txt[i] == '\t')
      n += indentSize;
    else
      break;
  }
  insertPlainText(QString(n,' '));
}

void CodeEditor::indentSelection()
{
  QTextCursor cur = textCursor();
  cur.beginEditBlock();
  int a = cur.anchor();
  int p = cur.position();

  cur.setPosition(a);
  cur.movePosition(QTextCursor::StartOfBlock,QTextCursor::MoveAnchor);
  cur.setPosition(p, QTextCursor::KeepAnchor);
  if (cur.atBlockStart()) cur.movePosition(QTextCursor::Left,QTextCursor::KeepAnchor);
  // set a new selection with the new beginning
  QString str = cur.selection().toPlainText();
  QStringList list = str.split("\n");
  // get the selected text and split into lines
  for (QString& line : list) line.insert(0,indentString);
//  for (int i=0;i<list.count();i++) list[i].insert(0,"  ");
  //insert a space at the beginning of each line

  str = list.join("\n");
  cur.removeSelectedText();
  cur.insertText(str);
  // put the new text back
  cur.setPosition(a);
  cur.setPosition(p, QTextCursor::KeepAnchor);
  cur.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,2*indentSize);
  // reselect the text for more indents

  setTextCursor(cur);
  cur.endEditBlock();
  // put the whole thing back into the main text
}

void CodeEditor::unindentSelection()
{
  QTextCursor cur = textCursor();
  cur.beginEditBlock();
  int a = cur.anchor();
  int p = cur.position();

  cur.setPosition(a);
  cur.movePosition(QTextCursor::StartOfBlock,QTextCursor::MoveAnchor);
  cur.setPosition(p, QTextCursor::KeepAnchor);
  if (cur.atBlockStart()) cur.movePosition(QTextCursor::Left,QTextCursor::KeepAnchor);
  // set a new selection with the new beginning
  QString str = cur.selection().toPlainText();
  QStringList list = str.split("\n");
  // get the selected text and split into lines
  for (QString& line : list)
  {
    if (line.startsWith(indentString))
      line.remove(0,indentSize);
    else if (line.startsWith("\t"))
      line.remove(0,1);
  }
//  for (int i=0;i<list.count();i++) list[i].insert(0,"  ");
  //insert a space at the beginning of each line

  str = list.join("\n");
  cur.removeSelectedText();
  cur.insertText(str);
  // put the new text back
  cur.setPosition(a);
  cur.setPosition(p, QTextCursor::KeepAnchor);
  cur.movePosition(QTextCursor::Left,QTextCursor::KeepAnchor,2*indentSize);
  // reselect the text for more indents

  setTextCursor(cur);
  cur.endEditBlock();
  // put the whole thing back into the main text
}



