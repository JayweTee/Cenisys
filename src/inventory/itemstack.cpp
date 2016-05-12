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
#include "inventory/itemstack.h"
#include "material/material.h"

namespace cenisys
{

ItemStack::ItemStack(std::unique_ptr<ItemMaterial> material, int amount)
    : _amount(amount), _material(std::move(material))
{
}

ItemStack::ItemStack(const ItemStack &other)
    : _material(other._material), _amount(other._amount)
{
}

ItemStack::~ItemStack()
{
}

ItemStack &ItemStack::operator=(const ItemStack &other)
{
    _material = other._material;
    _amount = other._amount;
    return *this;
}

int ItemStack::getAmount() const
{
    return _amount;
}

void ItemStack::setAmount(int amount)
{
    _amount = amount;
}

ItemMaterial &ItemStack::getMaterial()
{
    return *_material;
}

void ItemStack::setMaterial(std::unique_ptr<ItemMaterial> material)
{
    _material = std::move(material);
}

} // namespace cenisys
