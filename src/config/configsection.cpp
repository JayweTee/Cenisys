/*
 * ConfigSection
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

#include "config/configsection.h"
#include "server/server.h"
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast/try_lexical_convert.hpp>
#include <boost/locale/format.hpp>
#include <boost/locale/message.hpp>
#include <boost/system/error_code.hpp>
#include <fstream>
#include <mutex>
#include <yaml-cpp/yaml.h>

namespace
{
struct BoolAlpha
{
    bool data;
    BoolAlpha() {}
    BoolAlpha(bool data) : data(data) {}
    operator bool() const { return data; }
    friend std::ostream &operator<<(std::ostream &out, BoolAlpha b)
    {
        out << std::boolalpha << b.data;
        return out;
    }
    friend std::istream &operator>>(std::istream &in, BoolAlpha &b)
    {
        in >> std::boolalpha >> b.data;
        return in;
    }
};
}

namespace cenisys
{

ConfigSection::ConfigSection(Server &server,
                             const boost::filesystem::path &filePath)
    : _server(server), _filePath(filePath)

{
    // TODO: handle permissions
    if(boost::filesystem::is_regular_file(_filePath))
    {
        boost::filesystem::ifstream file(_filePath);
        _root = YAML::Load(file);
    }
    else
    {
        _server.log(
            Server::LogLevel::Info,
            boost::locale::format(boost::locale::translate(
                "Config file {1} does not exist, using default values")) %
                _filePath);
    }
    // HACK: yaml-cpp bug
    _root[""];
}

ConfigSection::~ConfigSection()
{
    if(!boost::filesystem::exists(_filePath.parent_path()))
    {
        boost::system::error_code err;
        boost::filesystem::create_directories(_filePath.parent_path(), err);
        // TODO: error handling of directory creating
    }
    boost::filesystem::ofstream file(_filePath);
    YAML::Emitter out(file);
    out.SetMapFormat(YAML::Block);
    out.SetSeqFormat(YAML::Block);
    out << _root;
}

bool ConfigSection::getBool(const ConfigSection::Path &path, bool defaultValue)
{
    return getValue<bool, BoolAlpha>(path, defaultValue);
}

int ConfigSection::getInt(const ConfigSection::Path &path, int defaultValue)
{
    return getValue(path, defaultValue);
}

unsigned int ConfigSection::getUInt(const ConfigSection::Path &path,
                                    unsigned int defaultValue)
{
    return getValue(path, defaultValue);
}

double ConfigSection::getDouble(const ConfigSection::Path &path,
                                double defaultValue)
{
    return getValue(path, defaultValue);
}

std::string ConfigSection::getString(const ConfigSection::Path &path,
                                     const std::string &defaultValue)
{
    return getValue(path, defaultValue);
}

std::vector<bool>
ConfigSection::getBoolList(const ConfigSection::Path &path,
                           const std::vector<bool> &defaultValue)
{
    return getList<bool, BoolAlpha>(path, defaultValue);
}

std::vector<int> ConfigSection::getIntList(const ConfigSection::Path &path,
                                           const std::vector<int> &defaultValue)
{
    return getList(path, defaultValue);
}

std::vector<unsigned int>
ConfigSection::getUIntList(const ConfigSection::Path &path,
                           const std::vector<unsigned int> &defaultValue)
{
    return getList(path, defaultValue);
}

std::vector<double>
ConfigSection::getDoubleList(const ConfigSection::Path &path,
                             const std::vector<double> &defaultValue)
{
    return getList(path, defaultValue);
}

std::vector<std::string>
ConfigSection::getStringList(const ConfigSection::Path &path,
                             const std::vector<std::string> &defaultValue)
{
    return getList(path, defaultValue);
}

void ConfigSection::setBool(const ConfigSection::Path &path, bool value)
{
    setValue(path, value);
}

void ConfigSection::setInt(const ConfigSection::Path &path, int value)
{
    setValue(path, value);
}

void ConfigSection::setUInt(const ConfigSection::Path &path, unsigned int value)
{
    setValue(path, value);
}

void ConfigSection::setDouble(const ConfigSection::Path &path, double value)
{
    setValue(path, value);
}

void ConfigSection::setString(const ConfigSection::Path &path,
                              const std::string &value)
{
    setValue(path, value);
}

void ConfigSection::setBoolList(const ConfigSection::Path &path,
                                const std::vector<bool> &value)
{
    setList(path, value);
}

void ConfigSection::setIntList(const ConfigSection::Path &path,
                               const std::vector<int> &value)
{
    setList(path, value);
}

void ConfigSection::setDoubleList(const ConfigSection::Path &path,
                                  const std::vector<double> &value)
{
    setList(path, value);
}

void ConfigSection::setStringList(const ConfigSection::Path &path,
                                  const std::vector<std::string> &value)
{
    setList(path, value);
}

std::vector<ConfigSection::Path>
ConfigSection::getKeys(const ConfigSection::Path &path)
{
    std::lock_guard<std::mutex> lock(_lock);
    YAML::Node current = correctParent(path);
    std::vector<ConfigSection::Path> result;
    std::vector<YAML::Node> removeList;
    for(const std::pair<YAML::Node, YAML::Node> &item : current)
    {
        if(!item.first.IsScalar() || !item.second.IsScalar())
        {
            _server.log(
                Server::LogLevel::Warning,
                boost::locale::format(boost::locale::translate(
                    "Invalid entry in map {1} of configuration file {2}: "
                    "removing")) %
                    path.getItems().back() % _filePath);
            removeList.emplace_back(item.first);
        }
        else
        {
            result.push_back(path / item.first.Scalar());
        }
    }
    for(const YAML::Node &key : removeList)
    {
        current.remove(key);
    }
    return result;
}

// Note: Truncate the keys before passing. This modifies everything include the
// end of the path.
YAML::Node ConfigSection::correctParent(const ConfigSection::Path &path)
{
    if(_root && !_root.IsNull())
    {
        if(!_root.IsMap())
        {
            _server.log(
                Server::LogLevel::Warning,
                boost::locale::format(boost::locale::translate(
                    "Configuration file {1} is invalid: using defaults")) %
                    _filePath);
            _root.reset();
        }
    }
    YAML::Node current = _root;
    for(const std::string &key : path.getItems())
    {
        if(current[key] && !current.IsNull())
        {
            if(!current[key].IsMap())
            {
                _server.log(
                    Server::LogLevel::Warning,
                    boost::locale::format(boost::locale::translate(
                        "Section {1} in configuration file {2} is invalid: "
                        "using defaults")) %
                        key % _filePath);
                current.remove(key);
            }
        }
        current.reset(current[key]);
    }
    return current;
}

template <typename T, typename Wrapper>
T ConfigSection::getValue(const ConfigSection::Path &path,
                          const T &defaultValue)
{
    std::lock_guard<std::mutex> lock(_lock);
    YAML::Node parent = correctParent(path.up());
    std::string key = path.getItems().back();
    Wrapper result;
    if(parent[key] && !parent.IsNull() &&
       (!parent[key].IsScalar() ||
        !boost::conversion::try_lexical_convert(parent[key].Scalar(), result)))
    {
        parent.remove(key);
        _server.log(Server::LogLevel::Warning,
                    boost::locale::format(boost::locale::translate(
                        "Item {1} in configuration file {2} is "
                        "invalid: using defaults")) %
                        key % _filePath);
    }
    if(!parent[key] || parent.IsNull())
    {
        parent[key] = defaultValue;
        return defaultValue;
    }
    return result;
}

template <typename T, typename Wrapper>
std::vector<T> ConfigSection::getList(const ConfigSection::Path &path,
                                      const std::vector<T> &defaultValue)
{
    std::lock_guard<std::mutex> lock(_lock);
    YAML::Node parent = correctParent(path.up());
    std::string key = path.getItems().back();
    std::vector<T> result;
    if(parent[key] && !parent.IsNull() &&
       (!parent[key].IsSequence() || [&result](const YAML::Node &seq) -> bool {
           for(const YAML::Node &scalar : seq)
           {
               if(!scalar.IsScalar())
                   return false;
               Wrapper value;
               if(!boost::conversion::try_lexical_convert(scalar.Scalar(),
                                                          value))
                   return false;
               result.push_back(value);
           }
           return true;
       }(parent[key])))
    {
        parent.remove(key);
        _server.log(Server::LogLevel::Warning,
                    boost::locale::format(boost::locale::translate(
                        "Item {1} in configuration file {2} is "
                        "invalid: using defaults")) %
                        key % _filePath);
    }
    if(!parent[key] || parent.IsNull())
    {
        parent[key] = defaultValue;
        return defaultValue;
    }
    return result;
}

template <typename T>
void ConfigSection::setValue(const ConfigSection::Path &path, const T &value)
{
    std::lock_guard<std::mutex> lock(_lock);
    YAML::Node parent = correctParent(path.up());
    std::string key = path.getItems().back();
    if(parent[key] && !parent.IsNull() && !parent[key].IsScalar())
    {
        parent.remove(key);
        _server.log(Server::LogLevel::Warning,
                    boost::locale::format(boost::locale::translate(
                        "Changing type of item {1} in "
                        "configuration file {2} to scalar")) %
                        key % _filePath);
    }
    parent[key] = value;
}

template <typename T>
void ConfigSection::setList(const ConfigSection::Path &path,
                            const std::vector<T> &value)
{
    std::lock_guard<std::mutex> lock(_lock);
    YAML::Node parent = correctParent(path.up());
    std::string key = path.getItems().back();
    std::vector<T> result;
    if(parent[key] && !parent.IsNull() &&
       (!parent[key].IsSequence() || [&result](const YAML::Node &seq) -> bool {
           for(const YAML::Node &scalar : seq)
           {
               if(!scalar.IsScalar())
                   return false;
           }
           return true;
       }(parent[key])))
    {
        parent.remove(key);
        _server.log(Server::LogLevel::Warning,
                    boost::locale::format(boost::locale::translate(
                        "Changing type of item {1} in "
                        "configuration file {2} to sequence")) %
                        key % _filePath);
    }
    parent[key] = value;
}

} // namespace cenisys
