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
#ifndef CENISYS_SERVER_H
#define CENISYS_SERVER_H

#include <condition_variable>
#include <forward_list>
#include <future>
#include <locale>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/coroutine.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/locale/format.hpp>
#include <boost/locale/generator.hpp>
#include <boost/locale/message.hpp>
#include <boost/scope_exit.hpp>
#include "server/configmanager.h"

namespace cenisys
{

class ConfigSection;
class DefaultCommandHandlers;
class CommandSender;
class ThreadedTerminalConsole;

class Server
{
public:
    using CommandHandler =
        std::function<void(CommandSender &, const std::string &)>;
    using CommandHandlerList =
        std::map<std::string,
                 std::tuple<boost::locale::message, CommandHandler>>;
    using RegisteredCommandHandler = CommandHandlerList::const_iterator;

    using LoggerBackend = std::function<void(const boost::locale::format &)>;
    using LoggerBackendList = std::forward_list<LoggerBackend>;
    using RegisteredLoggerBackend = LoggerBackendList::const_iterator;

    Server(const boost::filesystem::path &dataDir,
           boost::locale::generator &localeGen);
    ~Server();

    //!
    //! \brief Run the server. Blocks until termination.
    //! \return 0 if successfully terminated.
    //!
    int run();
    template <typename Executor>
    void terminate(Executor &e)
    {
        _ioService.post([this]
                        {
                            stop();
                        });
    }
    //!
    //! \brief Terminate the server.
    //!
    void terminate() { terminate(_ioService); }

    template <typename Executor, typename Fn>
    void processEvent(Executor &executor, Fn &&func)
    {
        std::promise<void> promise;
        std::future<void> future = promise.get_future();
        // HACK: asio cannot dispatch move-only handlers
        executor.dispatch([ this, func = std::forward<Fn>(func), &promise ]
                          {
                              if(!lockTask())
                                  return;
                              BOOST_SCOPE_EXIT_ALL(&)
                              {
                                  promise.set_value();
                                  unlockTask();
                              };
                              func();
                          });
        future.get();
    }

    template <typename Fn>
    void processEvent(Fn &&func)
    {
        processEvent(_ioService, std::forward<Fn>(func));
    }

    std::locale getLocale(std::string locale);
    bool dispatchCommand(CommandSender &sender, const std::string &command);

    RegisteredCommandHandler registerCommand(const std::string &command,
                                             const boost::locale::message &help,
                                             CommandHandler &&handler);
    void unregisterCommand(RegisteredCommandHandler handle);

    template <typename T>
    void log(const T &content)
    {
        boost::locale::format formatted("{1}");
        formatted % content;
        std::lock_guard<std::mutex> lock(_loggerBackendListLock);
        for(const auto &backend : _loggerBackends)
            backend(formatted);
    }

    RegisteredLoggerBackend registerBackend(LoggerBackend backend);
    void unregisterBackend(RegisteredLoggerBackend handle);

    std::shared_ptr<ConfigSection> getConfig(const std::string &name);

private:
    bool lockTask();
    void unlockTask();
    enum class LockType : bool
    {
        Start = true,
        Stop = false
    };
    template <LockType type>
    bool lockCritical();
    void unlockCritical();

    template <typename Handler, typename... Fn>
    void asyncRunCritical(Handler &&handler, Fn &&... func);

    void start(boost::asio::coroutine coroutine = {});
    void stop(boost::asio::coroutine coroutine = {});

    std::mutex _stateLock;
    std::condition_variable _stateWait;
    int _counter;
    bool _dropEvents;

    boost::filesystem::path _dataDir;

    boost::locale::generator &_localeGen;

    boost::asio::io_service _ioService;
    std::unique_ptr<boost::asio::io_service::work> _work;
    std::vector<std::thread> _threads;
    boost::asio::signal_set _termSignals;

    CommandHandlerList _commandList;
    std::mutex _commandListLock;

    ConfigManager _configManager;
    std::shared_ptr<ConfigSection> _config;

    RegisteredCommandHandler _helpCommand;
    std::unique_ptr<DefaultCommandHandlers> _defaultCommands;

    LoggerBackendList _loggerBackends;
    std::mutex _loggerBackendListLock;
    std::unique_ptr<ThreadedTerminalConsole> _terminalConsole;
};

} // namespace cenisys

#endif // CENISYS_SERVER_H
