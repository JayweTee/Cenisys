/*
 * ItemStack
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
#ifndef CENISYS_ITEMSTACK_H
#define CENISYS_ITEMSTACK_H

#include <memory>

namespace cenisys
{

class ItemMaterial;

class ItemStack
{
public:
    explicit ItemStack(std::unique_ptr<cenisys::ItemMaterial> material,
                       int amount = 1);
    ItemStack(const ItemStack &other);
    ~ItemStack();
    ItemStack &operator=(const ItemStack &other);

    // TODO: Enchantments, Metadata

    int getAmount() const;
    void setAmount(int amount);
    const ItemMaterial &getMaterial() const;
    ItemMaterial &getMaterial();
    void setMaterial(ItemMaterial *material);
    void setMaterial(std::unique_ptr<ItemMaterial> material);

private:
    std::shared_ptr<ItemMaterial> _material;
    int _amount;
};

} // namespace cenisys

#endif // CENISYS_ITEMSTACK_H
