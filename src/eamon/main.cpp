/********************************************************************************
 *                                                                              *
 * EamonInterpreter - application startup                                       *
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

#include "defines.h"
#include "mainwindow.h"
#include "palettefactory.h"
#include <QApplication>
#include <QDebug>
#include <QFontDatabase>
#include <QSettings>
#include <QStyleFactory>
#include <iostream>
#include <string>
#include <memory>


int main(int argc, char* argv[])
{
  QCoreApplication::setOrganizationName("hbr");
  QCoreApplication::setApplicationName("eamon");
  QCoreApplication::setApplicationVersion(QString("%1.%2.%3").arg(EAMON_VERSION_MAJOR).arg(EAMON_VERSION_MINOR).arg(EAMON_VERSION_PATCH));
  QApplication::setApplicationDisplayName("EAMON Interpreter");
  QApplication a(argc, argv);

  int32_t id = QFontDatabase::addApplicationFont(":/resources/fonts/PrintChar21.ttf");
  if (id < 0) qWarning("Failed to load font PrintChar21.ttf");

  PaletteFactory::registerDefaultPalette(QApplication::palette());

  QSettings settings;
  QString style = settings.value(SETTING_STYLE_STYLE,SETTING_VALUE_STYLE_STYLE).toString();
  QApplication::setStyle(QStyleFactory::create(style));

  QString palette = settings.value(SETTING_STYLE_PALETTE,SETTING_VALUE_STYLE_PALETTE).toString();
  if (!palette.isEmpty()) QApplication::setPalette(PaletteFactory::getPalette(palette));

  MainWindow w;
  w.setWindowTitle("EAMON");
  w.show();

  return a.exec();
}
