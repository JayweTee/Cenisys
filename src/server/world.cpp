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
#include "server/world.h"
#include "server/chunk.h"
#include "util/postochunk.h"
#include <cmath>

namespace cenisys
{

World::World()
{
}

BlockRef World::getBlockAt(int x, unsigned int y, int z)
{
    return getChunk(posToChunk(x), posToChunk(z))
        ->getBlock(posToChunkMod(x), y, posToChunkMod(z));
}

std::shared_ptr<Chunk> World::getChunk(int x, int z)
{
    // TODO
}

} // namespace cenisys
