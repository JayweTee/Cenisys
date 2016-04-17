/*
 * ThreadedTerminalConsole
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
#ifndef CENISYS_THREADEDTERMINALCONSOLE_H
#define CENISYS_THREADEDTERMINALCONSOLE_H

#include <atomic>
#include <condition_variable>
#include <locale>
#include <mutex>
#include <queue>
#include <thread>
#include "server/server.h"
#include "command/commandsender.h"

namespace cenisys
{

class ThreadedTerminalConsole : public CommandSender
{
public:
    ThreadedTerminalConsole(Server &server);
    ~ThreadedTerminalConsole();

    Server &getServer();

    void sendMessage(const boost::locale::format &content);
    void sendMessage(const boost::locale::message &content);

private:
    void readWorker();

    void writeWorker();
    template <typename T>
    void log(const T &content);

    Server &_server;
    std::atomic_bool _running;

    std::thread _readThread;

    std::thread _writeThread;
    std::locale _locale;
    Server::RegisteredLoggerBackend _loggerBackendHandle;
    std::queue<std::string> _writeQueue;
    std::mutex _writeQueueLock;
    std::condition_variable _writeQueueNotifier;
};

} // namespace cenisys

#endif // CENISYS_THREADEDTERMINALCONSOLE_H
