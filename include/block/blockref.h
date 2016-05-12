/*
 * BlockRef
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
#ifndef CENISYS_BLOCKREF_H
#define CENISYS_BLOCKREF_H

#include "block/blockface.h"
#include "util/location.h"
#include <mutex>

namespace cenisys
{

class Block;

class BlockRef : public std::unique_lock<std::mutex>
{
public:
    BlockRef(Block &block, Location location);
    const Block &block() const;
    Block &block();
    const Location &location() const;
    Location location();

    BlockRef getRelative(const BlockFace &face);

private:
    Block &_block;
    Location _location;
};

} // namespace cenisys

#endif // CENISYS_BLOCKREF_H
