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

#include "server/configmanager.h"
#include "server/console.h"
#include "util/textcolor.h"
#include <boost/asio/coroutine.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/locale/date_time.hpp>
#include <boost/locale/format.hpp>
#include <boost/locale/generator.hpp>
#include <boost/locale/message.hpp>
#include <boost/scope_exit.hpp>
#include <condition_variable>
#include <forward_list>
#include <future>
#include <locale>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace cenisys
{

class ConfigSection;
class DefaultCommandHandlers;
class CommandSender;

class Server
{
public:
    enum class LogLevel
    {
        Severe,
        Warning,
        Info,
        Debug,
    };

    using CommandHandler =
        std::function<void(CommandSender &, const std::string &)>;
    using CommandHandlerList =
        std::map<std::string,
                 std::tuple<boost::locale::message, CommandHandler>>;
    using RegisteredCommandHandler = CommandHandlerList::const_iterator;

    using ConsoleList = std::forward_list<Console>;
    using RegisteredConsole = ConsoleList::const_iterator;

    Server(const boost::filesystem::path &dataDir,
           boost::locale::generator &localeGen);
    ~Server();

    //!
    //! \brief Run the server. Blocks until termination.
    //! \return 0 if successfully terminated.
    //!
    int run();
    //!
    //! \brief Terminate the server.
    //!
    void terminate();

    template <typename Fn>
    void processEvent(Fn &&func)
    {
        std::promise<void> promise;
        std::future<void> future = promise.get_future();
        // HACK: asio cannot dispatch move-only handlers
        _ioService.dispatch([ this, func = std::forward<Fn>(func), &promise ] {
            BOOST_SCOPE_EXIT_ALL(&) { promise.set_value(); };
            if(!lockTask())
                return;
            BOOST_SCOPE_EXIT_ALL(&) { unlockTask(); };
            func();
        });
        future.get();
    }

    std::locale getLocale(std::string locale);
    void dispatchCommand(CommandSender &sender, const std::string &command);

    RegisteredCommandHandler registerCommand(const std::string &command,
                                             const boost::locale::message &help,
                                             CommandHandler &&handler);
    void unregisterCommand(RegisteredCommandHandler handle);

    RegisteredConsole registerConsole(ConsoleBackend &backend);
    void unregisterConsole(RegisteredConsole handle);

    template <typename T>
    void log(LogLevel level, const T &content)
    {
        TextFormat color;
        boost::locale::message levelText;
        switch(level)
        {
        case LogLevel::Severe:
            color = TextFormat::Red;
            levelText = boost::locale::translate("SEVERE");
            break;
        case LogLevel::Warning:
            color = TextFormat::Yellow;
            levelText = boost::locale::translate("WARNING");
            break;
        case LogLevel::Info:
            color = TextFormat::Gray;
            levelText = boost::locale::translate("INFO");
            break;
        case LogLevel::Debug:
            color = TextFormat::DarkCyan;
            levelText = boost::locale::translate("DEBUG");
            break;
        }
        boost::locale::format message(
            boost::locale::translate("{1}[{2}] [{3}] {4}{5}"));
        boost::locale::date_time time;
        message % color % time % levelText % content % TextFormat::Reset;
        std::lock_guard<std::mutex> lock(_consoleListLock);
        for(auto &console : _consoles)
            console.log(message);
    }

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
    std::size_t _counter;
    bool _critical;
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

    ConsoleList _consoles;
    std::mutex _consoleListLock;
    std::shared_ptr<ConsoleBackend> _terminalConsole;
    RegisteredConsole _terminalConsoleHandle;
};

} // namespace cenisys

#endif // CENISYS_SERVER_H
