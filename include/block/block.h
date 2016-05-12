/*
 * Block
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
#ifndef CENISYS_BLOCK_H
#define CENISYS_BLOCK_H

#include "material/material.h"
#include <memory>
#include <mutex>

namespace cenisys
{

class Block
{
    friend class BlockRef;

public:
    Block(std::unique_ptr<BlockMaterial> material = nullptr);

    BlockMaterial &getMaterial();
    void setMaterial(std::unique_ptr<BlockMaterial> material);

    unsigned char getSkyLight();
    unsigned char getBlockLight();
    unsigned char getLightLevel();

private:
    std::mutex _mutex;

    std::unique_ptr<BlockMaterial> _material;
    unsigned char _skyLight : 4;
    unsigned char _blockLight : 4;
};

} // namespace cenisys

#endif // CENISYS_BLOCK_H
