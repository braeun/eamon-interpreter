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

#ifndef EDITORWIDGET_H
#define EDITORWIDGET_H

#include <QWidget>

namespace Ui {
class EditorWidget;
}

class QSyntaxHighlighter;

class EditorWidget : public QWidget
{
  Q_OBJECT

public:
  explicit EditorWidget(QWidget *parent = nullptr);
  ~EditorWidget();

  void setText(const QString& text);

  QString getText() const;

  void updateSettings();

private slots:

  void on_findPreviousButton_clicked();

  void on_findNextButton_clicked();

  void on_replaceButton_clicked();

  void on_replaceFindButton_clicked();

  void on_replaceAllButton_clicked();

private:
  void findString(QString s, bool reverse, bool casesens, bool words);
  void replace(QString r);
  void replaceFind(QString s, QString r, bool casesens, bool words);
  void replaceAll(QString s, QString r, bool casesens, bool words);

  Ui::EditorWidget *ui;
  QSyntaxHighlighter* highlighter;
};

#endif // EDITORWIDGET_H
