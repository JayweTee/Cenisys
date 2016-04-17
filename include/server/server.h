/*
 * Interface of the server.
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

#include <functional>
#include <forward_list>
#include <locale>
#include <map>
#include <string>
#include <tuple>
#include <boost/locale/format.hpp>
#include <boost/locale/message.hpp>

namespace cenisys
{

class ConfigSection;
class CommandSender;

//!
//! \brief The interface for the server.
//!
class Server
{
public:
    using CommandHandler =
        std::function<void(CommandSender &, const std::string &)>;
    using CommandHandlerList =
        std::map<std::string,
                 std::tuple<boost::locale::message, CommandHandler>>;
    using RegisteredCommandHandler = CommandHandlerList::const_iterator;

    using LogFormat = std::function<void(const boost::locale::format &)>;
    using LogMessage = std::function<void(const boost::locale::message &)>;
    using LoggerBackend = std::tuple<LogFormat, LogMessage>;
    using LoggerBackendList = std::forward_list<LoggerBackend>;
    using RegisteredLoggerBackend = LoggerBackendList::const_iterator;

    virtual ~Server() = default;

    //!
    //! \brief Start running the server.
    //! \return Zero if stopped by user, non-zero if any error crashed the
    //! server.
    //!
    virtual int run() = 0;

    //!
    //! \brief Stop the server. May called from any thread.
    //!
    virtual void terminate() = 0;

    virtual void processEvent(const std::function<void()> &&func) = 0;

    virtual std::locale getLocale(std::string locale) = 0;

    //!
    //! \brief Process a command.
    //! \param command The command and arguments, without the leading slash.
    //! \return true on success, false if command does not exist.
    //!
    virtual bool dispatchCommand(CommandSender &sender,
                                 const std::string &command) = 0;

    //!
    //! \brief Register a command.
    //! \param handler The function which processes the command.
    //! \return A handle to unregister the command.
    //!
    virtual RegisteredCommandHandler
    registerCommand(const std::string &command,
                    const boost::locale::message &help,
                    Server::CommandHandler handler) = 0;

    //!
    //! \brief Unregister the command.
    //! \param handle The return value of registerCommand.
    //!
    virtual void unregisterCommand(RegisteredCommandHandler handle) = 0;

    //!
    //! \brief Log formatted text to the server log.
    //! \param content The content in string. May contain color codes.
    //!
    virtual void log(const boost::locale::format &content) = 0;

    //!
    //! \brief Log translated text to the server log.
    //! \param content The content in string.
    //!
    virtual void log(const boost::locale::message &content) = 0;

    //!
    //! \brief Register a logger backend.
    //! \param backend The function to call to log. Must not block.
    //! \return A handle to the registered backend.
    //!
    virtual RegisteredLoggerBackend registerBackend(LoggerBackend backend) = 0;

    //!
    //! \brief Unregister the logger backend.
    //! \param handle The handle returned by registerBackend.
    //!
    virtual void unregisterBackend(RegisteredLoggerBackend handle) = 0;

    virtual std::shared_ptr<ConfigSection>
    getConfig(const std::string &name) = 0;
};

} // namespace cenisys

#endif // CENISYS_SERVER_H
