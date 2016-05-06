/*
 * PosixAsyncTerminalConsole
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
#ifndef CENISYS_POSIXASYNCTERMINALCONSOLE_H
#define CENISYS_POSIXASYNCTERMINALCONSOLE_H

#include "config.h"
#if defined(UNIX)

#include <memory>
#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include "config.h"
#include "server/server.h"
#include "server/consolebackend.h"

namespace cenisys
{

class PosixAsyncTerminalConsole
    : public ConsoleBackend,
      public std::enable_shared_from_this<PosixAsyncTerminalConsole>
{
public:
    PosixAsyncTerminalConsole(boost::asio::io_service &ioService);
    ~PosixAsyncTerminalConsole();

    void attach(Console &console);
    void detach();
    void log(const boost::locale::format &content);

private:
    void asyncRead();
    void asyncWrite();

    Console *_console;
    std::mutex _consoleLock;
    boost::asio::io_service &_ioService;
    boost::asio::posix::stream_descriptor _stdin;
    boost::asio::posix::stream_descriptor _stdout;

    boost::asio::streambuf _readBuffer;

    boost::asio::strand _writeStrand;
    boost::asio::streambuf _writeBuffer;
};

} // namespace cenisys

#endif

#endif // CENISYS_POSIXASYNCTERMINALCONSOLE_H
