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
#ifndef CENISYSSERVER_H
#define CENISYSSERVER_H
#include <boost/asio/io_service.hpp>
#include "server/server.h"

namespace cenisys
{
//!
//! \brief The implementation of the Cenisys core.
//!
class CenisysServer : public Server
{
public:
    //!
    //! \brief Prepares for running server.
    //!
    CenisysServer();
    //!
    //! \brief Cleanup a stopped instance.
    //!
    ~CenisysServer();
    //!
    //! \brief run
    //! \return 0 if terminated gracefully, 1 if crashed.
    //!
    int run();
private:
    boost::asio::io_service io_service;
};
}
#endif // CENISYSSERVER_H
