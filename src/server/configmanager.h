/*
 * ConfigManager
 * Copyright (C) 2016 iTX Technologies
 *
 * This file is part of Cenisys.
 *
 * Cenisys is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cenisys is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cenisys.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef CENISYS_CONFIGMANAGER_H
#define CENISYS_CONFIGMANAGER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <boost/filesystem/path.hpp>

namespace cenisys
{

class Server;
class ConfigSection;

class ConfigManager
{
public:
    ConfigManager(Server &server, const boost::filesystem::path &basepath);
    ~ConfigManager();
    std::shared_ptr<ConfigSection> getConfig(const std::string &name);

private:
    Server &_server;
    boost::filesystem::path _basepath;
    std::unordered_map<std::string, std::weak_ptr<ConfigSection>> _loadedConfig;
};

} // namespace cenisys

#endif // CENISYS_CONFIGMANAGER_H
