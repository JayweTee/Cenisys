/*
 * Location
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
#include "util/location.h"
#include "server/chunk.h"
#include "server/world.h"
#include <boost/math/constants/constants.hpp>
#include <cmath>

namespace cenisys
{

Location::Location(std::shared_ptr<Chunk> chunk, double x, double y, double z,
                   float yaw, float pitch)
    : Vector(x, y, z), _chunk(chunk), _yaw(yaw), _pitch(pitch)
{
}

Location &Location::operator+=(const Location &target)
{
    if(getWorld() == target.getWorld())
    {
        Vector::operator+=(target);
    }
    // TODO: Error handling
    return *this;
}

Location &Location::operator+=(const Vector &target)
{
    Vector::operator+=(target);
    return *this;
}

Location &Location::operator-=(const Location &target)
{
    if(getWorld() == target.getWorld())
    {
        Vector::operator-=(target);
    }
    // TODO: Error handling
    return *this;
}

Location &Location::operator-=(const Vector &target)
{
    Vector::operator+=(target);
    return *this;
}

Vector Location::getDirection() const
{
    double xz = std::cos(_pitch * boost::math::constants::pi<double>() / 180);
    return Vector(
        -xz * std::sin(_yaw * boost::math::constants::pi<double>() / 180),
        -std::sin(_pitch * boost::math::constants::pi<double>() / 180),
        xz * std::cos(_yaw * boost::math::constants::pi<double>() / 180));
}

void Location::setDirection(const Vector &vector)
{
    if(vector.getX() == 0 && vector.getZ() == 0)
    {
        _pitch = vector.getY() > 0 ? -90 : 90;
        return;
    }

    _yaw = static_cast<float>(
        std::fmod((std::atan2(-vector.getX(), vector.getZ()) +
                   boost::math::constants::pi<double>() / 2),
                  boost::math::constants::pi<double>() / 2) *
        180 / boost::math::constants::pi<double>());

    _pitch = static_cast<float>(
        std::atan(-vector.getY() / std::sqrt(std::pow(vector.getX(), 2) +
                                             std::pow(vector.getZ(), 2))) *
        180 / boost::math::constants::pi<double>());
}

std::shared_ptr<Chunk> Location::getChunk() const
{
    return _chunk;
}
std::shared_ptr<World> Location::getWorld() const
{
    return _chunk->getWorld();
}

float Location::getYaw() const
{
    return _yaw;
}
void Location::setYaw(const float &yaw)
{
    _yaw = yaw;
}
float Location::getPitch() const
{
    return _pitch;
}
void Location::setPitch(const float &pitch)
{
    _pitch = pitch;
}

} // namespace cenisys
