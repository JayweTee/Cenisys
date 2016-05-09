/*
 * Definition of text colors.
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
#ifndef CENISYS_TEXTCOLOR_H
#define CENISYS_TEXTCOLOR_H

#include <ostream>

namespace cenisys
{

constexpr auto SECTION_MARK = "§";

enum class TextFormat : char
{
    Black = '0',
    DarkBlue,
    DarkGreen,
    DarkCyan,
    DarkRed,
    Purple,
    Gold,
    Gray,
    DarkGray,
    Blue,
    BrightGreen,
    Cyan,
    Red,
    Pink,
    Yellow,
    White,
    Random = 'k',
    Bold,
    Strikethrough,
    Underlined,
    Italic,
    Reset = 'r',
};

inline std::ostream &operator<<(std::ostream &output, TextFormat code)
{
    output << SECTION_MARK << static_cast<char>(code);
    return output;
}
} // namespace cenisys

#endif // CENISYS_TEXTCOLOR_H
