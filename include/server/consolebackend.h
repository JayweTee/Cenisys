/*
 * ConsoleBackend
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
#ifndef CENISYS_CONSOLEBACKEND_H
#define CENISYS_CONSOLEBACKEND_H

#include "command/commandsender.h"

namespace cenisys
{

class Console;

class ConsoleBackend
{
public:
    virtual ~ConsoleBackend() = default;

    //!
    //! \brief Attach to a console object.
    //! \param console The container object.
    //!
    virtual void attach(Console &console) = 0;
    //!
    //! \brief Detach from a console object. It will not be used after the call.
    //!
    virtual void detach() = 0;

    virtual void log(const boost::locale::format &content) = 0;
};

} // namespace cenisys

#endif // CENISYS_CONSOLEBACKEND_H
