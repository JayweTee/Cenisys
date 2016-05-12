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

#include "server/consolebackend.h"
#include "server/server.h"
#include <atomic>
#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>
#include <condition_variable>
#include <locale>
#include <mutex>
#include <queue>
#include <thread>

namespace cenisys
{

class ThreadedTerminalConsole : public ConsoleBackend
{
public:
    ThreadedTerminalConsole(bool enableColor);
    ~ThreadedTerminalConsole();

    void attach(Console &console);
    void detach();

    void log(const boost::locale::format &content);

private:
    void readWorker();
    void writeWorker();

    bool _enableColor;

    Console *_console;
    std::atomic_bool _running;

    std::thread _readThread;
    std::thread _writeThread;
    std::locale _locale;
    std::queue<std::string> _writeQueue;
    std::mutex _writeQueueLock;
    std::condition_variable _writeQueueNotifier;
};

} // namespace cenisys

#endif // CENISYS_THREADEDTERMINALCONSOLE_H
