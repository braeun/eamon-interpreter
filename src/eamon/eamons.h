/********************************************************************************
 *                                                                              *
 * EamonInterpreter - game collection and information                           *
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

#ifndef EAMONS_H
#define EAMONS_H

#include <string>
#include <vector>
#include <map>

class EamonInfo {
public:
  std::string name;
  std::string disk;
};

class Eamons
{
public:
  Eamons(const std::string& path);

  const std::string& getPath() const;

  const std::string& getMainHall() const;

  std::vector<std::string> getGames() const;

  const EamonInfo& getGameInfo(const std::string& game) const;

  bool exportCharacters(const std::string& filename) const;

  bool importCharacters(const std::string& filename) const;


  static const char* main_hall_name;

  static const char* players_manual;

  static const char* display_characters_script;

  static const char* resurrect_character_script;

  static const char* character_file_maintenance_script;

  static const char* view_remove_character_script;

private:
  std::string findDisk(const std::string& name);
  void scan();

  std::string gamepath;
  std::string main_hall;
  std::map<std::string,EamonInfo> games;

  static const char* pathList;

};

#endif // EAMONS_H
