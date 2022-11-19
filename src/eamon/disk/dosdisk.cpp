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

#include "dosdisk.h"
#include "extractionutils.h"

#define DISKINFO_TRACK  17
#define DISKINFO_SECTOR  0
#define SECTOR_SIZE 256

DosDisk::DosDisk(const QString& filename)
{
  file = new QFile(filename);
  if (file->open(QFile::ReadOnly))
  {
    QByteArray a = readSector(DISKINFO_TRACK,DISKINFO_SECTOR);
    firstCatalogTrack = static_cast<uint8_t>(a[0x01]);
    firstCatalogSector = static_cast<uint8_t>(a[0x02]);
    dosReleaseNumber = static_cast<uint8_t>(a[0x03]);
    volumeNumber = static_cast<uint8_t>(a[0x06]);
    tracksPerDisk = static_cast<uint8_t>(a[0x34]);
    sectorsPerTrack = static_cast<uint8_t>(a[0x35]);
    bytesPerSector = ExtractionUtils::getShortLH(a,0x36);
    freeSpace = 0;
    int32_t offset = 0x38;
    for (uint32_t i=0;i<tracksPerDisk;i++)
    {
      for (uint32_t j=0;j<2;j++)
      {
        freeSpace += countBits(static_cast<uint8_t>(a[offset++]));
      }
      offset += 2; /* two bytes not used */
    }
    freeSpace *= SECTOR_SIZE;
    readDirectory();
  }
}

DosDisk::~DosDisk()
{
  file->close();
  delete file;
}

bool DosDisk::isOpen() const
{
  return file->isOpen();
}

QByteArray DosDisk::readSector(uint32_t track, uint32_t sector)
{
  if (!isOpen()) return QByteArray();
  uint32_t block = track * 16 + sector;
  file->seek(block*SECTOR_SIZE);
  return file->read(SECTOR_SIZE);
}

uint32_t DosDisk::getDosReleaseNumber() const
{
  return dosReleaseNumber;
}

uint32_t DosDisk::getVolumeNumber() const
{
  return volumeNumber;
}

uint32_t DosDisk::getTracksPerDisk() const
{
  return tracksPerDisk;
}

uint32_t DosDisk::getSectorsPerTrack() const
{
  return sectorsPerTrack;
}

uint32_t DosDisk::getBytesPerSector() const
{
  return bytesPerSector;
}

uint32_t DosDisk::getFreeSpace() const
{
  return freeSpace;
}

const std::vector<DirectoryEntry>& DosDisk::getDirectory() const
{
  return directory;
}

QByteArray DosDisk::getFileData(const DirectoryEntry &e, bool compact)
{
  QByteArray filedata;
  uint32_t tl = e.trackFirstListSector;
  uint32_t sl = e.sectorFirstListSector;
  while (tl != 0)
  {
    QByteArray list = readSector(tl,sl);
    tl = static_cast<uint8_t>(list[0x01]);
    sl = static_cast<uint8_t>(list[0x02]);
    uint32_t offset = 0x0C; //Utils::getShortLH(list,0x05);
    uint32_t last = 121;
    if (tl == 0 && sl == 0)
    {
      while (last > 0)
      {
        uint32_t t = static_cast<uint32_t>(list[offset+last]);
        uint32_t s = static_cast<uint32_t>(list[offset+last]);
        if (t > 0 || s > 0) break;
        --last;
      }
    }
    for (uint32_t i=0;i<=last;i++)
    {
      uint32_t t = static_cast<uint32_t>(list[offset++]);
      uint32_t s = static_cast<uint32_t>(list[offset++]);
      if (t > 0 || s > 0)
      {
        filedata.append(readSector(t,s));
      }
      else if (!compact)
      {
        filedata.append(bytesPerSector,'\0');
      }
    }
  }
  if (e.type == DirectoryEntry::BINARY)
  {
    uint32_t length = ExtractionUtils::getShortLH(filedata,2);
    filedata = filedata.mid(4,length);
  }
  else if (e.type == DirectoryEntry::APPLESOFT_BASIC || e.type == DirectoryEntry::INTEGER_BASIC)
  {
    uint32_t length = ExtractionUtils::getShortLH(filedata,0);
    filedata = filedata.mid(2,length);
  }
  return filedata;
}





void DosDisk::readDirectory()
{
  directory.clear();
  uint32_t t = firstCatalogTrack;
  uint32_t s = firstCatalogSector;
  while (t > 0)
  {
    QByteArray a = readSector(t,s);
    t = static_cast<uint8_t>(a[0x01]);
    s = static_cast<uint8_t>(a[0x02]);
    int32_t offset = 0x0B;
    for (int32_t i=0;i<7;i++)
    {
      if (static_cast<uint8_t>(a[offset]) != 0x00 && static_cast<uint8_t>(a[offset]) != 0xFF)
      {
        DirectoryEntry e;
        e.trackFirstListSector = static_cast<uint8_t>(a[offset+0x00]);
        e.sectorFirstListSector = static_cast<uint8_t>(a[offset+0x01]);
        uint8_t type = static_cast<uint8_t>(a[offset+0x02]);
        e.locked = (type & 0x80) != 0;
        e.type = static_cast<DirectoryEntry::Type>(type&0x7F);
        e.filename = "";
        for (int32_t i=offset+0x03;i<offset+0x21;i++)
        {
          e.filename += ExtractionUtils::getAscii(static_cast<uint8_t>(a[i]));
        }
//        e.filename = a.mid(offset+0x03,30);
        /* note: this is only one byte -> filesize wraps around on 256 sectors */
        e.filesizeSectors = ExtractionUtils::getShortLH(a,offset+0x21);
        /* calculate filesize from used sectors first */
        e.filesizeBytes = e.filesizeSectors * bytesPerSector;
        /* get the actual filesize from the file or return the original value */
        e.filesizeBytes = getActualFilesize(e);
        directory.push_back(e);
      }
      offset += 0x23;
    }
  }
}

uint32_t DosDisk::getActualFilesize(const DirectoryEntry &e)
{
  QByteArray data = getFileData(e,false);
  uint32_t size = e.filesizeBytes;
  if (!data.isEmpty())
  {
    if (e.type == DirectoryEntry::TEXT)
    {
      size = data.size();
    }
    else if (e.type == DirectoryEntry::BINARY)
    {
      size = ExtractionUtils::getShortLH(data,0x02);
    }
    else if (e.type == DirectoryEntry::INTEGER_BASIC || e.type == DirectoryEntry::APPLESOFT_BASIC)
    {
      size = ExtractionUtils::getShortLH(data,0x00);
    }
  }
  return size;
}


uint32_t DosDisk::countBits(uint32_t v)
{
  uint32_t cnt = 0;
  while (v)
  {
    cnt += (v & 1);
    v >>= 1;
  }
  return cnt;
}


