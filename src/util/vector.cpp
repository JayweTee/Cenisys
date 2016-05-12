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
#include "util/vector.h"
#include <cmath>

namespace cenisys
{

Vector::Vector(double x, double y, double z) : _x(x), _y(y), _z(z)
{
}

Vector &Vector::operator+=(const Vector &other)
{
    _x += other._x;
    _y += other._y;
    _z += other._z;
    return *this;
}
Vector &Vector::operator-=(const Vector &other)
{
    _x -= other._x;
    _y -= other._y;
    _z -= other._z;
    return *this;
}
Vector &Vector::operator*=(const Vector &other)
{
    _x *= other._x;
    _y *= other._y;
    _z *= other._z;
    return *this;
}
Vector &Vector::operator/=(const Vector &other)
{
    _x /= other._x;
    _y /= other._y;
    _z /= other._z;
    return *this;
}

float Vector::angle(const Vector &other) const
{
    return static_cast<float>(
        std::acos(dot(other) / (length() * other.length())));
}
Vector Vector::crossProduct(const Vector &other) const
{
    return Vector(_y * other._z - other._z * _z, _z * other._x - other._z * _x,
                  _x * other._y - other._x * _y);
}
double Vector::distance(const Vector &other) const
{
    return std::pow(_x - other._x, 2) + std::pow(_y - other._y, 2) +
           std::pow(_z - other._z, 2);
}
double Vector::dot(const Vector &other) const
{
    return _x * other._x + _y * other._y + _z * other._z;
}
double Vector::length() const
{
    return std::sqrt(std::pow(_x, 2) + std::pow(_y, 2) + std::pow(_z, 2));
}
Vector Vector::midpoint(const Vector &other) const
{
    return Vector((_x + other._x) / 2, (_y + other._y) / 2,
                  (_z + other._z) / 2);
}
Vector Vector::normalize() const
{
    double len = length();
    return Vector(_x / len, _y / len, _z / len);
}

double Vector::getX() const
{
    return _x;
}
void Vector::setX(const double &value)
{
    _x = value;
}
double Vector::getY() const
{
    return _y;
}
void Vector::setY(const double &value)
{
    _y = value;
}
double Vector::getZ() const
{
    return _z;
}
void Vector::setZ(const double &value)
{
    _z = value;
}

} // namespace cenisys
