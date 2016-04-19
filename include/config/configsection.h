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
#ifndef CENISYS_CONFIGSECTION_H
#define CENISYS_CONFIGSECTION_H

#include <mutex>
#include <boost/filesystem/path.hpp>
#include <yaml-cpp/node/node.h>

namespace cenisys
{

class Server;

class ConfigSection
{
public:
    class Path
    {
    public:
        const std::vector<std::string> &getItems() const { return _items; }
        Path up() const
        {
            Path ret(*this);
            ret._items.pop_back();
            return ret;
        }
        friend Path operator/(const Path &lhs, const std::string &rhs)
        {
            Path ret(lhs);
            ret._items.push_back(rhs);
            return ret;
        }

    private:
        std::vector<std::string> _items;
    };

    ConfigSection(Server &server, const boost::filesystem::path &filePath);
    ~ConfigSection();

    bool getBool(const Path &path, bool defaultValue);
    int getInt(const Path &path, int defaultValue);
    unsigned int getUInt(const Path &path, unsigned int defaultValue);
    double getDouble(const Path &path, double defaultValue);
    std::string getString(const Path &path, const std::string &defaultValue);
    std::vector<bool> getBoolList(const Path &path,
                                  const std::vector<bool> &defaultValue = {});
    std::vector<int> getIntList(const Path &path,
                                const std::vector<int> &defaultValue = {});
    std::vector<unsigned int>
    getUIntList(const Path &path,
                const std::vector<unsigned int> &defaultValue = {});
    std::vector<double>
    getDoubleList(const Path &path,
                  const std::vector<double> &defaultValue = {});
    std::vector<std::string>
    getStringList(const Path &path,
                  const std::vector<std::string> &defaultValue = {});
    void setBool(const Path &path, bool value);
    void setInt(const Path &path, int value);
    void setUInt(const Path &path, unsigned int value);
    void setDouble(const Path &path, double value);
    void setString(const Path &path, const std::string &value);
    void setBoolList(const Path &path, const std::vector<bool> &value);
    void setIntList(const Path &path, const std::vector<int> &value);
    void setDoubleList(const Path &path, const std::vector<double> &value);
    void setStringList(const Path &path, const std::vector<std::string> &value);
    std::vector<Path> getKeys(const Path &path);

private:
    template <typename T>
    T getValue(const Path &path, const T &defaultValue);
    template <typename T>
    std::vector<T> getList(const Path &path,
                           const std::vector<T> &defaultValue);

    template <typename T>
    void setValue(const Path &path, const T &value);
    template <typename T>
    void setList(const Path &path, const std::vector<T> &value);

    YAML::Node correctParent(const Path &path);

    Server &_server;
    boost::filesystem::path _filePath;
    YAML::Node _root;
    std::mutex _lock;

    // TODO: implement modification monitoring
};

} // namespace cenisys

#endif // CENISYS_CONFIGSECTION_H
