/*
 * Console
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
#ifndef CENISYS_CONSOLE_H
#define CENISYS_CONSOLE_H

#include "command/commandsender.h"
#include "server/consolebackend.h"
#include <memory>

namespace cenisys
{

class Console : public CommandSender
{
public:
    Console(Server &server, ConsoleBackend &backend)
        : _server(server), _backend(&backend)
    {
        _backend->attach(*this);
    }
    Console(const Console &) = delete;
    Console(Console &&other) = delete;
    ~Console()
    {
        if(_backend)
            _backend->detach();
    }

    Server &getServer() { return _server; }
    void sendMessage(const boost::locale::format &content) { log(content); }

    void log(const boost::locale::format &content) { _backend->log(content); }

private:
    Server &_server;
    ConsoleBackend *_backend;
};

} // namespace cenisys

#endif // CENISYS_CONSOLE_H
