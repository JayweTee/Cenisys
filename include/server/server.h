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

namespace cenisys
{

//!
//! \brief The interface for the server.
//!
class Server
{
public:
    virtual ~Server() {}
    //!
    //! \brief Start running the server.
    //! \return Zero if stopped by user, non-zero if any error crashed the
    //! server.
    //!
    virtual int run() = 0;
};

} // namespace cenisys

#endif // CENISYS_SERVER_H
