/*
 * Implementation of the main server.
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
#include <functional>
#include <mutex>
#include <iostream>
#include <locale>
#include <boost/locale/format.hpp>
#include <boost/locale/generator.hpp>
#include <boost/locale/message.hpp>
#include "config.h"
#include "config/configsection.h"
#include "command/commandsender.h"
#include "command/defaultcommandhandlers.h"
#include "server/threadedterminalconsole.h"
#include "server/server.h"

namespace cenisys
{

Server::Server(const boost::filesystem::path &dataDir,
               boost::locale::generator &localeGen)
    : _counter(0), _dataDir(dataDir), _localeGen(localeGen),
      _termSignals(_ioService, SIGINT, SIGTERM),
      _configManager(*this, _dataDir / "config")
{
}

Server::~Server()
{
}

int Server::run()
{
    _ioService.post([this]
                    {
                        start();
                    });
    _ioService.run();
    // Currently there's no way to terminate threads in io_service.
    for(auto &item : _threads)
    {
        item.join();
    }
    return 0;
}

std::locale Server::getLocale(std::string locale)
{
    return _localeGen(locale);
}

bool Server::dispatchCommand(CommandSender &sender, const std::string &command)
{
    std::lock_guard<std::mutex> lock(_commandListLock);
    auto commandName = command.substr(0, command.find(' '));
    const auto &it = _commandList.find(commandName);
    if(it != _commandList.end())
    {
        std::get<Server::CommandHandler>(it->second)(sender, command);
        return true;
    }
    sender.sendMessage(
        boost::locale::format(boost::locale::translate("Unknown command {1}")) %
        commandName);
    return false;
}

Server::RegisteredCommandHandler
Server::registerCommand(const std::string &command,
                        const boost::locale::message &help,
                        Server::CommandHandler &&handler)
{
    std::lock_guard<std::mutex> lock(_commandListLock);
    // TODO: C++17 non-explicit tuples
    return _commandList.insert(
                           {command, std::make_tuple(help, std::move(handler))})
        .first;
}

void Server::unregisterCommand(Server::RegisteredCommandHandler handle)
{
    std::lock_guard<std::mutex> lock(_commandListLock);
    _commandList.erase(handle);
}

Server::RegisteredLoggerBackend
Server::registerBackend(Server::LoggerBackend backend)
{
    std::lock_guard<std::mutex> lock(_loggerBackendListLock);
    _loggerBackends.push_front(backend);
    return _loggerBackends.before_begin();
}

void Server::unregisterBackend(Server::RegisteredLoggerBackend handle)
{
    std::lock_guard<std::mutex> lock(_loggerBackendListLock);
    _loggerBackends.erase_after(handle);
}

std::shared_ptr<ConfigSection> Server::getConfig(const std::string &name)
{
    return _configManager.getConfig(name);
}

bool Server::lockTask()
{
    std::unique_lock<std::mutex> lock(_stateLock);
    std::size_t ret = 1;
    while(_counter < 0)
    {
        if(ret)
        {
            lock.unlock();
            ret = _ioService.poll_one();
            lock.lock();
        }
        else
        {
            _stateWait.wait(lock);
            ret = 1;
        }
    }
    if(_dropEvents)
        return false;
    _counter++;
    return true;
}

void Server::unlockTask()
{
    std::unique_lock<std::mutex> lock(_stateLock);
    if(--_counter == 0)
    {
        lock.unlock();
        _stateWait.notify_one();
    }
}

template <Server::LockType type>
bool Server::lockCritical()
{
    std::unique_lock<std::mutex> lock(_stateLock);
    if(type == LockType::Stop && _dropEvents)
        return false;
    std::size_t ret = 1;
    while(_counter != 0)
    {
        if(ret)
        {
            lock.unlock();
            ret = _ioService.poll_one();
            lock.lock();
        }
        else
        {
            _stateWait.wait(lock);
            ret = 1;
        }
    }
    _counter--;
    if(type == LockType::Start)
    {
        _dropEvents = false;
    }
    else
    {
        _dropEvents = true;
    }
    return true;
}

void Server::unlockCritical()
{
    std::unique_lock<std::mutex> lock(_stateLock);
    if(++_counter == 0)
    {
        lock.unlock();
        _stateWait.notify_all();
    }
}

template <typename Handler, typename... Fn>
void Server::asyncRunCritical(Handler &&handler, Fn &&... func)
{
    {
        // Simpler lock since we're holding already
        std::lock_guard<std::mutex> lock(_stateLock);
        _counter -= sizeof...(func);
    }
    // TODO: C++17 fold expressions
    // HACK: GCC bug
    auto lambda = [this, handler](auto &&func)
    {
        _ioService.post(
            [ this, func = std::forward<decltype(func)>(func), handler ]
            {
                func();
                // TODO: exceptions
                {
                    std::unique_lock<std::mutex> lock(_stateLock);
                    if(++_counter == -1)
                    {
                        lock.unlock();
                        handler();
                    }
                }
            });
    };
    int dummy[] = {0, (lambda(std::forward<Fn>(func)), 0)...};
}

void Server::start(boost::asio::coroutine coroutine)
{
    BOOST_ASIO_CORO_REENTER(coroutine)
    {
        lockCritical<LockType::Start>();
        _config = _configManager.getConfig("cenisys");
        if(_config->getBool(ConfigSection::Path() / "console", true))
        {
            _terminalConsole =
                std::make_unique<ThreadedTerminalConsole>(*this, _ioService);
        }
        log(boost::locale::format(
                boost::locale::translate("Starting Cenisys {1}.")) %
            SERVER_VERSION);
        {
            unsigned int threads =
                _config->getUInt(ConfigSection::Path() / "threads", 0);
            if(threads == 0)
                threads = std::thread::hardware_concurrency();
            if(threads == 0)
                threads = 1;
            log(boost::locale::format(boost::locale::translate(
                    "Spinning up {1} thread.", "Spinning up {1} threads.",
                    threads)) %
                threads);
            for(unsigned int i = 1; i < threads; i++)
            {
                _threads.emplace_back(
                    static_cast<std::size_t (boost::asio::io_service::*)()>(
                        &boost::asio::io_service::run),
                    &_ioService);
            }
        }
        BOOST_ASIO_CORO_YIELD asyncRunCritical(
            [this, coroutine]
            {
                start(coroutine);
            },
            [this]
            {
                _work =
                    std::make_unique<boost::asio::io_service::work>(_ioService);
            },
            [this]
            {
                _helpCommand = registerCommand(
                    "help", boost::locale::translate("Display this help"),
                    [this](CommandSender &sender, const std::string &command)
                    {
                        sender.sendMessage(
                            boost::locale::translate("List of commands:"));
                        // TODO: Paging and more
                        for(const auto &item : _commandList)
                        {
                            sender.sendMessage(
                                boost::locale::format("/{1}: {2}") %
                                item.first %
                                std::get<boost::locale::message>(item.second));
                        }
                    });
            },
            [this]
            {
                _defaultCommands =
                    std::make_unique<DefaultCommandHandlers>(*this);
            });
        _termSignals.async_wait([this]
                                {
                                    terminate();
                                });
        unlockCritical();
    }
}

void Server::stop(boost::asio::coroutine coroutine)
{
    BOOST_ASIO_CORO_REENTER(coroutine)
    {
        if(!lockCritical<LockType::Stop>())
            return;
        _termSignals.cancel();
        BOOST_ASIO_CORO_YIELD asyncRunCritical(
            [this, coroutine]
            {
                stop(coroutine);
            },
            [this]
            {
                _defaultCommands.reset();
            },
            [this]
            {
                unregisterCommand(_helpCommand);
            },
            [this]
            {
                _work.reset();
            });
        log(boost::locale::translate("Server successfully terminated."));
        _terminalConsole.reset();
        _config.reset();
        unlockCritical();
    }
}

} // namespace cenisys
