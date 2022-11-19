/********************************************************************************
 *                                                                              *
 * EamonInterpreter - game screen                                               *
 *                                                                              *
 * modified: 2022-11-19                                                         *
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

#include "screen.h"
#include "defines.h"
#include "disk/extractionutils.h"
#include <QDebug>
#include <QKeyEvent>
#include <QPainter>
#include <QSettings>
#include <QSizePolicy>

static const int style_normal = 0x0;
static const int style_inverse = 0x1;

Screen::Screen(QWidget *parent):QFrame(parent),
  ncols(40),
  nrows(24),
  crow(0),
  ccol(0),
  linespacing(0),
  lines(24),
  styles(24),
  currentStyle(style_normal),
  border(2),
  mode(Text),
  punctuation(false)
{
  clear(false);
  updateSettings();
}

void Screen::print(const QString &txt)
{
  for (int i=0;i<txt.length();i++)
  {
    if (txt[i] == '\n')
    {
      newline();
    }
    else
    {
      QChar c = txt[i];
      if (lowerCase)
      {
        if (capitalize && punctuation && c.isLetterOrNumber())
        {
          punctuation = false;
        }
        else
          c = c.toLower();
      }
      lines[crow][ccol] = c;
      styles[crow][ccol] = currentStyle;
      ccol++;
      if (ccol == ncols) newline();
      if (!punctuation) punctuation = isPuntuation(c);
    }
  }
  update();
}

void Screen::backspace()
{
  ccol--;
  if (ccol < 0)
  {
    crow--;
    ccol = ncols - 1;
    if (crow < 0)
    {
      crow = 0;
      ccol = 0;
    }
  }
  lines[crow][ccol] = ' ';
  update();
}

void Screen::clear(bool doupdate)
{
  for (QString& line : lines)
  {
    line = QString(ncols,' ');
  }
  for (auto& line : styles)
  {
    line = std::vector<int>(ncols,style_normal);
  }
  crow = 0;
  ccol = 0;
  capitalize = true;
  if (doupdate) update();
}

void Screen::moveToColumn(int c)
{
  ccol = c;
  if (ccol < 0) ccol = 0;
  if (ccol >= ncols) ccol = ncols - 1;
}

void Screen::moveToRow(int r)
{
  crow = r;
  if (crow < 0) crow = 0;
  if (crow >= nrows) crow = nrows - 1;
}

void Screen::inverse()
{
  currentStyle = style_inverse;
}

void Screen::normal()
{
  currentStyle = style_normal;
}

void Screen::updateSettings()
{
  QSettings settings;
  font.fromString(settings.value(SETTING_SCREEN_FONT,SETTING_VALUE_SCREEN_FONT).toString());
  setFont(font);
  lowerCase = settings.value(SETTING_SCREEN_LOWERCASE,SETTING_VALUE_SCREEN_LOWERCASE).toBool();
  capitalize = settings.value(SETTING_SCREEN_CAPITALIZE,SETTING_VALUE_SCREEN_CAPITALIZE).toBool();
  foreground = settings.value(SETTING_SCREEN_TEXT_COLOR,SETTING_VALUE_SCREEN_TEXT_COLOR).value<QColor>();
  background = settings.value(SETTING_SCREEN_BKG_COLOR,SETTING_VALUE_SCREEN_BKG_COLOR).value<QColor>();
  linespacing = settings.value(SETTING_SCREEN_LINESPACING,SETTING_VALUE_SCREEN_LINESPACING).toInt();
  QFontMetrics fm(font);
  int h = nrows * (fm.height() + linespacing) + 2 * lineWidth() + 2 * border;
  int w = ncols * fm.averageCharWidth() + 2 * lineWidth() + 2 * border;
  setMinimumSize(w,h);
  setMaximumSize(w,h);
  setSizePolicy(QSizePolicy::Policy::Fixed,QSizePolicy::Policy::Fixed);
  resize(w,h);
}

void Screen::setImage(const std::vector<uint8_t> &data)
{
  if (data.size() >= 8184 && data.size() <= 8192)
  {
    image = ExtractionUtils::convertHiresBW(data,foreground.rgba(),background.rgba());
    update();
  }
}

void Screen::setMode(Mode m)
{
  mode = m;
  update();
}

void Screen::setPunctuation(bool flag)
{
  punctuation = flag;
}

int Screen::getCursorColumn() const
{
  return ccol;
}

int Screen::getCursorRow() const
{
  return crow;
}






void Screen::paintEvent(QPaintEvent* event)
{
  QFrame::paintEvent(event);
  switch (mode)
  {
    case Text:
      paintText();
      break;
    case Graphics:
      paintGrapics();
      break;
  }
}




void Screen::paintText()
{
  QPainter p(this);
  QFontMetrics fm = p.fontMetrics();
  p.setRenderHint(QPainter::Antialiasing,false);
  int x = contentsRect().x();
  int y = contentsRect().y();
  p.fillRect(contentsRect().x(),contentsRect().y(),contentsRect().width(),contentsRect().height(),background);
  p.setPen(foreground);
  /* Render Text */
  int sy = y + border + fm.height() - fm.descent() + linespacing / 2;
  for (int row=0;row<nrows;row++)
  {
    const auto& line = lines[row];
    const auto& linestyles = styles[row];
    int s = linestyles[0];
    int startcol = 0;
    int sx = x + border;
    for (int col=0;col<ncols;col++)
    {
      if (linestyles[col] != s)
      {
        QString text = line.mid(startcol,col-startcol);
        applyStyle(p,s,sx,sy,fm.horizontalAdvance(text));
        p.drawText(sx,sy,text);
        sx += fm.horizontalAdvance(text);
        startcol = col;
        s = linestyles[col];
      }
    }
    QString text = line.mid(startcol,ncols-startcol);
    applyStyle(p,s,sx,sy,fm.horizontalAdvance(text));
    p.drawText(sx,sy,text);
    sy += fm.height() + linespacing;
  }
}

void Screen::paintGrapics()
{
  QPainter p(this);
  QRect r(contentsRect());
  p.fillRect(r,background);
  r.adjust(border,border,-border,-border);
  p.drawImage(r,image);
}



void Screen::newline()
{
  crow++;
  ccol = 0;
  if (crow >= nrows)
  {
    lines.erase(lines.begin());
    lines.push_back(QString(ncols,' '));
    styles.erase(styles.begin());
    styles.push_back(std::vector<int>(ncols,style_normal));
    crow = nrows - 1;
  }
  if (crow > 2)
  {
    if (!punctuation) punctuation = lines[crow-1].trimmed().isEmpty() && lines[crow-2].trimmed().isEmpty();
  }
}

void Screen::applyStyle(QPainter &p, int style, int sx, int sy, int w)
{
  p.setPen(foreground);
  QFontMetrics fm = p.fontMetrics();
  p.fillRect(sx,sy+fm.descent()-fm.height()-linespacing/2,w,fm.height()+linespacing,background);
  if (style & style_inverse)
  {
    p.setPen(background);
    p.fillRect(sx,sy+fm.descent()-fm.height()-linespacing/2,w,fm.height()+linespacing,foreground);
  }
}

bool Screen::isPuntuation(QChar c)
{
  if (c == '.') return true;
  if (c == '!') return true;
  if (c == '?') return true;
  return false;
}

