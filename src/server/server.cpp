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
    : _dataDir(dataDir), _localeGen(localeGen),
      _termSignals(_ioService, SIGINT, SIGTERM),
      _configManager(*this, _dataDir / "config")
{
}

Server::~Server()
{
}

int Server::run()
{
    _state = State::NotStarted;
    _ioService.post(std::bind(&Server::start, this));
    _config = _configManager.getConfig("cenisys");
    if(_config->getBool(ConfigSection::Path() / "console", true))
    {
        _terminalConsole = std::make_unique<ThreadedTerminalConsole>(*this);
    }
    log(boost::locale::format(
            boost::locale::translate("Starting Cenisys {1}.")) %
        SERVER_VERSION);
    unsigned int threads =
        _config->getUInt(ConfigSection::Path() / "threads", 0);
    if(threads == 0)
        threads = std::thread::hardware_concurrency();
    if(threads == 0)
        threads = 1;
    log(boost::locale::format(boost::locale::translate(
            "Spinning up {1} thread.", "Spinning up {1} threads.", threads)) %
        threads);
    for(unsigned int i = 1; i < threads; i++)
    {
        _threads.emplace_back(
            static_cast<std::size_t (boost::asio::io_service::*)()>(
                &boost::asio::io_service::run),
            &_ioService);
    }
    _ioService.run();
    log(boost::locale::translate("Waiting threads to finish…"));
    for(auto &item : _threads)
    {
        item.join();
    }
    log(boost::locale::translate("Server successfully terminated."));
    _terminalConsole.reset();
    _config.reset();
    return 0;
}

void Server::terminate()
{
    _ioService.post(
        [this]
        {
            State swap = State::Running;
            if(!_state.compare_exchange_strong(swap, State::Stopped))
            {
                if(swap == State::Stopped)
                {
                    return;
                }
                else
                {
                    return terminate();
                }
            }
            std::unique_lock<std::shared_timed_mutex> waitTask(_stateLock);
            stop();
        });
}

// HACK: Asio doesn't allow move handlers
namespace
{
template <typename F>
struct move_wrapper : F
{
    move_wrapper(F &&f) : F(std::move(f)) {}

    move_wrapper(move_wrapper &&) = default;
    move_wrapper &operator=(move_wrapper &&) = default;

    move_wrapper(const move_wrapper &);
    move_wrapper &operator=(const move_wrapper &);
};

template <typename T>
auto move_handler(T &&t) -> move_wrapper<typename std::decay<T>::type>
{
    return std::move(t);
}
}

void Server::processEvent(const std::function<void()> &&func)
{
    if(_state == State::Stopped)
        return;
    std::shared_lock<std::shared_timed_mutex> lock(_stateLock);
    _ioService.post(
        move_handler([ this, func(std::move(func)), lock(std::move(lock)) ]
                     {
                         if(_state == State::NotStarted)
                         {
                             return processEvent(std::move(func));
                         }
                         func();
                     }));
}

std::locale Server::getLocale(std::string locale)
{
    return _localeGen(locale);
}

bool Server::dispatchCommand(CommandSender &sender, const std::string &command)
{
    std::lock_guard<std::mutex> lock(_registerCommandLock);
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
                        Server::CommandHandler handler)
{
    std::lock_guard<std::mutex> lock(_registerCommandLock);
    // TODO: C++17 non-explicit tuples
    return _commandList.insert({command, std::make_tuple(help, handler)}).first;
}

void Server::unregisterCommand(Server::RegisteredCommandHandler handle)
{
    std::lock_guard<std::mutex> lock(_registerCommandLock);
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

void Server::start()
{
    std::atomic_size_t counter(0);
    runCriticalTask(
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
                            boost::locale::format("/{1}: {2}") % item.first %
                            std::get<boost::locale::message>(item.second));
                    }
                });
        },
        counter);
    runCriticalTask(
        [this]
        {
            _defaultCommands = std::make_unique<DefaultCommandHandlers>(*this);
        },
        counter);
    waitCriticalTask(counter);
    _termSignals.async_wait(std::bind(&Server::terminate, this));
    _state = State::Running;
}

void Server::stop()
{
    std::atomic_size_t counter(0);
    _termSignals.cancel();
    runCriticalTask(
        [this]
        {
            _defaultCommands.reset();
        },
        counter);
    runCriticalTask(
        [this]
        {
            unregisterCommand(_helpCommand);
        },
        counter);
    waitCriticalTask(counter);
}

template <typename Fn>
void Server::runCriticalTask(Fn &&func, std::atomic_size_t &counter)
{
    counter++;
    _ioService.post([ func(std::forward<Fn>(func)), &counter ]
                    {
                        func();
                        counter--;
                    });
}

void Server::waitCriticalTask(std::atomic_size_t &counter)
{
    while(counter != 0)
    {
        _ioService.poll_one();
    }
}

} // namespace cenisys
