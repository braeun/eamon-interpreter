/********************************************************************************
 *                                                                              *
 * EamonInterpreter - virtual base class for Apple ][ disks                     *
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

#ifndef DISK_H
#define DISK_H

#include <vector>
#include <QString>
#include <QByteArray>

class DirectoryEntry
{
public:
  enum Type { TEXT=0x00, INTEGER_BASIC=0x01, APPLESOFT_BASIC=0x02, BINARY=0x04, S_TYPE=0x08, OBJECT=0x10, A_TYPE=0x20, B_TYPE=0x40 };

  Type type;
  QString filename;
  bool locked;
  uint32_t filesizeBytes;
  uint32_t filesizeSectors;
  uint32_t trackFirstListSector;
  uint32_t sectorFirstListSector;
};


class AbstractDisk
{
public:
  AbstractDisk();
  virtual ~AbstractDisk();

  virtual const std::vector<DirectoryEntry>& getDirectory() const = 0;

  virtual QByteArray getFileData(const DirectoryEntry& e, bool compact=true) = 0;

};

#endif // DISK_H
