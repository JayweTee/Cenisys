/*
 * Material and some others.
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
#include "material/material.h"
#include "block/block.h"
#include "block/blockref.h"
#include "inventory/itemstack.h"
#include "material/air.h"
#include <memory>

namespace cenisys
{

int ItemMaterial::getMaxStackSize() const
{
    return 64;
}

void ItemMaterial::rightClickAir(Player &player, ItemStack &holding) const
{
    // Do nothing by default
}

void ItemMaterial::rightClickBlock(Player &player, BlockRef &target,
                                   const BlockFace &face, ItemStack &holding,
                                   const Vector &clickedLoc) const
{
    if(const BlockMaterial *block = dynamic_cast<const BlockMaterial *>(this))
    {
        // TODO: deadlock avoidance
        BlockRef relative = target.getRelative(face);
        BlockRef &targetPlace =
            target.block().getMaterial().canAbsorb() ? target : relative;
        bool canBuild = block->canPlaceAt(targetPlace, face);
        // TODO: Events
        if(!canBuild)
            return;
        block->placeBlock(player, targetPlace, face, clickedLoc);
        holding.setAmount(holding.getAmount() - 1);
    }
    else
    {
        rightClickAir(player, holding);
    }
}

std::vector<ItemStack> BlockMaterial::getDrops(const ItemStack &tool) const
{
    if(const ItemMaterial *drop = dynamic_cast<const ItemMaterial *>(this))
    {
        return {ItemStack(std::unique_ptr<ItemMaterial>(drop->clone()))};
    }
    else
    {
        return {};
    }
}

bool BlockMaterial::canAbsorb() const
{
    return false;
}

bool BlockMaterial::canOverride() const
{
    return false;
}

bool BlockMaterial::canPlaceAt(const BlockRef &target,
                               const BlockFace &face) const
{
    return true;
}

void BlockMaterial::placeBlock(Player &player, BlockRef &target,
                               const BlockFace &face,
                               const Vector &clickedLoc) const
{
    target.block().setMaterial(
        static_cast<std::unique_ptr<BlockMaterial>>(clone()));
}

bool BlockMaterial::blockInteract(Player &player, BlockRef &target,
                                  const BlockFace &face,
                                  const Vector &clickedLoc) const
{
    return false;
}

void BlockMaterial::destroyBlock(Player &player, BlockRef &target,
                                 const BlockFace &face) const
{
    // Beware: The following line commits suicide, the behavior is well-defined
    // only when it's the last line
    target.block().setMaterial(std::make_unique<Air>());
}

bool BlockMaterial::blockUpdate(BlockRef &target) const
{
    return false;
}
}
