/********************************************************************************
 *                                                                              *
 * EamonInterpreter - game screen                                               *
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

#ifndef SCREEN_H
#define SCREEN_H

#include <QFrame>
#include <QImage>
#include <vector>

class Screen : public QFrame
{
  Q_OBJECT
public:

  enum Mode { Text, Graphics };

  explicit Screen(QWidget *parent = nullptr);

  void print(const QString& txt);

  void backspace();

  void clear(bool doupdate=true);

  void moveToColumn(int c);

  void moveToRow(int r);

  void inverse();

  void normal();

  void updateSettings();

  void setImage(const std::vector<uint8_t>& data);

  void setMode(Mode m);

  void setPunctuation(bool flag);

signals:

protected:
  virtual void paintEvent(QPaintEvent* event) override;

private:
  void paintText();
  void paintGrapics();
  void newline();
  void applyStyle(QPainter& p, int style, int sx, int sy, int w);
  bool isPuntuation(QChar c);

  int ncols;
  int nrows;
  int crow;
  int ccol;
  int linespacing;
  std::vector<QString> lines;
  std::vector<std::vector<int>> styles;
  QFont font;
  bool lowerCase;
  bool capitalize;
  QColor foreground;
  QColor background;
  int currentStyle;
  int border;
  QImage image;
  Mode mode;
  bool punctuation;
};

#endif // SCREEN_H
