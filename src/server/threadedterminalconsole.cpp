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
#include "server/server.h"
#include "threadedterminalconsole.h"

namespace cenisys
{

ThreadedTerminalConsole::ThreadedTerminalConsole()
{
}

ThreadedTerminalConsole::~ThreadedTerminalConsole()
{
}

void ThreadedTerminalConsole::attach(Console &console)
{
    _console = &console;
    _running = true;
    _readThread = std::thread(&ThreadedTerminalConsole::readWorker, this);
    _writeThread = std::thread(&ThreadedTerminalConsole::writeWorker, this);
}

void ThreadedTerminalConsole::detach()
{
    std::unique_lock<std::mutex> lock(_writeQueueLock);
    _running = false;
    lock.unlock();
    _writeQueueNotifier.notify_all();
    _writeThread.join();
    if(std::cin)
    {
        std::cerr << boost::locale::translate("Please press Enter to continue…")
                         .str()
                  << std::endl;
    }
    _readThread.join();
    _console = nullptr;
}

void ThreadedTerminalConsole::readWorker()
{
    while(_running)
    {
        std::string buf;
        std::getline(std::cin, buf);
        if(!buf.empty())
            _console->getServer().processEvent(
                [ this, buf = std::move(buf) ]
                {
                    _console->getServer().dispatchCommand(*_console,
                                                          std::move(buf));
                });
        if(!std::cin)
        {
            _console->getServer().terminate();
            break;
        }
    }
}

void ThreadedTerminalConsole::writeWorker()
{
    std::unique_lock<std::mutex> lock(_writeQueueLock);
    while(_running || !_writeQueue.empty())
    {
        _writeQueueNotifier.wait(lock, [this]() -> bool
                                 {
                                     return !_running || !_writeQueue.empty();
                                 });
        std::string buf;
        // Ensure everything is written
        while(!_writeQueue.empty())
        {
            buf += std::move(_writeQueue.front()) + '\n';
            _writeQueue.pop();
        }
        lock.unlock();
        std::cout << buf << std::flush;
        lock.lock();
    }
}

void ThreadedTerminalConsole::log(const boost::locale::format &content)
{
    std::unique_lock<std::mutex> lock(_writeQueueLock);
    _writeQueue.push(content.str());
    lock.unlock();
    _writeQueueNotifier.notify_one();
}

} // namespace cenisys
