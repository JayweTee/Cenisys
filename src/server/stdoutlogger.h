/*
 * Logger for standard output.
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
#ifndef CENISYS_STDOUTLOGGER_H
#define CENISYS_STDOUTLOGGER_H

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <boost/asio/io_service.hpp>
#include "server/server.h"

namespace cenisys
{

class StdoutLogger
{
public:
    StdoutLogger(Server &server, boost::asio::io_service &ioService);
    ~StdoutLogger();

private:
    void log(const std::string &content);
    void asyncWorker();

    Server &_server;
    boost::asio::io_service &_ioService;
    ServerLogger::RegisteredLoggerBackend _backendHandle;
    bool _running;
    std::thread _asyncThread;
    std::queue<std::string> _writeQueue;
    std::mutex _writeQueueLock;
    std::condition_variable _queueNotifier;
};

} // namespace cenisys

#endif // CENISYS_STDOUTLOGGER_H