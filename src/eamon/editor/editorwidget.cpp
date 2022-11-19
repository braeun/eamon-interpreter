/********************************************************************************
 *                                                                              *
 * EamonInterpreter - editor widget                                             *
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

#include "editorwidget.h"
#include "ui_editorwidget.h"
#include "syntaxhighlighter.h"
#include "../defines.h"
#include <QMessageBox>
#include <QSettings>

EditorWidget::EditorWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::EditorWidget)
{
  ui->setupUi(this);
  highlighter = new SyntaxHighlighter(this);
  highlighter->setDocument(ui->codeEdit->document());
  updateSettings();
}

EditorWidget::~EditorWidget()
{
  delete ui;
}

void EditorWidget::setText(const QString &text)
{
  ui->codeEdit->setPlainText(text);
}

QString EditorWidget::getText() const
{
  return ui->codeEdit->toPlainText();
}

void EditorWidget::updateSettings()
{
  QSettings settings;
  QFont codeFont;
  codeFont.fromString(settings.value(SETTING_SCRIPT_FONT,SETTING_VALUE_SCRIPT_FONT).toString());
  ui->codeEdit->setFont(codeFont);
  QFontMetrics fm(codeFont);
  ui->codeEdit->setTabStopDistance(2*fm.horizontalAdvance(" "));
}



void EditorWidget::findString(QString s, bool reverse, bool casesens, bool words)
{
  QTextDocument::FindFlags flag;
  if (reverse) flag |= QTextDocument::FindBackward;
  if (casesens) flag |= QTextDocument::FindCaseSensitively;
  if (words) flag |= QTextDocument::FindWholeWords;

  QTextCursor cursor = ui->codeEdit->textCursor();
  // here , you save the cursor position
  QTextCursor cursorSaved = cursor;

  if (!ui->codeEdit->find(s,flag))
  {
    //nothing is found | jump to start/end
    cursor.movePosition(reverse?QTextCursor::End:QTextCursor::Start);

    /* following line :
      - the cursor is set at the beginning/end of the document (if search is reverse or not)
      - in the next "find", if the word is found, now you will change the cursor position
    */
    ui->codeEdit->setTextCursor(cursor);

    if (!ui->codeEdit->find(s, flag))
    {
      //no match in whole document
      QMessageBox msgBox;
      msgBox.setText(tr("String not found."));
      msgBox.setStandardButtons(QMessageBox::Ok);
      msgBox.setDefaultButton(QMessageBox::Ok);
      msgBox.exec();
      // word not found : we set the cursor back to its initial position
      ui->codeEdit->setTextCursor(cursorSaved);
    }
  }
}

void EditorWidget::replace(QString r)
{
  QTextCursor cursor = ui->codeEdit->textCursor();
  if (cursor.hasSelection())
  {
    cursor.beginEditBlock();
    cursor.insertText(r);
    cursor.endEditBlock();
  }
}

void EditorWidget::replaceFind(QString s, QString r, bool casesens, bool words)
{
  replace(r);
  findString(s,false,casesens,words);
}

void EditorWidget::replaceAll(QString s, QString r, bool casesens, bool words)
{
  QTextDocument::FindFlags flag;
  if (casesens) flag |= QTextDocument::FindCaseSensitively;
  if (words) flag |= QTextDocument::FindWholeWords;

  QTextCursor cursor = ui->codeEdit->textCursor();
  // here , you save the cursor position
  QTextCursor cursorSaved = cursor;

  cursor.beginEditBlock();
  cursor.movePosition(QTextCursor::Start);
  while (ui->codeEdit->find(s,flag))
  {
    QTextCursor qc = ui->codeEdit->textCursor();
    if (qc.hasSelection())
    {
      qc.insertText(r);
    }
  }
  cursor.endEditBlock();

  ui->codeEdit->setTextCursor(cursorSaved);
}

void EditorWidget::on_findPreviousButton_clicked()
{
  findString(ui->searchField->text(),true,false,false);
}

void EditorWidget::on_findNextButton_clicked()
{
  findString(ui->searchField->text(),false,false,false);
}

void EditorWidget::on_replaceButton_clicked()
{
  if (!ui->replaceField->text().isEmpty())
  {
    replace(ui->replaceField->text());
  }
}

void EditorWidget::on_replaceFindButton_clicked()
{
  if (!ui->searchField->text().isEmpty())
    if (!ui->searchField->text().isEmpty() && !ui->replaceField->text().isEmpty())
    {
      replaceFind(ui->searchField->text(),ui->replaceField->text(),false,false);
    }
}

void EditorWidget::on_replaceAllButton_clicked()
{
  if (!ui->searchField->text().isEmpty() && !ui->replaceField->text().isEmpty())
  {
    replaceAll(ui->searchField->text(),ui->replaceField->text(),false,false);
  }
}
