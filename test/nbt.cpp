/*
 * Test of NBT.
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

#include "util/nbt.h"
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <utility>

BOOST_TEST_DONT_PRINT_LOG_VALUE(cenisys::NBT::TagType)

template <cenisys::NBT::TagType TagType>
decltype(auto) castTagImpl(cenisys::NBT::Tag &tag)
{
    BOOST_TEST_REQUIRE(tag.getType() == TagType);
    return static_cast<typename cenisys::NBT::TagTypeToClass<TagType>::type &>(
        tag);
}

template <cenisys::NBT::TagType TagType>
void checkBasicImpl(
    cenisys::NBT::Tag &tag,
    typename cenisys::NBT::TagTypeToClass<TagType>::value_type value)
{
    BOOST_TEST((castTagImpl<TagType>(tag)) == value);
}

void checkByte(cenisys::NBT::Tag &tag, int_least8_t value)
{
    checkBasicImpl<cenisys::NBT::TagType::Byte>(tag, value);
}
void checkShort(cenisys::NBT::Tag &tag, int_least16_t value)
{
    checkBasicImpl<cenisys::NBT::TagType::Short>(tag, value);
}
void checkInt(cenisys::NBT::Tag &tag, int_least32_t value)
{
    checkBasicImpl<cenisys::NBT::TagType::Int>(tag, value);
}
void checkLong(cenisys::NBT::Tag &tag, int_least64_t value)
{
    checkBasicImpl<cenisys::NBT::TagType::Long>(tag, value);
}
void checkFloat(cenisys::NBT::Tag &tag, float value)
{
    checkBasicImpl<cenisys::NBT::TagType::Float>(tag, value);
}
void checkDouble(cenisys::NBT::Tag &tag, double value)
{
    checkBasicImpl<cenisys::NBT::TagType::Double>(tag, value);
}
void checkString(cenisys::NBT::Tag &tag, std::string value)
{
    checkBasicImpl<cenisys::NBT::TagType::String>(tag, value);
}
decltype(auto) castByteArray(cenisys::NBT::Tag &tag)
{
    return castTagImpl<cenisys::NBT::TagType::ByteArray>(tag);
}
decltype(auto) castIntArray(cenisys::NBT::Tag &tag)
{
    return castTagImpl<cenisys::NBT::TagType::IntArray>(tag);
}
decltype(auto) castList(cenisys::NBT::Tag &tag)
{
    return castTagImpl<cenisys::NBT::TagType::List>(tag);
}
decltype(auto) castCompound(cenisys::NBT::Tag &tag)
{
    return castTagImpl<cenisys::NBT::TagType::Compound>(tag);
}

BOOST_AUTO_TEST_SUITE(nbt)

constexpr unsigned char hello_world_nbt_data[] = {
    0x0a, 0x00, 0x0b, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x77, 0x6f,
    0x72, 0x6c, 0x64, 0x08, 0x00, 0x04, 0x6e, 0x61, 0x6d, 0x65, 0x00,
    0x09, 0x42, 0x61, 0x6e, 0x61, 0x6e, 0x72, 0x61, 0x6d, 0x61, 0x00};
constexpr std::size_t hello_world_nbt_len = 33;

class hello_world_nbt_fixture
{
public:
    hello_world_nbt_fixture()
        : source(hello_world_nbt_data,
                 hello_world_nbt_data + hello_world_nbt_len),
          input(source)
    {
        root = cenisys::NBT::parse<boost::endian::order::big>(input);
    }

    std::pair<std::string, std::unique_ptr<cenisys::NBT::Tag>> root;
    std::string source;
    std::istringstream input;
};

BOOST_FIXTURE_TEST_SUITE(hello_world_nbt, hello_world_nbt_fixture)

BOOST_AUTO_TEST_CASE(RootTag)
{
    BOOST_TEST_REQUIRE(root.second->getType() ==
                       cenisys::NBT::TagType::Compound);
    BOOST_TEST(root.first == "hello world");
}

BOOST_AUTO_TEST_CASE(
    stringtag, *boost::unit_test::depends_on("nbt/hello_world_nbt/RootTag"))
{
    checkString(*static_cast<cenisys::NBT::CompoundTag &>(*root.second)["name"],
                "Bananrama");
}

BOOST_AUTO_TEST_SUITE_END()

constexpr unsigned char bigtest_nbt_data[] = {
    0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xed, 0x54,
    0xcf, 0x4f, 0x1a, 0x41, 0x14, 0x7e, 0xc2, 0x02, 0xcb, 0x96, 0x82, 0xb1,
    0xc4, 0x10, 0x63, 0xcc, 0xab, 0xb5, 0x84, 0xa5, 0xdb, 0xcd, 0x42, 0x11,
    0x89, 0xb1, 0x88, 0x16, 0x2c, 0x9a, 0x0d, 0x1a, 0xd8, 0xa8, 0x31, 0x86,
    0xb8, 0x2b, 0xc3, 0x82, 0x2e, 0xbb, 0x66, 0x77, 0xb0, 0xf1, 0xd4, 0x4b,
    0x7b, 0x6c, 0x7a, 0xeb, 0x3f, 0xd3, 0x23, 0x7f, 0x43, 0xcf, 0xbd, 0xf6,
    0xbf, 0xa0, 0xc3, 0x2f, 0x7b, 0x69, 0xcf, 0xbd, 0xf0, 0x32, 0xc9, 0xf7,
    0xe6, 0xbd, 0x6f, 0xe6, 0x7b, 0x6f, 0x26, 0x79, 0x02, 0x04, 0x54, 0x72,
    0x4f, 0x2c, 0x0e, 0x78, 0xcb, 0xb1, 0x4d, 0x8d, 0x78, 0xf4, 0xe3, 0x70,
    0x62, 0x3e, 0x08, 0x7b, 0x1d, 0xc7, 0xa5, 0x93, 0x18, 0x0f, 0x82, 0x47,
    0xdd, 0xee, 0x84, 0x02, 0x62, 0xb5, 0xa2, 0xaa, 0xc7, 0x78, 0x76, 0x5c,
    0x57, 0xcb, 0xa8, 0x55, 0x0f, 0x1b, 0xc8, 0xd6, 0x1e, 0x6a, 0x95, 0x86,
    0x86, 0x0d, 0xad, 0x7e, 0x58, 0x7b, 0x8f, 0x83, 0xcf, 0x83, 0x4f, 0x83,
    0x6f, 0xcf, 0x03, 0x10, 0x6e, 0x5b, 0x8e, 0x3e, 0xbe, 0xa5, 0x38, 0x4c,
    0x64, 0xfd, 0x10, 0xea, 0xda, 0x74, 0xa6, 0x23, 0x40, 0xdc, 0x66, 0x2e,
    0x69, 0xe1, 0xb5, 0xd3, 0xbb, 0x73, 0xfa, 0x76, 0x0b, 0x29, 0xdb, 0x0b,
    0xe0, 0xef, 0xe8, 0x3d, 0x1e, 0x38, 0x5b, 0xef, 0x11, 0x08, 0x56, 0xf5,
    0xde, 0x5d, 0xdf, 0x0b, 0x40, 0xe0, 0x5e, 0xb7, 0xfa, 0x64, 0xb7, 0x04,
    0x00, 0x8c, 0x41, 0x4c, 0x73, 0xc6, 0x08, 0x55, 0x4c, 0xd3, 0x20, 0x2e,
    0x7d, 0xa4, 0xc0, 0xc8, 0xc2, 0x10, 0xb3, 0xba, 0xde, 0x58, 0x0b, 0x53,
    0xa3, 0xee, 0x44, 0x8e, 0x45, 0x03, 0x30, 0xb1, 0x27, 0x53, 0x8c, 0x4c,
    0xf1, 0xe9, 0x14, 0xa3, 0x53, 0x8c, 0x85, 0xe1, 0xd9, 0x9f, 0xe3, 0xb3,
    0xf2, 0x44, 0x81, 0xa5, 0x7c, 0x33, 0xdd, 0xd8, 0xbb, 0xc7, 0xaa, 0x75,
    0x13, 0x5f, 0x28, 0x1c, 0x08, 0xd7, 0x2e, 0xd1, 0x59, 0x3f, 0xaf, 0x1d,
    0x1b, 0x60, 0x21, 0x59, 0xdf, 0xfa, 0xf1, 0x05, 0xfe, 0xc1, 0xce, 0xfc,
    0x9d, 0xbd, 0x00, 0xbc, 0xf1, 0x40, 0xc9, 0xf8, 0x85, 0x42, 0x40, 0x46,
    0xfe, 0x9e, 0xeb, 0xea, 0x0f, 0x93, 0x3a, 0x68, 0x87, 0x60, 0xbb, 0xeb,
    0x32, 0x37, 0xa3, 0x28, 0x0a, 0x8e, 0xbb, 0xf5, 0xd0, 0x69, 0x63, 0xca,
    0x4e, 0xdb, 0xe9, 0xec, 0xe6, 0xe6, 0x2b, 0x3b, 0xbd, 0x25, 0xbe, 0x64,
    0x49, 0x09, 0x3d, 0xaa, 0xbb, 0x94, 0xfd, 0x18, 0x7e, 0xe8, 0xd2, 0x0e,
    0xda, 0x6f, 0x15, 0x4c, 0xb1, 0x68, 0x3e, 0x2b, 0xe1, 0x9b, 0x9c, 0x84,
    0x99, 0xbc, 0x84, 0x05, 0x09, 0x65, 0x59, 0x16, 0x45, 0x00, 0xff, 0x2f,
    0x28, 0xae, 0x2f, 0xf2, 0xc2, 0xb2, 0xa4, 0x2e, 0x1d, 0x20, 0x77, 0x5a,
    0x3b, 0xb9, 0x8c, 0xca, 0xe7, 0x29, 0xdf, 0x51, 0x41, 0xc9, 0x16, 0xb5,
    0xc5, 0x6d, 0xa1, 0x2a, 0xad, 0x2c, 0xc5, 0x31, 0x7f, 0xba, 0x7a, 0x92,
    0x8e, 0x5e, 0x9d, 0x5f, 0xf8, 0x12, 0x05, 0x23, 0x1b, 0xd1, 0xf6, 0xb7,
    0x77, 0xaa, 0xcd, 0x95, 0x72, 0xbc, 0x9e, 0xdf, 0x58, 0x5d, 0x4b, 0x97,
    0xae, 0x92, 0x17, 0xb9, 0x44, 0xd0, 0x80, 0xc8, 0xfa, 0x3e, 0xbf, 0xb3,
    0xdc, 0x54, 0xcb, 0x07, 0x75, 0x6e, 0xa3, 0xb6, 0x76, 0x59, 0x92, 0x93,
    0xa9, 0xdc, 0x51, 0x50, 0x99, 0x6b, 0xcc, 0x35, 0xe6, 0x1a, 0xff, 0x57,
    0x23, 0x08, 0x42, 0xcb, 0xe9, 0x1b, 0xd6, 0x78, 0xc2, 0xec, 0xfe, 0xfc,
    0x7a, 0xfb, 0x7d, 0x78, 0xd3, 0x84, 0xdf, 0xd4, 0xf2, 0xa4, 0xfb, 0x08,
    0x06, 0x00, 0x00};
constexpr std::size_t bigtest_nbt_len = 507;

class bigtest_nbt_fixture
{
public:
    bigtest_nbt_fixture()
        : source(bigtest_nbt_data, bigtest_nbt_data + bigtest_nbt_len),
          iss(source)
    {
        input.push(decompressor);
        input.push(iss);
        root = cenisys::NBT::parse<boost::endian::order::big>(input);
    }

    std::pair<std::string, std::unique_ptr<cenisys::NBT::Tag>> root;
    std::string source;
    std::istringstream iss;
    boost::iostreams::gzip_decompressor decompressor;
    boost::iostreams::filtering_istream input;
};

BOOST_FIXTURE_TEST_SUITE(bigtest_nbt, bigtest_nbt_fixture)

BOOST_AUTO_TEST_CASE(RootTag)
{
    BOOST_TEST_REQUIRE(root.second->getType() ==
                       cenisys::NBT::TagType::Compound);
    BOOST_TEST(root.first == "Level");
}

BOOST_AUTO_TEST_CASE(NestedCompound,
                     *boost::unit_test::depends_on("nbt/bigtest_nbt/RootTag"))
{
    auto &compoundTag = castCompound(*static_cast<cenisys::NBT::CompoundTag &>(
        *root.second)["nested compound test"]);

    auto &compoundEgg = castCompound(*compoundTag["egg"]);
    checkString(*compoundEgg["name"], "Eggbert");
    checkFloat(*compoundEgg["value"], 0.5);

    auto &compoundHam = castCompound(*compoundTag["ham"]);
    checkString(*compoundHam["name"], "Hampus");
    checkFloat(*compoundHam["value"], 0.75);
}

BOOST_AUTO_TEST_CASE(IntTest,
                     *boost::unit_test::depends_on("nbt/bigtest_nbt/RootTag"))
{
    checkInt(*static_cast<cenisys::NBT::CompoundTag &>(*root.second)["intTest"],
             2147483647);
}

BOOST_AUTO_TEST_CASE(ByteTest,
                     *boost::unit_test::depends_on("nbt/bigtest_nbt/RootTag"))
{
    checkByte(
        *static_cast<cenisys::NBT::CompoundTag &>(*root.second)["byteTest"],
        127);
}

BOOST_AUTO_TEST_CASE(StringTest,
                     *boost::unit_test::depends_on("nbt/bigtest_nbt/RootTag"))
{
    checkString(
        *static_cast<cenisys::NBT::CompoundTag &>(*root.second)["stringTest"],
        "HELLO WORLD THIS IS A TEST STRING \xc3\x85\xc3\x84\xc3\x96!");
}

BOOST_AUTO_TEST_CASE(ListTest,
                     *boost::unit_test::depends_on("nbt/bigtest_nbt/RootTag"))
{
    cenisys::NBT::ListTag &listTag =
        castList(*static_cast<cenisys::NBT::CompoundTag &>(
            *root.second)["listTest (long)"]);
    BOOST_TEST_REQUIRE(listTag.size() == 5);
    checkLong(*listTag[0], 11L);
    checkLong(*listTag[1], 12L);
    checkLong(*listTag[2], 13L);
    checkLong(*listTag[3], 14L);
    checkLong(*listTag[4], 15L);
}

BOOST_AUTO_TEST_CASE(DoubleTest,
                     *boost::unit_test::depends_on("nbt/bigtest_nbt/RootTag"))
{
    checkDouble(
        *static_cast<cenisys::NBT::CompoundTag &>(*root.second)["doubleTest"],
        0.49312871321823148);
}

BOOST_AUTO_TEST_CASE(FloatTest,
                     *boost::unit_test::depends_on("nbt/bigtest_nbt/RootTag"))
{
    checkFloat(
        *static_cast<cenisys::NBT::CompoundTag &>(*root.second)["floatTest"],
        0.49823147058486938f);
}

BOOST_AUTO_TEST_CASE(LongTest,
                     *boost::unit_test::depends_on("nbt/bigtest_nbt/RootTag"))
{
    checkLong(
        *static_cast<cenisys::NBT::CompoundTag &>(*root.second)["longTest"],
        9223372036854775807L);
}

BOOST_AUTO_TEST_CASE(CompoundListTest,
                     *boost::unit_test::depends_on("nbt/bigtest_nbt/RootTag"))
{
    auto &listTag = castList(*static_cast<cenisys::NBT::CompoundTag &>(
        *root.second)["listTest (compound)"]);
    BOOST_TEST_REQUIRE(listTag.size() == 2);
    auto &compoundTag0 = castCompound(*listTag[0]);
    checkLong(*compoundTag0["created-on"], 1264099775885L);
    checkString(*compoundTag0["name"], "Compound tag #0");
    auto &compoundTag1 = castCompound(*listTag[1]);
    checkLong(*compoundTag1["created-on"], 1264099775885L);
    checkString(*compoundTag1["name"], "Compound tag #1");
}

/*constexpr*/ decltype(auto) genByteArray()
{
    int counter = 0;
    auto array = std::array<int_least8_t, 1000>();
    // TODO: constexpr iterators available in C++17
    for(auto &item : array)
    {
        item = (counter * counter * 255 + counter * 7) % 100;
        counter++;
    }
    return array;
}

BOOST_AUTO_TEST_CASE(ByteArrayTest,
                     *boost::unit_test::depends_on("nbt/bigtest_nbt/RootTag"))
{
    auto &byteArrayTag = castByteArray(*static_cast<
                                       cenisys::NBT::CompoundTag &>(
        *root.second)["byteArrayTest (the first 1000 values of "
                      "(n*n*255+n*7)%100, starting with n=0 (0, 62, 34, 16, 8, "
                      "...))"]);
    auto targetArray = genByteArray();
    BOOST_TEST_REQUIRE(byteArrayTag.size() == targetArray.size());
    for(std::size_t i = 0; i < targetArray.size(); i++)
    {
        checkByte(*byteArrayTag[i], targetArray[i]);
    }
}

BOOST_AUTO_TEST_CASE(ShortTest,
                     *boost::unit_test::depends_on("nbt/bigtest_nbt/RootTag"))
{
    checkShort(
        *static_cast<cenisys::NBT::CompoundTag &>(*root.second)["shortTest"],
        32767);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
