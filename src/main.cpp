/*
 * Launcher for the server.
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

#include "config.h"
#include "server/server.h"
#include <boost/filesystem/path.hpp>
#include <boost/locale/format.hpp>
#include <boost/locale/generator.hpp>
#include <boost/locale/message.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <locale>
#include <memory>
#include <string>
#include <vector>

int main(int argc, char *argv[])
{
    boost::locale::generator localeGen;
    localeGen.add_messages_path(PACKAGE_LOCALE_DIR);
    localeGen.set_default_messages_domain(GETTEXT_PACKAGE);
    std::locale oldLoc = std::locale::global(localeGen(""));
    std::vector<boost::filesystem::path> dataDir;
    boost::program_options::options_description desc;
    desc.add_options()(
        "help,h",
        boost::locale::translate("display this help and exit").str().c_str())(
        "version,V",
        boost::locale::translate("output version information and exit")
            .str()
            .c_str())(
        "data-dir,d",
        boost::program_options::value<std::vector<boost::filesystem::path>>(
            &dataDir)
            ->value_name("directory")
            ->default_value({"."}, "current directory"),
        boost::locale::translate("path to the data directory").str().c_str());
    boost::program_options::variables_map vm;
    boost::program_options::store(
        boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);

    if(vm.count("help"))
    {
        std::cout << desc;
        return 0;
    }

    if(vm.count("version"))
    {
        std::locale oldCoutLoc = std::cout.imbue(std::locale());
        std::cout
            << boost::locale::format(boost::locale::translate(
                   "cenisyslauncher (Cenisys) {1}\n")) %
                   SERVER_VERSION
            << boost::locale::translate(
                   "Copyright (C) 2016 iTX Technologies.\n"
                   "License GPLv3+: GNU GPL version 3 or later "
                   "<http://gnu.org/licenses/gpl.html>.\n"
                   "This is free software: you are free to change and "
                   "redistribute it.\n"
                   "There is NO WARRANTY, to the extent permitted by law.\n");
        std::cout.imbue(oldCoutLoc);
        return 0;
    }

    std::unique_ptr<cenisys::Server> server =
        std::make_unique<cenisys::Server>(dataDir[0], localeGen);
    int ret = server->run();
    std::locale::global(oldLoc);
    return ret;
}
