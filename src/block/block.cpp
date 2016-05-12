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
#include "block/block.h"
#include "material/material.h"

namespace cenisys
{

Block::Block(std::unique_ptr<BlockMaterial> material)
    : _material(std::move(material))
{
}

BlockMaterial &Block::getMaterial()
{
    return *_material;
}

void Block::setMaterial(std::unique_ptr<BlockMaterial> material)
{
    _material = std::move(material);
}

unsigned char Block::getSkyLight()
{
    return _skyLight;
}

unsigned char Block::getBlockLight()
{
    return _blockLight;
}

unsigned char Block::getLightLevel()
{
    return std::max(_skyLight, _blockLight);
}

} // namespace cenisys
