/*
 * Helpers for terminal colors.
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

#include <cstring>
#include "util/textcolor.h"
#include "server/terminal/terminalcolor.h"

namespace cenisys
{
std::string ansiColorize(const std::string &str)
{
    std::string result;
    std::string::size_type oldPos = 0, pos = 0;
    while((pos = str.find(SECTION_MARK, pos)) <
          (str.size() - std::strlen(SECTION_MARK)))
    {
        result += str.substr(oldPos, pos - oldPos);
        oldPos = pos + 1 + std::strlen(SECTION_MARK);
        switch(static_cast<TextFormat>(str[pos + std::strlen(SECTION_MARK)]))
        {
        case TextFormat::Black:
            result += "\033[0;30m";
            break;
        case TextFormat::DarkBlue:
            result += "\033[0;34m";
            break;
        case TextFormat::DarkGreen:
            result += "\033[0;32m";
            break;
        case TextFormat::DarkCyan:
            result += "\033[0;36m";
            break;
        case TextFormat::DarkRed:
            result += "\033[0;31m";
            break;
        case TextFormat::Purple:
            result += "\033[0;35m";
            break;
        case TextFormat::Gold:
            result += "\033[0;33m";
            break;
        case TextFormat::Gray:
            result += "\033[0;37m";
            break;
        case TextFormat::DarkGray:
            result += "\033[0;1;30m";
            break;
        case TextFormat::Blue:
            result += "\033[0;1;34m";
            break;
        case TextFormat::BrightGreen:
            result += "\033[0;1;32m";
            break;
        case TextFormat::Cyan:
            result += "\033[0;1;36m";
            break;
        case TextFormat::Red:
            result += "\033[0;1;31m";
            break;
        case TextFormat::Pink:
            result += "\033[0;1;35m";
            break;
        case TextFormat::Yellow:
            result += "\033[0;1;33m";
            break;
        case TextFormat::White:
            result += "\033[0;1;37m";
            break;
        case TextFormat::Random:
            result += "\033[5m";
            break;
        case TextFormat::Bold:
            result += "\033[1m";
            break;
        case TextFormat::Strikethrough:
            result += "\033[53m";
            break;
        case TextFormat::Underlined:
            result += "\033[4m";
            break;
        case TextFormat::Italic:
            result += "\033[3m";
            break;
        case TextFormat::Reset:
            result += "\033[0m";
            break;
        default:
            oldPos -= std::strlen(SECTION_MARK) + 1;
            break;
        }
        pos++;
    }
    result += str.substr(oldPos, std::string::npos);
    return result;
}

std::string stripColor(const std::string &str)
{
    std::string result;
    std::string::size_type oldPos = 0, pos = 0;
    while((pos = str.find(SECTION_MARK, pos)) < (str.size() - 1))
    {
        result += str.substr(oldPos, pos - oldPos);
        oldPos = pos + 1 + std::strlen(SECTION_MARK);
        switch(static_cast<TextFormat>(str[pos + std::strlen(SECTION_MARK)]))
        {
        case TextFormat::Black:
        case TextFormat::DarkBlue:
        case TextFormat::DarkGreen:
        case TextFormat::DarkCyan:
        case TextFormat::DarkRed:
        case TextFormat::Purple:
        case TextFormat::Gold:
        case TextFormat::Gray:
        case TextFormat::DarkGray:
        case TextFormat::Blue:
        case TextFormat::BrightGreen:
        case TextFormat::Cyan:
        case TextFormat::Red:
        case TextFormat::Pink:
        case TextFormat::Yellow:
        case TextFormat::White:
        case TextFormat::Random:
        case TextFormat::Bold:
        case TextFormat::Strikethrough:
        case TextFormat::Underlined:
        case TextFormat::Italic:
        case TextFormat::Reset:
            break;
        default:
            oldPos -= std::strlen(SECTION_MARK) + 1;
            break;
        }
        pos++;
    }
    result += str.substr(oldPos, std::string::npos);
    return result;
}

} // namespace cenisys
