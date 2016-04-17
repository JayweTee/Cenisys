/*
 * Interface of a sender of a command.
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

#ifndef CENISYS_COMMANDSENDER_H
#define CENISYS_COMMANDSENDER_H

#include <boost/locale/format.hpp>
#include <boost/locale/message.hpp>

namespace cenisys
{

class Server;

class CommandSender
{
public:
    virtual ~CommandSender() = default;

    virtual Server &getServer() = 0;

    virtual void sendMessage(const boost::locale::format &content) = 0;

    //!
    //! \brief Log translated text to the server log.
    //! \param content The content in string.
    //!
    virtual void sendMessage(const boost::locale::message &content) = 0;
};

} // namespace cenisys

#endif // COMMANDSENDER_H
