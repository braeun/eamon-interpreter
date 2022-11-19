/********************************************************************************
 *                                                                              *
 * EamonInterpreter - application palette factory                               *
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

#include "palettefactory.h"

static const char* defaultPaletteName = "Default";
static const char* darkPaletteName = "Dark";

QPalette PaletteFactory::defaultPalette;

std::vector<QString> PaletteFactory::getPaletteNames()
{
  return std::vector<QString>{defaultPaletteName,darkPaletteName};
}

QPalette PaletteFactory::getPalette(const QString &name)
{
  if (name == darkPaletteName) return darkPalette();
  return defaultPalette;
}

void PaletteFactory::registerDefaultPalette(const QPalette &p)
{
  defaultPalette = p;
}



PaletteFactory::PaletteFactory()
{
}

QPalette PaletteFactory::darkPalette()
{
  QPalette p;
  p.setColor(QPalette::Window,QColor(53,53,53));
  p.setColor(QPalette::WindowText,Qt::white);
  p.setColor(QPalette::Disabled,QPalette::WindowText,QColor(127,127,127));
  p.setColor(QPalette::Base,QColor(42,42,42));
  p.setColor(QPalette::AlternateBase,QColor(66,66,66));
  p.setColor(QPalette::ToolTipBase,Qt::black);
  p.setColor(QPalette::ToolTipText,Qt::yellow);
  p.setColor(QPalette::Text,Qt::white);
  p.setColor(QPalette::Disabled,QPalette::Text,QColor(127,127,127));
  p.setColor(QPalette::Dark,QColor(35,35,35));
  p.setColor(QPalette::Shadow,QColor(20,20,20));
  p.setColor(QPalette::Button,QColor(53,53,53));
  p.setColor(QPalette::ButtonText,Qt::white);
  p.setColor(QPalette::Disabled,QPalette::ButtonText,QColor(127,127,127));
  p.setColor(QPalette::BrightText,Qt::red);
  p.setColor(QPalette::Link,QColor(42,130,218));
  p.setColor(QPalette::Highlight,QColor(42,130,218));
  p.setColor(QPalette::Disabled,QPalette::Highlight,QColor(80,80,80));
  p.setColor(QPalette::HighlightedText,Qt::white);
  p.setColor(QPalette::Disabled,QPalette::HighlightedText,QColor(127,127,127));
  return p;
}
