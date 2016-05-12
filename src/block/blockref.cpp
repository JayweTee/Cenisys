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
#include "block/block.h"
#include "block/blockref.h"
#include "server/world.h"

namespace cenisys
{

BlockRef::BlockRef(Block &block, Location location)
    : std::unique_lock<std::mutex>(block._mutex), _block(block),
      _location(location)
{
}

const Block &BlockRef::block() const
{
    return _block;
}

Block &BlockRef::block()
{
    return _block;
}

const Location &BlockRef::location() const
{
    return _location;
}

Location BlockRef::location()
{
    return _location;
}

BlockRef BlockRef::getRelative(const BlockFace &face)
{
    return _location.getWorld()->getBlockAt(
        static_cast<int>(_location.getX()) + face.getModX(),
        static_cast<unsigned int>(_location.getY()) + face.getModY(),
        static_cast<int>(_location.getZ()) + face.getModZ());
}
} // namespace cenisys
