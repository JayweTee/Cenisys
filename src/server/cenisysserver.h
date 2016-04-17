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
#ifndef CENISYS_CENISYSSERVER_H
#define CENISYS_CENISYSSERVER_H

#include <atomic>
#include <locale>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <vector>
#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/locale/generator.hpp>
#include "server/server.h"
#include "server/threadedterminalconsole.h"
#include "server/cenisysconfigmanager.h"

namespace cenisys
{

class ConfigSection;
class DefaultCommandHandlers;

class CenisysServer : public Server
{
public:
    CenisysServer(const boost::filesystem::path &dataDir,
                  boost::locale::generator &localeGen);
    ~CenisysServer();

    int run();
    void terminate();

    void processEvent(const std::function<void()> &&func);

    std::locale getLocale(std::string locale);
    bool dispatchCommand(CommandSender &sender, const std::string &command);

    RegisteredCommandHandler registerCommand(const std::string &command,
                                             const boost::locale::message &help,
                                             CommandHandler handler);
    void unregisterCommand(RegisteredCommandHandler handle);

    void log(const boost::locale::format &content);
    void log(const boost::locale::message &content);
    RegisteredLoggerBackend registerBackend(LoggerBackend backend);
    void unregisterBackend(RegisteredLoggerBackend handle);

    std::shared_ptr<ConfigSection> getConfig(const std::string &name);

private:
    enum class State
    {
        NotStarted, // Postpone all events
        Running,    // Accept events
        Stopped,    // Discard any events
    };

    void start();
    void stop();

    template <typename Fn>
    void runCriticalTask(Fn &&func, std::atomic_size_t &counter);
    void waitCriticalTask(std::atomic_size_t &counter);

    State _state;
    // TODO: shared_mutex is C++17
    std::shared_timed_mutex _stateLock;
    std::locale _oldCoutLoc;
    boost::filesystem::path _dataDir;
    boost::locale::generator &_localeGen;
    boost::asio::io_service _ioService;
    std::vector<std::thread> _threads;
    boost::asio::signal_set _termSignals;
    CommandHandlerList _commandList;
    std::mutex _registerCommandLock;
    CenisysConfigManager _configManager;
    std::unique_ptr<ThreadedTerminalConsole> _terminalConsole;
    std::unique_ptr<DefaultCommandHandlers> _defaultCommands;
    LoggerBackendList _loggerBackends;
    std::mutex _loggerBackendListLock;
    std::shared_ptr<ConfigSection> _config;
};

} // namespace cenisys

#endif // CENISYS_CENISYSSERVER_H
