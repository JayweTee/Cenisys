/*
 * World
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
#ifndef CENISYS_WORLD_H
#define CENISYS_WORLD_H

#include "block/blockref.h"
#include <memory>
#include <mutex>

namespace cenisys
{

class Chunk;

class World
{
public:
    World();
    BlockRef getBlockAt(int x, unsigned int y, int z);
    std::shared_ptr<Chunk> getChunk(int x, int z);

private:
    std::weak_ptr<Chunk> _loadedChunk;
    std::mutex _loadedChunkLock;
};

} // namespace cenisys

#endif // CENISYS_WORLD_H