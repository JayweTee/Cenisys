/*
 * Vector
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
#ifndef CENISYS_VECTOR_H
#define CENISYS_VECTOR_H

namespace cenisys
{

class Vector
{
public:
    Vector(double x, double y, double z);

    Vector &operator+=(const Vector &other);
    Vector &operator-=(const Vector &other);
    Vector &operator*=(const Vector &other);
    Vector &operator/=(const Vector &other);

    float angle(const Vector &other) const;
    Vector crossProduct(const Vector &other) const;
    double distance(const Vector &other) const;
    double dot(const Vector &other) const;
    double length() const;
    Vector midpoint(const Vector &other) const;
    Vector normalize() const;

    double getX() const;
    void setX(const double &value);
    double getY() const;
    void setY(const double &value);
    double getZ() const;
    void setZ(const double &value);

private:
    double _x;
    double _y;
    double _z;
};

} // namespace cenisys

#endif // CENISYS_VECTOR_H
