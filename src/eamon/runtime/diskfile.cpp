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

#include "diskfile.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <filesystem>

int DiskFile::errcode = 0;

DiskFile::DiskFile():
  filename(""),
  recordlength(0),
  modified(false)
{
}

DiskFile::DiskFile(const std::string& filename, int recordlength):
  filename(filename),
  recordlength(recordlength),
  modified(false)
{
  if (recordlength == 0)
    readSequential();
  else
    readRandom();
}

DiskFile::~DiskFile()
{
  if (modified)
  {
    if (recordlength == 0)
      writeSequential();
    else
      writeRandom();
  }
}

const std::string& DiskFile::getFilename() const
{
  return filename;
}

void DiskFile::setIndex(uint32_t i, bool read)
{
  errcode = 0;
  index = i;
  if (recordlength > 0)
  {
    if (read && index >= records.size())
    {
      errcode = 5;
      throw std::runtime_error("Out of data");
    }
    while (index >= records.size())
    {
      records.push_back(std::vector<char>(recordlength));
    }
  }
  pos = 0;
}

std::string DiskFile::read()
{
  errcode = 0;
  if (recordlength == 0)
  {
    if (index >= lines.size())
    {
      errcode = 5;
      throw  std::runtime_error("End of data");
    }
    return lines[index++];
  }
  const char* ptr = records[index].data() + pos;
  if (*ptr == '\0') return "";
  int n = 0;
  while (ptr[n] != '\r' && ptr[n] != '\n' && pos < recordlength)
  {
    pos++;
    n++;
  }
  if (pos < recordlength) pos++; /* skip the cr/lf */
  std::string s(ptr,n);
  return s;
}

void DiskFile::erase()
{
  errcode = 0;
  if (recordlength == 0)
  {
    lines.clear();
  }
  else
  {
    records[index] = std::vector<char>(recordlength);
  }
}

void DiskFile::write(const std::string &s)
{
  errcode = 0;
  modified = true;
  if (recordlength == 0)
  {
    if (lines.empty() || lines.back().find('\n') == std::string::npos)
      lines.push_back(s);
    else
      lines.back() += s;
  }
  else
  {
    char *ptr = records[index].data();
    int n = recordlength;
    while (*ptr && n > 0)
    {
      ptr++;
      n--;
    }
    strncpy(ptr,s.c_str(),std::min(static_cast<int>(s.length()),n));
  }
}



int DiskFile::getErrorCode()
{
  return errcode;
}

void DiskFile::verify(const std::string &filename)
{
  errcode = 0;
  if (!std::filesystem::exists(filename))
  {
    errcode = 6;
    throw std::runtime_error("File not found");
  }
}

std::vector<uint8_t> DiskFile::readBinaryFile(const std::string &file)
{
  std::vector<uint8_t> data;
  std::ifstream is(file);
  if (!is.is_open()) return data;
  while (!is.eof())
  {
    char c;
    if (is.get(c))
    {
      data.push_back(static_cast<uint8_t>(c));
    }
  }
  return data;
}

std::string DiskFile::correctFilename(std::string file)
{
  std::replace(file.begin(),file.end(),' ','_');
  std::replace(file.begin(),file.end(),'\'','_');
  std::transform(file.begin(),file.end(),file.begin(),::tolower);
  return file;
}






void DiskFile::readSequential()
{
  index = 0;
  std::ifstream is(filename);
  if (!is.is_open()) return;
  std::vector<char> buffer;
  while (!is.eof())
  {
    char c;
    if (is.get(c))
    {
      if (c == '\r' || c == '\n')
      {
        std::string line(buffer.data(),buffer.size());
        lines.push_back(line);
        buffer.clear();
      }
      else
        buffer.push_back(c);
    }
  }
  if (!buffer.empty())
  {
    std::string line(buffer.data(),buffer.size());
    lines.push_back(line);
  }
}

void DiskFile::readRandom()
{
  index = 0;
  std::ifstream is(filename);
  if (!is.is_open()) return;
  while (!is.eof())
  {
    std::vector<char> record(recordlength);
    is.read(reinterpret_cast<char*>(record.data()),recordlength);
    records.push_back(record);
  }
}

void DiskFile::writeSequential()
{
  std::ofstream os(filename);
  for (const auto& s : lines)
  {
    os << s;
  }
}

void DiskFile::writeRandom()
{
  std::ofstream os(filename);
  for (const auto& r : records)
  {
    os.write(r.data(),recordlength);
  }
}
