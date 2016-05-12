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
#ifndef CENISYS_LOCATION_H
#define CENISYS_LOCATION_H

#include "util/vector.h"
#include <memory>

namespace cenisys
{

class Chunk;
class World;

class Location : public Vector
{
public:
    Location(std::shared_ptr<Chunk> chunk, double x, double y, double z,
             float yaw = 0, float pitch = 0);

    Location &operator+=(const Location &target);
    Location &operator+=(const Vector &target);
    Location &operator-=(const Location &target);
    Location &operator-=(const Vector &target);

    Vector getDirection() const;
    void setDirection(const Vector &vector);

    std::shared_ptr<Chunk> getChunk() const;
    std::shared_ptr<World> getWorld() const;

    float getYaw() const;
    void setYaw(const float &yaw);
    float getPitch() const;
    void setPitch(const float &pitch);

private:
    std::shared_ptr<Chunk> _chunk;
    float _yaw;
    float _pitch;
};

} // namespace cenisys

#endif // CENISYS_LOCATION_H
