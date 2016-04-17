/*
 * DefaultCommandHandlers
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
#include "command/commandsender.h"
#include "defaultcommandhandlers.h"

namespace cenisys
{

DefaultCommandHandlers::DefaultCommandHandlers(Server &server) : _server(server)
{
    _server.registerCommand(
        "stop", boost::locale::translate("Terminate the server"),
        [this](CommandSender &sender, const std::string &command)
        {
            _server.terminate();
        });
    _server.registerCommand(
        "version", boost::locale::translate("Show the server version"),
        [this](CommandSender &sender, const std::string &command)
        {
            sender.sendMessage(
                boost::locale::format(boost::locale::translate("Cenisys {1}")) %
                SERVER_VERSION);
        });
}

DefaultCommandHandlers::~DefaultCommandHandlers()
{
    for(const auto &item : _handles)
        _server.unregisterCommand(item);
}

} // namespace cenisys
