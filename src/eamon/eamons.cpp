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

#include "eamons.h"
#include "runtime/diskfile.h"
#include <QDomDocument>
#include <QFile>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <fstream>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

const char* Eamons::main_hall_name = "main_hall";
const char* Eamons::players_manual = "player_s_manual";
const char* Eamons::display_characters_script = "display_characters";
const char* Eamons::resurrect_character_script = "resurrect_character";
const char* Eamons::character_file_maintenance_script = "eamon_character_file_maint";
const char* Eamons::view_remove_character_script = "view_remove_characters";

const char* Eamons::pathList = TOSTRING(SOURCE_DIR) ";/usr/share/eamon;/usr/local/share/eamon";

Eamons::Eamons(const std::string& path):
  gamepath(path)
{
  main_hall = findDisk(main_hall_name);
  scan();
}

const std::string& Eamons::getPath() const
{
  return gamepath;
}

const std::string& Eamons::getMainHall() const
{
  return main_hall;
}

std::vector<std::string> Eamons::getGames() const
{
  std::vector<std::string> list;
  for (const auto& entry : games)
  {
    list.push_back(entry.first);
  }
  return list;
}

const EamonInfo& Eamons::getGameInfo(const std::string &game) const
{
  return games.at(game);
}

bool Eamons::exportCharacters(const std::string &filename) const
{
  QDomDocument doc("eamon");
  QDomElement root = doc.createElement("eamon");
  doc.appendChild(root);
  std::filesystem::path p = std::filesystem::path(main_hall) / "characters";
  DiskFile file(p.string(),150);
  file.setIndex(0,true);
  int n = std::stoi(file.read());
  for (int i=0;i<n;i++)
  {
    file.setIndex(i+1,true);
    QDomElement e = doc.createElement("character");
    e.setAttribute("name",QString::fromStdString(file.read()));
    e.setAttribute("hd",QString::fromStdString(file.read()));
    e.setAttribute("ag",QString::fromStdString(file.read()));
    e.setAttribute("ch",QString::fromStdString(file.read()));
    for (int j=0;j<4;j++) e.setAttribute("sa"+QString::number(j),QString::fromStdString(file.read()));
    for (int j=0;j<5;j++) e.setAttribute("wa"+QString::number(j),QString::fromStdString(file.read()));
    e.setAttribute("ae",QString::fromStdString(file.read()));
    e.setAttribute("sex",QString::fromStdString(file.read()));
    e.setAttribute("gold",QString::fromStdString(file.read()));
    e.setAttribute("bank",QString::fromStdString(file.read()));
    e.setAttribute("ac",QString::fromStdString(file.read()));
    for (int j=0;j<4;j++)
    {
      e.setAttribute("wname"+QString::number(j),QString::fromStdString(file.read()));
      e.setAttribute("wtype"+QString::number(j),QString::fromStdString(file.read()));
      e.setAttribute("woods"+QString::number(j),QString::fromStdString(file.read()));
      e.setAttribute("wdice"+QString::number(j),QString::fromStdString(file.read()));
      e.setAttribute("wsides"+QString::number(j),QString::fromStdString(file.read()));
    }
    root.appendChild(e);
  }
  QString xml = doc.toString();
  std::ofstream os(filename);
  os.write(xml.toStdString().c_str(),xml.size());
  os.close();
  return false;
}

bool Eamons::importCharacters(const std::string &filename) const
{
  QDomDocument doc("eamon");
  QFile f(QString::fromStdString(filename));
  if (!f.open(QIODevice::ReadOnly)) return false;
  if (!doc.setContent(&f))
  {
    f.close();
    return false;
  }
  f.close();
  std::filesystem::path p = std::filesystem::path(main_hall) / "characters";
  DiskFile file(p.string(),150);
  file.erase();
  int cnt = 0;
  QDomElement docElem = doc.documentElement();
  QDomNode n = docElem.firstChild();
  while(!n.isNull())
  {
    QDomElement e = n.toElement(); // try to convert the node to an element.
    if(!e.isNull())
    {
      file.setIndex(cnt+1,false);
      file.write(e.attribute("name").toStdString()+"\n");
      file.write(e.attribute("hd").toStdString()+"\n");
      file.write(e.attribute("ag").toStdString()+"\n");
      file.write(e.attribute("ch").toStdString()+"\n");
      for (int j=0;j<4;j++) file.write(e.attribute("sa"+QString::number(j)).toStdString()+"\n");
      for (int j=0;j<5;j++) file.write(e.attribute("wa"+QString::number(j)).toStdString()+"\n");
      file.write(e.attribute("ae").toStdString()+"\n");
      file.write(e.attribute("sex").toStdString()+"\n");
      file.write(e.attribute("gold").toStdString()+"\n");
      file.write(e.attribute("bank").toStdString()+"\n");
      file.write(e.attribute("ac").toStdString()+"\n");
      for (int j=0;j<4;j++)
      {
        file.write(e.attribute("wname"+QString::number(j)).toStdString()+"\n");
        file.write(e.attribute("wtype"+QString::number(j)).toStdString()+"\n");
        file.write(e.attribute("woods"+QString::number(j)).toStdString()+"\n");
        file.write(e.attribute("wdice"+QString::number(j)).toStdString()+"\n");
        file.write(e.attribute("wsides"+QString::number(j)).toStdString()+"\n");
      }
      cnt++;
    }
    n = n.nextSibling();
  }
  file.setIndex(0,false);
  file.write(std::to_string(cnt));
  return false;
}



std::string Eamons::findDisk(const std::string &name)
{
//  std::istringstream f(pathList);
  std::string path = gamepath;
//  while (std::getline(f,path,';'))
//  {
//    std::cout << path << std::endl;
//    path += "/eamons";
    if (std::filesystem::exists(path))
    {
      for (const auto & entry : std::filesystem::directory_iterator(path))
      {
        if (entry.is_directory())
        {
//          std::cout << entry.path() << std::endl;
          if (entry.path().filename() == name) return entry.path();
        }
      }
    }
//  }
  return "";
}

void Eamons::scan()
{
  games.clear();
  if (!std::filesystem::exists(gamepath)) return;
  for (const auto & entry : std::filesystem::directory_iterator(gamepath))
  {
    if (entry.is_directory())
    {
      std::filesystem::path filename = entry.path() / "eamon.name";
      std::ifstream is(filename);
      if (is.is_open())
      {
        char line[81];
        is.getline(line,sizeof(line)-1,'\r');
        if (!is.fail())
        {
          std::string name(line);
          if (!name.empty())
          {
            if (games.find(name) != games.end())
            {
              continue;
            }
            EamonInfo eamon;
            eamon.name = name;
            eamon.disk = entry.path();
            games.insert(std::make_pair(name,eamon));
          }
        }
      }
    }
  }
}
