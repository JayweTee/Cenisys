/*
 * Template class for NBT operations.
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
#ifndef CENISYS_NBT_H
#define CENISYS_NBT_H

#include <boost/endian/buffers.hpp>
#include <cstdint>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace cenisys
{

namespace NBT
{

enum class TagType : std::int8_t
{
    End = 0,
    Byte = 1,
    Short = 2,
    Int = 3,
    Long = 4,
    Float = 5,
    Double = 6,
    ByteArray = 7,
    String = 8,
    List = 9,
    Compound = 10,
    IntArray = 11,
};

class Tag
{
public:
    TagType getType() const { return _type; }

protected:
    Tag(TagType type) : _type(type) {}

private:
    TagType _type;
};

template <typename T>
class BasicTag : public Tag
{
public:
    // Making use of SFINAE, it will crash if no specialization available
    BasicTag(T data = {}) {}
    BasicTag(TagType type, T data = {}) : Tag(type), _data(data) {}
    operator T &() { return _data; }
    operator const T &() const { return _data; }

private:
    T _data;
};

template <>
BasicTag<int8_t>::BasicTag(int8_t data) : BasicTag(TagType::Byte, data)
{
}

template <>
BasicTag<int16_t>::BasicTag(int16_t data) : BasicTag(TagType::Short, data)
{
}

template <>
BasicTag<int32_t>::BasicTag(int32_t data) : BasicTag(TagType::Int, data)
{
}

template <>
BasicTag<int64_t>::BasicTag(int64_t data) : BasicTag(TagType::Long, data)
{
}

template <>
BasicTag<float>::BasicTag(float data) : BasicTag(TagType::Float, data)
{
}

template <>
BasicTag<double>::BasicTag(double data) : BasicTag(TagType::Double, data)
{
}

class ListTag : public Tag, public std::vector<std::unique_ptr<Tag>>
{
public:
    ListTag() : ListTag(TagType::End, {}) {}
    ListTag(TagType type, std::vector<std::unique_ptr<Tag>> vector)
        : Tag(type), std::vector<std::unique_ptr<Tag>>(std::move(vector))
    {
    }
};

class StringTag : public Tag, public std::string
{
public:
    StringTag(std::string string = {})
        : Tag(TagType::String), std::string(std::move(string))
    {
    }
};

class CompoundTag : public Tag,
                    public std::unordered_map<std::string, std::unique_ptr<Tag>>
{
public:
    CompoundTag(std::unordered_map<std::string, std::unique_ptr<Tag>> map = {})
        : Tag(TagType::Compound),
          std::unordered_map<std::string, std::unique_ptr<Tag>>(std::move(map))
    {
    }
};

class InvalidFormatException : public std::exception
{
public:
    explicit InvalidFormatException(const std::string &what_arg)
        : _what(what_arg)
    {
    }
    explicit InvalidFormatException(const char *what_arg) : _what(what_arg) {}
    const char *what() const noexcept { return _what.c_str(); }
private:
    std::string _what;
};

template <TagType type>
class TagTypeToClass;

template <>
class TagTypeToClass<TagType::Byte>
{
public:
    using value_type = int8_t;
    using type = BasicTag<value_type>;
};
template <>
class TagTypeToClass<TagType::Short>
{
public:
    using value_type = int16_t;
    using type = BasicTag<value_type>;
};
template <>
class TagTypeToClass<TagType::Int>
{
public:
    using value_type = int32_t;
    using type = BasicTag<value_type>;
};
template <>
class TagTypeToClass<TagType::Long>
{
public:
    using value_type = int64_t;
    using type = BasicTag<value_type>;
};
template <>
class TagTypeToClass<TagType::Float>
{
public:
    using value_type = float;
    using type = BasicTag<value_type>;
};
template <>
class TagTypeToClass<TagType::Double>
{
public:
    using value_type = double;
    using type = BasicTag<value_type>;
};
template <>
class TagTypeToClass<TagType::ByteArray>
{
public:
    using type = ListTag;
};
template <>
class TagTypeToClass<TagType::String>
{
public:
    using value_type = std::string;
    using type = StringTag;
};
template <>
class TagTypeToClass<TagType::List>
{
public:
    using type = ListTag;
};
template <>
class TagTypeToClass<TagType::IntArray>
{
public:
    using type = ListTag;
};
template <>
class TagTypeToClass<TagType::Compound>
{
public:
    using type = CompoundTag;
};

template <boost::endian::order endian>
std::pair<std::string, std::unique_ptr<Tag>> parse(std::istream &input);

template <boost::endian::order endian>
void emit(std::ostream &output, const std::string &name, const Tag &tag);
}
}

#endif // CENISYS_NBT_H
