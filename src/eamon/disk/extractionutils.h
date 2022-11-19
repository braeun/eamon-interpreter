/********************************************************************************
 *                                                                              *
 * EamonInterpreter - extraction utilities for Apple ][ disk image readers      *
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

#ifndef UTILS_H
#define UTILS_H

#include <QByteArray>
#include <QChar>
#include <QImage>
#include <QString>
#include <vector>

class ExtractionUtils
{
public:

  static QChar getAscii(uint8_t c);

  static uint32_t getShortLH(const QByteArray& a, uint32_t offset);

  static QString convertText(const QByteArray& a);

  /**
   * @brief Converts a random access text file keeping record length by using spaces.
   * @param a
   * @return
   */
  static QString convertTextKeepLength(const QByteArray& a);

  /**
   * @brief Converts the text in a byte array keeping zero bytes intact.
   * @param a the input array
   * @return an array containing the converted data
   */
  static QByteArray convertByteArray(const QByteArray& a);

  static QString convertApplesoftBasic(const QByteArray& data);

  static QString convertIntegerBasic(const QByteArray& data);

  static QImage convertHiresBW(const QByteArray& data, uint32_t foreground=0xFFFFFFFF, uint32_t background=0);

  static QImage convertHiresBW(const std::vector<uint8_t>& data, uint32_t foreground=0xFFFFFFFF, uint32_t background=0);

  static QImage convertHiresColor(const QByteArray& data);

  static bool extractDisk(const QString& diskimg, const QString& dirname);

private:
  ExtractionUtils();

  static void saveText(const QString &filename, const QString &text);

  static void saveByteArray(const QString &filename, const QByteArray &a);


  static const QChar screen_map[];
};

#endif // UTILS_H
