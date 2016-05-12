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
#ifndef CENISYS_CHUNK_H
#define CENISYS_CHUNK_H

#include "block/block.h"
#include "block/blockref.h"
#include <memory>
#include <utility>

namespace cenisys
{

class World;

class Chunk : public std::enable_shared_from_this<Chunk>
{
public:
    Chunk(int x, int z);
    ~Chunk();

    std::shared_ptr<World> getWorld();
    BlockRef getBlock(unsigned int x, unsigned int y, unsigned int z);
    int getX() const;
    int getZ() const;

private:
    int _x, _z;
    std::shared_ptr<World> _world;

    // [section][x][y][z]
    std::array<std::array<std::array<std::array<Block, 16>, 16>, 16>, 16>
        _blocks;
};

} // namespace cenisys

#endif // CENISYS_CHUNK_H
