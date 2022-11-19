/********************************************************************************
 *                                                                              *
 * EamonInterpreter - disk file handling                                        *
 *                                                                              *
 * modified: 2022-11-16                                                         *
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

#ifndef DISKFILE_H
#define DISKFILE_H

#include <string>
#include <vector>

class DiskFile
{
public:
  DiskFile();
  DiskFile(const std::string& filename, int recordlength=0);
  ~DiskFile();

  const std::string& getFilename() const;

  /**
   * @brief Set index for read/write operations.
   *
   * If the index is set for reading an exception is thrown if the index
   * points past the available data.
   * @param i the index to set (starts with 0)
   * @param read if true, the index is set for reading
   */
  void setIndex(uint32_t i, bool read);

  std::string read();

  void erase();

  void write(const std::string& s);

  static int getErrorCode();

  static void verify(const std::string& filename);

  static std::vector<uint8_t> readBinaryFile(const std::string& file);

  static std::string correctFilename(std::string file);


private:
  void readSequential();
  void readRandom();
  void writeSequential();
  void writeRandom();

  std::string filename;
  int recordlength;
  std::vector<std::vector<char>> records;
  std::vector<std::string> lines;
  uint32_t index;
  int pos;
  bool modified;

  static int errcode;
};

#endif // DISKFILE_H
