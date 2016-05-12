/*
 * Chunk
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
#include "server/chunk.h"
#include "material/material.h"
#include "util/location.h"

namespace cenisys
{

Chunk::Chunk(int x, int z) : _x(x), _z(z)
{
}

std::shared_ptr<World> Chunk::getWorld()
{
    return _world;
}

BlockRef Chunk::getBlock(unsigned int x, unsigned int y, unsigned int z)
{
    return {_blocks[y / 16][x][y % 16][z],
            Location(shared_from_this(), _x * 16 + x, y, _z * 16 + z)};
}

int Chunk::getX() const
{
    return _x;
}

int Chunk::getZ() const
{
    return _z;
}

} // namespace cenisys
