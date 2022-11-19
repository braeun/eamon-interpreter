/********************************************************************************
 *                                                                              *
 * EamonInterpreter - Apple ][ dos disk image reader                            *
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

#ifndef DOSDISK_H
#define DOSDISK_H

#include "abstractdisk.h"
#include <QByteArray>
#include <QFile>
#include <vector>


class DosDisk: public AbstractDisk
{
public:
  DosDisk(const QString& filename);
  virtual ~DosDisk();

  bool isOpen() const;

  /**
   * @brief Read a sector from the disk
   * @param track the track number (starting with 0)
   * @param sector the sector number (starting with 0)
   * @return the 256 bytes of data
   */
  QByteArray readSector(uint32_t track, uint32_t sector);

  uint32_t getDosReleaseNumber() const;

  uint32_t getVolumeNumber() const;

  uint32_t getTracksPerDisk() const;

  uint32_t getSectorsPerTrack() const;

  uint32_t getBytesPerSector() const;

  uint32_t getFreeSpace() const;

  virtual const std::vector<DirectoryEntry>& getDirectory() const override;

  virtual QByteArray getFileData(const DirectoryEntry& e, bool compact=true) override;

private:
  void readDirectory();
  uint32_t getActualFilesize(const DirectoryEntry& e);

  static uint32_t countBits(uint32_t v);

  QFile* file;
  uint32_t dosReleaseNumber;
  uint32_t volumeNumber;
  uint32_t tracksPerDisk;
  uint32_t sectorsPerTrack;
  uint32_t bytesPerSector;
  uint32_t firstCatalogTrack;
  uint32_t firstCatalogSector;
  uint32_t freeSpace;
  std::vector<uint32_t> freeSectors;
  std::vector<DirectoryEntry> directory;
};

#endif // DOSDISK_H
