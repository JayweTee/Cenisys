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
#include <iostream>
#include <boost/locale/format.hpp>
#include <boost/locale/message.hpp>
#include "server/server.h"
#include "threadedterminalconsole.h"

namespace cenisys
{

ThreadedTerminalConsole::ThreadedTerminalConsole(Server &server)
    : _server(server)
{
    _running = true;
    _readThread = std::thread(&ThreadedTerminalConsole::readWorker, this);
    _writeThread = std::thread(&ThreadedTerminalConsole::writeWorker, this);
    _loggerBackendHandle = _server.registerBackend(std::make_tuple(
        std::bind(&ThreadedTerminalConsole::log<boost::locale::format>, this,
                  std::placeholders::_1),
        std::bind(&ThreadedTerminalConsole::log<boost::locale::message>, this,
                  std::placeholders::_1)));
}

ThreadedTerminalConsole::~ThreadedTerminalConsole()
{
    _server.unregisterBackend(_loggerBackendHandle);
    std::unique_lock<std::mutex> lock(_writeQueueLock);
    _running = false;
    lock.unlock();
    if(std::cin)
        std::cerr << boost::locale::translate("Please press Enter to continue…")
                         .str()
                  << std::endl;
    _readThread.join();
    _writeQueueNotifier.notify_all();
    _writeThread.join();
}

void ThreadedTerminalConsole::readWorker()
{
    while(_running)
    {
        std::string buf;
        std::getline(std::cin, buf);
        if(!buf.empty())
            _server.processEvent(
                std::bind(&Server::dispatchCommand, &_server, buf));
        if(!std::cin)
        {
            _server.terminate();
            break;
        }
    }
}

void ThreadedTerminalConsole::writeWorker()
{
    while(_running || !_writeQueue.empty())
    {
        std::unique_lock<std::mutex> lock(_writeQueueLock);
        if(_running && _writeQueue.empty())
            _writeQueueNotifier.wait(lock);
        std::string buf;
        // Ensure everything is written
        while(!_writeQueue.empty())
        {
            buf += _writeQueue.front();
            buf += '\n';
            _writeQueue.pop();
        }
        lock.unlock();
        std::cout << buf << std::flush;
    }
}

template <typename T>
void ThreadedTerminalConsole::log(const T &content)
{
    std::unique_lock<std::mutex> lock(_writeQueueLock);
    _writeQueue.push(content.str());
    lock.unlock();
    _writeQueueNotifier.notify_one();
}

} // namespace cenisys
