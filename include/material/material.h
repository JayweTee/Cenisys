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
#ifndef CENISYS_MATERIAL_H
#define CENISYS_MATERIAL_H

#include <memory>
#include <string>
#include <vector>

namespace cenisys
{

class ItemStack;
class Player;
class BlockRef;
class BlockFace;
class Vector;

//!
//! \brief The Material class covers the behavior and data of a block/item.
//!
class Material
{
public:
    virtual ~Material() = default;
    virtual Material *clone() const = 0;
    virtual std::string getName() const = 0;
};

class ItemMaterial : public virtual Material
{
public:
    virtual ItemMaterial *clone() const = 0;
    virtual int getMaxStackSize() const;
    virtual void rightClickAir(Player &player, ItemStack &holding) const;
    virtual void rightClickBlock(Player &player, BlockRef &target,
                                 const BlockFace &face, ItemStack &holding,
                                 const Vector &clickedLoc) const;
};

class BlockMaterial : public virtual Material
{
public:
    virtual BlockMaterial *clone() const = 0;
    virtual std::vector<ItemStack> getDrops(const ItemStack &tool) const;
    virtual bool canAbsorb() const;
    virtual bool canOverride() const;
    virtual bool canPlaceAt(const BlockRef &target,
                            const BlockFace &face) const;
    //!
    //! \brief placeBlock Place a block, set target to the material.
    //! \param player The player placed the block.
    //! \param target The block to change.
    //! \param face The face of the block which player's cursor is on.
    //! \param clickedLoc The clicked position of the block which the player
    //! tried to place against.
    //!
    virtual void placeBlock(Player &player, BlockRef &target,
                            const BlockFace &face,
                            const Vector &clickedLoc) const;
    //!
    //! \brief blockInteract Interact with a block.
    //! \param player The player did the action.
    //! \param target The block to interact.
    //! \param face The face of the block which player's cursor is on.
    //! \param clickedLoc The clicked position of the block which player did the
    //! action.
    //! \return Whether the interaction occured.
    //!
    virtual bool blockInteract(Player &player, BlockRef &target,
                               const BlockFace &face,
                               const Vector &clickedLoc) const;
    //!
    //! \brief destroyBlock Break the block, set target to air.
    //! \param player The player destroyed the block.
    //! \param target The block to destroy.
    //! \param face The face of the block which player's cursor is on.
    //!
    virtual void destroyBlock(Player &player, BlockRef &target,
                              const BlockFace &face) const;
    virtual bool blockUpdate(BlockRef &target) const;
};

} // namespace cenisys

#endif // CENISYS_MATERIAL_H
