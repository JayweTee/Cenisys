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
#include "config.h"
#if defined(UNIX)

#include <unistd.h>
#include <future>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>
#include "server/terminal/terminalcolor.h"
#include "server/terminal/posixasyncterminalconsole.h"

namespace cenisys
{

PosixAsyncTerminalConsole::PosixAsyncTerminalConsole(
    boost::asio::io_service &ioService, bool enableColor)
    : _enableColor(enableColor), _ioService(ioService),
      _stdin(ioService, dup(STDIN_FILENO)),
      _stdout(ioService, dup(STDOUT_FILENO)), _writeStrand(ioService)
{
}

PosixAsyncTerminalConsole::~PosixAsyncTerminalConsole()
{
}

void PosixAsyncTerminalConsole::attach(Console &console)
{
    {
        std::lock_guard<std::mutex> lock(_consoleLock);
        _console = &console;
    }
    asyncRead();
}

void PosixAsyncTerminalConsole::detach()
{
    _stdin.cancel();
    {
        std::lock_guard<std::mutex> lock(_consoleLock);
        _console = nullptr;
    }
}

void PosixAsyncTerminalConsole::log(const boost::locale::format &content)
{
    std::string message = (_enableColor ? ansiColorize(content.str())
                                        : stripColor(content.str())) +
                          '\n';
    _writeStrand.dispatch(
        [ this, self(shared_from_this()), message = std::move(message) ]
        {
            bool flag = _writeBuffer.size() == 0;
            _writeBuffer.commit(
                boost::asio::buffer_copy(_writeBuffer.prepare(message.size()),
                                         boost::asio::buffer(message)));
            if(flag)
                asyncWrite();
        });
}

void PosixAsyncTerminalConsole::asyncRead()
{
    boost::asio::async_read_until(
        _stdin, _readBuffer, '\n',
        [ this, self(shared_from_this()) ](const boost::system::error_code &ec,
                                           std::size_t bytes_transferred)
        {
            if(ec == boost::asio::error::operation_aborted)
                return;
            if(ec == boost::asio::error::eof)
            {
                std::lock_guard<std::mutex> lock(_consoleLock);
                if(_console)
                {
                    _console->getServer().processEvent(
                        [&]
                        {
                            _console->getServer().terminate();
                        });
                }
                return;
            }

            std::string buf(
                boost::asio::buffer_cast<const char *>(_readBuffer.data()),
                bytes_transferred - 1);
            _readBuffer.consume(bytes_transferred);
            std::lock_guard<std::mutex> lock(_consoleLock);
            if(_console)
            {
                if(!buf.empty())
                {
                    _console->getServer().processEvent(
                        [&]
                        {
                            _console->getServer().dispatchCommand(
                                *_console, std::move(buf));
                        });
                }
                asyncRead();
            }
        });
}

void PosixAsyncTerminalConsole::asyncWrite()
{
    boost::asio::async_write(
        _stdout, _writeBuffer,
        _writeStrand.wrap([ this, self(shared_from_this()) ](
            const boost::system::error_code &ec, std::size_t bytes_transferred)
                          {
                              if(_writeBuffer.size() > 0)
                                  asyncWrite();
                          }));
}

} // namespace cenisys

#endif
