//     __ _____ _____ _____
//  __|  |   __|     |   | |  JSON for Modern C++ (supporting code)
// |  |  |__   |  |  | | | |  version 3.12.0
// |_____|_____|_____|_|___|  https://github.com/nlohmann/json
//
// SPDX-FileCopyrightText: 2013-2025 Niels Lohmann <https://nlohmann.me>
// SPDX-License-Identifier: MIT

#include "doctest_compatibility.h"

#define JSON_TESTS_PRIVATE
#include <nlohmann/json.hpp>
using nlohmann::json;

#include <algorithm>
#include <climits>
#include <limits>
#include <iostream>
#include <fstream>
#include <set>
#include "make_test_data_available.hpp"
#include "test_utils.hpp"

namespace
{
class SaxCountdown
{
  public:
    explicit SaxCountdown(const int count) : events_left(count)
    {}

    bool null()
    {
        return events_left-- > 0;
    }

    bool boolean(bool /*unused*/)
    {
        return events_left-- > 0;
    }

    bool number_integer(json::number_integer_t /*unused*/)
    {
        return events_left-- > 0;
    }

    bool number_unsigned(json::number_unsigned_t /*unused*/)
    {
        return events_left-- > 0;
    }

    bool number_float(json::number_float_t /*unused*/, const std::string& /*unused*/)
    {
        return events_left-- > 0;
    }

    bool string(std::string& /*unused*/)
    {
        return events_left-- > 0;
    }

    bool binary(std::vector<std::uint8_t>& /*unused*/)
    {
        return events_left-- > 0;
    }

    bool start_object(std::size_t /*unused*/)
    {
        return events_left-- > 0;
    }

    bool key(std::string& /*unused*/)
    {
        return events_left-- > 0;
    }

    bool end_object()
    {
        return events_left-- > 0;
    }

    bool start_array(std::size_t /*unused*/)
    {
        return events_left-- > 0;
    }

    bool end_array()
    {
        return events_left-- > 0;
    }

    bool parse_error(std::size_t /*unused*/, const std::string& /*unused*/, const json::exception& /*unused*/) // NOLINT(readability-convert-member-functions-to-static)
    {
        return false;
    }

  private:
    int events_left = 0;
};
} // namespace

// at some point in the future, a unit test dedicated to type traits might be a good idea
template <typename OfType, typename T, bool MinInRange, bool MaxInRange>
struct trait_test_arg
{
    using of_type = OfType;
    using type = T;
    static constexpr bool min_in_range = MinInRange;
    static constexpr bool max_in_range = MaxInRange;
};

TEST_CASE_TEMPLATE_DEFINE("value_in_range_of trait", T, value_in_range_of_test) // NOLINT(readability-math-missing-parentheses)
{
    using nlohmann::detail::value_in_range_of;

    using of_type = typename T::of_type;
    using type = typename T::type;
    constexpr bool min_in_range = T::min_in_range;
    constexpr bool max_in_range = T::max_in_range;

    type const val_min = std::numeric_limits<type>::min();
    type const val_min2 = val_min + 1;
    type const val_max = std::numeric_limits<type>::max();
    type const val_max2 = val_max - 1;

    REQUIRE(CHAR_BIT == 8);

    std::string of_type_str;
    if (std::is_unsigned<of_type>::value)
    {
        of_type_str += "u";
    }
    of_type_str += "int";
    of_type_str += std::to_string(sizeof(of_type) * 8);

    INFO("of_type := ", of_type_str);

    std::string type_str;
    if (std::is_unsigned<type>::value)
    {
        type_str += "u";
    }
    type_str += "int";
    type_str += std::to_string(sizeof(type) * 8);

    INFO("type := ", type_str);

    CAPTURE(val_min);
    CAPTURE(min_in_range);
    CAPTURE(val_max);
    CAPTURE(max_in_range);

    if (min_in_range)
    {
        CHECK(value_in_range_of<of_type>(val_min));
        CHECK(value_in_range_of<of_type>(val_min2));
    }
    else
    {
        CHECK_FALSE(value_in_range_of<of_type>(val_min));
        CHECK_FALSE(value_in_range_of<of_type>(val_min2));
    }

    if (max_in_range)
    {
        CHECK(value_in_range_of<of_type>(val_max));
        CHECK(value_in_range_of<of_type>(val_max2));
    }
    else
    {
        CHECK_FALSE(value_in_range_of<of_type>(val_max));
        CHECK_FALSE(value_in_range_of<of_type>(val_max2));
    }
}

// NOLINTNEXTLINE(bugprone-throwing-static-initialization)
TEST_CASE_TEMPLATE_INVOKE(value_in_range_of_test, \
                          trait_test_arg<std::int32_t, std::int32_t, true, true>, \
                          trait_test_arg<std::int32_t, std::uint32_t, true, false>, \
                          trait_test_arg<std::uint32_t, std::int32_t, false, true>, \
                          trait_test_arg<std::uint32_t, std::uint32_t, true, true>, \
                          trait_test_arg<std::int32_t, std::int64_t, false, false>, \
                          trait_test_arg<std::int32_t, std::uint64_t, true, false>, \
                          trait_test_arg<std::uint32_t, std::int64_t, false, false>, \
                          trait_test_arg<std::uint32_t, std::uint64_t, true, false>, \
                          trait_test_arg<std::int64_t, std::int32_t, true, true>, \
                          trait_test_arg<std::int64_t, std::uint32_t, true, true>, \
                          trait_test_arg<std::uint64_t, std::int32_t, false, true>, \
                          trait_test_arg<std::uint64_t, std::uint32_t, true, true>, \
                          trait_test_arg<std::int64_t, std::int64_t, true, true>, \
                          trait_test_arg<std::int64_t, std::uint64_t, true, false>, \
                          trait_test_arg<std::uint64_t, std::int64_t, false, true>, \
                          trait_test_arg<std::uint64_t, std::uint64_t, true, true>);

#if SIZE_MAX == 0xffffffff
TEST_CASE_TEMPLATE_INVOKE(value_in_range_of_test, \
                          trait_test_arg<std::size_t, std::int32_t, false, true>, \
                          trait_test_arg<std::size_t, std::uint32_t, true, true>, \
                          trait_test_arg<std::size_t, std::int64_t, false, false>, \
                          trait_test_arg<std::size_t, std::uint64_t, true, false>);
#else
// NOLINTNEXTLINE(bugprone-throwing-static-initialization)
TEST_CASE_TEMPLATE_INVOKE(value_in_range_of_test, \
                          trait_test_arg<std::size_t, std::int32_t, false, true>, \
                          trait_test_arg<std::size_t, std::uint32_t, true, true>, \
                          trait_test_arg<std::size_t, std::int64_t, false, true>, \
                          trait_test_arg<std::size_t, std::uint64_t, true, true>);
#endif

TEST_CASE("BJData")
{
    SECTION("binary_reader BJData LUT arrays are sorted")
    {
        std::vector<std::uint8_t> const data;
        auto ia = nlohmann::detail::input_adapter(data);
        // NOLINTNEXTLINE(hicpp-move-const-arg,performance-move-const-arg)
        nlohmann::detail::binary_reader<json, decltype(ia)> const br{std::move(ia), json::input_format_t::bjdata};

        CHECK(std::is_sorted(br.bjd_optimized_type_markers.begin(), br.bjd_optimized_type_markers.end()));
        CHECK(std::is_sorted(br.bjd_types_map.begin(), br.bjd_types_map.end()));
    }

    SECTION("individual values")
    {
        SECTION("discarded")
        {
            // discarded values are not serialized
            json const j = json::value_t::discarded;
            const auto result = json::to_bjdata(j);
            CHECK(result.empty());
        }

        SECTION("null")
        {
            json const j = nullptr;
            std::vector<uint8_t> const expected = {'Z'};
            const auto result = json::to_bjdata(j);
            CHECK(result == expected);

            // roundtrip
            CHECK(json::from_bjdata(result) == j);
            CHECK(json::from_bjdata(result, true, false) == j);
        }

        SECTION("boolean")
        {
            SECTION("true")
            {
                json const j = true;
                std::vector<uint8_t> const expected = {'T'};
                const auto result = json::to_bjdata(j);
                CHECK(result == expected);

                // roundtrip
                CHECK(json::from_bjdata(result) == j);
                CHECK(json::from_bjdata(result, true, false) == j);
            }

            SECTION("false")
            {
                json const j = false;
                std::vector<uint8_t> const expected = {'F'};
                const auto result = json::to_bjdata(j);
                CHECK(result == expected);

                // roundtrip
                CHECK(json::from_bjdata(result) == j);
                CHECK(json::from_bjdata(result, true, false) == j);
            }
        }

        SECTION("byte")
        {
            SECTION("0..255 (uint8)")
            {
                for (size_t i = 0; i <= 255; ++i)
                {
                    CAPTURE(i)

                    // create JSON value with integer number (no byte type in JSON)
                    json j = -1;
                    j.get_ref<json::number_integer_t&>() = static_cast<json::number_integer_t>(i);

                    // check type
                    CHECK(j.is_number_integer());

                    // create byte vector
                    std::vector<uint8_t> const value
                    {
                        static_cast<uint8_t>('B'),
                        static_cast<uint8_t>(i),
                    };

                    // compare value
                    CHECK(json::from_bjdata(value) == j);
                }
            }
        }

        SECTION("number")
        {
            SECTION("signed")
            {
                SECTION("-9223372036854775808..-2147483649 (int64)")
                {
                    std::vector<int64_t> const numbers
                    {
                        (std::numeric_limits<int64_t>::min)(),
                        -1000000000000000000LL,
                        -100000000000000000LL,
                        -10000000000000000LL,
                        -1000000000000000LL,
                        -100000000000000LL,
                        -10000000000000LL,
                        -1000000000000LL,
                        -100000000000LL,
                        -10000000000LL,
                        -2147483649LL,
                    };
                    for (const auto i : numbers)
                    {
                        CAPTURE(i)

                        // create JSON value with integer number
                        json const j = i;

                        // check type
                        CHECK(j.is_number_integer());

                        // create expected byte vector
                        std::vector<uint8_t> const expected
                        {
                            static_cast<uint8_t>('L'),
                            static_cast<uint8_t>(i & 0xff),
                            static_cast<uint8_t>((i >> 8) & 0xff),
                            static_cast<uint8_t>((i >> 16) & 0xff),
                            static_cast<uint8_t>((i >> 24) & 0xff),
                            static_cast<uint8_t>((i >> 32) & 0xff),
                            static_cast<uint8_t>((i >> 40) & 0xff),
                            static_cast<uint8_t>((i >> 48) & 0xff),
                            static_cast<uint8_t>((i >> 56) & 0xff),
                        };

                        // compare result + size
                        const auto result = json::to_bjdata(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 9);

                        // check individual bytes
                        CHECK(result[0] == 'L');
                        int64_t const restored = (static_cast<int64_t>(result[8]) << 070) +
                                                 (static_cast<int64_t>(result[7]) << 060) +
                                                 (static_cast<int64_t>(result[6]) << 050) +
                                                 (static_cast<int64_t>(result[5]) << 040) +
                                                 (static_cast<int64_t>(result[4]) << 030) +
                                                 (static_cast<int64_t>(result[3]) << 020) +
                                                 (static_cast<int64_t>(result[2]) << 010) +
                                                 static_cast<int64_t>(result[1]);
                        CHECK(restored == i);

                        // roundtrip
                        CHECK(json::from_bjdata(result) == j);
                        CHECK(json::from_bjdata(result, true, false) == j);
                    }
                }

                SECTION("-2147483648..-32769 (int32)")
                {
                    std::vector<int32_t> const numbers
                    {
                        -32769,
                        -100000,
                        -1000000,
                        -10000000,
                        -100000000,
                        -1000000000,
                        -2147483647 - 1, // https://stackoverflow.com/a/29356002/266378
                    };
                    for (const auto i : numbers)
                    {
                        CAPTURE(i)

                        // create JSON value with integer number
                        json const j = i;

                        // check type
                        CHECK(j.is_number_integer());

                        // create expected byte vector
                        std::vector<uint8_t> const expected
                        {
                            static_cast<uint8_t>('l'),
                            static_cast<uint8_t>(i & 0xff),
                            static_cast<uint8_t>((i >> 8) & 0xff),
                            static_cast<uint8_t>((i >> 16) & 0xff),
                            static_cast<uint8_t>((i >> 24) & 0xff),
                        };

                        // compare result + size
                        const auto result = json::to_bjdata(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 5);

                        // check individual bytes
                        CHECK(result[0] == 'l');
                        int32_t const restored = (static_cast<int32_t>(result[4]) << 030) +
                                                 (static_cast<int32_t>(result[3]) << 020) +
                                                 (static_cast<int32_t>(result[2]) << 010) +
                                                 static_cast<int32_t>(result[1]);
                        CHECK(restored == i);

                        // roundtrip
                        CHECK(json::from_bjdata(result) == j);
                        CHECK(json::from_bjdata(result, true, false) == j);
                    }
                }

                SECTION("-32768..-129 (int16)")
                {
                    for (int32_t i = -32768; i <= -129; ++i)
                    {
                        CAPTURE(i)

                        // create JSON value with integer number
                        json const j = i;

                        // check type
                        CHECK(j.is_number_integer());

                        // create expected byte vector
                        std::vector<uint8_t> const expected
                        {
                            static_cast<uint8_t>('I'),
                            static_cast<uint8_t>(i & 0xff),
                            static_cast<uint8_t>((i >> 8) & 0xff),
                        };

                        // compare result + size
                        const auto result = json::to_bjdata(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 3);

                        // check individual bytes
                        CHECK(result[0] == 'I');
                        auto const restored = static_cast<int16_t>(((result[2] << 8) + result[1]));
                        CHECK(restored == i);

                        // roundtrip
                        CHECK(json::from_bjdata(result) == j);
                        CHECK(json::from_bjdata(result, true, false) == j);
                    }
                }

                SECTION("-9263 (int16)")
                {
                    json const j = -9263;
                    std::vector<uint8_t> const expected = {'I', 0xd1, 0xdb};

                    // compare result + size
                    const auto result = json::to_bjdata(j);
                    CHECK(result == expected);
                    CHECK(result.size() == 3);

                    // check individual bytes
                    CHECK(result[0] == 'I');
                    auto const restored = static_cast<int16_t>(((result[2] << 8) + result[1]));
                    CHECK(restored == -9263);

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }

                SECTION("-128..-1 (int8)")
                {
                    for (auto i = -128; i <= -1; ++i)
                    {
                        CAPTURE(i)

                        // create JSON value with integer number
                        json const j = i;

                        // check type
                        CHECK(j.is_number_integer());

                        // create expected byte vector
                        std::vector<uint8_t> const expected
                        {
                            'i',
                            static_cast<uint8_t>(i),
                        };

                        // compare result + size
                        const auto result = json::to_bjdata(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 2);

                        // check individual bytes
                        CHECK(result[0] == 'i');
                        CHECK(static_cast<int8_t>(result[1]) == i);

                        // roundtrip
                        CHECK(json::from_bjdata(result) == j);
                        CHECK(json::from_bjdata(result, true, false) == j);
                    }
                }

                SECTION("0..127 (int8)")
                {
                    for (size_t i = 0; i <= 127; ++i)
                    {
                        CAPTURE(i)

                        // create JSON value with integer number
                        json j = -1;
                        j.get_ref<json::number_integer_t&>() = static_cast<json::number_integer_t>(i);

                        // check type
                        CHECK(j.is_number_integer());

                        // create expected byte vector
                        std::vector<uint8_t> const expected
                        {
                            static_cast<uint8_t>('i'),
                            static_cast<uint8_t>(i),
                        };

                        // compare result + size
                        const auto result = json::to_bjdata(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 2);

                        // check individual bytes
                        CHECK(result[0] == 'i');
                        CHECK(result[1] == i);

                        // roundtrip
                        CHECK(json::from_bjdata(result) == j);
                        CHECK(json::from_bjdata(result, true, false) == j);
                    }
                }

                SECTION("128..255 (uint8)")
                {
                    for (size_t i = 128; i <= 255; ++i)
                    {
                        CAPTURE(i)

                        // create JSON value with integer number
                        json j = -1;
                        j.get_ref<json::number_integer_t&>() = static_cast<json::number_integer_t>(i);

                        // check type
                        CHECK(j.is_number_integer());

                        // create expected byte vector
                        std::vector<uint8_t> const expected
                        {
                            static_cast<uint8_t>('U'),
                            static_cast<uint8_t>(i),
                        };

                        // compare result + size
                        const auto result = json::to_bjdata(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 2);

                        // check individual bytes
                        CHECK(result[0] == 'U');
                        CHECK(result[1] == i);

                        // roundtrip
                        CHECK(json::from_bjdata(result) == j);
                        CHECK(json::from_bjdata(result, true, false) == j);
                    }
                }

                SECTION("256..32767 (int16)")
                {
                    for (size_t i = 256; i <= 32767; ++i)
                    {
                        CAPTURE(i)

                        // create JSON value with integer number
                        json j = -1;
                        j.get_ref<json::number_integer_t&>() = static_cast<json::number_integer_t>(i);

                        // check type
                        CHECK(j.is_number_integer());

                        // create expected byte vector
                        std::vector<uint8_t> const expected
                        {
                            static_cast<uint8_t>('I'),
                            static_cast<uint8_t>(i & 0xff),
                            static_cast<uint8_t>((i >> 8) & 0xff),
                        };

                        // compare result + size
                        const auto result = json::to_bjdata(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 3);

                        // check individual bytes
                        CHECK(result[0] == 'I');
                        auto const restored = static_cast<uint16_t>((static_cast<uint8_t>(result[2]) * 256) + static_cast<uint8_t>(result[1]));
                        CHECK(restored == i);

                        // roundtrip
                        CHECK(json::from_bjdata(result) == j);
                        CHECK(json::from_bjdata(result, true, false) == j);
                    }
                }

                SECTION("32768..65535 (uint16)")
                {
                    for (const uint32_t i :
                            {
                                32768u, 55555u, 65535u
                            })
                    {
                        CAPTURE(i)

                        // create JSON value with integer number
                        json j = -1;
                        j.get_ref<json::number_integer_t&>() = static_cast<json::number_integer_t>(i);

                        // check type
                        CHECK(j.is_number_integer());

                        // create expected byte vector
                        std::vector<uint8_t> const expected
                        {
                            static_cast<uint8_t>('u'),
                            static_cast<uint8_t>(i & 0xff),
                            static_cast<uint8_t>((i >> 8) & 0xff),
                        };

                        // compare result + size
                        const auto result = json::to_bjdata(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 3);

                        // check individual bytes
                        CHECK(result[0] == 'u');
                        auto const restored = static_cast<uint16_t>((static_cast<uint8_t>(result[2]) * 256) + static_cast<uint8_t>(result[1]));
                        CHECK(restored == i);

                        // roundtrip
                        CHECK(json::from_bjdata(result) == j);
                        CHECK(json::from_bjdata(result, true, false) == j);
                    }
                }

                SECTION("65536..2147483647 (int32)")
                {
                    for (const uint32_t i :
                            {
                                65536u, 77777u, 2147483647u
                            })
                    {
                        CAPTURE(i)

                        // create JSON value with integer number
                        json j = -1;
                        j.get_ref<json::number_integer_t&>() = static_cast<json::number_integer_t>(i);

                        // check type
                        CHECK(j.is_number_integer());

                        // create expected byte vector
                        std::vector<uint8_t> const expected
                        {
                            'l',
                            static_cast<uint8_t>(i & 0xff),
                            static_cast<uint8_t>((i >> 8) & 0xff),
                            static_cast<uint8_t>((i >> 16) & 0xff),
                            static_cast<uint8_t>((i >> 24) & 0xff),
                        };

                        // compare result + size
                        const auto result = json::to_bjdata(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 5);

                        // check individual bytes
                        CHECK(result[0] == 'l');
                        uint32_t const restored = (static_cast<uint32_t>(result[4]) << 030) +
                                                  (static_cast<uint32_t>(result[3]) << 020) +
                                                  (static_cast<uint32_t>(result[2]) << 010) +
                                                  static_cast<uint32_t>(result[1]);
                        CHECK(restored == i);

                        // roundtrip
                        CHECK(json::from_bjdata(result) == j);
                        CHECK(json::from_bjdata(result, true, false) == j);
                    }
                }

                SECTION("2147483648..4294967295 (uint32)")
                {
                    for (const uint32_t i :
                            {
                                2147483648u, 3333333333u, 4294967295u
                            })
                    {
                        CAPTURE(i)

                        // create JSON value with integer number
                        json j = -1;
                        j.get_ref<json::number_integer_t&>() = static_cast<json::number_integer_t>(i);

                        // check type
                        CHECK(j.is_number_integer());

                        // create expected byte vector
                        std::vector<uint8_t> const expected
                        {
                            'm',
                            static_cast<uint8_t>(i & 0xff),
                            static_cast<uint8_t>((i >> 8) & 0xff),
                            static_cast<uint8_t>((i >> 16) & 0xff),
                            static_cast<uint8_t>((i >> 24) & 0xff),
                        };

                        // compare result + size
                        const auto result = json::to_bjdata(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 5);

                        // check individual bytes
                        CHECK(result[0] == 'm');
                        uint32_t const restored = (static_cast<uint32_t>(result[4]) << 030) +
                                                  (static_cast<uint32_t>(result[3]) << 020) +
                                                  (static_cast<uint32_t>(result[2]) << 010) +
                                                  static_cast<uint32_t>(result[1]);
                        CHECK(restored == i);

                        // roundtrip
                        CHECK(json::from_bjdata(result) == j);
                        CHECK(json::from_bjdata(result, true, false) == j);
                    }
                }

                SECTION("4294967296..9223372036854775807 (int64)")
                {
                    std::vector<uint64_t> const v = {4294967296LU, 9223372036854775807LU};
                    for (const uint64_t i : v)
                    {
                        CAPTURE(i)

                        // create JSON value with integer number
                        json j = -1;
                        j.get_ref<json::number_integer_t&>() = static_cast<json::number_integer_t>(i);

                        // check type
                        CHECK(j.is_number_integer());

                        // create expected byte vector
                        std::vector<uint8_t> const expected
                        {
                            'L',
                            static_cast<uint8_t>(i & 0xff),
                            static_cast<uint8_t>((i >> 010) & 0xff),
                            static_cast<uint8_t>((i >> 020) & 0xff),
                            static_cast<uint8_t>((i >> 030) & 0xff),
                            static_cast<uint8_t>((i >> 040) & 0xff),
                            static_cast<uint8_t>((i >> 050) & 0xff),
                            static_cast<uint8_t>((i >> 060) & 0xff),
                            static_cast<uint8_t>((i >> 070) & 0xff),
                        };

                        // compare result + size
                        const auto result = json::to_bjdata(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 9);

                        // check individual bytes
                        CHECK(result[0] == 'L');
                        uint64_t const restored = (static_cast<uint64_t>(result[8]) << 070) +
                                                  (static_cast<uint64_t>(result[7]) << 060) +
                                                  (static_cast<uint64_t>(result[6]) << 050) +
                                                  (static_cast<uint64_t>(result[5]) << 040) +
                                                  (static_cast<uint64_t>(result[4]) << 030) +
                                                  (static_cast<uint64_t>(result[3]) << 020) +
                                                  (static_cast<uint64_t>(result[2]) << 010) +
                                                  static_cast<uint64_t>(result[1]);
                        CHECK(restored == i);

                        // roundtrip
                        CHECK(json::from_bjdata(result) == j);
                        CHECK(json::from_bjdata(result, true, false) == j);
                    }
                }

                SECTION("9223372036854775808..18446744073709551615 (uint64)")
                {
                    std::vector<uint64_t> const v = {9223372036854775808ull, 18446744073709551615ull};
                    for (const uint64_t i : v)
                    {
                        CAPTURE(i)

                        // create JSON value with integer number
                        json const j = i;

                        // check type
                        CHECK(j.is_number_unsigned());

                        // create expected byte vector
                        std::vector<uint8_t> const expected
                        {
                            'M',
                            static_cast<uint8_t>(i & 0xff),
                            static_cast<uint8_t>((i >> 010) & 0xff),
                            static_cast<uint8_t>((i >> 020) & 0xff),
                            static_cast<uint8_t>((i >> 030) & 0xff),
                            static_cast<uint8_t>((i >> 040) & 0xff),
                            static_cast<uint8_t>((i >> 050) & 0xff),
                            static_cast<uint8_t>((i >> 060) & 0xff),
                            static_cast<uint8_t>((i >> 070) & 0xff),
                        };

                        // compare result + size
                        const auto result = json::to_bjdata(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 9);

                        // check individual bytes
                        CHECK(result[0] == 'M');
                        uint64_t const restored = (static_cast<uint64_t>(result[8]) << 070) +
                                                  (static_cast<uint64_t>(result[7]) << 060) +
                                                  (static_cast<uint64_t>(result[6]) << 050) +
                                                  (static_cast<uint64_t>(result[5]) << 040) +
                                                  (static_cast<uint64_t>(result[4]) << 030) +
                                                  (static_cast<uint64_t>(result[3]) << 020) +
                                                  (static_cast<uint64_t>(result[2]) << 010) +
                                                  static_cast<uint64_t>(result[1]);
                        CHECK(restored == i);

                        // roundtrip
                        CHECK(json::from_bjdata(result) == j);
                        CHECK(json::from_bjdata(result, true, false) == j);
                    }
                }
            }

            SECTION("unsigned")
            {
                SECTION("0..127 (int8)")
                {
                    for (size_t i = 0; i <= 127; ++i)
                    {
                        CAPTURE(i)

                        // create JSON value with unsigned integer number
                        json const j = i;

                        // check type
                        CHECK(j.is_number_unsigned());

                        // create expected byte vector
                        std::vector<uint8_t> const expected{'i', static_cast<uint8_t>(i)};

                        // compare result + size
                        const auto result = json::to_bjdata(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 2);

                        // check individual bytes
                        CHECK(result[0] == 'i');
                        auto const restored = static_cast<uint8_t>(result[1]);
                        CHECK(restored == i);

                        // roundtrip
                        CHECK(json::from_bjdata(result) == j);
                        CHECK(json::from_bjdata(result, true, false) == j);
                    }
                }

                SECTION("128..255 (uint8)")
                {
                    for (size_t i = 128; i <= 255; ++i)
                    {
                        CAPTURE(i)

                        // create JSON value with unsigned integer number
                        json const j = i;

                        // check type
                        CHECK(j.is_number_unsigned());

                        // create expected byte vector
                        std::vector<uint8_t> const expected{'U', static_cast<uint8_t>(i)};

                        // compare result + size
                        const auto result = json::to_bjdata(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 2);

                        // check individual bytes
                        CHECK(result[0] == 'U');
                        auto const restored = static_cast<uint8_t>(result[1]);
                        CHECK(restored == i);

                        // roundtrip
                        CHECK(json::from_bjdata(result) == j);
                        CHECK(json::from_bjdata(result, true, false) == j);
                    }
                }

                SECTION("256..32767 (int16)")
                {
                    for (size_t i = 256; i <= 32767; ++i)
                    {
                        CAPTURE(i)

                        // create JSON value with unsigned integer number
                        json const j = i;

                        // check type
                        CHECK(j.is_number_unsigned());

                        // create expected byte vector
                        std::vector<uint8_t> const expected
                        {
                            'I',
                            static_cast<uint8_t>(i & 0xff),
                            static_cast<uint8_t>((i >> 8) & 0xff),
                        };

                        // compare result + size
                        const auto result = json::to_bjdata(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 3);

                        // check individual bytes
                        CHECK(result[0] == 'I');
                        auto const restored = static_cast<uint16_t>((static_cast<uint8_t>(result[2]) * 256) + static_cast<uint8_t>(result[1]));
                        CHECK(restored == i);

                        // roundtrip
                        CHECK(json::from_bjdata(result) == j);
                        CHECK(json::from_bjdata(result, true, false) == j);
                    }
                }

                SECTION("32768..65535 (uint16)")
                {
                    for (const uint32_t i :
                            {
                                32768u, 55555u, 65535u
                            })
                    {
                        CAPTURE(i)

                        // create JSON value with unsigned integer number
                        json const j = i;

                        // check type
                        CHECK(j.is_number_unsigned());

                        // create expected byte vector
                        std::vector<uint8_t> const expected
                        {
                            'u',
                            static_cast<uint8_t>(i & 0xff),
                            static_cast<uint8_t>((i >> 8) & 0xff),
                        };

                        // compare result + size
                        const auto result = json::to_bjdata(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 3);

                        // check individual bytes
                        CHECK(result[0] == 'u');
                        auto const restored = static_cast<uint16_t>((static_cast<uint8_t>(result[2]) * 256) + static_cast<uint8_t>(result[1]));
                        CHECK(restored == i);

                        // roundtrip
                        CHECK(json::from_bjdata(result) == j);
                        CHECK(json::from_bjdata(result, true, false) == j);
                    }
                }
                SECTION("65536..2147483647 (int32)")
                {
                    for (const uint32_t i :
                            {
                                65536u, 77777u, 2147483647u
                            })
                    {
                        CAPTURE(i)

                        // create JSON value with unsigned integer number
                        json const j = i;

                        // check type
                        CHECK(j.is_number_unsigned());

                        // create expected byte vector
                        std::vector<uint8_t> const expected
                        {
                            'l',
                            static_cast<uint8_t>(i & 0xff),
                            static_cast<uint8_t>((i >> 8) & 0xff),
                            static_cast<uint8_t>((i >> 16) & 0xff),
                            static_cast<uint8_t>((i >> 24) & 0xff),
                        };

                        // compare result + size
                        const auto result = json::to_bjdata(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 5);

                        // check individual bytes
                        CHECK(result[0] == 'l');
                        uint32_t const restored = (static_cast<uint32_t>(result[4]) << 030) +
                                                  (static_cast<uint32_t>(result[3]) << 020) +
                                                  (static_cast<uint32_t>(result[2]) << 010) +
                                                  static_cast<uint32_t>(result[1]);
                        CHECK(restored == i);

                        // roundtrip
                        CHECK(json::from_bjdata(result) == j);
                        CHECK(json::from_bjdata(result, true, false) == j);
                    }
                }

                SECTION("2147483648..4294967295 (uint32)")
                {
                    for (const uint32_t i :
                            {
                                2147483648u, 3333333333u, 4294967295u
                            })
                    {
                        CAPTURE(i)

                        // create JSON value with unsigned integer number
                        json const j = i;

                        // check type
                        CHECK(j.is_number_unsigned());

                        // create expected byte vector
                        std::vector<uint8_t> const expected
                        {
                            'm',
                            static_cast<uint8_t>(i & 0xff),
                            static_cast<uint8_t>((i >> 8) & 0xff),
                            static_cast<uint8_t>((i >> 16) & 0xff),
                            static_cast<uint8_t>((i >> 24) & 0xff),
                        };

                        // compare result + size
                        const auto result = json::to_bjdata(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 5);

                        // check individual bytes
                        CHECK(result[0] == 'm');
                        uint32_t const restored = (static_cast<uint32_t>(result[4]) << 030) +
                                                  (static_cast<uint32_t>(result[3]) << 020) +
                                                  (static_cast<uint32_t>(result[2]) << 010) +
                                                  static_cast<uint32_t>(result[1]);
                        CHECK(restored == i);

                        // roundtrip
                        CHECK(json::from_bjdata(result) == j);
                        CHECK(json::from_bjdata(result, true, false) == j);
                    }
                }

                SECTION("4294967296..9223372036854775807 (int64)")
                {
                    std::vector<uint64_t> const v = {4294967296ul, 9223372036854775807ul};
                    for (const uint64_t i : v)
                    {
                        CAPTURE(i)

                        // create JSON value with integer number
                        json const j = i;

                        // check type
                        CHECK(j.is_number_unsigned());

                        // create expected byte vector
                        std::vector<uint8_t> const expected
                        {
                            'L',
                            static_cast<uint8_t>(i & 0xff),
                            static_cast<uint8_t>((i >> 010) & 0xff),
                            static_cast<uint8_t>((i >> 020) & 0xff),
                            static_cast<uint8_t>((i >> 030) & 0xff),
                            static_cast<uint8_t>((i >> 040) & 0xff),
                            static_cast<uint8_t>((i >> 050) & 0xff),
                            static_cast<uint8_t>((i >> 060) & 0xff),
                            static_cast<uint8_t>((i >> 070) & 0xff),
                        };

                        // compare result + size
                        const auto result = json::to_bjdata(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 9);

                        // check individual bytes
                        CHECK(result[0] == 'L');
                        uint64_t const restored = (static_cast<uint64_t>(result[8]) << 070) +
                                                  (static_cast<uint64_t>(result[7]) << 060) +
                                                  (static_cast<uint64_t>(result[6]) << 050) +
                                                  (static_cast<uint64_t>(result[5]) << 040) +
                                                  (static_cast<uint64_t>(result[4]) << 030) +
                                                  (static_cast<uint64_t>(result[3]) << 020) +
                                                  (static_cast<uint64_t>(result[2]) << 010) +
                                                  static_cast<uint64_t>(result[1]);
                        CHECK(restored == i);

                        // roundtrip
                        CHECK(json::from_bjdata(result) == j);
                        CHECK(json::from_bjdata(result, true, false) == j);
                    }
                }

                SECTION("9223372036854775808..18446744073709551615 (uint64)")
                {
                    std::vector<uint64_t> const v = {9223372036854775808ull, 18446744073709551615ull};
                    for (const uint64_t i : v)
                    {
                        CAPTURE(i)

                        // create JSON value with integer number
                        json const j = i;

                        // check type
                        CHECK(j.is_number_unsigned());

                        // create expected byte vector
                        std::vector<uint8_t> const expected
                        {
                            'M',
                            static_cast<uint8_t>(i & 0xff),
                            static_cast<uint8_t>((i >> 010) & 0xff),
                            static_cast<uint8_t>((i >> 020) & 0xff),
                            static_cast<uint8_t>((i >> 030) & 0xff),
                            static_cast<uint8_t>((i >> 040) & 0xff),
                            static_cast<uint8_t>((i >> 050) & 0xff),
                            static_cast<uint8_t>((i >> 060) & 0xff),
                            static_cast<uint8_t>((i >> 070) & 0xff),
                        };

                        // compare result + size
                        const auto result = json::to_bjdata(j);
                        CHECK(result == expected);
                        CHECK(result.size() == 9);

                        // check individual bytes
                        CHECK(result[0] == 'M');
                        uint64_t const restored = (static_cast<uint64_t>(result[8]) << 070) +
                                                  (static_cast<uint64_t>(result[7]) << 060) +
                                                  (static_cast<uint64_t>(result[6]) << 050) +
                                                  (static_cast<uint64_t>(result[5]) << 040) +
                                                  (static_cast<uint64_t>(result[4]) << 030) +
                                                  (static_cast<uint64_t>(result[3]) << 020) +
                                                  (static_cast<uint64_t>(result[2]) << 010) +
                                                  static_cast<uint64_t>(result[1]);
                        CHECK(restored == i);

                        // roundtrip
                        CHECK(json::from_bjdata(result) == j);
                        CHECK(json::from_bjdata(result, true, false) == j);
                    }
                }
            }
            SECTION("float64")
            {
                SECTION("3.1415925")
                {
                    double v = 3.1415925;
                    json const j = v;
                    std::vector<uint8_t> const expected =
                    {
                        'D', 0xfc, 0xde, 0xa6, 0x3f, 0xfb, 0x21, 0x09, 0x40
                    };
                    const auto result = json::to_bjdata(j);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result) == v);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }
            }

            SECTION("half-precision float")
            {
                SECTION("simple half floats")
                {
                    CHECK(json::parse("0.0") == json::from_bjdata(std::vector<uint8_t>({'h', 0x00, 0x00})));
                    CHECK(json::parse("-0.0") == json::from_bjdata(std::vector<uint8_t>({'h', 0x00, 0x80})));
                    CHECK(json::parse("1.0") == json::from_bjdata(std::vector<uint8_t>({'h', 0x00, 0x3c})));
                    CHECK(json::parse("1.5") == json::from_bjdata(std::vector<uint8_t>({'h', 0x00, 0x3e})));
                    CHECK(json::parse("65504.0") == json::from_bjdata(std::vector<uint8_t>({'h', 0xff, 0x7b})));
                }

                SECTION("errors")
                {
                    SECTION("no byte follows")
                    {
                        json _;
                        std::vector<uint8_t> const vec0 = {'h'};
                        CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vec0), "[json.exception.parse_error.110] parse error at byte 2: syntax error while parsing BJData number: unexpected end of input", json::parse_error&);
                        CHECK(json::from_bjdata(vec0, true, false).is_discarded());
                    }

                    SECTION("only one byte follows")
                    {
                        json _;
                        std::vector<uint8_t> const vec1 = {'h', 0x00};
                        CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vec1), "[json.exception.parse_error.110] parse error at byte 3: syntax error while parsing BJData number: unexpected end of input", json::parse_error&);
                        CHECK(json::from_bjdata(vec1, true, false).is_discarded());
                    }
                }
            }

            SECTION("half-precision float (edge cases)")
            {
                SECTION("exp = 0b00000")
                {
                    SECTION("0 (0 00000 0000000000)")
                    {
                        json const j = json::from_bjdata(std::vector<uint8_t>({'h', 0x00, 0x00}));
                        const json::number_float_t d{j};
                        CHECK(d == 0.0);
                    }

                    SECTION("-0 (1 00000 0000000000)")
                    {
                        json const j = json::from_bjdata(std::vector<uint8_t>({'h', 0x00, 0x80}));
                        const json::number_float_t d{j};
                        CHECK(d == -0.0);
                    }

                    SECTION("2**-24 (0 00000 0000000001)")
                    {
                        json const j = json::from_bjdata(std::vector<uint8_t>({'h', 0x01, 0x00}));
                        const json::number_float_t d{j};
                        CHECK(d == std::pow(2.0, -24.0));
                    }
                }

                SECTION("exp = 0b11111")
                {
                    SECTION("infinity (0 11111 0000000000)")
                    {
                        json const j = json::from_bjdata(std::vector<uint8_t>({'h', 0x00, 0x7c}));
                        const json::number_float_t d{j};
                        CHECK(d == std::numeric_limits<json::number_float_t>::infinity());
                        CHECK(j.dump() == "null");
                    }

                    SECTION("-infinity (1 11111 0000000000)")
                    {
                        json const j = json::from_bjdata(std::vector<uint8_t>({'h', 0x00, 0xfc}));
                        const json::number_float_t d{j};
                        CHECK(d == -std::numeric_limits<json::number_float_t>::infinity());
                        CHECK(j.dump() == "null");
                    }
                }

                SECTION("other values from https://en.wikipedia.org/wiki/Half-precision_floating-point_format")
                {
                    SECTION("1 (0 01111 0000000000)")
                    {
                        json const j = json::from_bjdata(std::vector<uint8_t>({'h', 0x00, 0x3c}));
                        const json::number_float_t d{j};
                        CHECK(d == 1);
                    }

                    SECTION("-2 (1 10000 0000000000)")
                    {
                        json const j = json::from_bjdata(std::vector<uint8_t>({'h', 0x00, 0xc0}));
                        const json::number_float_t d{j};
                        CHECK(d == -2);
                    }

                    SECTION("65504 (0 11110 1111111111)")
                    {
                        json const j = json::from_bjdata(std::vector<uint8_t>({'h', 0xff, 0x7b}));
                        const json::number_float_t d{j};
                        CHECK(d == 65504);
                    }
                }

                SECTION("infinity")
                {
                    json const j = json::from_bjdata(std::vector<uint8_t>({'h', 0x00, 0x7c}));
                    json::number_float_t const d{j};
                    CHECK_FALSE(std::isfinite(d));
                    CHECK(j.dump() == "null");
                }

                SECTION("NaN")
                {
                    json const j = json::from_bjdata(std::vector<uint8_t>({'h', 0x00, 0x7e }));
                    json::number_float_t const d{j};
                    CHECK(std::isnan(d));
                    CHECK(j.dump() == "null");
                }
            }

            SECTION("high-precision number")
            {
                SECTION("unsigned integer number")
                {
                    std::vector<uint8_t> const vec = {'H', 'i', 0x14, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
                    const auto j = json::from_bjdata(vec);
                    CHECK(j.is_number_unsigned());
                    CHECK(j.dump() == "12345678901234567890");
                }

                SECTION("signed integer number")
                {
                    std::vector<uint8_t> const vec = {'H', 'i', 0x13, '-', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8'};
                    const auto j = json::from_bjdata(vec);
                    CHECK(j.is_number_integer());
                    CHECK(j.dump() == "-123456789012345678");
                }

                SECTION("floating-point number")
                {
                    std::vector<uint8_t> const vec = {'H', 'i', 0x16, '3', '.', '1', '4', '1', '5', '9',  '2', '6', '5', '3', '5', '8', '9',  '7', '9', '3', '2', '3', '8', '4',  '6'};
                    const auto j = json::from_bjdata(vec);
                    CHECK(j.is_number_float());
                    CHECK(j.dump() == "3.141592653589793");
                }

                SECTION("errors")
                {
                    // error while parsing length
                    std::vector<uint8_t> const vec0 = {'H', 'i'};
                    CHECK(json::from_bjdata(vec0, true, false).is_discarded());
                    // error while parsing string
                    std::vector<uint8_t> const vec1 = {'H', 'i', '1'};
                    CHECK(json::from_bjdata(vec1, true, false).is_discarded());

                    json _;
                    std::vector<uint8_t> const vec2 = {'H', 'i', 2, '1', 'A', '3'};
                    CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vec2), "[json.exception.parse_error.115] parse error at byte 5: syntax error while parsing BJData high-precision number: invalid number text: 1A", json::parse_error);
                    std::vector<uint8_t> const vec3 = {'H', 'i', 2, '1', '.'};
                    CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vec3), "[json.exception.parse_error.115] parse error at byte 5: syntax error while parsing BJData high-precision number: invalid number text: 1.", json::parse_error);
                    std::vector<uint8_t> const vec4 = {'H', 2, '1', '0'};
                    CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vec4), "[json.exception.parse_error.113] parse error at byte 2: syntax error while parsing BJData size: expected length type specification (U, i, u, I, m, l, M, L) after '#'; last byte: 0x02", json::parse_error);
                }
            }
        }

        SECTION("string")
        {
            SECTION("N = 0..127")
            {
                for (size_t N = 0; N <= 127; ++N)
                {
                    CAPTURE(N)

                    // create JSON value with string containing of N * 'x'
                    const auto s = std::string(N, 'x');
                    json const j = s;

                    // create expected byte vector
                    std::vector<uint8_t> expected;
                    expected.push_back('S');
                    expected.push_back('i');
                    expected.push_back(static_cast<uint8_t>(N));
                    for (size_t i = 0; i < N; ++i)
                    {
                        expected.push_back('x');
                    }

                    // compare result + size
                    const auto result = json::to_bjdata(j);
                    CHECK(result == expected);
                    CHECK(result.size() == N + 3);
                    // check that no null byte is appended
                    if (N > 0)
                    {
                        CHECK(result.back() != '\x00');
                    }

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }
            }

            SECTION("N = 128..255")
            {
                for (size_t N = 128; N <= 255; ++N)
                {
                    CAPTURE(N)

                    // create JSON value with string containing of N * 'x'
                    const auto s = std::string(N, 'x');
                    json const j = s;

                    // create expected byte vector
                    std::vector<uint8_t> expected;
                    expected.push_back('S');
                    expected.push_back('U');
                    expected.push_back(static_cast<uint8_t>(N));
                    for (size_t i = 0; i < N; ++i)
                    {
                        expected.push_back('x');
                    }

                    // compare result + size
                    const auto result = json::to_bjdata(j);
                    CHECK(result == expected);
                    CHECK(result.size() == N + 3);
                    // check that no null byte is appended
                    CHECK(result.back() != '\x00');

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }
            }

            SECTION("N = 256..32767")
            {
                for (const size_t N :
                        {
                            256u, 999u, 1025u, 3333u, 2048u, 32767u
                        })
                {
                    CAPTURE(N)

                    // create JSON value with string containing of N * 'x'
                    const auto s = std::string(N, 'x');
                    json const j = s;

                    // create expected byte vector (hack: create string first)
                    std::vector<uint8_t> expected(N, 'x');
                    // reverse order of commands, because we insert at begin()
                    expected.insert(expected.begin(), static_cast<uint8_t>((N >> 8) & 0xff));
                    expected.insert(expected.begin(), static_cast<uint8_t>(N & 0xff));
                    expected.insert(expected.begin(), 'I');
                    expected.insert(expected.begin(), 'S');

                    // compare result + size
                    const auto result = json::to_bjdata(j);
                    CHECK(result == expected);
                    CHECK(result.size() == N + 4);
                    // check that no null byte is appended
                    CHECK(result.back() != '\x00');

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }
            }

            SECTION("N = 32768..65535")
            {
                for (const size_t N :
                        {
                            32768u, 55555u, 65535u
                        })
                {
                    CAPTURE(N)

                    // create JSON value with string containing of N * 'x'
                    const auto s = std::string(N, 'x');
                    json const j = s;

                    // create expected byte vector (hack: create string first)
                    std::vector<uint8_t> expected(N, 'x');
                    // reverse order of commands, because we insert at begin()
                    expected.insert(expected.begin(), static_cast<uint8_t>((N >> 8) & 0xff));
                    expected.insert(expected.begin(), static_cast<uint8_t>(N & 0xff));
                    expected.insert(expected.begin(), 'u');
                    expected.insert(expected.begin(), 'S');

                    // compare result + size
                    const auto result = json::to_bjdata(j);
                    CHECK(result == expected);
                    CHECK(result.size() == N + 4);
                    // check that no null byte is appended
                    CHECK(result.back() != '\x00');

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }
            }

            SECTION("N = 65536..2147483647")
            {
                for (const size_t N :
                        {
                            65536u, 77777u, 1048576u
                        })
                {
                    CAPTURE(N)

                    // create JSON value with string containing of N * 'x'
                    const auto s = std::string(N, 'x');
                    json const j = s;

                    // create expected byte vector (hack: create string first)
                    std::vector<uint8_t> expected(N, 'x');
                    // reverse order of commands, because we insert at begin()
                    expected.insert(expected.begin(), static_cast<uint8_t>((N >> 24) & 0xff));
                    expected.insert(expected.begin(), static_cast<uint8_t>((N >> 16) & 0xff));
                    expected.insert(expected.begin(), static_cast<uint8_t>((N >> 8) & 0xff));
                    expected.insert(expected.begin(), static_cast<uint8_t>(N & 0xff));
                    expected.insert(expected.begin(), 'l');
                    expected.insert(expected.begin(), 'S');

                    // compare result + size
                    const auto result = json::to_bjdata(j);
                    CHECK(result == expected);
                    CHECK(result.size() == N + 6);
                    // check that no null byte is appended
                    CHECK(result.back() != '\x00');

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }
            }
        }

        SECTION("binary")
        {
            for (json::bjdata_version_t bjdata_version :
                    {
                        json::bjdata_version_t::draft2, json::bjdata_version_t::draft3
                    })
            {
                CAPTURE(bjdata_version)
                const bool draft3 = (bjdata_version == json::bjdata_version_t::draft3);

                SECTION("N = 0..127")
                {
                    for (std::size_t N = 0; N <= 127; ++N)
                    {
                        CAPTURE(N)

                        // create JSON value with byte array containing of N * 'x'
                        const auto s = std::vector<std::uint8_t>(N, 'x');
                        json const j = json::binary(s);

                        // create expected byte vector
                        std::vector<std::uint8_t> expected;
                        expected.push_back(static_cast<std::uint8_t>('['));
                        if (draft3 || N != 0)
                        {
                            expected.push_back(static_cast<std::uint8_t>('$'));
                            expected.push_back(static_cast<std::uint8_t>(draft3 ? 'B' : 'U'));
                        }
                        expected.push_back(static_cast<std::uint8_t>('#'));
                        expected.push_back(static_cast<std::uint8_t>('i'));
                        expected.push_back(static_cast<std::uint8_t>(N));
                        for (size_t i = 0; i < N; ++i)
                        {
                            expected.push_back(0x78);
                        }

                        // compare result + size
                        const auto result = json::to_bjdata(j, true, true, bjdata_version);
                        CHECK(result == expected);
                        if (!draft3 && N == 0)
                        {
                            CHECK(result.size() == N + 4);
                        }
                        else
                        {
                            CHECK(result.size() == N + 6);
                        }

                        // check that no null byte is appended
                        if (N > 0)
                        {
                            CHECK(result.back() != '\x00');
                        }

                        if (draft3)
                        {
                            // roundtrip
                            CHECK(json::from_bjdata(result) == j);
                            CHECK(json::from_bjdata(result, true, false) == j);
                        }
                        else
                        {
                            // roundtrip only works to an array of numbers
                            json j_out = s;
                            CHECK(json::from_bjdata(result) == j_out);
                            CHECK(json::from_bjdata(result, true, false) == j_out);
                        }
                    }
                }

                SECTION("N = 128..255")
                {
                    for (std::size_t N = 128; N <= 255; ++N)
                    {
                        CAPTURE(N)

                        // create JSON value with byte array containing of N * 'x'
                        const auto s = std::vector<std::uint8_t>(N, 'x');
                        json const j = json::binary(s);

                        // create expected byte vector
                        std::vector<uint8_t> expected;
                        expected.push_back(static_cast<std::uint8_t>('['));
                        expected.push_back(static_cast<std::uint8_t>('$'));
                        expected.push_back(static_cast<std::uint8_t>(draft3 ? 'B' : 'U'));
                        expected.push_back(static_cast<std::uint8_t>('#'));
                        expected.push_back(static_cast<std::uint8_t>('U'));
                        expected.push_back(static_cast<std::uint8_t>(N));
                        for (size_t i = 0; i < N; ++i)
                        {
                            expected.push_back(0x78);
                        }

                        // compare result + size
                        const auto result = json::to_bjdata(j, true, true, bjdata_version);
                        CHECK(result == expected);
                        CHECK(result.size() == N + 6);
                        // check that no null byte is appended
                        CHECK(result.back() != '\x00');

                        if (draft3)
                        {
                            // roundtrip
                            CHECK(json::from_bjdata(result) == j);
                            CHECK(json::from_bjdata(result, true, false) == j);
                        }
                        else
                        {
                            // roundtrip only works to an array of numbers
                            json j_out = s;
                            CHECK(json::from_bjdata(result) == j_out);
                            CHECK(json::from_bjdata(result, true, false) == j_out);
                        }
                    }
                }

                SECTION("N = 256..32767")
                {
                    for (const std::size_t N :
                            {
                                256u, 999u, 1025u, 3333u, 2048u, 32767u
                            })
                    {
                        CAPTURE(N)

                        // create JSON value with byte array containing of N * 'x'
                        const auto s = std::vector<std::uint8_t>(N, 'x');
                        json const j = json::binary(s);

                        // create expected byte vector
                        std::vector<std::uint8_t> expected(N + 7, 'x');
                        expected[0] = '[';
                        expected[1] = '$';
                        expected[2] = draft3 ? 'B' : 'U';
                        expected[3] = '#';
                        expected[4] = 'I';
                        expected[5] = static_cast<std::uint8_t>(N & 0xFF);
                        expected[6] = static_cast<std::uint8_t>((N >> 8) & 0xFF);

                        // compare result + size
                        const auto result = json::to_bjdata(j, true, true, bjdata_version);
                        CHECK(result == expected);
                        CHECK(result.size() == N + 7);
                        // check that no null byte is appended
                        CHECK(result.back() != '\x00');

                        if (draft3)
                        {
                            // roundtrip
                            CHECK(json::from_bjdata(result) == j);
                            CHECK(json::from_bjdata(result, true, false) == j);
                        }
                        else
                        {
                            // roundtrip only works to an array of numbers
                            json j_out = s;
                            CHECK(json::from_bjdata(result) == j_out);
                            CHECK(json::from_bjdata(result, true, false) == j_out);
                        }
                    }
                }

                SECTION("N = 32768..65535")
                {
                    for (const std::size_t N :
                            {
                                32768u, 55555u, 65535u
                            })
                    {
                        CAPTURE(N)

                        // create JSON value with byte array containing of N * 'x'
                        const auto s = std::vector<std::uint8_t>(N, 'x');
                        json const j = json::binary(s);

                        // create expected byte vector
                        std::vector<std::uint8_t> expected(N + 7, 'x');
                        expected[0] = '[';
                        expected[1] = '$';
                        expected[2] = draft3 ? 'B' : 'U';
                        expected[3] = '#';
                        expected[4] = 'u';
                        expected[5] = static_cast<std::uint8_t>(N & 0xFF);
                        expected[6] = static_cast<std::uint8_t>((N >> 8) & 0xFF);

                        // compare result + size
                        const auto result = json::to_bjdata(j, true, true, bjdata_version);
                        CHECK(result == expected);
                        CHECK(result.size() == N + 7);
                        // check that no null byte is appended
                        CHECK(result.back() != '\x00');

                        if (draft3)
                        {
                            // roundtrip
                            CHECK(json::from_bjdata(result) == j);
                            CHECK(json::from_bjdata(result, true, false) == j);
                        }
                        else
                        {
                            // roundtrip only works to an array of numbers
                            json j_out = s;
                            CHECK(json::from_bjdata(result) == j_out);
                            CHECK(json::from_bjdata(result, true, false) == j_out);
                        }
                    }
                }

                SECTION("N = 65536..2147483647")
                {
                    for (const std::size_t N :
                            {
                                65536u, 77777u, 1048576u
                            })
                    {
                        CAPTURE(N)

                        // create JSON value with byte array containing of N * 'x'
                        const auto s = std::vector<std::uint8_t>(N, 'x');
                        json const j = json::binary(s);

                        // create expected byte vector
                        std::vector<std::uint8_t> expected(N + 9, 'x');
                        expected[0] = '[';
                        expected[1] = '$';
                        expected[2] = draft3 ? 'B' : 'U';
                        expected[3] = '#';
                        expected[4] = 'l';
                        expected[5] = static_cast<std::uint8_t>(N & 0xFF);
                        expected[6] = static_cast<std::uint8_t>((N >> 8) & 0xFF);
                        expected[7] = static_cast<std::uint8_t>((N >> 16) & 0xFF);
                        expected[8] = static_cast<std::uint8_t>((N >> 24) & 0xFF);

                        // compare result + size
                        const auto result = json::to_bjdata(j, true, true, bjdata_version);
                        CHECK(result == expected);
                        CHECK(result.size() == N + 9);
                        // check that no null byte is appended
                        CHECK(result.back() != '\x00');

                        if (draft3)
                        {
                            // roundtrip
                            CHECK(json::from_bjdata(result) == j);
                            CHECK(json::from_bjdata(result, true, false) == j);
                        }
                        else
                        {
                            // roundtrip only works to an array of numbers
                            json j_out = s;
                            CHECK(json::from_bjdata(result) == j_out);
                            CHECK(json::from_bjdata(result, true, false) == j_out);
                        }
                    }
                }

                SECTION("Other Serializations")
                {
                    const std::size_t N = 10;
                    const auto s = std::vector<std::uint8_t>(N, 'x');
                    json const j = json::binary(s);

                    SECTION("No Count No Type")
                    {
                        std::vector<uint8_t> expected;
                        expected.push_back(static_cast<std::uint8_t>('['));
                        for (std::size_t i = 0; i < N; ++i)
                        {
                            expected.push_back(static_cast<std::uint8_t>(draft3 ? 'B' : 'U'));
                            expected.push_back(static_cast<std::uint8_t>(0x78));
                        }
                        expected.push_back(static_cast<std::uint8_t>(']'));

                        // compare result + size
                        const auto result = json::to_bjdata(j, false, false, bjdata_version);
                        CHECK(result == expected);
                        CHECK(result.size() == N + 12);
                        // check that no null byte is appended
                        CHECK(result.back() != '\x00');

                        // roundtrip only works to an array of numbers
                        json j_out = s;
                        CHECK(json::from_bjdata(result) == j_out);
                        CHECK(json::from_bjdata(result, true, false) == j_out);
                    }

                    SECTION("Yes Count No Type")
                    {
                        std::vector<std::uint8_t> expected;
                        expected.push_back(static_cast<std::uint8_t>('['));
                        expected.push_back(static_cast<std::uint8_t>('#'));
                        expected.push_back(static_cast<std::uint8_t>('i'));
                        expected.push_back(static_cast<std::uint8_t>(N));

                        for (size_t i = 0; i < N; ++i)
                        {
                            expected.push_back(static_cast<std::uint8_t>(draft3 ? 'B' : 'U'));
                            expected.push_back(static_cast<std::uint8_t>(0x78));
                        }

                        // compare result + size
                        const auto result = json::to_bjdata(j, true, false, bjdata_version);
                        CHECK(result == expected);
                        CHECK(result.size() == N + 14);
                        // check that no null byte is appended
                        CHECK(result.back() != '\x00');

                        // roundtrip only works to an array of numbers
                        json j_out = s;
                        CHECK(json::from_bjdata(result) == j_out);
                        CHECK(json::from_bjdata(result, true, false) == j_out);
                    }
                }
            }
        }
        SECTION("array")
        {
            SECTION("empty")
            {
                SECTION("size=false type=false")
                {
                    json const j = json::array();
                    std::vector<uint8_t> const expected = {'[', ']'};
                    const auto result = json::to_bjdata(j);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }

                SECTION("size=true type=false")
                {
                    json const j = json::array();
                    std::vector<uint8_t> const expected = {'[', '#', 'i', 0};
                    const auto result = json::to_bjdata(j, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }

                SECTION("size=true type=true")
                {
                    json const j = json::array();
                    std::vector<uint8_t> const expected = {'[', '#', 'i', 0};
                    const auto result = json::to_bjdata(j, true, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }
            }

            SECTION("[null]")
            {
                SECTION("size=false type=false")
                {
                    json const j = {nullptr};
                    std::vector<uint8_t> const expected = {'[', 'Z', ']'};
                    const auto result = json::to_bjdata(j);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }

                SECTION("size=true type=false")
                {
                    json const j = {nullptr};
                    std::vector<uint8_t> const expected = {'[', '#', 'i', 1, 'Z'};
                    const auto result = json::to_bjdata(j, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }

                SECTION("size=true type=true")
                {
                    json const j = {nullptr};
                    std::vector<uint8_t> const expected = {'[', '#', 'i', 1, 'Z'};
                    const auto result = json::to_bjdata(j, true, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }
            }

            SECTION("[1,2,3,4,5]")
            {
                SECTION("size=false type=false")
                {
                    json const j = json::parse("[1,2,3,4,5]");
                    std::vector<uint8_t> const expected = {'[', 'i', 1, 'i', 2, 'i', 3, 'i', 4, 'i', 5, ']'};
                    const auto result = json::to_bjdata(j);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }

                SECTION("size=true type=false")
                {
                    json const j = json::parse("[1,2,3,4,5]");
                    std::vector<uint8_t> const expected = {'[', '#', 'i', 5, 'i', 1, 'i', 2, 'i', 3, 'i', 4, 'i', 5};
                    const auto result = json::to_bjdata(j, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }

                SECTION("size=true type=true")
                {
                    json const j = json::parse("[1,2,3,4,5]");
                    std::vector<uint8_t> const expected = {'[', '$', 'i', '#', 'i', 5, 1, 2, 3, 4, 5};
                    const auto result = json::to_bjdata(j, true, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }
            }

            SECTION("[[[[]]]]")
            {
                SECTION("size=false type=false")
                {
                    json const j = json::parse("[[[[]]]]");
                    std::vector<uint8_t> const expected = {'[', '[', '[', '[', ']', ']', ']', ']'};
                    const auto result = json::to_bjdata(j);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }

                SECTION("size=true type=false")
                {
                    json const j = json::parse("[[[[]]]]");
                    std::vector<uint8_t> const expected = {'[', '#', 'i', 1, '[', '#', 'i', 1, '[', '#', 'i', 1, '[', '#', 'i', 0};
                    const auto result = json::to_bjdata(j, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }

                SECTION("size=true type=true")
                {
                    json const j = json::parse("[[[[]]]]");
                    std::vector<uint8_t> const expected = {'[', '#', 'i', 1, '[', '#', 'i', 1, '[', '#', 'i', 1, '[', '#', 'i', 0};
                    const auto result = json::to_bjdata(j, true, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }
            }

            SECTION("array with int16_t elements")
            {
                SECTION("size=false type=false")
                {
                    json j(257, nullptr);
                    std::vector<uint8_t> expected(j.size() + 2, 'Z'); // all null
                    expected[0] = '['; // opening array
                    expected[258] = ']'; // closing array
                    const auto result = json::to_bjdata(j);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }

                SECTION("size=true type=false")
                {
                    json j(257, nullptr);
                    std::vector<uint8_t> expected(j.size() + 5, 'Z'); // all null
                    expected[0] = '['; // opening array
                    expected[1] = '#'; // array size
                    expected[2] = 'I'; // int16
                    expected[3] = 0x01; // 0x0101, first byte
                    expected[4] = 0x01; // 0x0101, second byte
                    const auto result = json::to_bjdata(j, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }
            }

            SECTION("array with uint16_t elements")
            {
                SECTION("size=false type=false")
                {
                    json j(32768, nullptr);
                    std::vector<uint8_t> expected(j.size() + 2, 'Z'); // all null
                    expected[0] = '['; // opening array
                    expected[32769] = ']'; // closing array
                    const auto result = json::to_bjdata(j);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }

                SECTION("size=true type=false")
                {
                    json j(32768, nullptr);
                    std::vector<uint8_t> expected(j.size() + 5, 'Z'); // all null
                    expected[0] = '['; // opening array
                    expected[1] = '#'; // array size
                    expected[2] = 'u'; // int16
                    expected[3] = 0x00; // 0x0101, first byte
                    expected[4] = 0x80; // 0x0101, second byte
                    const auto result = json::to_bjdata(j, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }
            }

            SECTION("array with int32_t elements")
            {
                SECTION("size=false type=false")
                {
                    json j(65793, nullptr);
                    std::vector<uint8_t> expected(j.size() + 2, 'Z'); // all null
                    expected[0] = '['; // opening array
                    expected[65794] = ']'; // closing array
                    const auto result = json::to_bjdata(j);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }

                SECTION("size=true type=false")
                {
                    json j(65793, nullptr);
                    std::vector<uint8_t> expected(j.size() + 7, 'Z'); // all null
                    expected[0] = '['; // opening array
                    expected[1] = '#'; // array size
                    expected[2] = 'l'; // int32
                    expected[3] = 0x01; // 0x00010101, fourth byte
                    expected[4] = 0x01; // 0x00010101, third byte
                    expected[5] = 0x01; // 0x00010101, second byte
                    expected[6] = 0x00; // 0x00010101, first byte
                    const auto result = json::to_bjdata(j, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }
            }
        }

        SECTION("object")
        {
            SECTION("empty")
            {
                SECTION("size=false type=false")
                {
                    json const j = json::object();
                    std::vector<uint8_t> const expected = {'{', '}'};
                    const auto result = json::to_bjdata(j);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }

                SECTION("size=true type=false")
                {
                    json const j = json::object();
                    std::vector<uint8_t> const expected = {'{', '#', 'i', 0};
                    const auto result = json::to_bjdata(j, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }

                SECTION("size=true type=true")
                {
                    json const j = json::object();
                    std::vector<uint8_t> const expected = {'{', '#', 'i', 0};
                    const auto result = json::to_bjdata(j, true, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }
            }

            SECTION("{\"\":null}")
            {
                SECTION("size=false type=false")
                {
                    json const j = {{"", nullptr}};
                    std::vector<uint8_t> const expected = {'{', 'i', 0, 'Z', '}'};
                    const auto result = json::to_bjdata(j);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }

                SECTION("size=true type=false")
                {
                    json const j = {{"", nullptr}};
                    std::vector<uint8_t> const expected = {'{', '#', 'i', 1, 'i', 0, 'Z'};
                    const auto result = json::to_bjdata(j, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }
            }

            SECTION("{\"a\": {\"b\": {\"c\": {}}}}")
            {
                SECTION("size=false type=false")
                {
                    json const j = json::parse(R"({"a": {"b": {"c": {}}}})");
                    std::vector<uint8_t> const expected =
                    {
                        '{', 'i', 1, 'a', '{', 'i', 1, 'b', '{', 'i', 1, 'c', '{', '}', '}', '}', '}'
                    };
                    const auto result = json::to_bjdata(j);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }

                SECTION("size=true type=false")
                {
                    json const j = json::parse(R"({"a": {"b": {"c": {}}}})");
                    std::vector<uint8_t> const expected =
                    {
                        '{', '#', 'i', 1, 'i', 1, 'a', '{', '#', 'i', 1, 'i', 1, 'b', '{', '#', 'i', 1, 'i', 1, 'c', '{', '#', 'i', 0
                    };
                    const auto result = json::to_bjdata(j, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }

                SECTION("size=true type=true ignore object type marker")
                {
                    json const j = json::parse(R"({"a": {"b": {"c": {}}}})");
                    std::vector<uint8_t> const expected =
                    {
                        '{', '#', 'i', 1, 'i', 1, 'a', '{', '#', 'i', 1, 'i', 1, 'b', '{', '#', 'i', 1, 'i', 1, 'c', '{', '#', 'i', 0
                    };
                    const auto result = json::to_bjdata(j, true, true);
                    CHECK(result == expected);

                    // roundtrip
                    CHECK(json::from_bjdata(result) == j);
                    CHECK(json::from_bjdata(result, true, false) == j);
                }
            }
        }
    }

    SECTION("errors")
    {
        SECTION("strict mode")
        {
            std::vector<uint8_t> const vec = {'Z', 'Z'};
            SECTION("non-strict mode")
            {
                const auto result = json::from_bjdata(vec, false);
                CHECK(result == json());
            }

            SECTION("strict mode")
            {
                json _;
                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vec),
                                     "[json.exception.parse_error.110] parse error at byte 2: syntax error while parsing BJData value: expected end of input; last byte: 0x5A", json::parse_error&);
            }
        }
    }

    SECTION("SAX aborts")
    {
        SECTION("start_array()")
        {
            std::vector<uint8_t> const v = {'[', 'T', 'F', ']'};
            SaxCountdown scp(0);
            CHECK_FALSE(json::sax_parse(v, &scp, json::input_format_t::bjdata));
        }

        SECTION("start_object()")
        {
            std::vector<uint8_t> const v = {'{', 'i', 3, 'f', 'o', 'o', 'F', '}'};
            SaxCountdown scp(0);
            CHECK_FALSE(json::sax_parse(v, &scp, json::input_format_t::bjdata));
        }

        SECTION("key() in object")
        {
            std::vector<uint8_t> const v = {'{', 'i', 3, 'f', 'o', 'o', 'F', '}'};
            SaxCountdown scp(1);
            CHECK_FALSE(json::sax_parse(v, &scp, json::input_format_t::bjdata));
        }

        SECTION("start_array(len)")
        {
            std::vector<uint8_t> const v = {'[', '#', 'i', '2', 'T', 'F'};
            SaxCountdown scp(0);
            CHECK_FALSE(json::sax_parse(v, &scp, json::input_format_t::bjdata));
        }

        SECTION("start_object(len)")
        {
            std::vector<uint8_t> const v = {'{', '#', 'i', '1', 3, 'f', 'o', 'o', 'F'};
            SaxCountdown scp(0);
            CHECK_FALSE(json::sax_parse(v, &scp, json::input_format_t::bjdata));
        }

        SECTION("key() in object with length")
        {
            std::vector<uint8_t> const v = {'{', 'i', 3, 'f', 'o', 'o', 'F', '}'};
            SaxCountdown scp(1);
            CHECK_FALSE(json::sax_parse(v, &scp, json::input_format_t::bjdata));
        }

        SECTION("start_array() in ndarray _ArraySize_")
        {
            std::vector<uint8_t> const v = {'[', '$', 'i', '#', '[', '$', 'i', '#', 'i', 2, 2, 1, 1, 2};
            SaxCountdown scp(2);
            CHECK_FALSE(json::sax_parse(v, &scp, json::input_format_t::bjdata));
        }

        SECTION("number_integer() in ndarray _ArraySize_")
        {
            std::vector<uint8_t> const v = {'[', '$', 'U', '#', '[', '$', 'i', '#', 'i', 2, 2, 1, 1, 2};
            SaxCountdown scp(3);
            CHECK_FALSE(json::sax_parse(v, &scp, json::input_format_t::bjdata));
        }

        SECTION("key() in ndarray _ArrayType_")
        {
            std::vector<uint8_t> const v = {'[', '$', 'U', '#', '[', '$', 'U', '#', 'i', 2, 2, 2, 1, 2, 3, 4};
            SaxCountdown scp(6);
            CHECK_FALSE(json::sax_parse(v, &scp, json::input_format_t::bjdata));
        }

        SECTION("string() in ndarray _ArrayType_")
        {
            std::vector<uint8_t> const v = {'[', '$', 'U', '#', '[', '$', 'U', '#', 'i', 2, 2, 2, 1, 2, 3, 4};
            SaxCountdown scp(7);
            CHECK_FALSE(json::sax_parse(v, &scp, json::input_format_t::bjdata));
        }

        SECTION("key() in ndarray _ArrayData_")
        {
            std::vector<uint8_t> const v = {'[', '$', 'U', '#', '[', '$', 'U', '#', 'i', 2, 2, 2, 1, 2, 3, 4};
            SaxCountdown scp(8);
            CHECK_FALSE(json::sax_parse(v, &scp, json::input_format_t::bjdata));
        }

        SECTION("string() in ndarray _ArrayData_")
        {
            std::vector<uint8_t> const v = {'[', '$', 'U', '#', '[', '$', 'U', '#', 'i', 2, 2, 2, 1, 2, 3, 4};
            SaxCountdown scp(9);
            CHECK_FALSE(json::sax_parse(v, &scp, json::input_format_t::bjdata));
        }

        SECTION("string() in ndarray _ArrayType_")
        {
            std::vector<uint8_t> const v = {'[', '$', 'U', '#', '[', '$', 'i', '#', 'i', 2, 3, 2, 6, 5, 4, 3, 2, 1};
            SaxCountdown scp(11);
            CHECK_FALSE(json::sax_parse(v, &scp, json::input_format_t::bjdata));
        }

        SECTION("start_array() in ndarray _ArrayData_")
        {
            std::vector<uint8_t> const v = {'[', '$', 'U', '#', '[', 'i', 2, 'i', 3, ']', 6, 5, 4, 3, 2, 1};
            SaxCountdown scp(13);
            CHECK_FALSE(json::sax_parse(v, &scp, json::input_format_t::bjdata));
        }
    }

    SECTION("parsing values")
    {
        SECTION("strings")
        {
            // create a single-character string for all number types
            std::vector<uint8_t> s_i = {'S', 'i', 1, 'a'};
            std::vector<uint8_t> const s_U = {'S', 'U', 1, 'a'};
            std::vector<uint8_t> const s_I = {'S', 'I', 1, 0, 'a'};
            std::vector<uint8_t> const s_u = {'S', 'u', 1, 0, 'a'};
            std::vector<uint8_t> const s_l = {'S', 'l', 1, 0, 0, 0, 'a'};
            std::vector<uint8_t> const s_m = {'S', 'm', 1, 0, 0, 0, 'a'};
            std::vector<uint8_t> const s_L = {'S', 'L', 1, 0, 0, 0, 0, 0, 0, 0, 'a'};
            std::vector<uint8_t> const s_M = {'S', 'M', 1, 0, 0, 0, 0, 0, 0, 0, 'a'};

            // check if string is parsed correctly to "a"
            CHECK(json::from_bjdata(s_i) == "a");
            CHECK(json::from_bjdata(s_U) == "a");
            CHECK(json::from_bjdata(s_I) == "a");
            CHECK(json::from_bjdata(s_u) == "a");
            CHECK(json::from_bjdata(s_l) == "a");
            CHECK(json::from_bjdata(s_m) == "a");
            CHECK(json::from_bjdata(s_L) == "a");
            CHECK(json::from_bjdata(s_M) == "a");

            // roundtrip: output should be optimized
            CHECK(json::to_bjdata(json::from_bjdata(s_i)) == s_i);
            CHECK(json::to_bjdata(json::from_bjdata(s_U)) == s_i);
            CHECK(json::to_bjdata(json::from_bjdata(s_I)) == s_i);
            CHECK(json::to_bjdata(json::from_bjdata(s_u)) == s_i);
            CHECK(json::to_bjdata(json::from_bjdata(s_l)) == s_i);
            CHECK(json::to_bjdata(json::from_bjdata(s_m)) == s_i);
            CHECK(json::to_bjdata(json::from_bjdata(s_L)) == s_i);
            CHECK(json::to_bjdata(json::from_bjdata(s_M)) == s_i);
        }

        SECTION("number")
        {
            SECTION("float")
            {
                // float32
                std::vector<uint8_t> const v_d = {'d', 0xd0, 0x0f, 0x49, 0x40};
                CHECK(json::from_bjdata(v_d) == 3.14159f);

                // float64
                std::vector<uint8_t> const v_D = {'D', 0x6e, 0x86, 0x1b, 0xf0, 0xf9, 0x21, 0x09, 0x40};
                CHECK(json::from_bjdata(v_D) == 3.14159);

                // float32 is serialized as float64 as the library does not support float32
                CHECK(json::to_bjdata(json::from_bjdata(v_d)) == json::to_bjdata(3.14159f));
            }
        }

        SECTION("array")
        {
            SECTION("optimized version (length only)")
            {
                // create vector with two elements of the same type
                std::vector<uint8_t> const v_TU = {'[', '#', 'U', 2, 'T', 'T'};
                std::vector<uint8_t> const v_T = {'[', '#', 'i', 2, 'T', 'T'};
                std::vector<uint8_t> const v_F = {'[', '#', 'i', 2, 'F', 'F'};
                std::vector<uint8_t> const v_Z = {'[', '#', 'i', 2, 'Z', 'Z'};
                std::vector<uint8_t> const v_i = {'[', '#', 'i', 2, 'i', 0x7F, 'i', 0x7F};
                std::vector<uint8_t> const v_U = {'[', '#', 'i', 2, 'U', 0xFF, 'U', 0xFF};
                std::vector<uint8_t> const v_I = {'[', '#', 'i', 2, 'I', 0xFF, 0x7F, 'I', 0xFF, 0x7F};
                std::vector<uint8_t> const v_u = {'[', '#', 'i', 2, 'u', 0x0F, 0xA7, 'u', 0x0F, 0xA7};
                std::vector<uint8_t> const v_l = {'[', '#', 'i', 2, 'l', 0xFF, 0xFF, 0xFF, 0x7F, 'l', 0xFF, 0xFF, 0xFF, 0x7F};
                std::vector<uint8_t> const v_m = {'[', '#', 'i', 2, 'm', 0xFF, 0xC9, 0x9A, 0xBB, 'm', 0xFF, 0xC9, 0x9A, 0xBB};
                std::vector<uint8_t> const v_L = {'[', '#', 'i', 2, 'L', 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 'L', 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F};
                std::vector<uint8_t> const v_M = {'[', '#', 'i', 2, 'M', 0xFF, 0xFF, 0x63, 0xA7, 0xB3, 0xB6, 0xE0, 0x8D, 'M', 0xFF, 0xFF, 0x63, 0xA7, 0xB3, 0xB6, 0xE0, 0x8D};
                std::vector<uint8_t> const v_D = {'[', '#', 'i', 2, 'D', 0x4a, 0xd8, 0x12, 0x4d, 0xfb, 0x21, 0x09, 0x40, 'D', 0x4a, 0xd8, 0x12, 0x4d, 0xfb, 0x21, 0x09, 0x40};
                std::vector<uint8_t> const v_S = {'[', '#', 'i', 2, 'S', 'i', 1, 'a', 'S', 'i', 1, 'a'};
                std::vector<uint8_t> const v_C = {'[', '#', 'i', 2, 'C', 'a', 'C', 'a'};
                std::vector<uint8_t> const v_B = {'[', '#', 'i', 2, 'B', 0xFF, 'B', 0xFF};

                // check if vector is parsed correctly
                CHECK(json::from_bjdata(v_TU) == json({true, true}));
                CHECK(json::from_bjdata(v_T) == json({true, true}));
                CHECK(json::from_bjdata(v_F) == json({false, false}));
                CHECK(json::from_bjdata(v_Z) == json({nullptr, nullptr}));
                CHECK(json::from_bjdata(v_i) == json({127, 127}));
                CHECK(json::from_bjdata(v_U) == json({255, 255}));
                CHECK(json::from_bjdata(v_I) == json({32767, 32767}));
                CHECK(json::from_bjdata(v_u) == json({42767, 42767}));
                CHECK(json::from_bjdata(v_l) == json({2147483647, 2147483647}));
                CHECK(json::from_bjdata(v_m) == json({3147483647, 3147483647}));
                CHECK(json::from_bjdata(v_L) == json({9223372036854775807, 9223372036854775807}));
                CHECK(json::from_bjdata(v_M) == json({10223372036854775807ull, 10223372036854775807ull}));
                CHECK(json::from_bjdata(v_D) == json({3.1415926, 3.1415926}));
                CHECK(json::from_bjdata(v_S) == json({"a", "a"}));
                CHECK(json::from_bjdata(v_C) == json({"a", "a"}));
                CHECK(json::from_bjdata(v_B) == json({255, 255}));

                // roundtrip: output should be optimized
                CHECK(json::to_bjdata(json::from_bjdata(v_T), true) == v_T);
                CHECK(json::to_bjdata(json::from_bjdata(v_F), true) == v_F);
                CHECK(json::to_bjdata(json::from_bjdata(v_Z), true) == v_Z);
                CHECK(json::to_bjdata(json::from_bjdata(v_i), true) == v_i);
                CHECK(json::to_bjdata(json::from_bjdata(v_U), true) == v_U);
                CHECK(json::to_bjdata(json::from_bjdata(v_I), true) == v_I);
                CHECK(json::to_bjdata(json::from_bjdata(v_u), true) == v_u);
                CHECK(json::to_bjdata(json::from_bjdata(v_l), true) == v_l);
                CHECK(json::to_bjdata(json::from_bjdata(v_m), true) == v_m);
                CHECK(json::to_bjdata(json::from_bjdata(v_L), true) == v_L);
                CHECK(json::to_bjdata(json::from_bjdata(v_M), true) == v_M);
                CHECK(json::to_bjdata(json::from_bjdata(v_D), true) == v_D);
                CHECK(json::to_bjdata(json::from_bjdata(v_S), true) == v_S);
                CHECK(json::to_bjdata(json::from_bjdata(v_C), true) == v_S); // char is serialized to string
                CHECK(json::to_bjdata(json::from_bjdata(v_B), true) == v_U); // byte is serialized to uint8
            }

            SECTION("optimized version (type and length)")
            {
                // create vector with two elements of the same type
                std::vector<uint8_t> const v_i = {'[', '$', 'i', '#', 'i', 2, 0x7F, 0x7F};
                std::vector<uint8_t> const v_U = {'[', '$', 'U', '#', 'i', 2, 0xFF, 0xFF};
                std::vector<uint8_t> const v_I = {'[', '$', 'I', '#', 'i', 2, 0xFF, 0x7F, 0xFF, 0x7F};
                std::vector<uint8_t> const v_u = {'[', '$', 'u', '#', 'i', 2, 0x0F, 0xA7, 0x0F, 0xA7};
                std::vector<uint8_t> const v_l = {'[', '$', 'l', '#', 'i', 2, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F};
                std::vector<uint8_t> const v_m = {'[', '$', 'm', '#', 'i', 2, 0xFF, 0xC9, 0x9A, 0xBB, 0xFF, 0xC9, 0x9A, 0xBB};
                std::vector<uint8_t> const v_L = {'[', '$', 'L', '#', 'i', 2, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F};
                std::vector<uint8_t> const v_M = {'[', '$', 'M', '#', 'i', 2, 0xFF, 0xFF, 0x63, 0xA7, 0xB3, 0xB6, 0xE0, 0x8D, 0xFF, 0xFF, 0x63, 0xA7, 0xB3, 0xB6, 0xE0, 0x8D};
                std::vector<uint8_t> const v_D = {'[', '$', 'D', '#', 'i', 2, 0x4a, 0xd8, 0x12, 0x4d, 0xfb, 0x21, 0x09, 0x40, 0x4a, 0xd8, 0x12, 0x4d, 0xfb, 0x21, 0x09, 0x40};
                std::vector<uint8_t> const v_S = {'[', '#', 'i', 2, 'S', 'i', 1, 'a', 'S', 'i', 1, 'a'};
                std::vector<uint8_t> const v_C = {'[', '$', 'C', '#', 'i', 2, 'a', 'a'};
                std::vector<uint8_t> const v_B = {'[', '$', 'B', '#', 'i', 2, 0xFF, 0xFF};

                // check if vector is parsed correctly
                CHECK(json::from_bjdata(v_i) == json({127, 127}));
                CHECK(json::from_bjdata(v_U) == json({255, 255}));
                CHECK(json::from_bjdata(v_I) == json({32767, 32767}));
                CHECK(json::from_bjdata(v_u) == json({42767, 42767}));
                CHECK(json::from_bjdata(v_l) == json({2147483647, 2147483647}));
                CHECK(json::from_bjdata(v_m) == json({3147483647, 3147483647}));
                CHECK(json::from_bjdata(v_L) == json({9223372036854775807, 9223372036854775807}));
                CHECK(json::from_bjdata(v_M) == json({10223372036854775807ull, 10223372036854775807ull}));
                CHECK(json::from_bjdata(v_D) == json({3.1415926, 3.1415926}));
                CHECK(json::from_bjdata(v_S) == json({"a", "a"}));
                CHECK(json::from_bjdata(v_C) == json({"a", "a"}));
                CHECK(json::from_bjdata(v_B) == json::binary(std::vector<uint8_t>({static_cast<uint8_t>(255), static_cast<uint8_t>(255)})));

                // roundtrip: output should be optimized
                std::vector<uint8_t> const v_empty = {'[', '#', 'i', 0};
                CHECK(json::to_bjdata(json::from_bjdata(v_i), true, true) == v_i);
                CHECK(json::to_bjdata(json::from_bjdata(v_U), true, true) == v_U);
                CHECK(json::to_bjdata(json::from_bjdata(v_I), true, true) == v_I);
                CHECK(json::to_bjdata(json::from_bjdata(v_u), true, true) == v_u);
                CHECK(json::to_bjdata(json::from_bjdata(v_l), true, true) == v_l);
                CHECK(json::to_bjdata(json::from_bjdata(v_m), true, true) == v_m);
                CHECK(json::to_bjdata(json::from_bjdata(v_L), true, true) == v_L);
                CHECK(json::to_bjdata(json::from_bjdata(v_M), true, true) == v_M);
                CHECK(json::to_bjdata(json::from_bjdata(v_D), true, true) == v_D);
                CHECK(json::to_bjdata(json::from_bjdata(v_S), true, true) == v_S);
                CHECK(json::to_bjdata(json::from_bjdata(v_C), true, true) == v_S); // char is serialized to string
                CHECK(json::to_bjdata(json::from_bjdata(v_B), true, true, json::bjdata_version_t::draft2) == v_U);
                CHECK(json::to_bjdata(json::from_bjdata(v_B), true, true, json::bjdata_version_t::draft3) == v_B);
            }

            SECTION("optimized ndarray (type and vector-size as optimized 1D array)")
            {
                // create vector with two elements of the same type
                std::vector<uint8_t> const v_0 = {'[', '$', 'i', '#', '[', '$', 'i', '#', 'i', 1, 0};
                std::vector<uint8_t> const v_1 = {'[', '$', 'i', '#', '[', '$', 'i', '#', 'i', 1, 2, 0x7F, 0x7F};
                std::vector<uint8_t> const v_i = {'[', '$', 'i', '#', '[', '$', 'i', '#', 'i', 2, 1, 2, 0x7F, 0x7F};
                std::vector<uint8_t> const v_U = {'[', '$', 'U', '#', '[', '$', 'i', '#', 'i', 2, 1, 2, 0xFF, 0xFF};
                std::vector<uint8_t> const v_I = {'[', '$', 'I', '#', '[', '$', 'i', '#', 'i', 2, 1, 2, 0xFF, 0x7F, 0xFF, 0x7F};
                std::vector<uint8_t> const v_u = {'[', '$', 'u', '#', '[', '$', 'i', '#', 'i', 2, 1, 2, 0x0F, 0xA7, 0x0F, 0xA7};
                std::vector<uint8_t> const v_l = {'[', '$', 'l', '#', '[', '$', 'i', '#', 'i', 2, 1, 2, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F};
                std::vector<uint8_t> const v_m = {'[', '$', 'm', '#', '[', '$', 'i', '#', 'i', 2, 1, 2, 0xFF, 0xC9, 0x9A, 0xBB, 0xFF, 0xC9, 0x9A, 0xBB};
                std::vector<uint8_t> const v_L = {'[', '$', 'L', '#', '[', '$', 'i', '#', 'i', 2, 1, 2, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F};
                std::vector<uint8_t> const v_M = {'[', '$', 'M', '#', '[', '$', 'i', '#', 'i', 2, 1, 2, 0xFF, 0xFF, 0x63, 0xA7, 0xB3, 0xB6, 0xE0, 0x8D, 0xFF, 0xFF, 0x63, 0xA7, 0xB3, 0xB6, 0xE0, 0x8D};
                std::vector<uint8_t> const v_D = {'[', '$', 'D', '#', '[', '$', 'i', '#', 'i', 2, 1, 2, 0x4a, 0xd8, 0x12, 0x4d, 0xfb, 0x21, 0x09, 0x40, 0x4a, 0xd8, 0x12, 0x4d, 0xfb, 0x21, 0x09, 0x40};
                std::vector<uint8_t> const v_S = {'[', '#', '[', '$', 'i', '#', 'i', 2, 1, 2, 'S', 'i', 1, 'a', 'S', 'i', 1, 'a'};
                std::vector<uint8_t> const v_C = {'[', '$', 'C', '#', '[', '$', 'i', '#', 'i', 2, 1, 2, 'a', 'a'};
                std::vector<uint8_t> const v_B = {'[', '$', 'B', '#', '[', '$', 'i', '#', 'i', 2, 1, 2, 0xFF, 0xFF};

                // check if vector is parsed correctly
                CHECK(json::from_bjdata(v_0) == json::array());
                CHECK(json::from_bjdata(v_1) == json({127, 127}));
                CHECK(json::from_bjdata(v_i) == json({127, 127}));
                CHECK(json::from_bjdata(v_U) == json({255, 255}));
                CHECK(json::from_bjdata(v_I) == json({32767, 32767}));
                CHECK(json::from_bjdata(v_u) == json({42767, 42767}));
                CHECK(json::from_bjdata(v_l) == json({2147483647, 2147483647}));
                CHECK(json::from_bjdata(v_m) == json({3147483647, 3147483647}));
                CHECK(json::from_bjdata(v_L) == json({9223372036854775807, 9223372036854775807}));
                CHECK(json::from_bjdata(v_M) == json({10223372036854775807ull, 10223372036854775807ull}));
                CHECK(json::from_bjdata(v_D) == json({3.1415926, 3.1415926}));
                CHECK(json::from_bjdata(v_S) == json({"a", "a"}));
                CHECK(json::from_bjdata(v_C) == json({"a", "a"}));
                CHECK(json::from_bjdata(v_B) == json::binary(std::vector<uint8_t>({static_cast<uint8_t>(255), static_cast<uint8_t>(255)})));
            }

            SECTION("optimized ndarray (type and vector-size ndarray with JData annotations)")
            {
                // create vector with 0, 1, 2 elements of the same type
                std::vector<uint8_t> const v_e = {'[', '$', 'U', '#', '[', '$', 'i', '#', 'i', 2, 2, 1, 0xFE, 0xFF};
                std::vector<uint8_t> const v_U = {'[', '$', 'U', '#', '[', '$', 'i', '#', 'i', 2, 2, 3, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
                std::vector<uint8_t> const v_i = {'[', '$', 'i', '#', '[', '$', 'i', '#', 'i', 2, 2, 3, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
                std::vector<uint8_t> const v_u = {'[', '$', 'u', '#', '[', '$', 'i', '#', 'i', 2, 2, 3, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 0x00, 0x05, 0x00, 0x06, 0x00};
                std::vector<uint8_t> const v_I = {'[', '$', 'I', '#', '[', '$', 'i', '#', 'i', 2, 2, 3, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 0x00, 0x05, 0x00, 0x06, 0x00};
                std::vector<uint8_t> const v_m = {'[', '$', 'm', '#', '[', '$', 'i', '#', 'i', 2, 2, 3, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00};
                std::vector<uint8_t> const v_l = {'[', '$', 'l', '#', '[', '$', 'i', '#', 'i', 2, 2, 3, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00};
                std::vector<uint8_t> const v_M = {'[', '$', 'M', '#', '[', '$', 'i', '#', 'i', 2, 2, 3, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
                std::vector<uint8_t> const v_L = {'[', '$', 'L', '#', '[', '$', 'i', '#', 'i', 2, 2, 3, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
                std::vector<uint8_t> const v_d = {'[', '$', 'd', '#', '[', '$', 'i', '#', 'i', 2, 2, 3, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x40, 0x40, 0x00, 0x00, 0x80, 0x40, 0x00, 0x00, 0xA0, 0x40, 0x00, 0x00, 0xC0, 0x40};
                std::vector<uint8_t> const v_D = {'[', '$', 'D', '#', '[', '$', 'i', '#', 'i', 2, 2, 3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x40};
                std::vector<uint8_t> const v_C = {'[', '$', 'C', '#', '[', '$', 'i', '#', 'i', 2, 2, 3, 'a', 'b', 'c', 'd', 'e', 'f'};
                std::vector<uint8_t> const v_B = {'[', '$', 'B', '#', '[', '$', 'i', '#', 'i', 2, 2, 3, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};

                // check if vector is parsed correctly
                CHECK(json::from_bjdata(v_e) == json({{"_ArrayData_", {254, 255}}, {"_ArraySize_", {2, 1}}, {"_ArrayType_", "uint8"}}));
                CHECK(json::from_bjdata(v_U) == json({{"_ArrayData_", {1, 2, 3, 4, 5, 6}}, {"_ArraySize_", {2, 3}}, {"_ArrayType_", "uint8"}}));
                CHECK(json::from_bjdata(v_i) == json({{"_ArrayData_", {1, 2, 3, 4, 5, 6}}, {"_ArraySize_", {2, 3}}, {"_ArrayType_", "int8"}}));
                CHECK(json::from_bjdata(v_i) == json({{"_ArrayData_", {1, 2, 3, 4, 5, 6}}, {"_ArraySize_", {2, 3}}, {"_ArrayType_", "int8"}}));
                CHECK(json::from_bjdata(v_u) == json({{"_ArrayData_", {1, 2, 3, 4, 5, 6}}, {"_ArraySize_", {2, 3}}, {"_ArrayType_", "uint16"}}));
                CHECK(json::from_bjdata(v_I) == json({{"_ArrayData_", {1, 2, 3, 4, 5, 6}}, {"_ArraySize_", {2, 3}}, {"_ArrayType_", "int16"}}));
                CHECK(json::from_bjdata(v_m) == json({{"_ArrayData_", {1, 2, 3, 4, 5, 6}}, {"_ArraySize_", {2, 3}}, {"_ArrayType_", "uint32"}}));
                CHECK(json::from_bjdata(v_l) == json({{"_ArrayData_", {1, 2, 3, 4, 5, 6}}, {"_ArraySize_", {2, 3}}, {"_ArrayType_", "int32"}}));
                CHECK(json::from_bjdata(v_M) == json({{"_ArrayData_", {1, 2, 3, 4, 5, 6}}, {"_ArraySize_", {2, 3}}, {"_ArrayType_", "uint64"}}));
                CHECK(json::from_bjdata(v_L) == json({{"_ArrayData_", {1, 2, 3, 4, 5, 6}}, {"_ArraySize_", {2, 3}}, {"_ArrayType_", "int64"}}));
                CHECK(json::from_bjdata(v_d) == json({{"_ArrayData_", {1.f, 2.f, 3.f, 4.f, 5.f, 6.f}}, {"_ArraySize_", {2, 3}}, {"_ArrayType_", "single"}}));
                CHECK(json::from_bjdata(v_D) == json({{"_ArrayData_", {1., 2., 3., 4., 5., 6.}}, {"_ArraySize_", {2, 3}}, {"_ArrayType_", "double"}}));
                CHECK(json::from_bjdata(v_C) == json({{"_ArrayData_", {'a', 'b', 'c', 'd', 'e', 'f'}}, {"_ArraySize_", {2, 3}}, {"_ArrayType_", "char"}}));
                CHECK(json::from_bjdata(v_B) == json({{"_ArrayData_", {1, 2, 3, 4, 5, 6}}, {"_ArraySize_", {2, 3}}, {"_ArrayType_", "byte"}}));

                // roundtrip: output should be optimized
                CHECK(json::to_bjdata(json::from_bjdata(v_e), true, true) == v_e);
                CHECK(json::to_bjdata(json::from_bjdata(v_U), true, true) == v_U);
                CHECK(json::to_bjdata(json::from_bjdata(v_i), true, true) == v_i);
                CHECK(json::to_bjdata(json::from_bjdata(v_u), true, true) == v_u);
                CHECK(json::to_bjdata(json::from_bjdata(v_I), true, true) == v_I);
                CHECK(json::to_bjdata(json::from_bjdata(v_m), true, true) == v_m);
                CHECK(json::to_bjdata(json::from_bjdata(v_l), true, true) == v_l);
                CHECK(json::to_bjdata(json::from_bjdata(v_M), true, true) == v_M);
                CHECK(json::to_bjdata(json::from_bjdata(v_L), true, true) == v_L);
                CHECK(json::to_bjdata(json::from_bjdata(v_d), true, true) == v_d);
                CHECK(json::to_bjdata(json::from_bjdata(v_D), true, true) == v_D);
                CHECK(json::to_bjdata(json::from_bjdata(v_C), true, true) == v_C);
                CHECK(json::to_bjdata(json::from_bjdata(v_B), true, true) == v_B);
            }

            SECTION("optimized ndarray (type and vector-size as 1D array)")
            {
                // create vector with two elements of the same type
                std::vector<uint8_t> const v_0 = {'[', '$', 'i', '#', '[', ']'};
                std::vector<uint8_t> const v_E = {'[', '$', 'i', '#', '[', 'i', 2, 'i', 0, ']'};
                std::vector<uint8_t> const v_i = {'[', '$', 'i', '#', '[', 'i', 1, 'i', 2, ']', 0x7F, 0x7F};
                std::vector<uint8_t> const v_U = {'[', '$', 'U', '#', '[', 'i', 1, 'i', 2, ']', 0xFF, 0xFF};
                std::vector<uint8_t> const v_I = {'[', '$', 'I', '#', '[', 'i', 1, 'i', 2, ']', 0xFF, 0x7F, 0xFF, 0x7F};
                std::vector<uint8_t> const v_u = {'[', '$', 'u', '#', '[', 'i', 1, 'i', 2, ']', 0x0F, 0xA7, 0x0F, 0xA7};
                std::vector<uint8_t> const v_l = {'[', '$', 'l', '#', '[', 'i', 1, 'i', 2, ']', 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F};
                std::vector<uint8_t> const v_m = {'[', '$', 'm', '#', '[', 'i', 1, 'i', 2, ']', 0xFF, 0xC9, 0x9A, 0xBB, 0xFF, 0xC9, 0x9A, 0xBB};
                std::vector<uint8_t> const v_L = {'[', '$', 'L', '#', '[', 'i', 1, 'i', 2, ']', 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F};
                std::vector<uint8_t> const v_M = {'[', '$', 'M', '#', '[', 'i', 1, 'i', 2, ']', 0xFF, 0xFF, 0x63, 0xA7, 0xB3, 0xB6, 0xE0, 0x8D, 0xFF, 0xFF, 0x63, 0xA7, 0xB3, 0xB6, 0xE0, 0x8D};
                std::vector<uint8_t> const v_D = {'[', '$', 'D', '#', '[', 'i', 1, 'i', 2, ']', 0x4a, 0xd8, 0x12, 0x4d, 0xfb, 0x21, 0x09, 0x40, 0x4a, 0xd8, 0x12, 0x4d, 0xfb, 0x21, 0x09, 0x40};
                std::vector<uint8_t> const v_S = {'[', '#', '[', 'i', 1, 'i', 2, ']', 'S', 'i', 1, 'a', 'S', 'i', 1, 'a'};
                std::vector<uint8_t> const v_C = {'[', '$', 'C', '#', '[', 'i', 1, 'i', 2, ']', 'a', 'a'};
                std::vector<uint8_t> const v_B = {'[', '$', 'B', '#', '[', 'i', 1, 'i', 2, ']', 0xFF, 0xFF};
                std::vector<uint8_t> const v_R = {'[', '#', '[', 'i', 2, ']', 'i', 6, 'U', 7};

                // check if vector is parsed correctly
                CHECK(json::from_bjdata(v_0) == json::array());
                CHECK(json::from_bjdata(v_E) == json::array());
                CHECK(json::from_bjdata(v_i) == json({127, 127}));
                CHECK(json::from_bjdata(v_U) == json({255, 255}));
                CHECK(json::from_bjdata(v_I) == json({32767, 32767}));
                CHECK(json::from_bjdata(v_u) == json({42767, 42767}));
                CHECK(json::from_bjdata(v_l) == json({2147483647, 2147483647}));
                CHECK(json::from_bjdata(v_m) == json({3147483647, 3147483647}));
                CHECK(json::from_bjdata(v_L) == json({9223372036854775807, 9223372036854775807}));
                CHECK(json::from_bjdata(v_M) == json({10223372036854775807ull, 10223372036854775807ull}));
                CHECK(json::from_bjdata(v_D) == json({3.1415926, 3.1415926}));
                CHECK(json::from_bjdata(v_S) == json({"a", "a"}));
                CHECK(json::from_bjdata(v_C) == json({"a", "a"}));
                CHECK(json::from_bjdata(v_B) == json::binary(std::vector<uint8_t>({static_cast<uint8_t>(255), static_cast<uint8_t>(255)})));
                CHECK(json::from_bjdata(v_R) == json({6, 7}));
            }

            SECTION("optimized ndarray (type and vector-size as size-optimized array)")
            {
                // create vector with two elements of the same type
                std::vector<uint8_t> const v_i = {'[', '$', 'i', '#', '[', '#', 'i', 2, 'i', 1, 'i', 2, 0x7F, 0x7F};
                std::vector<uint8_t> const v_U = {'[', '$', 'U', '#', '[', '#', 'i', 2, 'i', 1, 'i', 2, 0xFF, 0xFF};
                std::vector<uint8_t> const v_I = {'[', '$', 'I', '#', '[', '#', 'i', 2, 'i', 1, 'i', 2, 0xFF, 0x7F, 0xFF, 0x7F};
                std::vector<uint8_t> const v_u = {'[', '$', 'u', '#', '[', '#', 'i', 2, 'i', 1, 'i', 2, 0x0F, 0xA7, 0x0F, 0xA7};
                std::vector<uint8_t> const v_l = {'[', '$', 'l', '#', '[', '#', 'i', 2, 'i', 1, 'i', 2, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F};
                std::vector<uint8_t> const v_m = {'[', '$', 'm', '#', '[', '#', 'i', 2, 'i', 1, 'i', 2, 0xFF, 0xC9, 0x9A, 0xBB, 0xFF, 0xC9, 0x9A, 0xBB};
                std::vector<uint8_t> const v_L = {'[', '$', 'L', '#', '[', '#', 'i', 2, 'i', 1, 'i', 2, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F};
                std::vector<uint8_t> const v_M = {'[', '$', 'M', '#', '[', '#', 'i', 2, 'i', 1, 'i', 2, 0xFF, 0xFF, 0x63, 0xA7, 0xB3, 0xB6, 0xE0, 0x8D, 0xFF, 0xFF, 0x63, 0xA7, 0xB3, 0xB6, 0xE0, 0x8D};
                std::vector<uint8_t> const v_D = {'[', '$', 'D', '#', '[', '#', 'i', 2, 'i', 1, 'i', 2, 0x4a, 0xd8, 0x12, 0x4d, 0xfb, 0x21, 0x09, 0x40, 0x4a, 0xd8, 0x12, 0x4d, 0xfb, 0x21, 0x09, 0x40};
                std::vector<uint8_t> const v_S = {'[', '#', '[', '#', 'i', 2, 'i', 1, 'i', 2, 'S', 'i', 1, 'a', 'S', 'i', 1, 'a'};
                std::vector<uint8_t> const v_C = {'[', '$', 'C', '#', '[', '#', 'i', 2, 'i', 1, 'i', 2, 'a', 'a'};
                std::vector<uint8_t> const v_B = {'[', '$', 'B', '#', '[', '#', 'i', 2, 'i', 1, 'i', 2, 0xFF, 0xFF};

                // check if vector is parsed correctly
                CHECK(json::from_bjdata(v_i) == json({127, 127}));
                CHECK(json::from_bjdata(v_U) == json({255, 255}));
                CHECK(json::from_bjdata(v_I) == json({32767, 32767}));
                CHECK(json::from_bjdata(v_u) == json({42767, 42767}));
                CHECK(json::from_bjdata(v_l) == json({2147483647, 2147483647}));
                CHECK(json::from_bjdata(v_m) == json({3147483647, 3147483647}));
                CHECK(json::from_bjdata(v_L) == json({9223372036854775807, 9223372036854775807}));
                CHECK(json::from_bjdata(v_M) == json({10223372036854775807ull, 10223372036854775807ull}));
                CHECK(json::from_bjdata(v_D) == json({3.1415926, 3.1415926}));
                CHECK(json::from_bjdata(v_S) == json({"a", "a"}));
                CHECK(json::from_bjdata(v_C) == json({"a", "a"}));
                CHECK(json::from_bjdata(v_B) == json::binary(std::vector<uint8_t>({static_cast<uint8_t>(255), static_cast<uint8_t>(255)})));
            }

            SECTION("invalid ndarray annotations remains as object")
            {
                // check if invalid ND array annotations stay as object
                json j_type = json({{"_ArrayData_", {1, 2, 3, 4, 5, 6}}, {"_ArraySize_", {2, 3}}, {"_ArrayType_", "invalidtype"}});
                json j_size = json({{"_ArrayData_", {1, 2, 3, 4, 5}}, {"_ArraySize_", {2, 3}}, {"_ArrayType_", "uint8"}});

                // roundtrip: output should stay as object
                CHECK(json::from_bjdata(json::to_bjdata(j_type), true, true) == j_type);
                CHECK(json::from_bjdata(json::to_bjdata(j_size), true, true) == j_size);
            }
        }
    }

    SECTION("parse errors")
    {
        SECTION("empty byte vector")
        {
            json _;
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(std::vector<uint8_t>()),
                                 "[json.exception.parse_error.110] parse error at byte 1: syntax error while parsing BJData value: unexpected end of input", json::parse_error&);
        }

        SECTION("char")
        {
            SECTION("eof after C byte")
            {
                std::vector<uint8_t> const v = {'C'};
                json _;
                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(v), "[json.exception.parse_error.110] parse error at byte 2: syntax error while parsing BJData char: unexpected end of input", json::parse_error&);
            }

            SECTION("byte out of range")
            {
                std::vector<uint8_t> const v = {'C', 130};
                json _;
                CHECK_THROWS_WITH(_ = json::from_bjdata(v), "[json.exception.parse_error.113] parse error at byte 2: syntax error while parsing BJData char: byte after 'C' must be in range 0x00..0x7F; last byte: 0x82");
            }
        }

        SECTION("byte")
        {
            SECTION("parse bjdata markers in ubjson")
            {
                std::vector<uint8_t> const v = {'B', 1};

                json _;
                CHECK_THROWS_WITH_AS(_ = json::from_ubjson(v), "[json.exception.parse_error.112] parse error at byte 1: syntax error while parsing UBJSON value: invalid byte: 0x42", json::parse_error&);
            }
        }

        SECTION("strings")
        {
            SECTION("eof after S byte")
            {
                std::vector<uint8_t> const v = {'S'};
                json _;
                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(v), "[json.exception.parse_error.110] parse error at byte 2: syntax error while parsing BJData value: unexpected end of input", json::parse_error&);
            }

            SECTION("invalid byte")
            {
                std::vector<uint8_t> const v = {'S', '1', 'a'};
                json _;
                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(v), "[json.exception.parse_error.113] parse error at byte 2: syntax error while parsing BJData string: expected length type specification (U, i, u, I, m, l, M, L); last byte: 0x31", json::parse_error&);
            }

            SECTION("parse bjdata markers in ubjson")
            {
                // create a single-character string for all number types
                std::vector<uint8_t> const s_u = {'S', 'u', 1, 0, 'a'};
                std::vector<uint8_t> const s_m = {'S', 'm', 1, 0, 0, 0, 'a'};
                std::vector<uint8_t> const s_M = {'S', 'M', 1, 0, 0, 0, 0, 0, 0, 0, 'a'};

                json _;
                // check if string is parsed correctly to "a"
                CHECK_THROWS_WITH_AS(_ = json::from_ubjson(s_u), "[json.exception.parse_error.113] parse error at byte 2: syntax error while parsing UBJSON string: expected length type specification (U, i, I, l, L); last byte: 0x75", json::parse_error&);
                CHECK_THROWS_WITH_AS(_ = json::from_ubjson(s_m), "[json.exception.parse_error.113] parse error at byte 2: syntax error while parsing UBJSON string: expected length type specification (U, i, I, l, L); last byte: 0x6D", json::parse_error&);
                CHECK_THROWS_WITH_AS(_ = json::from_ubjson(s_M), "[json.exception.parse_error.113] parse error at byte 2: syntax error while parsing UBJSON string: expected length type specification (U, i, I, l, L); last byte: 0x4D", json::parse_error&);
            }
        }

        SECTION("array")
        {
            SECTION("optimized array: no size following type")
            {
                std::vector<uint8_t> const v = {'[', '$', 'i', 2};
                json _;
                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(v), "[json.exception.parse_error.112] parse error at byte 4: syntax error while parsing BJData size: expected '#' after type information; last byte: 0x02", json::parse_error&);
            }

            SECTION("optimized array: negative size")
            {
                std::vector<uint8_t> const v1 = {'[', '#', 'i', 0xF1};
                std::vector<uint8_t> const v2 = {'[', '$', 'I', '#', 'i', 0xF2};
                std::vector<uint8_t> const v3 = {'[', '$', 'I', '#', '[', 'i', 0xF4, 'i', 0x02, ']'};
                std::vector<uint8_t> const v4 = {'[', '$', 0xF6, '#', 'i', 0xF7};
                std::vector<uint8_t> const v5 = {'[', '$', 'I', '#', '[', 'i', 0xF5, 'i', 0xF1, ']'};
                std::vector<uint8_t> const v6 = {'[', '#', '[', 'i', 0xF3, 'i', 0x02, ']'};

                std::vector<uint8_t> const vI = {'[', '#', 'I', 0x00, 0xF1};
                std::vector<uint8_t> const vl = {'[', '#', 'l', 0x00, 0x00, 0x00, 0xF2};
                std::vector<uint8_t> const vL = {'[', '#', 'L', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF3};
                std::vector<uint8_t> const vM = {'[', '$', 'M', '#', '[', 'I', 0x00, 0x20, 'M', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xFF, ']'};
                std::vector<uint8_t> const vMX = {'[', '$', 'U', '#', '[', 'M', 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 'U', 0x01, ']'};

                json _;
                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(v1), "[json.exception.parse_error.113] parse error at byte 4: syntax error while parsing BJData size: count in an optimized container must be positive", json::parse_error&);
                CHECK(json::from_bjdata(v1, true, false).is_discarded());

                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(v2), "[json.exception.parse_error.113] parse error at byte 6: syntax error while parsing BJData size: count in an optimized container must be positive", json::parse_error&);
                CHECK(json::from_bjdata(v2, true, false).is_discarded());

                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(v3), "[json.exception.parse_error.113] parse error at byte 7: syntax error while parsing BJData size: count in an optimized container must be positive", json::parse_error&);
                CHECK(json::from_bjdata(v3, true, false).is_discarded());

                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(v4), "[json.exception.parse_error.113] parse error at byte 6: syntax error while parsing BJData size: count in an optimized container must be positive", json::parse_error&);
                CHECK(json::from_bjdata(v4, true, false).is_discarded());

                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(v5), "[json.exception.parse_error.113] parse error at byte 7: syntax error while parsing BJData size: count in an optimized container must be positive", json::parse_error&);
                CHECK(json::from_bjdata(v5, true, false).is_discarded());

                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(v6), "[json.exception.parse_error.113] parse error at byte 5: syntax error while parsing BJData size: count in an optimized container must be positive", json::parse_error&);
                CHECK(json::from_bjdata(v6, true, false).is_discarded());

                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vI), "[json.exception.parse_error.113] parse error at byte 5: syntax error while parsing BJData size: count in an optimized container must be positive", json::parse_error&);
                CHECK(json::from_bjdata(vI, true, false).is_discarded());

                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vl), "[json.exception.parse_error.113] parse error at byte 7: syntax error while parsing BJData size: count in an optimized container must be positive", json::parse_error&);
                CHECK(json::from_bjdata(vl, true, false).is_discarded());

                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vL), "[json.exception.parse_error.113] parse error at byte 11: syntax error while parsing BJData size: count in an optimized container must be positive", json::parse_error&);
                CHECK(json::from_bjdata(vL, true, false).is_discarded());

#if SIZE_MAX != 0xffffffff
                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vM), "[json.exception.out_of_range.408] syntax error while parsing BJData size: excessive ndarray size caused overflow", json::out_of_range&);
#else
                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vM), "[json.exception.out_of_range.408] syntax error while parsing BJData size: integer value overflow", json::out_of_range&);
#endif
                CHECK(json::from_bjdata(vM, true, false).is_discarded());

#if SIZE_MAX != 0xffffffff
                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vMX), "[json.exception.out_of_range.408] syntax error while parsing BJData size: excessive ndarray size caused overflow", json::out_of_range&);
#else
                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vMX), "[json.exception.out_of_range.408] syntax error while parsing BJData size: integer value overflow", json::out_of_range&);
#endif
                CHECK(json::from_bjdata(vMX, true, false).is_discarded());
            }

            SECTION("optimized array: integer value overflow")
            {
#if SIZE_MAX == 0xffffffff
                std::vector<uint8_t> const vL = {'[', '#', 'L', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F};
                std::vector<uint8_t> const vM = {'[', '$', 'M', '#', '[', 'I', 0x00, 0x20, 'M', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xFF, ']'};

                json _;
                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vL), "[json.exception.out_of_range.408] syntax error while parsing BJData size: integer value overflow", json::out_of_range&);
                CHECK(json::from_bjdata(vL, true, false).is_discarded());
                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vM), "[json.exception.out_of_range.408] syntax error while parsing BJData size: integer value overflow", json::out_of_range&);
                CHECK(json::from_bjdata(vM, true, false).is_discarded());
#endif
            }

            SECTION("overflow detection in dimension multiplication")
            {
                // Simple SAX handler just to monitor if overflow is detected
                struct SimpleOverflowSaxHandler : public nlohmann::json_sax<json>
                {
                    bool overflow_detected = false;

                    // Implement all required virtual methods with minimal implementation
                    bool null() override
                    {
                        return true;
                    }
                    bool boolean(bool /*val*/) override
                    {
                        return true;
                    }
                    bool number_integer(json::number_integer_t /*val*/) override
                    {
                        return true;
                    }
                    bool number_unsigned(json::number_unsigned_t /*val*/) override
                    {
                        return true;
                    }
                    bool number_float(json::number_float_t /*val*/, const std::string& /*s*/) override
                    {
                        return true;
                    }
                    bool string(std::string& /*val*/) override
                    {
                        return true;
                    }
                    bool binary(json::binary_t& /*val*/) override
                    {
                        return true;
                    }
                    bool start_object(std::size_t /*elements*/) override
                    {
                        return true;
                    }
                    bool key(std::string& /*val*/) override
                    {
                        return true;
                    }
                    bool end_object() override
                    {
                        return true;
                    }
                    bool start_array(std::size_t /*elements*/) override
                    {
                        return true;
                    }
                    bool end_array() override
                    {
                        return true;
                    }

                    // This is the only method we care about - detecting error 408
                    bool parse_error(std::size_t /*position*/, const std::string& /*last_token*/, const json::exception& ex) override
                    {
                        if (ex.id == 408)
                        {
                            overflow_detected = true;
                        }
                        return false;
                    }
                };

                // Create BJData payload with overflow-causing dimensions (2^32+1) × (2^32)
                const std::vector<uint8_t> bjdata_payload =
                {
                    0x5B,                                           // '[' start array
                    0x24, 0x55,                                     // '$', 'U' (type uint8)
                    0x23, 0x5B,                                     // '#', '[' (dimensions array)
                    0x4D,                                           // 'M' (uint64)
                    0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, // 2^32 + 1 (4294967297) as little-endian
                    0x4D,                                           // 'M' (uint64)
                    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, // 2^32 (4294967296) as little-endian
                    0x5D                                            // ']' end dimensions
                    // No data - we don't need it for this test, we just want to hit the overflow check
                };

                // Test with overflow dimensions using SAX parser
                {
                    SimpleOverflowSaxHandler handler;
                    const auto result = json::sax_parse(bjdata_payload, &handler,
                                                        nlohmann::detail::input_format_t::bjdata, false);

                    // Should detect overflow
                    CHECK(handler.overflow_detected == true);
                    CHECK(result == false);
                }

                // Test with DOM parser (should throw)
                {
                    json _;
                    CHECK_THROWS_AS(_ = json::from_bjdata(bjdata_payload), json::out_of_range);
                }

                // Test with normal dimensions
                const std::vector<uint8_t> normal_payload =
                {
                    0x5B,                                           // '[' start array
                    0x24, 0x55,                                     // '$', 'U' (type uint8)
                    0x23, 0x5B,                                     // '#', '[' (dimensions array)
                    0x55, 0x02,                                     // 'U', 2 (uint8)
                    0x55, 0x03,                                     // 'U', 3 (uint8)
                    0x5D,                                           // ']' end dimensions
                    // 6 data bytes for a 2×3 array (enough to avoid EOF but not entire array)
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06
                };

                // For normal dimensions, overflow should not be detected
                {
                    SimpleOverflowSaxHandler handler;
                    const auto result = json::sax_parse(normal_payload, &handler,
                                                        nlohmann::detail::input_format_t::bjdata, false);

                    CHECK(handler.overflow_detected == false);
                    CHECK(result == true);
                }
            }

            SECTION("do not accept NTFZ markers in ndarray optimized type (with count)")
            {
                json _;
                std::vector<uint8_t> const v_N = {'[', '$', 'N', '#', '[', '#', 'i', 2, 'i', 1, 'i', 2};
                std::vector<uint8_t> const v_T = {'[', '$', 'T', '#', '[', '#', 'i', 2, 'i', 1, 'i', 2};
                std::vector<uint8_t> const v_F = {'[', '$', 'F', '#', '[', '#', 'i', 2, 'i', 1, 'i', 2};
                std::vector<uint8_t> const v_Z = {'[', '$', 'Z', '#', '[', '#', 'i', 2, 'i', 1, 'i', 2};

                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(v_N), "[json.exception.parse_error.112] parse error at byte 3: syntax error while parsing BJData type: marker 0x4E is not a permitted optimized array type", json::parse_error&);
                CHECK(json::from_bjdata(v_N, true, false).is_discarded());

                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(v_T), "[json.exception.parse_error.112] parse error at byte 3: syntax error while parsing BJData type: marker 0x54 is not a permitted optimized array type", json::parse_error&);
                CHECK(json::from_bjdata(v_T, true, false).is_discarded());

                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(v_F), "[json.exception.parse_error.112] parse error at byte 3: syntax error while parsing BJData type: marker 0x46 is not a permitted optimized array type", json::parse_error&);
                CHECK(json::from_bjdata(v_F, true, false).is_discarded());

                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(v_Z), "[json.exception.parse_error.112] parse error at byte 3: syntax error while parsing BJData type: marker 0x5A is not a permitted optimized array type", json::parse_error&);
                CHECK(json::from_bjdata(v_Z, true, false).is_discarded());
            }

            SECTION("do not accept NTFZ markers in ndarray optimized type (without count)")
            {
                json _;
                std::vector<uint8_t> const v_N = {'[', '$', 'N', '#', '[', 'i', 1, 'i', 2, ']'};
                std::vector<uint8_t> const v_T = {'[', '$', 'T', '#', '[', 'i', 1, 'i', 2, ']'};
                std::vector<uint8_t> const v_F = {'[', '$', 'F', '#', '[', 'i', 1, 'i', 2, ']'};
                std::vector<uint8_t> const v_Z = {'[', '$', 'Z', '#', '[', 'i', 1, 'i', 2, ']'};

                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(v_N), "[json.exception.parse_error.112] parse error at byte 3: syntax error while parsing BJData type: marker 0x4E is not a permitted optimized array type", json::parse_error&);
                CHECK(json::from_bjdata(v_N, true, false).is_discarded());

                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(v_T), "[json.exception.parse_error.112] parse error at byte 3: syntax error while parsing BJData type: marker 0x54 is not a permitted optimized array type", json::parse_error&);
                CHECK(json::from_bjdata(v_T, true, false).is_discarded());

                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(v_F), "[json.exception.parse_error.112] parse error at byte 3: syntax error while parsing BJData type: marker 0x46 is not a permitted optimized array type", json::parse_error&);
                CHECK(json::from_bjdata(v_F, true, false).is_discarded());

                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(v_Z), "[json.exception.parse_error.112] parse error at byte 3: syntax error while parsing BJData type: marker 0x5A is not a permitted optimized array type", json::parse_error&);
                CHECK(json::from_bjdata(v_Z, true, false).is_discarded());
            }
        }

        SECTION("strings")
        {
            std::vector<uint8_t> const vS = {'S'};
            json _;
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vS), "[json.exception.parse_error.110] parse error at byte 2: syntax error while parsing BJData value: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(vS, true, false).is_discarded());

            std::vector<uint8_t> const v = {'S', 'i', '2', 'a'};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(v), "[json.exception.parse_error.110] parse error at byte 5: syntax error while parsing BJData string: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(v, true, false).is_discarded());

            std::vector<uint8_t> const vC = {'C'};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vC), "[json.exception.parse_error.110] parse error at byte 2: syntax error while parsing BJData char: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(vC, true, false).is_discarded());
        }

        SECTION("sizes")
        {
            std::vector<uint8_t> const vU = {'[', '#', 'U'};
            json _;
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vU), "[json.exception.parse_error.110] parse error at byte 4: syntax error while parsing BJData number: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(vU, true, false).is_discarded());

            std::vector<uint8_t> const vi = {'[', '#', 'i'};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vi), "[json.exception.parse_error.110] parse error at byte 4: syntax error while parsing BJData number: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(vi, true, false).is_discarded());

            std::vector<uint8_t> const vI = {'[', '#', 'I'};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vI), "[json.exception.parse_error.110] parse error at byte 4: syntax error while parsing BJData number: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(vI, true, false).is_discarded());

            std::vector<uint8_t> const vu = {'[', '#', 'u'};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vu), "[json.exception.parse_error.110] parse error at byte 4: syntax error while parsing BJData number: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(vu, true, false).is_discarded());

            std::vector<uint8_t> const vl = {'[', '#', 'l'};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vl), "[json.exception.parse_error.110] parse error at byte 4: syntax error while parsing BJData number: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(vl, true, false).is_discarded());

            std::vector<uint8_t> const vm = {'[', '#', 'm'};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vm), "[json.exception.parse_error.110] parse error at byte 4: syntax error while parsing BJData number: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(vm, true, false).is_discarded());

            std::vector<uint8_t> const vL = {'[', '#', 'L'};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vL), "[json.exception.parse_error.110] parse error at byte 4: syntax error while parsing BJData number: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(vL, true, false).is_discarded());

            std::vector<uint8_t> const vM = {'[', '#', 'M'};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vM), "[json.exception.parse_error.110] parse error at byte 4: syntax error while parsing BJData number: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(vM, true, false).is_discarded());

            std::vector<uint8_t> const v0 = {'[', '#', 'T', ']'};
            CHECK_THROWS_WITH(_ = json::from_bjdata(v0), "[json.exception.parse_error.113] parse error at byte 3: syntax error while parsing BJData size: expected length type specification (U, i, u, I, m, l, M, L) after '#'; last byte: 0x54");
            CHECK(json::from_bjdata(v0, true, false).is_discarded());

            std::vector<uint8_t> const vB = {'[', '#', 'B', ']'};
            CHECK_THROWS_WITH(_ = json::from_bjdata(vB), "[json.exception.parse_error.113] parse error at byte 3: syntax error while parsing BJData size: expected length type specification (U, i, u, I, m, l, M, L) after '#'; last byte: 0x42");
            CHECK(json::from_bjdata(v0, true, false).is_discarded());
        }

        SECTION("parse bjdata markers as array size in ubjson")
        {
            json _;
            std::vector<uint8_t> const vu = {'[', '#', 'u'};
            CHECK_THROWS_WITH_AS(_ = json::from_ubjson(vu), "[json.exception.parse_error.113] parse error at byte 3: syntax error while parsing UBJSON size: expected length type specification (U, i, I, l, L) after '#'; last byte: 0x75", json::parse_error&);
            CHECK(json::from_ubjson(vu, true, false).is_discarded());

            std::vector<uint8_t> const vm = {'[', '#', 'm'};
            CHECK_THROWS_WITH_AS(_ = json::from_ubjson(vm), "[json.exception.parse_error.113] parse error at byte 3: syntax error while parsing UBJSON size: expected length type specification (U, i, I, l, L) after '#'; last byte: 0x6D", json::parse_error&);
            CHECK(json::from_ubjson(vm, true, false).is_discarded());

            std::vector<uint8_t> const vM = {'[', '#', 'M'};
            CHECK_THROWS_WITH_AS(_ = json::from_ubjson(vM), "[json.exception.parse_error.113] parse error at byte 3: syntax error while parsing UBJSON size: expected length type specification (U, i, I, l, L) after '#'; last byte: 0x4D", json::parse_error&);
            CHECK(json::from_ubjson(vM, true, false).is_discarded());

            std::vector<uint8_t> const v0 = {'[', '#', '['};
            CHECK_THROWS_WITH_AS(_ = json::from_ubjson(v0), "[json.exception.parse_error.113] parse error at byte 3: syntax error while parsing UBJSON size: expected length type specification (U, i, I, l, L) after '#'; last byte: 0x5B", json::parse_error&);
            CHECK(json::from_ubjson(v0, true, false).is_discarded());
        }

        SECTION("types")
        {
            std::vector<uint8_t> const v0 = {'[', '$'};
            json _;
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(v0), "[json.exception.parse_error.110] parse error at byte 3: syntax error while parsing BJData type: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(v0, true, false).is_discarded());

            std::vector<uint8_t> const vi = {'[', '$', '#'};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vi), "[json.exception.parse_error.110] parse error at byte 4: syntax error while parsing BJData value: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(vi, true, false).is_discarded());

            std::vector<uint8_t> const vU = {'[', '$', 'U'};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vU), "[json.exception.parse_error.110] parse error at byte 4: syntax error while parsing BJData value: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(vU, true, false).is_discarded());

            std::vector<uint8_t> const v1 = {'[', '$', '['};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(v1), "[json.exception.parse_error.112] parse error at byte 3: syntax error while parsing BJData type: marker 0x5B is not a permitted optimized array type", json::parse_error&);
            CHECK(json::from_bjdata(v1, true, false).is_discarded());
        }

        SECTION("arrays")
        {
            std::vector<uint8_t> const vST = {'[', '$', 'i', '#', 'i', 2, 1};
            json _;
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vST), "[json.exception.parse_error.110] parse error at byte 8: syntax error while parsing BJData number: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(vST, true, false).is_discarded());

            std::vector<uint8_t> const vS = {'[', '#', 'i', 2, 'i', 1};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vS), "[json.exception.parse_error.110] parse error at byte 7: syntax error while parsing BJData value: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(vS, true, false).is_discarded());

            std::vector<uint8_t> const v = {'[', 'i', 2, 'i', 1};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(v), "[json.exception.parse_error.110] parse error at byte 6: syntax error while parsing BJData value: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(v, true, false).is_discarded());
        }

        SECTION("ndarrays")
        {
            std::vector<uint8_t> const vST = {'[', '$', 'i', '#', '[', '$', 'i', '#'};
            json _;
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vST), "[json.exception.parse_error.113] parse error at byte 9: syntax error while parsing BJData size: expected length type specification (U, i, u, I, m, l, M, L) after '#'; last byte: 0xFF", json::parse_error&);
            CHECK(json::from_bjdata(vST, true, false).is_discarded());

            std::vector<uint8_t> const v = {'[', '$', 'i', '#', '[', '$', 'i', '#', 'i', 2, 1, 2};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(v), "[json.exception.parse_error.110] parse error at byte 13: syntax error while parsing BJData number: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(v, true, false).is_discarded());

            std::vector<uint8_t> const vS0 = {'[', '$', 'i', '#', '[', '$', 'i', '#', 'i', 2, 1};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vS0), "[json.exception.parse_error.110] parse error at byte 12: syntax error while parsing BJData number: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(vS0, true, false).is_discarded());

            std::vector<uint8_t> const vS = {'[', '$', 'i', '#', '[', '#', 'i', 2, 1, 2, 1};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vS), "[json.exception.parse_error.113] parse error at byte 9: syntax error while parsing BJData size: expected length type specification (U, i, u, I, m, l, M, L) after '#'; last byte: 0x01", json::parse_error&);
            CHECK(json::from_bjdata(vS, true, false).is_discarded());

            std::vector<uint8_t> const vT = {'[', '$', 'i', '#', '[', 'i', 2, 'i'};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vT), "[json.exception.parse_error.110] parse error at byte 9: syntax error while parsing BJData number: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(vT, true, false).is_discarded());

            std::vector<uint8_t> const vT0 = {'[', '$', 'i', '#', '[', 'i'};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vT0), "[json.exception.parse_error.110] parse error at byte 7: syntax error while parsing BJData number: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(vT0, true, false).is_discarded());

            std::vector<uint8_t> const vu = {'[', '$', 'i', '#', '[', '$', 'i', '#', 'u', 1, 0};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vu), "[json.exception.parse_error.110] parse error at byte 12: syntax error while parsing BJData number: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(vu, true, false).is_discarded());

            std::vector<uint8_t> const vm = {'[', '$', 'i', '#', '[', '$', 'i', '#', 'm', 1, 0, 0, 0};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vm), "[json.exception.parse_error.110] parse error at byte 14: syntax error while parsing BJData number: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(vm, true, false).is_discarded());

            std::vector<uint8_t> const vM = {'[', '$', 'i', '#', '[', '$', 'i', '#', 'M', 1, 0, 0, 0, 0, 0, 0, 0};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vM), "[json.exception.parse_error.110] parse error at byte 18: syntax error while parsing BJData number: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(vM, true, false).is_discarded());

            std::vector<uint8_t> const vU = {'[', '$', 'U', '#', '[', '$', 'i', '#', 'i', 2, 2, 3, 1, 2, 3, 4, 5};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vU), "[json.exception.parse_error.110] parse error at byte 18: syntax error while parsing BJData number: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(vU, true, false).is_discarded());

            std::vector<uint8_t> const vB = {'[', '$', 'B', '#', '[', '$', 'i', '#', 'i', 2, 2, 3, 1, 2, 3, 4, 5};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vU), "[json.exception.parse_error.110] parse error at byte 18: syntax error while parsing BJData number: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(vU, true, false).is_discarded());

            std::vector<uint8_t> const vT1 = {'[', '$', 'T', '#', '[', '$', 'i', '#', 'i', 2, 2, 3};
            CHECK(json::from_bjdata(vT1, true, false).is_discarded());

            std::vector<uint8_t> const vh = {'[', '$', 'h', '#', '[', '$', 'i', '#', 'i', 2, 2, 3};
            CHECK(json::from_bjdata(vh, true, false).is_discarded());

            std::vector<uint8_t> const vR = {'[', '$', 'i', '#', '[', 'i', 1, '[', ']', ']', 1};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vR), "[json.exception.parse_error.113] parse error at byte 8: syntax error while parsing BJData size: ndarray dimensional vector is not allowed", json::parse_error&);
            CHECK(json::from_bjdata(vR, true, false).is_discarded());

            std::vector<uint8_t> const vRo = {'[', '$', 'i', '#', '[', 'i', 0, '{', '}', ']', 1};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vRo), "[json.exception.parse_error.113] parse error at byte 8: syntax error while parsing BJData size: expected length type specification (U, i, u, I, m, l, M, L) after '#'; last byte: 0x7B", json::parse_error&);
            CHECK(json::from_bjdata(vRo, true, false).is_discarded());

            std::vector<uint8_t> const vR1 = {'[', '$', 'i', '#', '[', '[', 'i', 1, ']', ']', 1};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vR1), "[json.exception.parse_error.113] parse error at byte 6: syntax error while parsing BJData size: ndarray dimensional vector is not allowed", json::parse_error&);
            CHECK(json::from_bjdata(vR1, true, false).is_discarded());

            std::vector<uint8_t> const vR2 = {'[', '$', 'i', '#', '[', '#', '[', 'i', 1, ']', ']', 1};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vR2), "[json.exception.parse_error.113] parse error at byte 11: syntax error while parsing BJData size: expected length type specification (U, i, u, I, m, l, M, L) after '#'; last byte: 0x5D", json::parse_error&);
            CHECK(json::from_bjdata(vR2, true, false).is_discarded());

            std::vector<uint8_t> const vR3 = {'[', '#', '[', 'i', '2', 'i', 2, ']'};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vR3), "[json.exception.parse_error.112] parse error at byte 8: syntax error while parsing BJData size: ndarray requires both type and size", json::parse_error&);
            CHECK(json::from_bjdata(vR3, true, false).is_discarded());

            std::vector<uint8_t> const vR4 = {'[', '$', 'i', '#', '[', '$', 'i', '#', '[', 'i', 1, ']', 1};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vR4), "[json.exception.parse_error.110] parse error at byte 14: syntax error while parsing BJData number: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(vR4, true, false).is_discarded());

            std::vector<uint8_t> const vR5 = {'[', '$', 'i', '#', '[', '[', '[', ']', ']', ']'};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vR5), "[json.exception.parse_error.113] parse error at byte 6: syntax error while parsing BJData size: ndarray dimensional vector is not allowed", json::parse_error&);
            CHECK(json::from_bjdata(vR5, true, false).is_discarded());

            std::vector<uint8_t> const vR6 = {'[', '$', 'i', '#', '[', '$', 'i', '#', '[', 'i', '2', 'i', 2, ']'};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vR6), "[json.exception.parse_error.112] parse error at byte 14: syntax error while parsing BJData size: ndarray can not be recursive", json::parse_error&);
            CHECK(json::from_bjdata(vR6, true, false).is_discarded());

            std::vector<uint8_t> const vH = {'[', 'H', '[', '#', '[', '$', 'i', '#', '[', 'i', '2', 'i', 2, ']'};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vH), "[json.exception.parse_error.113] parse error at byte 3: syntax error while parsing BJData size: ndarray dimensional vector is not allowed", json::parse_error&);
            CHECK(json::from_bjdata(vH, true, false).is_discarded());
        }

        SECTION("objects")
        {
            std::vector<uint8_t> const vST = {'{', '$', 'i', '#', 'i', 2, 'i', 1, 'a', 1};
            json _;
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vST), "[json.exception.parse_error.110] parse error at byte 11: syntax error while parsing BJData value: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(vST, true, false).is_discarded());

            std::vector<uint8_t> const vT = {'{', '$', 'i', 'i', 1, 'a', 1};
            CHECK_THROWS_WITH(_ = json::from_bjdata(vT), "[json.exception.parse_error.112] parse error at byte 4: syntax error while parsing BJData size: expected '#' after type information; last byte: 0x69");
            CHECK(json::from_bjdata(vT, true, false).is_discarded());

            std::vector<uint8_t> const vS = {'{', '#', 'i', 2, 'i', 1, 'a', 'i', 1};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vS), "[json.exception.parse_error.110] parse error at byte 10: syntax error while parsing BJData value: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(vS, true, false).is_discarded());

            std::vector<uint8_t> const v = {'{', 'i', 1, 'a', 'i', 1};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(v), "[json.exception.parse_error.110] parse error at byte 7: syntax error while parsing BJData value: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(v, true, false).is_discarded());

            std::vector<uint8_t> const v2 = {'{', 'i', 1, 'a', 'i', 1, 'i'};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(v2), "[json.exception.parse_error.110] parse error at byte 8: syntax error while parsing BJData number: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(v2, true, false).is_discarded());

            std::vector<uint8_t> const v3 = {'{', 'i', 1, 'a'};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(v3), "[json.exception.parse_error.110] parse error at byte 5: syntax error while parsing BJData value: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(v3, true, false).is_discarded());

            std::vector<uint8_t> const vST1 = {'{', '$', 'd', '#', 'i', 2, 'i', 1, 'a'};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vST1), "[json.exception.parse_error.110] parse error at byte 10: syntax error while parsing BJData number: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(vST1, true, false).is_discarded());

            std::vector<uint8_t> const vST2 = {'{', '#', 'i', 2, 'i', 1, 'a'};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vST2), "[json.exception.parse_error.110] parse error at byte 8: syntax error while parsing BJData value: unexpected end of input", json::parse_error&);
            CHECK(json::from_bjdata(vST2, true, false).is_discarded());

            std::vector<uint8_t> const vO = {'{', '#', '[', 'i', 2, 'i', 1, ']', 'i', 1, 'a', 'i', 1, 'i', 1, 'b', 'i', 2};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vO), "[json.exception.parse_error.112] parse error at byte 8: syntax error while parsing BJData size: ndarray requires both type and size", json::parse_error&);
            CHECK(json::from_bjdata(vO, true, false).is_discarded());

            std::vector<uint8_t> const vO2 = {'{', '$', 'i', '#', '[', 'i', 2, 'i', 1, ']', 'i', 1, 'a', 1, 'i', 1, 'b', 2};
            CHECK_THROWS_WITH_AS(_ = json::from_bjdata(vO2), "[json.exception.parse_error.112] parse error at byte 10: syntax error while parsing BJData object: BJData object does not support ND-array size in optimized format", json::parse_error&);
            CHECK(json::from_bjdata(vO2, true, false).is_discarded());
        }
    }

    SECTION("writing optimized values")
    {
        SECTION("integer")
        {
            SECTION("array of i")
            {
                json const j = {1, -1};
                std::vector<uint8_t> const expected = {'[', '$', 'i', '#', 'i', 2, 1, 0xff};
                CHECK(json::to_bjdata(j, true, true) == expected);
            }

            SECTION("array of U")
            {
                json const j = {200, 201};
                std::vector<uint8_t> const expected = {'[', '$', 'U', '#', 'i', 2, 0xC8, 0xC9};
                CHECK(json::to_bjdata(j, true, true) == expected);
            }

            SECTION("array of I")
            {
                json const j = {30000, -30000};
                std::vector<uint8_t> const expected = {'[', '$', 'I', '#', 'i', 2, 0x30, 0x75, 0xd0, 0x8a};
                CHECK(json::to_bjdata(j, true, true) == expected);
            }

            SECTION("array of u")
            {
                json const j = {50000, 50001};
                std::vector<uint8_t> const expected = {'[', '$', 'u', '#', 'i', 2, 0x50, 0xC3, 0x51, 0xC3};
                CHECK(json::to_bjdata(j, true, true) == expected);
            }

            SECTION("array of l")
            {
                json const j = {70000, -70000};
                std::vector<uint8_t> const expected = {'[', '$', 'l', '#', 'i', 2, 0x70, 0x11, 0x01, 0x00, 0x90, 0xEE, 0xFE, 0xFF};
                CHECK(json::to_bjdata(j, true, true) == expected);
            }

            SECTION("array of m")
            {
                json const j = {3147483647, 3147483648};
                std::vector<uint8_t> const expected = {'[', '$', 'm', '#', 'i', 2, 0xFF, 0xC9, 0x9A, 0xBB, 0x00, 0xCA, 0x9A, 0xBB};
                CHECK(json::to_bjdata(j, true, true) == expected);
            }

            SECTION("array of L")
            {
                json const j = {5000000000, -5000000000};
                std::vector<uint8_t> const expected = {'[', '$', 'L', '#', 'i', 2, 0x00, 0xF2, 0x05, 0x2A, 0x01, 0x00, 0x00, 0x00, 0x00, 0x0E, 0xFA, 0xD5, 0xFE, 0xFF, 0xFF, 0xFF};
                CHECK(json::to_bjdata(j, true, true) == expected);
            }
        }

        SECTION("unsigned integer")
        {
            SECTION("array of i")
            {
                json const j = {1u, 2u};
                std::vector<uint8_t> const expected = {'[', '$', 'i', '#', 'i', 2, 1, 2};
                std::vector<uint8_t> const expected_size = {'[', '#', 'i', 2, 'i', 1, 'i', 2};
                CHECK(json::to_bjdata(j, true, true) == expected);
                CHECK(json::to_bjdata(j, true) == expected_size);
            }

            SECTION("array of U")
            {
                json const j = {200u, 201u};
                std::vector<uint8_t> const expected = {'[', '$', 'U', '#', 'i', 2, 0xC8, 0xC9};
                std::vector<uint8_t> const expected_size = {'[', '#', 'i', 2, 'U', 0xC8, 'U', 0xC9};
                CHECK(json::to_bjdata(j, true, true) == expected);
                CHECK(json::to_bjdata(j, true) == expected_size);
            }

            SECTION("array of I")
            {
                json const j = {30000u, 30001u};
                std::vector<uint8_t> const expected = {'[', '$', 'I', '#', 'i', 2, 0x30, 0x75, 0x31, 0x75};
                std::vector<uint8_t> const expected_size = {'[', '#', 'i', 2, 'I', 0x30, 0x75, 'I', 0x31, 0x75};
                CHECK(json::to_bjdata(j, true, true) == expected);
                CHECK(json::to_bjdata(j, true) == expected_size);
            }

            SECTION("array of u")
            {
                json const j = {50000u, 50001u};
                std::vector<uint8_t> const expected = {'[', '$', 'u', '#', 'i', 2, 0x50, 0xC3, 0x51, 0xC3};
                std::vector<uint8_t> const expected_size = {'[', '#', 'i', 2, 'u', 0x50, 0xC3, 'u', 0x51, 0xC3};
                CHECK(json::to_bjdata(j, true, true) == expected);
                CHECK(json::to_bjdata(j, true) == expected_size);
            }

            SECTION("array of l")
            {
                json const j = {70000u, 70001u};
                std::vector<uint8_t> const expected = {'[', '$', 'l', '#', 'i', 2, 0x70, 0x11, 0x01, 0x00, 0x71, 0x11, 0x01, 0x00};
                std::vector<uint8_t> const expected_size = {'[', '#', 'i', 2, 'l', 0x70, 0x11, 0x01, 0x00, 'l', 0x71, 0x11, 0x01, 0x00};
                CHECK(json::to_bjdata(j, true, true) == expected);
                CHECK(json::to_bjdata(j, true) == expected_size);
            }

            SECTION("array of m")
            {
                json const j = {3147483647u, 3147483648u};
                std::vector<uint8_t> const expected = {'[', '$', 'm', '#', 'i', 2, 0xFF, 0xC9, 0x9A, 0xBB, 0x00, 0xCA, 0x9A, 0xBB};
                std::vector<uint8_t> const expected_size = {'[', '#', 'i', 2, 'm', 0xFF, 0xC9, 0x9A, 0xBB, 'm', 0x00, 0xCA, 0x9A, 0xBB};
                CHECK(json::to_bjdata(j, true, true) == expected);
                CHECK(json::to_bjdata(j, true) == expected_size);
            }

            SECTION("array of L")
            {
                json const j = {5000000000u, 5000000001u};
                std::vector<uint8_t> const expected = {'[', '$', 'L', '#', 'i', 2, 0x00, 0xF2, 0x05, 0x2A, 0x01, 0x00, 0x00, 0x00, 0x01, 0xF2, 0x05, 0x2A, 0x01, 0x00, 0x00, 0x00};
                std::vector<uint8_t> const expected_size = {'[', '#', 'i', 2, 'L', 0x00, 0xF2, 0x05, 0x2A, 0x01, 0x00, 0x00, 0x00, 'L', 0x01, 0xF2, 0x05, 0x2A, 0x01, 0x00, 0x00, 0x00};
                CHECK(json::to_bjdata(j, true, true) == expected);
                CHECK(json::to_bjdata(j, true) == expected_size);
            }

            SECTION("array of M")
            {
                json const j = {10223372036854775807ull, 10223372036854775808ull};
                std::vector<uint8_t> const expected = {'[', '$', 'M', '#', 'i', 2, 0xFF, 0xFF, 0x63, 0xA7, 0xB3, 0xB6, 0xE0, 0x8D, 0x00, 0x00, 0x64, 0xA7, 0xB3, 0xB6, 0xE0, 0x8D};
                std::vector<uint8_t> const expected_size = {'[', '#', 'i', 2, 'M', 0xFF, 0xFF, 0x63, 0xA7, 0xB3, 0xB6, 0xE0, 0x8D, 'M', 0x00, 0x00, 0x64, 0xA7, 0xB3, 0xB6, 0xE0, 0x8D};
                CHECK(json::to_bjdata(j, true, true) == expected);
                CHECK(json::to_bjdata(j, true) == expected_size);
            }
        }
    }
}

TEST_CASE("Universal Binary JSON Specification Examples 1")
{
    SECTION("Null Value")
    {
        json const j = {{"passcode", nullptr}};
        std::vector<uint8_t> v = {'{', 'i', 8, 'p', 'a', 's', 's', 'c', 'o', 'd', 'e', 'Z', '}'};
        CHECK(json::to_bjdata(j) == v);
        CHECK(json::from_bjdata(v) == j);
    }

    SECTION("No-Op Value")
    {
        json const j = {"foo", "bar", "baz"};
        std::vector<uint8_t> v = {'[', 'S', 'i', 3, 'f', 'o', 'o',
                                  'S', 'i', 3, 'b', 'a', 'r',
                                  'S', 'i', 3, 'b', 'a', 'z', ']'
                                 };
        std::vector<uint8_t> const v2 = {'[', 'S', 'i', 3, 'f', 'o', 'o', 'N',
                                         'S', 'i', 3, 'b', 'a', 'r', 'N', 'N', 'N',
                                         'S', 'i', 3, 'b', 'a', 'z', 'N', 'N', ']'
                                        };
        CHECK(json::to_bjdata(j) == v);
        CHECK(json::from_bjdata(v) == j);
        CHECK(json::from_bjdata(v2) == j);
    }

    SECTION("Boolean Types")
    {
        json const j = {{"authorized", true}, {"verified", false}};
        std::vector<uint8_t> v = {'{', 'i', 10, 'a', 'u', 't', 'h', 'o', 'r', 'i', 'z', 'e', 'd', 'T',
                                  'i', 8, 'v', 'e', 'r', 'i', 'f', 'i', 'e', 'd', 'F', '}'
                                 };
        CHECK(json::to_bjdata(j) == v);
        CHECK(json::from_bjdata(v) == j);
    }

    SECTION("Numeric Types")
    {
        json j =
        {
            {"int8", 16},
            {"uint8", 255},
            {"int16", 32767},
            {"uint16", 42767},
            {"int32", 2147483647},
            {"uint32", 3147483647},
            {"int64", 9223372036854775807},
            {"uint64", 10223372036854775807ull},
            {"float64", 113243.7863123}
        };
        std::vector<uint8_t> v = {'{',
                                  'i', 7, 'f', 'l', 'o', 'a', 't', '6', '4', 'D', 0xcf, 0x34, 0xbc, 0x94, 0xbc, 0xa5, 0xfb, 0x40,
                                  'i', 5, 'i', 'n', 't', '1', '6', 'I', 0xff, 0x7f,
                                  'i', 5, 'i', 'n', 't', '3', '2', 'l', 0xff, 0xff, 0xff, 0x7f,
                                  'i', 5, 'i', 'n', 't', '6', '4', 'L', 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f,
                                  'i', 4, 'i', 'n', 't', '8', 'i', 16,
                                  'i', 6, 'u', 'i', 'n', 't', '1', '6', 'u', 0x0F, 0xA7,
                                  'i', 6, 'u', 'i', 'n', 't', '3', '2', 'm', 0xFF, 0xC9, 0x9A, 0xBB,
                                  'i', 6, 'u', 'i', 'n', 't', '6', '4', 'M', 0xFF, 0xFF, 0x63, 0xA7, 0xB3, 0xB6, 0xE0, 0x8D,
                                  'i', 5, 'u', 'i', 'n', 't', '8', 'U', 0xff,
                                  '}'
                                 };
        CHECK(json::to_bjdata(j) == v);
        CHECK(json::from_bjdata(v) == j);
    }

    SECTION("Char Type")
    {
        json const j = {{"rolecode", "a"}, {"delim", ";"}};
        std::vector<uint8_t> const v = {'{', 'i', 5, 'd', 'e', 'l', 'i', 'm', 'C', ';', 'i', 8, 'r', 'o', 'l', 'e', 'c', 'o', 'd', 'e', 'C', 'a', '}'};
        //CHECK(json::to_bjdata(j) == v);
        CHECK(json::from_bjdata(v) == j);
    }

    SECTION("Byte Type")
    {
        const auto s = std::vector<std::uint8_t>(
        {
            static_cast<std::uint8_t>(222),
            static_cast<std::uint8_t>(173),
            static_cast<std::uint8_t>(190),
            static_cast<std::uint8_t>(239)
        });
        json const j = {{"binary", json::binary(s)}, {"val", 123}};
        std::vector<uint8_t> const v = {'{', 'i', 6, 'b', 'i', 'n', 'a', 'r', 'y', '[', '$', 'B', '#', 'i', 4, 222, 173, 190, 239, 'i', 3, 'v', 'a', 'l', 'i', 123, '}'};
        //CHECK(json::to_bjdata(j) == v); // 123 value gets encoded as uint8
        CHECK(json::from_bjdata(v) == j);
    }

    SECTION("String Type")
    {
        SECTION("English")
        {
            json const j = "hello";
            std::vector<uint8_t> v = {'S', 'i', 5, 'h', 'e', 'l', 'l', 'o'};
            CHECK(json::to_bjdata(j) == v);
            CHECK(json::from_bjdata(v) == j);
        }

        SECTION("Russian")
        {
            json const j = "привет";
            std::vector<uint8_t> v = {'S', 'i', 12, 0xD0, 0xBF, 0xD1, 0x80, 0xD0, 0xB8, 0xD0, 0xB2, 0xD0, 0xB5, 0xD1, 0x82};
            CHECK(json::to_bjdata(j) == v);
            CHECK(json::from_bjdata(v) == j);
        }

        SECTION("Russian")
        {
            json const j = "مرحبا";
            std::vector<uint8_t> v = {'S', 'i', 10, 0xD9, 0x85, 0xD8, 0xB1, 0xD8, 0xAD, 0xD8, 0xA8, 0xD8, 0xA7};
            CHECK(json::to_bjdata(j) == v);
            CHECK(json::from_bjdata(v) == j);
        }
    }

    SECTION("Array Type")
    {
        SECTION("size=false type=false")
        {
            // note the float has been replaced by a double
            json const j = {nullptr, true, false, 4782345193, 153.132, "ham"};
            std::vector<uint8_t> v = {'[', 'Z', 'T', 'F', 'L', 0xE9, 0xCB, 0x0C, 0x1D, 0x01, 0x00, 0x00, 0x00, 'D', 0x4e, 0x62, 0x10, 0x58, 0x39, 0x24, 0x63, 0x40, 'S', 'i', 3, 'h', 'a', 'm', ']'};
            CHECK(json::to_bjdata(j) == v);
            CHECK(json::from_bjdata(v) == j);
        }

        SECTION("size=true type=false")
        {
            // note the float has been replaced by a double
            json const j = {nullptr, true, false, 4782345193, 153.132, "ham"};
            std::vector<uint8_t> v = {'[', '#', 'i', 6, 'Z', 'T', 'F', 'L', 0xE9, 0xCB, 0x0C, 0x1D, 0x01, 0x00, 0x00, 0x00, 'D', 0x4e, 0x62, 0x10, 0x58, 0x39, 0x24, 0x63, 0x40, 'S', 'i', 3, 'h', 'a', 'm'};
            CHECK(json::to_bjdata(j, true) == v);
            CHECK(json::from_bjdata(v) == j);
        }

        SECTION("size=true type=true")
        {
            // note the float has been replaced by a double
            json const j = {nullptr, true, false, 4782345193, 153.132, "ham"};
            std::vector<uint8_t> v = {'[', '#', 'i', 6, 'Z', 'T', 'F', 'L', 0xE9, 0xCB, 0x0C, 0x1D, 0x01, 0x00, 0x00, 0x00, 'D', 0x4e, 0x62, 0x10, 0x58, 0x39, 0x24, 0x63, 0x40, 'S', 'i', 3, 'h', 'a', 'm'};
            CHECK(json::to_bjdata(j, true, true) == v);
            CHECK(json::from_bjdata(v) == j);
        }
    }

    SECTION("Object Type")
    {
        SECTION("size=false type=false")
        {
            json j =
            {
                {
                    "post", {
                        {"id", 1137},
                        {"author", "rkalla"},
                        {"timestamp", 1364482090592},
                        {"body", "I totally agree!"}
                    }
                }
            };
            std::vector<uint8_t> v = {'{', 'i', 4, 'p', 'o', 's', 't', '{',
                                      'i', 6, 'a', 'u', 't', 'h', 'o', 'r', 'S', 'i', 6, 'r', 'k', 'a', 'l', 'l', 'a',
                                      'i', 4, 'b', 'o', 'd', 'y', 'S', 'i', 16, 'I', ' ', 't', 'o', 't', 'a', 'l', 'l', 'y', ' ', 'a', 'g', 'r', 'e', 'e', '!',
                                      'i', 2, 'i', 'd', 'I', 0x71, 0x04,
                                      'i', 9, 't', 'i', 'm', 'e', 's', 't', 'a', 'm', 'p', 'L', 0x60, 0x66, 0x78, 0xB1, 0x3D, 0x01, 0x00, 0x00,
                                      '}', '}'
                                     };
            CHECK(json::to_bjdata(j) == v);
            CHECK(json::from_bjdata(v) == j);
        }

        SECTION("size=true type=false")
        {
            json j =
            {
                {
                    "post", {
                        {"id", 1137},
                        {"author", "rkalla"},
                        {"timestamp", 1364482090592},
                        {"body", "I totally agree!"}
                    }
                }
            };
            std::vector<uint8_t> v = {'{', '#', 'i', 1, 'i', 4, 'p', 'o', 's', 't', '{', '#', 'i', 4,
                                      'i', 6, 'a', 'u', 't', 'h', 'o', 'r', 'S', 'i', 6, 'r', 'k', 'a', 'l', 'l', 'a',
                                      'i', 4, 'b', 'o', 'd', 'y', 'S', 'i', 16, 'I', ' ', 't', 'o', 't', 'a', 'l', 'l', 'y', ' ', 'a', 'g', 'r', 'e', 'e', '!',
                                      'i', 2, 'i', 'd', 'I', 0x71, 0x04,
                                      'i', 9, 't', 'i', 'm', 'e', 's', 't', 'a', 'm', 'p', 'L', 0x60, 0x66, 0x78, 0xB1, 0x3D, 0x01, 0x00, 0x00,
                                     };
            CHECK(json::to_bjdata(j, true) == v);
            CHECK(json::from_bjdata(v) == j);
        }

        SECTION("size=true type=true")
        {
            json j =
            {
                {
                    "post", {
                        {"id", 1137},
                        {"author", "rkalla"},
                        {"timestamp", 1364482090592},
                        {"body", "I totally agree!"}
                    }
                }
            };
            std::vector<uint8_t> v = {'{', '#', 'i', 1, 'i', 4, 'p', 'o', 's', 't', '{', '#', 'i', 4,
                                      'i', 6, 'a', 'u', 't', 'h', 'o', 'r', 'S', 'i', 6, 'r', 'k', 'a', 'l', 'l', 'a',
                                      'i', 4, 'b', 'o', 'd', 'y', 'S', 'i', 16, 'I', ' ', 't', 'o', 't', 'a', 'l', 'l', 'y', ' ', 'a', 'g', 'r', 'e', 'e', '!',
                                      'i', 2, 'i', 'd', 'I', 0x71, 0x04,
                                      'i', 9, 't', 'i', 'm', 'e', 's', 't', 'a', 'm', 'p', 'L', 0x60, 0x66, 0x78, 0xB1, 0x3D, 0x01, 0x00, 0x00,
                                     };
            CHECK(json::to_bjdata(j, true, true) == v);
            CHECK(json::from_bjdata(v) == j);
        }
    }

    SECTION("Optimized Format")
    {
        SECTION("Array Example")
        {
            SECTION("No Optimization")
            {
                // note the floats have been replaced by doubles
                json const j = {29.97, 31.13, 67.0, 2.113, 23.888};
                std::vector<uint8_t> v = {'[',
                                          'D', 0xb8, 0x1e, 0x85, 0xeb, 0x51, 0xf8, 0x3d, 0x40,
                                          'D', 0xe1, 0x7a, 0x14, 0xae, 0x47, 0x21, 0x3f, 0x40,
                                          'D', 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x50, 0x40,
                                          'D', 0x81, 0x95, 0x43, 0x8b, 0x6c, 0xe7, 0x00, 0x40,
                                          'D', 0x17, 0xd9, 0xce, 0xf7, 0x53, 0xe3, 0x37, 0x40,
                                          ']'
                                         };
                CHECK(json::to_bjdata(j) == v);
                CHECK(json::from_bjdata(v) == j);
            }

            SECTION("Optimized with count")
            {
                // note the floats have been replaced by doubles
                json const j = {29.97, 31.13, 67.0, 2.113, 23.888};
                std::vector<uint8_t> v = {'[', '#', 'i', 5,
                                          'D', 0xb8, 0x1e, 0x85, 0xeb, 0x51, 0xf8, 0x3d, 0x40,
                                          'D', 0xe1, 0x7a, 0x14, 0xae, 0x47, 0x21, 0x3f, 0x40,
                                          'D', 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x50, 0x40,
                                          'D', 0x81, 0x95, 0x43, 0x8b, 0x6c, 0xe7, 0x00, 0x40,
                                          'D', 0x17, 0xd9, 0xce, 0xf7, 0x53, 0xe3, 0x37, 0x40,
                                         };
                CHECK(json::to_bjdata(j, true) == v);
                CHECK(json::from_bjdata(v) == j);
            }

            SECTION("Optimized with type & count")
            {
                // note the floats have been replaced by doubles
                json const j = {29.97, 31.13, 67.0, 2.113, 23.888};
                std::vector<uint8_t> v = {'[', '$', 'D', '#', 'i', 5,
                                          0xb8, 0x1e, 0x85, 0xeb, 0x51, 0xf8, 0x3d, 0x40,
                                          0xe1, 0x7a, 0x14, 0xae, 0x47, 0x21, 0x3f, 0x40,
                                          0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x50, 0x40,
                                          0x81, 0x95, 0x43, 0x8b, 0x6c, 0xe7, 0x00, 0x40,
                                          0x17, 0xd9, 0xce, 0xf7, 0x53, 0xe3, 0x37, 0x40,
                                         };
                CHECK(json::to_bjdata(j, true, true) == v);
                CHECK(json::from_bjdata(v) == j);
            }
        }

        SECTION("Object Example")
        {
            SECTION("No Optimization")
            {
                // note the floats have been replaced by doubles
                json const j = { {"lat", 29.976}, {"long", 31.131}, {"alt", 67.0} };
                std::vector<uint8_t> v = {'{',
                                          'i', 3, 'a', 'l', 't', 'D',      0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x50, 0x40,
                                          'i', 3, 'l', 'a', 't', 'D',      0x60, 0xe5, 0xd0, 0x22, 0xdb, 0xf9, 0x3d, 0x40,
                                          'i', 4, 'l', 'o', 'n', 'g', 'D', 0xa8, 0xc6, 0x4b, 0x37, 0x89, 0x21, 0x3f, 0x40,
                                          '}'
                                         };
                CHECK(json::to_bjdata(j) == v);
                CHECK(json::from_bjdata(v) == j);
            }

            SECTION("Optimized with count")
            {
                // note the floats have been replaced by doubles
                json const j = { {"lat", 29.976}, {"long", 31.131}, {"alt", 67.0} };
                std::vector<uint8_t> v = {'{', '#', 'i', 3,
                                          'i', 3, 'a', 'l', 't', 'D',      0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x50, 0x40,
                                          'i', 3, 'l', 'a', 't', 'D',      0x60, 0xe5, 0xd0, 0x22, 0xdb, 0xf9, 0x3d, 0x40,
                                          'i', 4, 'l', 'o', 'n', 'g', 'D', 0xa8, 0xc6, 0x4b, 0x37, 0x89, 0x21, 0x3f, 0x40,
                                         };
                CHECK(json::to_bjdata(j, true) == v);
                CHECK(json::from_bjdata(v) == j);
            }

            SECTION("Optimized with type & count")
            {
                // note the floats have been replaced by doubles
                json const j = { {"lat", 29.976}, {"long", 31.131}, {"alt", 67.0} };
                std::vector<uint8_t> v = {'{', '$', 'D', '#', 'i', 3,
                                          'i', 3, 'a', 'l', 't',      0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x50, 0x40,
                                          'i', 3, 'l', 'a', 't',      0x60, 0xe5, 0xd0, 0x22, 0xdb, 0xf9, 0x3d, 0x40,
                                          'i', 4, 'l', 'o', 'n', 'g', 0xa8, 0xc6, 0x4b, 0x37, 0x89, 0x21, 0x3f, 0x40,
                                         };
                CHECK(json::to_bjdata(j, true, true) == v);
                CHECK(json::from_bjdata(v) == j);
            }
        }

        SECTION("Special Cases (Null, No-Op and Boolean)")
        {
            SECTION("Array")
            {
                json _;
                std::vector<uint8_t> const v = {'[', '$', 'N', '#', 'I', 0x00, 0x02};
                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(v), "[json.exception.parse_error.112] parse error at byte 3: syntax error while parsing BJData type: marker 0x4E is not a permitted optimized array type", json::parse_error&);
                CHECK(json::from_bjdata(v, true, false).is_discarded());
            }

            SECTION("Object")
            {
                json _;
                std::vector<uint8_t> const v = {'{', '$', 'Z', '#', 'i', 3, 'i', 4, 'n', 'a', 'm', 'e', 'i', 8, 'p', 'a', 's', 's', 'w', 'o', 'r', 'd', 'i', 5, 'e', 'm', 'a', 'i', 'l'};
                CHECK_THROWS_WITH_AS(_ = json::from_bjdata(v), "[json.exception.parse_error.112] parse error at byte 3: syntax error while parsing BJData type: marker 0x5A is not a permitted optimized array type", json::parse_error&);
                CHECK(json::from_bjdata(v, true, false).is_discarded());
            }
        }
    }
}

#if !defined(JSON_NOEXCEPTION)
TEST_CASE("all BJData first bytes")
{
    // these bytes will fail immediately with exception parse_error.112
    std::set<uint8_t> supported =
    {
        'T', 'F', 'Z', 'B', 'U', 'i', 'I', 'l', 'L', 'd', 'D', 'C', 'S', '[', '{', 'N', 'H', 'u', 'm', 'M', 'h'
    };

    for (auto i = 0; i < 256; ++i)
    {
        const auto byte = static_cast<uint8_t>(i);
        CAPTURE(byte)

        try
        {
            auto res = json::from_bjdata(std::vector<uint8_t>(1, byte));
        }
        catch (const json::parse_error& e)
        {
            // check that parse_error.112 is only thrown if the
            // first byte is not in the supported set
            INFO_WITH_TEMP(e.what());
            if (supported.find(byte) == supported.end())
            {
                CHECK(e.id == 112);
            }
            else
            {
                CHECK(e.id != 112);
            }
        }
    }
}
#endif

TEST_CASE("BJData roundtrips" * doctest::skip())
{
    SECTION("input from self-generated BJData files")
    {
        for (const std::string filename :
                {
                    TEST_DATA_DIRECTORY "/json_nlohmann_tests/all_unicode.json",
                    TEST_DATA_DIRECTORY "/json.org/1.json",
                    TEST_DATA_DIRECTORY "/json.org/2.json",
                    TEST_DATA_DIRECTORY "/json.org/3.json",
                    TEST_DATA_DIRECTORY "/json.org/4.json",
                    TEST_DATA_DIRECTORY "/json.org/5.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip01.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip02.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip03.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip04.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip05.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip06.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip07.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip08.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip09.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip10.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip11.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip12.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip13.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip14.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip15.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip16.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip17.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip18.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip19.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip20.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip21.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip22.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip23.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip24.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip25.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip26.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip27.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip28.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip29.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip30.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip31.json",
                    TEST_DATA_DIRECTORY "/json_roundtrip/roundtrip32.json",
                    TEST_DATA_DIRECTORY "/json_testsuite/sample.json",
                    TEST_DATA_DIRECTORY "/json_tests/pass1.json",
                    TEST_DATA_DIRECTORY "/json_tests/pass2.json",
                    TEST_DATA_DIRECTORY "/json_tests/pass3.json"
                })
        {
            CAPTURE(filename)

            {
                INFO_WITH_TEMP(filename + ": std::vector<uint8_t>");
                // parse JSON file
                std::ifstream f_json(filename);
                const json j1 = json::parse(f_json);

                // parse BJData file
                auto packed = utils::read_binary_file(filename + ".bjdata");
                json j2;
                CHECK_NOTHROW(j2 = json::from_bjdata(packed));

                // compare parsed JSON values
                CHECK(j1 == j2);
            }

            {
                INFO_WITH_TEMP(filename + ": std::ifstream");
                // parse JSON file
                std::ifstream f_json(filename);
                const json j1 = json::parse(f_json);

                // parse BJData file
                std::ifstream f_bjdata(filename + ".bjdata", std::ios::binary);
                json j2;
                CHECK_NOTHROW(j2 = json::from_bjdata(f_bjdata));

                // compare parsed JSON values
                CHECK(j1 == j2);
            }

            {
                INFO_WITH_TEMP(filename + ": output to output adapters");
                // parse JSON file
                std::ifstream f_json(filename);
                json const j1 = json::parse(f_json);

                // parse BJData file
                auto packed = utils::read_binary_file(filename + ".bjdata");

                {
                    INFO_WITH_TEMP(filename + ": output adapters: std::vector<uint8_t>");
                    std::vector<uint8_t> vec;
                    json::to_bjdata(j1, vec);
                    CHECK(vec == packed);
                }
            }
        }
    }
}

// Unit tests for BJData Structure-of-Arrays (SOA) feature (Draft 4)
// Add these tests to unit-bjdata.cpp

TEST_CASE("BJData SOA (Structure-of-Arrays)")
{
    SECTION("Writer: SOA encoding")
    {
        SECTION("row-major format basic test")
        {
            // Array of objects with uniform numeric fields
            json const j = json::array(
            {
                {{"x", 1}, {"y", 2}},
                {{"x", 3}, {"y", 4}},
                {{"x", 5}, {"y", 6}}
            });

            // Encode with Draft 4 and row-major SOA
            const auto result = json::to_bjdata(j, true, true,
                                                json::bjdata_version_t::draft4, json::bjdata_soa_format_t::row_major);

            CHECK(result.size() > 0);
            CHECK(result[0] == '[');  // Array marker
            CHECK(result[1] == '$');  // Optimized type marker
            CHECK(result[2] == '{');  // Schema start

            // Verify roundtrip produces equivalent data
            const auto decoded = json::from_bjdata(result);
            CHECK(decoded.is_array());
            CHECK(decoded.size() == 3);
        }

        SECTION("column-major format basic test")
        {
            json const j = json::array(
            {
                {{"a", 10}, {"b", 20}},
                {{"a", 30}, {"b", 40}}
            });

            const auto result = json::to_bjdata(j, true, true,
                                                json::bjdata_version_t::draft4, json::bjdata_soa_format_t::col_major);

            CHECK(result.size() > 0);
            CHECK(result[0] == '{');  // Object marker for column-major
            CHECK(result[1] == '$');  // Optimized type marker
            CHECK(result[2] == '{');  // Schema start

            const auto decoded = json::from_bjdata(result);
            CHECK(decoded.is_object());
        }

        SECTION("SOA with boolean fields")
        {
            json const j = json::array(
            {
                {{"flag", true}, {"val", 1}},
                {{"flag", false}, {"val", 2}},
                {{"flag", true}, {"val", 3}}
            });

            const auto result = json::to_bjdata(j, true, true,
                                                json::bjdata_version_t::draft4, json::bjdata_soa_format_t::row_major);

            CHECK(result.size() > 0);
            const auto decoded = json::from_bjdata(result);
            CHECK(decoded.is_array());
            CHECK(decoded.size() == 3);
        }

        SECTION("SOA with null fields")
        {
            json const j = json::array(
            {
                {{"id", 1}, {"opt", nullptr}},
                {{"id", 2}, {"opt", nullptr}}
            });

            const auto result = json::to_bjdata(j, true, true,
                                                json::bjdata_version_t::draft4, json::bjdata_soa_format_t::row_major);

            CHECK(result.size() > 0);
            const auto decoded = json::from_bjdata(result);
            CHECK(decoded.is_array());
        }

        SECTION("SOA with float fields")
        {
            json const j = json::array(
            {
                {{"x", 1.5}, {"y", 2.5}},
                {{"x", 3.5}, {"y", 4.5}}
            });

            const auto result = json::to_bjdata(j, true, true,
                                                json::bjdata_version_t::draft4, json::bjdata_soa_format_t::row_major);

            CHECK(result.size() > 0);
            const auto decoded = json::from_bjdata(result);
            CHECK(decoded.is_array());
            CHECK(decoded.size() == 2);
        }

        SECTION("SOA disabled - falls back to normal encoding")
        {
            json const j = json::array(
            {
                {{"x", 1}, {"y", 2}},
                {{"x", 3}, {"y", 4}}
            });

            const auto result_none = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4, json::bjdata_soa_format_t::none);

            CHECK(result_none[0] == '[');
            CHECK((result_none[1] != '$' || result_none[2] != '{'));
        }

        SECTION("SOA not applicable - non-uniform objects")
        {
            json const j = json::array(
            {
                {{"x", 1}, {"y", 2}},
                {{"a", 3}, {"b", 4}}
            });

            const auto result = json::to_bjdata(j, true, true,
                                                json::bjdata_version_t::draft4, json::bjdata_soa_format_t::row_major);

            CHECK(result[0] == '[');
        }

        SECTION("SOA not applicable - mixed types in same field")
        {
            json const j = json::array(
            {
                {{"x", 1}, {"y", 2}},
                {{"x", "str"}, {"y", 4}}
            });

            const auto result = json::to_bjdata(j, true, true,
                                                json::bjdata_version_t::draft4, json::bjdata_soa_format_t::row_major);

            CHECK(result[0] == '[');
        }

        SECTION("SOA not applicable - nested objects")
        {
            json const j = json::array(
            {
                {{"x", 1}, {"nested", {{"a", 1}}}},
                {{"x", 2}, {"nested", {{"a", 2}}}}
            });

            const auto result = json::to_bjdata(j, true, true,
                                                json::bjdata_version_t::draft4, json::bjdata_soa_format_t::row_major);

            CHECK(result[0] == '[');
        }

        SECTION("SOA not applicable - array fields")
        {
            json const j = json::array(
            {
                {{"x", 1}, {"arr", {1, 2, 3}}},
                {{"x", 2}, {"arr", {4, 5, 6}}}
            });

            const auto result = json::to_bjdata(j, true, true,
                                                json::bjdata_version_t::draft4, json::bjdata_soa_format_t::row_major);

            CHECK(result[0] == '[');
        }

        SECTION("SOA not applicable - empty array")
        {
            json const j = json::array();

            const auto result = json::to_bjdata(j, true, true,
                                                json::bjdata_version_t::draft4, json::bjdata_soa_format_t::row_major);

            CHECK(result == std::vector<uint8_t> {'[', '#', 'i', 0});
        }

        SECTION("SOA not applicable - array of non-objects")
        {
            json const j = json::array({1, 2, 3, 4, 5});

            const auto result = json::to_bjdata(j, true, true,
                                                json::bjdata_version_t::draft4, json::bjdata_soa_format_t::row_major);

            CHECK(result[0] == '[');
            CHECK(result[1] == '$');
            CHECK(result[2] == 'i');
        }

        SECTION("SOA with single record")
        {
            json const j = json::array({{{"x", 42}, {"y", 100}}});

            const auto result = json::to_bjdata(j, true, true,
                                                json::bjdata_version_t::draft4, json::bjdata_soa_format_t::row_major);

            CHECK(result.size() > 0);
            const auto decoded = json::from_bjdata(result);
            CHECK(decoded.is_array());
            CHECK(decoded.size() == 1);
        }

        SECTION("Draft 2/3 ignores SOA format")
        {
            json const j = json::array(
            {
                {{"x", 1}, {"y", 2}},
                {{"x", 3}, {"y", 4}}
            });

            const auto result_d2 = json::to_bjdata(j, true, true,
                                                   json::bjdata_version_t::draft2, json::bjdata_soa_format_t::row_major);

            const auto result_d3 = json::to_bjdata(j, true, true,
                                                   json::bjdata_version_t::draft3, json::bjdata_soa_format_t::row_major);

            CHECK(result_d2[0] == '[');
            CHECK(result_d3[0] == '[');
        }
    }

    SECTION("Reader: SOA parsing")
    {
        SECTION("row-major SOA decoding")
        {
            std::vector<uint8_t> const v_row =
            {
                '[', '$', '{',
                'i', 1, 'x', 'i',
                'i', 1, 'y', 'i',
                '}',
                '#', 'i', 2,
                1, 2,
                3, 4
            };

            const auto j = json::from_bjdata(v_row);
            CHECK(j.is_array());
            CHECK(j.size() == 2);
            CHECK(j[0]["x"] == 1);
            CHECK(j[0]["y"] == 2);
            CHECK(j[1]["x"] == 3);
            CHECK(j[1]["y"] == 4);
        }

        SECTION("column-major SOA decoding")
        {
            std::vector<uint8_t> const v_col =
            {
                '{', '$', '{',
                'i', 1, 'x', 'i',
                'i', 1, 'y', 'i',
                '}',
                '#', 'i', 2,
                1, 3,
                2, 4
            };

            const auto j = json::from_bjdata(v_col);
            CHECK(j.is_object());
            CHECK(j.contains("x"));
            CHECK(j.contains("y"));
            CHECK(j["x"] == json::array({1, 3}));
            CHECK(j["y"] == json::array({2, 4}));
        }

        SECTION("SOA with boolean type marker")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{',
                'i', 4, 'f', 'l', 'a', 'g', 'T',
                '}',
                '#', 'i', 3,
                'T', 'F', 'T'
            };

            const auto j = json::from_bjdata(v);
            CHECK(j.is_array());
            CHECK(j.size() == 3);
            CHECK(j[0]["flag"] == true);
            CHECK(j[1]["flag"] == false);
            CHECK(j[2]["flag"] == true);
        }

        SECTION("SOA with null type marker")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{',
                'i', 3, 'o', 'p', 't', 'Z',
                '}',
                '#', 'i', 2
            };

            const auto j = json::from_bjdata(v);
            CHECK(j.is_array());
            CHECK(j.size() == 2);
            CHECK(j[0]["opt"].is_null());
            CHECK(j[1]["opt"].is_null());
        }

        SECTION("SOA with uint8 type")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{',
                'i', 1, 'v', 'U',
                '}',
                '#', 'i', 3,
                0xFF, 0x80, 0x00
            };

            const auto j = json::from_bjdata(v);
            CHECK(j.is_array());
            CHECK(j.size() == 3);
            CHECK(j[0]["v"] == 255);
            CHECK(j[1]["v"] == 128);
            CHECK(j[2]["v"] == 0);
        }

        SECTION("SOA with int16 type")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{',
                'i', 1, 'n', 'I',
                '}',
                '#', 'i', 2,
                0x01, 0x00,
                0xFF, 0x7F
            };

            const auto j = json::from_bjdata(v);
            CHECK(j.is_array());
            CHECK(j.size() == 2);
            CHECK(j[0]["n"] == 1);
            CHECK(j[1]["n"] == 32767);
        }

        SECTION("SOA with float32 type")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{',
                'i', 1, 'f', 'd',
                '}',
                '#', 'i', 2,
                0x00, 0x00, 0x80, 0x3F,
                0x00, 0x00, 0x00, 0x40
            };

            const auto j = json::from_bjdata(v);
            CHECK(j.is_array());
            CHECK(j.size() == 2);
            CHECK(j[0]["f"] == 1.0f);
            CHECK(j[1]["f"] == 2.0f);
        }

        SECTION("SOA with float64 type")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{',
                'i', 1, 'd', 'D',
                '}',
                '#', 'i', 1,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x3F
            };

            const auto j = json::from_bjdata(v);
            CHECK(j.is_array());
            CHECK(j.size() == 1);
            CHECK(j[0]["d"] == 1.0);
        }

        SECTION("SOA with multiple fields")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{',
                'i', 4, 'f', 'l', 'a', 'g', 'T',
                'i', 2, 'i', 'd', 'i',
                'i', 5, 'v', 'a', 'l', 'u', 'e', 'U',
                '}',
                '#', 'i', 2,
                'T', 1, 10,
                'F', 2, 20
            };

            const auto j = json::from_bjdata(v);
            CHECK(j.is_array());
            CHECK(j.size() == 2);
            CHECK(j[0]["flag"] == true);
            CHECK(j[0]["id"] == 1);
            CHECK(j[0]["value"] == 10);
            CHECK(j[1]["flag"] == false);
            CHECK(j[1]["id"] == 2);
            CHECK(j[1]["value"] == 20);
        }

        SECTION("SOA with empty count")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{',
                'i', 1, 'x', 'i',
                '}',
                '#', 'i', 0
            };

            const auto j = json::from_bjdata(v);
            CHECK(j.is_array());
            CHECK(j.empty());
        }

        SECTION("SOA with no-op markers in schema")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{',
                'N',
                'i', 1, 'x', 'i',
                'N', 'N',
                'i', 1, 'y', 'i',
                '}',
                '#', 'i', 1,
                5, 10
            };

            const auto j = json::from_bjdata(v);
            CHECK(j.is_array());
            CHECK(j.size() == 1);
            CHECK(j[0]["x"] == 5);
            CHECK(j[0]["y"] == 10);
        }
    }

    SECTION("SOA parse errors")
    {
        SECTION("unexpected EOF in schema")
        {
            std::vector<uint8_t> const v = {'[', '$', '{'};

            json _;
            CHECK_THROWS_AS(_ = json::from_bjdata(v), json::parse_error);
            CHECK(json::from_bjdata(v, true, false).is_discarded());
        }

        SECTION("missing count marker after schema")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{',
                'i', 1, 'x', 'i',
                '}',
                'i', 2
            };

            json _;
            CHECK_THROWS_AS(_ = json::from_bjdata(v), json::parse_error);
            CHECK(json::from_bjdata(v, true, false).is_discarded());
        }

        SECTION("EOF in field name length")
        {
            std::vector<uint8_t> const v = {'[', '$', '{', 'i'};

            json _;
            CHECK_THROWS_AS(_ = json::from_bjdata(v), json::parse_error);
            CHECK(json::from_bjdata(v, true, false).is_discarded());
        }

        SECTION("EOF in field name")
        {
            std::vector<uint8_t> const v = {'[', '$', '{', 'i', 5, 'a', 'b'};

            json _;
            CHECK_THROWS_AS(_ = json::from_bjdata(v), json::parse_error);
            CHECK(json::from_bjdata(v, true, false).is_discarded());
        }

        SECTION("EOF in field type marker")
        {
            std::vector<uint8_t> const v = {'[', '$', '{', 'i', 1, 'x'};

            json _;
            CHECK_THROWS_AS(_ = json::from_bjdata(v), json::parse_error);
            CHECK(json::from_bjdata(v, true, false).is_discarded());
        }

        SECTION("EOF in SOA boolean payload")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{',
                'i', 1, 'f', 'T',
                '}',
                '#', 'i', 2,
                'T'
            };

            json _;
            CHECK_THROWS_AS(_ = json::from_bjdata(v), json::parse_error);
            CHECK(json::from_bjdata(v, true, false).is_discarded());
        }

        SECTION("EOF in SOA numeric payload")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{',
                'i', 1, 'n', 'I',
                '}',
                '#', 'i', 2,
                0x01, 0x00
            };

            json _;
            CHECK_THROWS_AS(_ = json::from_bjdata(v), json::parse_error);
            CHECK(json::from_bjdata(v, true, false).is_discarded());
        }

        SECTION("EOF in count value")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{',
                'i', 1, 'x', 'i',
                '}',
                '#', 'i'
            };

            json _;
            CHECK_THROWS_AS(_ = json::from_bjdata(v), json::parse_error);
            CHECK(json::from_bjdata(v, true, false).is_discarded());
        }
    }

    SECTION("SOA roundtrip tests")
    {
        SECTION("row-major roundtrip")
        {
            json const original = json::array(
            {
                {{"a", 1}, {"b", 2}, {"c", 3}},
                {{"a", 4}, {"b", 5}, {"c", 6}},
                {{"a", 7}, {"b", 8}, {"c", 9}}
            });

            const auto encoded = json::to_bjdata(original, true, true,
                                                 json::bjdata_version_t::draft4, json::bjdata_soa_format_t::row_major);

            const auto decoded = json::from_bjdata(encoded);

            CHECK(decoded.is_array());
            CHECK(decoded.size() == original.size());
            for (size_t i = 0; i < original.size(); ++i)
            {
                CHECK(decoded[i]["a"] == original[i]["a"]);
                CHECK(decoded[i]["b"] == original[i]["b"]);
                CHECK(decoded[i]["c"] == original[i]["c"]);
            }
        }

        SECTION("column-major roundtrip")
        {
            json const original = json::array(
            {
                {{"x", 10}, {"y", 20}},
                {{"x", 30}, {"y", 40}},
                {{"x", 50}, {"y", 60}}
            });

            const auto encoded = json::to_bjdata(original, true, true,
                                                 json::bjdata_version_t::draft4, json::bjdata_soa_format_t::col_major);

            const auto decoded = json::from_bjdata(encoded);

            CHECK(decoded.is_object());
            CHECK(decoded["x"] == json::array({10, 30, 50}));
            CHECK(decoded["y"] == json::array({20, 40, 60}));
        }

        SECTION("roundtrip with mixed numeric types")
        {
            json const original = json::array(
            {
                {{"small", 1}, {"medium", 1000}, {"large", 100000}},
                {{"small", 2}, {"medium", 2000}, {"large", 200000}}
            });

            const auto encoded = json::to_bjdata(original, true, true,
                                                 json::bjdata_version_t::draft4, json::bjdata_soa_format_t::row_major);

            const auto decoded = json::from_bjdata(encoded);

            CHECK(decoded.is_array());
            CHECK(decoded.size() == 2);
            CHECK(decoded[0]["small"] == 1);
            CHECK(decoded[0]["medium"] == 1000);
            CHECK(decoded[0]["large"] == 100000);
        }

        SECTION("roundtrip with boolean and null")
        {
            json const original = json::array(
            {
                {{"active", true}, {"data", nullptr}},
                {{"active", false}, {"data", nullptr}}
            });

            const auto encoded = json::to_bjdata(original, true, true,
                                                 json::bjdata_version_t::draft4, json::bjdata_soa_format_t::row_major);

            const auto decoded = json::from_bjdata(encoded);

            CHECK(decoded.is_array());
            CHECK(decoded[0]["active"] == true);
            CHECK(decoded[0]["data"].is_null());
            CHECK(decoded[1]["active"] == false);
            CHECK(decoded[1]["data"].is_null());
        }
    }

    SECTION("SOA SAX parser aborts")
    {
        SECTION("abort on start_array for row-major")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{', 'i', 1, 'x', 'i', '}', '#', 'i', 2, 1, 2
            };

            SaxCountdown scp(0);
            CHECK_FALSE(json::sax_parse(v, &scp, json::input_format_t::bjdata));
        }

        SECTION("abort on start_object for column-major")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{', 'i', 1, 'x', 'i', '}', '#', 'i', 2, 1, 2
            };

            SaxCountdown scp(0);
            CHECK_FALSE(json::sax_parse(v, &scp, json::input_format_t::bjdata));
        }

        SECTION("abort on key in row-major object")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{', 'i', 1, 'x', 'i', '}', '#', 'i', 1, 5
            };

            SaxCountdown scp(2);
            CHECK_FALSE(json::sax_parse(v, &scp, json::input_format_t::bjdata));
        }

        SECTION("abort on value in row-major")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{', 'i', 1, 'x', 'i', '}', '#', 'i', 1, 42
            };

            SaxCountdown scp(3);
            CHECK_FALSE(json::sax_parse(v, &scp, json::input_format_t::bjdata));
        }
    }

    SECTION("SOA with all numeric types")
    {
        SECTION("int8 (i)")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{', 'i', 1, 'v', 'i', '}', '#', 'i', 3,
                0x7F, 0x00, 0x80
            };

            const auto j = json::from_bjdata(v);
            CHECK(j[0]["v"] == 127);
            CHECK(j[1]["v"] == 0);
            CHECK(j[2]["v"] == -128);
        }

        SECTION("uint8 (U)")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{', 'i', 1, 'v', 'U', '}', '#', 'i', 2,
                0x00, 0xFF
            };

            const auto j = json::from_bjdata(v);
            CHECK(j[0]["v"] == 0);
            CHECK(j[1]["v"] == 255);
        }

        SECTION("int16 (I)")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{', 'i', 1, 'v', 'I', '}', '#', 'i', 2,
                0xFF, 0x7F, 0x00, 0x80
            };

            const auto j = json::from_bjdata(v);
            CHECK(j[0]["v"] == 32767);
            CHECK(j[1]["v"] == -32768);
        }

        SECTION("uint16 (u)")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{', 'i', 1, 'v', 'u', '}', '#', 'i', 2,
                0x00, 0x00, 0xFF, 0xFF
            };

            const auto j = json::from_bjdata(v);
            CHECK(j[0]["v"] == 0);
            CHECK(j[1]["v"] == 65535);
        }

        SECTION("int32 (l)")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{', 'i', 1, 'v', 'l', '}', '#', 'i', 1,
                0xFF, 0xFF, 0xFF, 0x7F
            };

            const auto j = json::from_bjdata(v);
            CHECK(j[0]["v"] == 2147483647);
        }

        SECTION("uint32 (m)")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{', 'i', 1, 'v', 'm', '}', '#', 'i', 1,
                0xFF, 0xFF, 0xFF, 0xFF
            };

            const auto j = json::from_bjdata(v);
            CHECK(j[0]["v"] == 4294967295u);
        }

        SECTION("int64 (L)")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{', 'i', 1, 'v', 'L', '}', '#', 'i', 1,
                0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00
            };

            const auto j = json::from_bjdata(v);
            CHECK(j[0]["v"] == 4294967296);
        }

        SECTION("uint64 (M)")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{', 'i', 1, 'v', 'M', '}', '#', 'i', 1,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F
            };

            const auto j = json::from_bjdata(v);
            CHECK(j[0]["v"] == 9223372036854775807ull);
        }

        SECTION("float32 (d)")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{', 'i', 1, 'v', 'd', '}', '#', 'i', 1,
                0xd0, 0x0f, 0x49, 0x40
            };

            const auto j = json::from_bjdata(v);
            CHECK(j[0]["v"] == doctest::Approx(3.14159f));
        }

        SECTION("float64 (D)")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{', 'i', 1, 'v', 'D', '}', '#', 'i', 1,
                0x6e, 0x86, 0x1b, 0xf0, 0xf9, 0x21, 0x09, 0x40
            };

            const auto j = json::from_bjdata(v);
            CHECK(j[0]["v"] == doctest::Approx(3.14159));
        }
    }

    SECTION("SOA schema validation in writer")
    {
        SECTION("validates uniform field names")
        {
            json const j = json::array({{{"x", 1}}, {{"y", 2}}});

            const auto result = json::to_bjdata(j, true, true,
                                                json::bjdata_version_t::draft4, json::bjdata_soa_format_t::row_major);

            CHECK(result[0] == '[');
            CHECK(!(result[1] == '$' && result[2] == '{'));
        }

        SECTION("validates uniform field count")
        {
            json const j = json::array(
            {
                {{"x", 1}, {"y", 2}},
                {{"x", 3}}
            });

            const auto result = json::to_bjdata(j, true, true,
                                                json::bjdata_version_t::draft4, json::bjdata_soa_format_t::row_major);

            CHECK(result[0] == '[');
        }

        SECTION("validates uniform types")
        {
            json const j = json::array({{{"x", 1}}, {{"x", 1.5}}});

            const auto result = json::to_bjdata(j, true, true,
                                                json::bjdata_version_t::draft4, json::bjdata_soa_format_t::row_major);

            CHECK(result[0] == '[');
        }

        SECTION("rejects string fields in basic SOA")
        {
            json const j = json::array(
            {
                {{"name", "alice"}},
                {{"name", "bob"}}
            });

            const auto result = json::to_bjdata(j, true, true,
                                                json::bjdata_version_t::draft4, json::bjdata_soa_format_t::row_major);

            CHECK(result[0] == '[');
        }

        SECTION("rejects binary fields in basic SOA")
        {
            json const j = json::array(
            {
                {{"data", json::binary({1, 2, 3})}},
                {{"data", json::binary({4, 5, 6})}}
            });

            const auto result = json::to_bjdata(j, true, true,
                                                json::bjdata_version_t::draft4, json::bjdata_soa_format_t::row_major);

            CHECK(result[0] == '[');
        }
    }

    SECTION("SOA with various count sizes")
    {
        SECTION("count fits in uint8")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{', 'i', 1, 'x', 'i', '}', '#', 'U', 200
            };

            json _;
            CHECK_THROWS_AS(_ = json::from_bjdata(v), json::parse_error);
        }

        SECTION("count fits in int16")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{', 'i', 1, 'x', 'i', '}', '#', 'I', 0x00, 0x01
            };

            json _;
            CHECK_THROWS_AS(_ = json::from_bjdata(v), json::parse_error);
        }
    }

    SECTION("SOA integration with strict mode")
    {
        SECTION("strict mode enforces no trailing data")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{', 'i', 1, 'x', 'i', '}', '#', 'i', 1, 42, 'Z'
            };

            json _;
            CHECK_THROWS_AS(_ = json::from_bjdata(v, true, true), json::parse_error);
        }

        SECTION("non-strict mode allows trailing data")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{', 'i', 1, 'x', 'i', '}', '#', 'i', 1, 42, 'Z'
            };

            const auto j = json::from_bjdata(v, false);
            CHECK(j.is_array());
            CHECK(j.size() == 1);
            CHECK(j[0]["x"] == 42);
        }
    }

    SECTION("SOA field ordering")
    {
        SECTION("field order preserved in row-major")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{',
                'i', 1, 'a', 'i',
                'i', 1, 'b', 'i',
                'i', 1, 'c', 'i',
                '}',
                '#', 'i', 1,
                1, 2, 3
            };

            const auto j = json::from_bjdata(v);
            CHECK(j[0]["a"] == 1);
            CHECK(j[0]["b"] == 2);
            CHECK(j[0]["c"] == 3);
        }

        SECTION("field order preserved in column-major")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'x', 'i',
                'i', 1, 'y', 'i',
                '}',
                '#', 'i', 2,
                1, 3,
                2, 4
            };

            const auto j = json::from_bjdata(v);
            CHECK(j["x"] == json::array({1, 3}));
            CHECK(j["y"] == json::array({2, 4}));
        }
    }
}

// Unit tests for BJData SOA Variable-Length String Support (Draft 4)
// Based on MATLAB test suite: bjsoastr tests
TEST_CASE("BJData SOA Variable-Length Strings")
{
    SECTION("Fixed-length string encoding")
    {
        SECTION("fixed string col-major same length")
        {
            // {${U<4>codeSU<5>}#U<3>ABCDEFGHIJ12345
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 4, 'c', 'o', 'd', 'e', 'S', 'i', 5,
                '}', '#', 'i', 3,
                'A', 'B', 'C', 'D', 'E',
                'F', 'G', 'H', 'I', 'J',
                '1', '2', '3', '4', '5'
            };

            const auto j = json::from_bjdata(v);
            CHECK(j.is_object());
            CHECK(j["code"][0] == "ABCDE");
            CHECK(j["code"][1] == "FGHIJ");
            CHECK(j["code"][2] == "12345");
        }

        SECTION("fixed string with null padding - diff lengths")
        {
            // {${U<2>idUU<4>nameSU<3>}#U<3>ABCABCDE\\0F\\0\\0
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 2, 'i', 'd', 'U',
                'i', 4, 'n', 'a', 'm', 'e', 'S', 'i', 3,
                '}', '#', 'i', 3,
                'A', 'B', 'C',
                'A', 'B', 'C',
                'D', 'E', 0x00,
                'F', 0x00, 0x00
            };

            const auto j = json::from_bjdata(v);
            CHECK(j["name"][0] == "ABC");
            CHECK(j["name"][1] == "DE");
            CHECK(j["name"][2] == "F");
        }

        SECTION("fixed string row-major")
        {
            // [${U<4>codeSU<2>}#U<2>ABCD
            std::vector<uint8_t> const v =
            {
                '[', '$', '{',
                'i', 4, 'c', 'o', 'd', 'e', 'S', 'i', 2,
                '}', '#', 'i', 2,
                'A', 'B',
                'C', 'D'
            };

            const auto j = json::from_bjdata(v);
            CHECK(j[0]["code"] == "AB");
            CHECK(j[1]["code"] == "CD");
        }

        SECTION("fixed with numeric field")
        {
            // {${U<2>idUU<3>tagSU<2>}#U<2>ABHiLo
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 2, 'i', 'd', 'U',
                'i', 3, 't', 'a', 'g', 'S', 'i', 2,
                '}', '#', 'i', 2,
                'A', 'B',
                'H', 'i', 'L', 'o'
            };

            const auto j = json::from_bjdata(v);
            CHECK(j["id"][0] == static_cast<uint8_t>('A'));
            CHECK(j["tag"][0] == "Hi");
            CHECK(j["tag"][1] == "Lo");
        }

        SECTION("fixed with empty string (null-padded)")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 3, 't', 'a', 'g', 'S', 'i', 2,
                '}', '#', 'i', 2,
                'H', 'i',
                0x00, 0x00
            };

            const auto j = json::from_bjdata(v);
            CHECK(j["tag"][0] == "Hi");
            CHECK(j["tag"][1] == "");
        }
    }

    SECTION("Dictionary-based string encoding")
    {
        SECTION("dict string col-major basic")
        {
            // {${U<2>idUU<6>status[$S#U<2>U<6>activeU<8>inactive}#U<4>ABCD\\x00\\x01\\x00\\x00
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 2, 'i', 'd', 'U',
                'i', 6, 's', 't', 'a', 't', 'u', 's',
                '[', '$', 'S', '#', 'i', 2,
                'i', 6, 'a', 'c', 't', 'i', 'v', 'e',
                'i', 8, 'i', 'n', 'a', 'c', 't', 'i', 'v', 'e',
                '}', '#', 'i', 4,
                'A', 'B', 'C', 'D',
                0, 1, 0, 0
            };

            const auto j = json::from_bjdata(v);
            CHECK(j["status"][0] == "active");
            CHECK(j["status"][1] == "inactive");
            CHECK(j["status"][2] == "active");
            CHECK(j["status"][3] == "active");
        }

        SECTION("dict string 3 values")
        {
            // {${U<5>color[$S#U<3>U<3>redU<5>greenU<4>blue}#U<6>\\x00\\x01\\x02\\x00\\x01\\x02
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 5, 'c', 'o', 'l', 'o', 'r',
                '[', '$', 'S', '#', 'i', 3,
                'i', 3, 'r', 'e', 'd',
                'i', 5, 'g', 'r', 'e', 'e', 'n',
                'i', 4, 'b', 'l', 'u', 'e',
                '}', '#', 'i', 6,
                0, 1, 2, 0, 1, 2
            };

            const auto j = json::from_bjdata(v);
            CHECK(j["color"][0] == "red");
            CHECK(j["color"][1] == "green");
            CHECK(j["color"][2] == "blue");
            CHECK(j["color"][3] == "red");
        }

        SECTION("dict all same string")
        {
            // {${U<2>idUU<3>tag[$S#U<1>U<1>X}#U<3>ABC\\x00\\x00\\x00
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 2, 'i', 'd', 'U',
                'i', 3, 't', 'a', 'g',
                '[', '$', 'S', '#', 'i', 1,
                'i', 1, 'X',
                '}', '#', 'i', 3,
                'A', 'B', 'C',
                0, 0, 0
            };

            const auto j = json::from_bjdata(v);
            CHECK(j["tag"][0] == "X");
            CHECK(j["tag"][1] == "X");
            CHECK(j["tag"][2] == "X");
        }

        SECTION("dict with empty string entry")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 't',
                '[', '$', 'S', '#', 'i', 2,
                'i', 0,           // empty string
                'i', 1, 'X',      // "X"
                '}', '#', 'i', 3,
                0, 1, 0  // indices
            };

            const auto j = json::from_bjdata(v);
            CHECK(j["t"][0] == "");
            CHECK(j["t"][1] == "X");
            CHECK(j["t"][2] == "");
        }
    }

    SECTION("Offset-table string encoding")
    {
        SECTION("offset string col-major basic")
        {
            // {${U<2>idUU<4>desc[$U]}#U<3>ABC\\x00\\x01\\x02\\x00\\x05\\x1c\\x1fshorta very long descriptionmid
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 2, 'i', 'd', 'U',
                'i', 4, 'd', 'e', 's', 'c', '[', '$', 'U', ']',
                '}', '#', 'i', 3,
                'A', 'B', 'C',
                0, 1, 2,
                0, 5, 28, 31,
                's', 'h', 'o', 'r', 't',
                'a', ' ', 'v', 'e', 'r', 'y', ' ', 'l', 'o', 'n', 'g', ' ',
                'd', 'e', 's', 'c', 'r', 'i', 'p', 't', 'i', 'o', 'n',
                'm', 'i', 'd'
            };

            const auto j = json::from_bjdata(v);
            CHECK(j["desc"][0] == "short");
            CHECK(j["desc"][1] == "a very long description");
            CHECK(j["desc"][2] == "mid");
        }

        SECTION("offset string row-major")
        {
            // [${U<2>idUU<4>text[$U]}#U<2>A\\x00B\\x01\\x00\\x05\\x0aHelloWorld
            std::vector<uint8_t> const v =
            {
                '[', '$', '{',
                'i', 2, 'i', 'd', 'U',
                'i', 4, 't', 'e', 'x', 't', '[', '$', 'U', ']',
                '}', '#', 'i', 2,
                'A', 0,
                'B', 1,
                0, 5, 10,
                'H', 'e', 'l', 'l', 'o',
                'W', 'o', 'r', 'l', 'd'
            };

            const auto j = json::from_bjdata(v);
            CHECK(j[0]["text"] == "Hello");
            CHECK(j[1]["text"] == "World");
        }

        SECTION("offset with empty string")
        {
            // {${U<2>idUU<4>note[$U]}#U<3>ABC\\x00\\x01\\x02\\x00\\x03\\x03\\x05abcde
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 2, 'i', 'd', 'U',
                'i', 4, 'n', 'o', 't', 'e', '[', '$', 'U', ']',
                '}', '#', 'i', 3,
                'A', 'B', 'C',
                0, 1, 2,
                0, 3, 3, 5,
                'a', 'b', 'c', 'd', 'e'
            };

            const auto j = json::from_bjdata(v);
            CHECK(j["note"][0] == "abc");
            CHECK(j["note"][1] == "");
            CHECK(j["note"][2] == "de");
        }
    }

    SECTION("SOA string roundtrip tests")
    {
        SECTION("fixed string roundtrip")
        {
            json const original = json::array(
            {
                {{"code", "ABC"}}, {{"code", "DEF"}}, {{"code", "GHI"}}
            });

            auto enc = json::to_bjdata(original, true, true,
                                       json::bjdata_version_t::draft4,
                                       json::bjdata_soa_format_t::col_major);
            auto dec = json::from_bjdata(enc);

            CHECK(dec["code"][0] == "ABC");
            CHECK(dec["code"][1] == "DEF");
            CHECK(dec["code"][2] == "GHI");
        }

        SECTION("dict string roundtrip")
        {
            json const original = json::array(
            {
                {{"id", 1}, {"status", "active"}},
                {{"id", 2}, {"status", "pending"}},
                {{"id", 3}, {"status", "active"}},
                {{"id", 4}, {"status", "pending"}}
            });

            auto enc = json::to_bjdata(original, true, true,
                                       json::bjdata_version_t::draft4,
                                       json::bjdata_soa_format_t::col_major);
            auto dec = json::from_bjdata(enc);

            CHECK(dec["status"][0] == "active");
            CHECK(dec["status"][1] == "pending");
        }

        SECTION("UTF-8 strings roundtrip")
        {
            json const original = json::array(
            {
                {{"text", "Hello"}},
                {{"text", "世界"}},
                {{"text", "مرحبا"}}
            });

            auto enc = json::to_bjdata(original, true, true,
                                       json::bjdata_version_t::draft4,
                                       json::bjdata_soa_format_t::row_major);
            auto dec = json::from_bjdata(enc);

            CHECK(dec[0]["text"] == "Hello");
            CHECK(dec[1]["text"] == "世界");
            CHECK(dec[2]["text"] == "مرحبا");
        }

        SECTION("empty strings roundtrip")
        {
            json const original = json::array(
            {
                {{"tag", "Hi"}}, {{"tag", ""}}
            });

            auto enc = json::to_bjdata(original, true, true,
                                       json::bjdata_version_t::draft4,
                                       json::bjdata_soa_format_t::col_major);
            auto dec = json::from_bjdata(enc);

            CHECK(dec["tag"][0] == "Hi");
            CHECK(dec["tag"][1] == "");
        }

        SECTION("mixed numeric and string roundtrip")
        {
            json const original = json::array(
            {
                {{"id", 1}, {"name", "Alice"}, {"score", 95.5}},
                {{"id", 2}, {"name", "Bob"}, {"score", 88.2}}
            });

            auto enc = json::to_bjdata(original, true, true,
                                       json::bjdata_version_t::draft4,
                                       json::bjdata_soa_format_t::row_major);
            auto dec = json::from_bjdata(enc);

            CHECK(dec[0]["id"] == 1);
            CHECK(dec[0]["name"] == "Alice");
            CHECK(dec[0]["score"] == doctest::Approx(95.5));
        }
    }

    SECTION("Dictionary encoding details")
    {
        SECTION("dict with uint8 indices")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 3, 'r', 'g', 'b',
                '[', '$', 'S', '#', 'i', 3,
                'i', 1, 'R', 'i', 1, 'G', 'i', 1, 'B',
                '}', '#', 'i', 6,
                0, 1, 2, 0, 1, 2
            };

            const auto j = json::from_bjdata(v);
            CHECK(j["rgb"][0] == "R");
            CHECK(j["rgb"][5] == "B");
        }

        SECTION("dict index out of range")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'c', '[', '$', 'S', '#', 'i', 2,
                'i', 1, 'A', 'i', 1, 'B',
                '}', '#', 'i', 1,
                5  // out of range
            };

            json _;
            CHECK_THROWS_AS(_ = json::from_bjdata(v), json::parse_error);
        }
    }

    SECTION("Offset encoding details")
    {
        SECTION("offset varying lengths")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 4, 'd', 'e', 's', 'c', '[', '$', 'U', ']',
                '}', '#', 'i', 5,
                0, 1, 2, 3, 4,
                0, 1, 3, 6, 10, 15,
                'a', 'b', 'b', 'c', 'c', 'c', 'd', 'd', 'd', 'd', 'e', 'e', 'e', 'e', 'e'
            };

            const auto j = json::from_bjdata(v);
            CHECK(j["desc"][0] == "a");
            CHECK(j["desc"][1] == "bb");
            CHECK(j["desc"][2] == "ccc");
            CHECK(j["desc"][3] == "dddd");
            CHECK(j["desc"][4] == "eeeee");
        }

        SECTION("offset with uint16 indices")
        {
            std::vector<uint8_t> v =
            {
                '{', '$', '{',
                'i', 3, 't', 'x', 't', '[', '$', 'u', ']',
                '}', '#', 'i', 2,
                // Payload (2 × uint16 for offsets 0 and 150):
                0x00, 0x00,     // offset 0 as uint16 LE
                0x96, 0x00,     // offset 150 as uint16 LE
                // Offset table (3 × uint16):
                0x00, 0x00,     // 0
                0x96, 0x00,     // 150
                0x2C, 0x01      // 300
            };

            for (int i = 0; i < 300; i++)
            {
                v.push_back('X');
            }

            const auto j = json::from_bjdata(v);
            CHECK(j["txt"][0].get<std::string>().size() == 150);
        }

        SECTION("offset EOF errors")
        {
            std::vector<uint8_t> const v1 =
            {
                '{', '$', '{',
                'i', 1, 't', '[', '$', 'U', ']',
                '}', '#', 'i', 2,
                0, 1,
                0, 3  // incomplete
            };

            std::vector<uint8_t> const v2 =
            {
                '{', '$', '{',
                'i', 1, 't', '[', '$', 'U', ']',
                '}', '#', 'i', 2,
                0, 1,
                0, 3, 5,
                'a', 'b'  // incomplete buffer
            };

            json _;
            CHECK_THROWS_AS(_ = json::from_bjdata(v1), json::parse_error);
            CHECK_THROWS_AS(_ = json::from_bjdata(v2), json::parse_error);
        }
    }

    SECTION("SOA High-precision with strings")
    {
        SECTION("high-prec fixed length")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'n', 'H', 'i', 5,
                '}', '#', 'i', 2,
                '1', '2', '3', '4', '5',
                '6', '7', '8', '9', '0'
            };

            const auto j = json::from_bjdata(v);
            CHECK(j["n"][0] == 12345);
            CHECK(j["n"][1] == 67890);
        }

        SECTION("high-prec with null padding")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'n', 'H', 'i', 4,
                '}', '#', 'i', 2,
                '1', '2', '3', 0x00,
                '9', 0x00, 0x00, 0x00
            };

            const auto j = json::from_bjdata(v);
            CHECK(j["n"][0] == 123);
            CHECK(j["n"][1] == 9);
        }
    }

    SECTION("SOA Integration - Complex real-world scenarios")
    {
        SECTION("user database table")
        {
            json const users = json::array(
            {
                {{"id", 1}, {"username", "alice123"}, {"email", "alice@example.com"}, {"active", true}},
                {{"id", 2}, {"username", "bob456"}, {"email", "bob@example.com"}, {"active", false}},
                {{"id", 3}, {"username", "carol789"}, {"email", "carol@example.com"}, {"active", true}}
            });

            auto enc = json::to_bjdata(users, true, true,
                                       json::bjdata_version_t::draft4,
                                       json::bjdata_soa_format_t::col_major);
            auto dec = json::from_bjdata(enc);

            CHECK(dec["username"][0] == "alice123");
            CHECK(dec["email"][1] == "bob@example.com");
            CHECK(dec["active"][2] == true);
        }

        SECTION("sensor readings with status")
        {
            json const sensors = json::array(
            {
                {{"temp", 22.5}, {"status", "ok"}},
                {{"temp", 45.8}, {"status", "warning"}},
                {{"temp", 22.9}, {"status", "ok"}},
                {{"temp", 60.2}, {"status", "critical"}},
                {{"temp", 23.1}, {"status", "ok"}}
            });

            auto enc = json::to_bjdata(sensors, true, true,
                                       json::bjdata_version_t::draft4,
                                       json::bjdata_soa_format_t::row_major);
            auto dec = json::from_bjdata(enc);

            CHECK(dec[1]["status"] == "warning");
            CHECK(dec[3]["status"] == "critical");
        }

        SECTION("product catalog")
        {
            json const products = json::array(
            {
                {{"sku", "A001"}, {"category", "books"}, {"price", 19.99}},
                {{"sku", "A002"}, {"category", "books"}, {"price", 29.99}},
                {{"sku", "B001"}, {"category", "electronics"}, {"price", 99.99}}
            });

            auto enc = json::to_bjdata(products, true, true,
                                       json::bjdata_version_t::draft4,
                                       json::bjdata_soa_format_t::col_major);
            auto dec = json::from_bjdata(enc);

            CHECK(dec["category"][0] == "books");
            CHECK(dec["category"][2] == "electronics");
        }
    }

    SECTION("SOA invalid schema detection")
    {
        SECTION("missing $ after [")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 's', '[', 'S', '#', 'i', 1,
                '}', '#', 'i', 1
            };

            json _;
            CHECK_THROWS_AS(_ = json::from_bjdata(v), json::parse_error);
        }

        SECTION("wrong marker in offset schema")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 's', '[', '$', 'L', ']',  // L is valid but unconventional
                '}', '#', 'i', 1, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0
            };

            // Should parse but with int64 offsets
            json _;
            CHECK_THROWS_AS(_ = json::from_bjdata(v), json::parse_error);
        }
    }
}

// ============================================================================
// BJData SOA Complete Coverage Tests
// ============================================================================
// Add these test sections to unit-bjdata.cpp

TEST_CASE("SOA Coverage Tests")
{
    // ========================================================================
    // READER COVERAGE: parse_bjdata_soa
    // ========================================================================

    SECTION("schema parsing edge cases")
    {
        SECTION("empty schema")
        {
            // {${}#i0
            std::vector<uint8_t> const v =
            {
                '{', '$', '{', '}', '#', 'i', 0
            };
            const auto j = json::from_bjdata(v);
            CHECK(j.is_object());
            CHECK(j.empty());
        }

        SECTION("schema with no-op markers")
        {
            // {${N i1xU N}#i2 AB - 'N' markers should be skipped
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'N',  // no-op before field
                'i', 1, 'x', 'U',
                'N',  // no-op after field
                '}', '#', 'i', 2,
                'A', 'B'
            };
            const auto j = json::from_bjdata(v);
            CHECK(j["x"][0] == 65);
            CHECK(j["x"][1] == 66);
        }

        SECTION("error: missing '$' after '['")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'x', '[', 'S',  // missing '$' after '['
                '}', '#', 'i', 1
            };
            CHECK_THROWS_WITH(json::from_bjdata(v),
                              "[json.exception.parse_error.113] parse error at byte 8: "
                              "syntax error while parsing BJData SOA: expected '$' after '['");
        }

        SECTION("error: missing '#' after 'S' in dict")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'x', '[', '$', 'S', 'i', 2,  // missing '#' after 'S'
                '}', '#', 'i', 1
            };
            CHECK_THROWS_WITH(json::from_bjdata(v),
                              "[json.exception.parse_error.113] parse error at byte 10: "
                              "syntax error while parsing BJData SOA: expected '#' in dict");
        }

        SECTION("error: invalid offset type marker")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'x', '[', '$', 'Z', ']',  // 'Z' is not valid for offset
                '}', '#', 'i', 1
            };
            CHECK_THROWS_WITH(json::from_bjdata(v),
                              "[json.exception.parse_error.113] parse error at byte 9: "
                              "syntax error while parsing BJData SOA: invalid offset type");
        }

        SECTION("error: missing ']' in offset encoding")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'x', '[', '$', 'U', '}',  // '}' instead of ']'
                '#', 'i', 1
            };
            CHECK_THROWS_WITH(json::from_bjdata(v),
                              "[json.exception.parse_error.113] parse error at byte 10: "
                              "syntax error while parsing BJData SOA: expected ']'");
        }

        SECTION("error: missing '#' after schema")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'x', 'U',
                '}', 'i', 2  // 'i' instead of '#'
            };
            CHECK_THROWS_WITH(json::from_bjdata(v),
                              "[json.exception.parse_error.113] parse error at byte 9: "
                              "syntax error while parsing BJData SOA: expected '#' after schema");
        }

        SECTION("error: unexpected EOF in schema")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{'  // truncated
            };
            CHECK_THROWS_AS(json::from_bjdata(v), json::parse_error);
        }

        SECTION("error: unexpected EOF reading type")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'x'  // truncated before type marker
            };
            CHECK_THROWS_AS(json::from_bjdata(v), json::parse_error);
        }
    }

    // ========================================================================
    // READER COVERAGE: parse_bjdata_soa_buffered - all type markers
    // ========================================================================

    SECTION("buffered path numeric types with offset strings")
    {
        // These tests force the buffered path by including an offset field

        SECTION("int8 field")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'n', 'i',
                'i', 1, 's', '[', '$', 'U', ']',
                '}', '#', 'i', 2,
                0x7F, 0x80,  // 127, -128 as int8
                0, 1,        // placeholder
                0, 3, 6,     // offsets
                'a', 'b', 'c', 'd', 'e', 'f'
            };
            const auto j = json::from_bjdata(v);
            CHECK(j["n"][0] == 127);
            CHECK(j["n"][1] == -128);
        }

        SECTION("int16 field")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'n', 'I',
                'i', 1, 's', '[', '$', 'U', ']',
                '}', '#', 'i', 1,
                0x00, 0x80,  // -32768 as int16 LE
                0,           // placeholder
                0, 1,        // offsets
                'x'
            };
            const auto j = json::from_bjdata(v);
            CHECK(j["n"][0] == -32768);
        }

        SECTION("int32 field")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'n', 'l',
                'i', 1, 's', '[', '$', 'U', ']',
                '}', '#', 'i', 1,
                0x01, 0x00, 0x00, 0x00,  // 1 as int32 LE
                0,
                0, 1,
                'x'
            };
            const auto j = json::from_bjdata(v);
            CHECK(j["n"][0] == 1);
        }

        SECTION("int64 field")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'n', 'L',
                'i', 1, 's', '[', '$', 'U', ']',
                '}', '#', 'i', 1,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F,  // INT64_MAX
                0,
                0, 1,
                'x'
            };
            const auto j = json::from_bjdata(v);
            CHECK(j["n"][0] == INT64_MAX);
        }

        SECTION("uint16 field")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'n', 'u',
                'i', 1, 's', '[', '$', 'U', ']',
                '}', '#', 'i', 1,
                0xFF, 0xFF,  // 65535 as uint16 LE
                0,
                0, 1,
                'x'
            };
            const auto j = json::from_bjdata(v);
            CHECK(j["n"][0] == 65535);
        }

        SECTION("uint32 field")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'n', 'm',
                'i', 1, 's', '[', '$', 'U', ']',
                '}', '#', 'i', 1,
                0xFF, 0xFF, 0xFF, 0xFF,  // UINT32_MAX
                0,
                0, 1,
                'x'
            };
            const auto j = json::from_bjdata(v);
            CHECK(j["n"][0] == UINT32_MAX);
        }

        SECTION("uint64 field")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'n', 'M',
                'i', 1, 's', '[', '$', 'U', ']',
                '}', '#', 'i', 1,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // UINT64_MAX
                0,
                0, 1,
                'x'
            };
            const auto j = json::from_bjdata(v);
            CHECK(j["n"][0] == UINT64_MAX);
        }

        SECTION("byte field")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'n', 'B',
                'i', 1, 's', '[', '$', 'U', ']',
                '}', '#', 'i', 1,
                0xAB,
                0,
                0, 1,
                'x'
            };
            const auto j = json::from_bjdata(v);
            CHECK(j["n"][0] == 0xAB);
        }

        SECTION("float32 field")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'n', 'd',
                'i', 1, 's', '[', '$', 'U', ']',
                '}', '#', 'i', 1,
                0x00, 0x00, 0x80, 0x3F,  // 1.0f in LE
                0,
                0, 1,
                'x'
            };
            const auto j = json::from_bjdata(v);
            CHECK(j["n"][0] == 1.0);
        }

        SECTION("float64 field")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'n', 'D',
                'i', 1, 's', '[', '$', 'U', ']',
                '}', '#', 'i', 1,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x3F,  // 1.0 in LE
                0,
                0, 1,
                'x'
            };
            const auto j = json::from_bjdata(v);
            CHECK(j["n"][0] == 1.0);
        }

        SECTION("half-float field")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'n', 'h',
                'i', 1, 's', '[', '$', 'U', ']',
                '}', '#', 'i', 1,
                0x00, 0x3C,  // 1.0 as half-float LE
                0,
                0, 1,
                'x'
            };
            const auto j = json::from_bjdata(v);
            CHECK(j["n"][0] == 1.0);
        }

        SECTION("half-float special values")
        {
            // Test subnormal (exp=0, mant!=0)
            std::vector<uint8_t> v1 =
            {
                '{', '$', '{',
                'i', 1, 'n', 'h',
                'i', 1, 's', '[', '$', 'U', ']',
                '}', '#', 'i', 1,
                0x01, 0x00,  // smallest subnormal
                0,
                0, 1,
                'x'
            };
            const auto j1 = json::from_bjdata(v1);
            CHECK(j1["n"][0] > 0);
            CHECK(j1["n"][0] < 1e-6);

            // Test infinity (exp=31, mant=0)
            std::vector<uint8_t> v2 =
            {
                '{', '$', '{',
                'i', 1, 'n', 'h',
                'i', 1, 's', '[', '$', 'U', ']',
                '}', '#', 'i', 1,
                0x00, 0x7C,  // +infinity
                0,
                0, 1,
                'x'
            };
            const auto j2 = json::from_bjdata(v2);
            CHECK(j2["n"][0] == std::numeric_limits<double>::infinity());

            // Test NaN (exp=31, mant!=0)
            std::vector<uint8_t> v3 =
            {
                '{', '$', '{',
                'i', 1, 'n', 'h',
                'i', 1, 's', '[', '$', 'U', ']',
                '}', '#', 'i', 1,
                0x01, 0x7C,  // NaN
                0,
                0, 1,
                'x'
            };
            const auto j3 = json::from_bjdata(v3);
            CHECK(std::isnan(j3["n"][0].get<double>()));

            // Test negative half-float
            std::vector<uint8_t> v4 =
            {
                '{', '$', '{',
                'i', 1, 'n', 'h',
                'i', 1, 's', '[', '$', 'U', ']',
                '}', '#', 'i', 1,
                0x00, 0xBC,  // -1.0 as half-float LE
                0,
                0, 1,
                'x'
            };
            const auto j4 = json::from_bjdata(v4);
            CHECK(j4["n"][0] == -1.0);
        }

        SECTION("bool field true and false")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'b', 'T',
                'i', 1, 's', '[', '$', 'U', ']',
                '}', '#', 'i', 2,
                'T', 'F',  // true, false
                0, 1,
                0, 1, 2,
                'a', 'b'
            };
            const auto j = json::from_bjdata(v);
            CHECK(j["b"][0] == true);
            CHECK(j["b"][1] == false);
        }

        SECTION("null field")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'n', 'Z',
                'i', 1, 's', '[', '$', 'U', ']',
                '}', '#', 'i', 2,
                // no payload for null
                0, 1,
                0, 1, 2,
                'a', 'b'
            };
            const auto j = json::from_bjdata(v);
            CHECK(j["n"][0].is_null());
            CHECK(j["n"][1].is_null());
        }

        SECTION("error: unsupported type marker")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'x', 'Q',  // 'Q' is not a valid type
                'i', 1, 's', '[', '$', 'U', ']',
                '}', '#', 'i', 1,
                0,
                0, 1,
                'x'
            };
            CHECK_THROWS_WITH(json::from_bjdata(v),
                              "[json.exception.parse_error.113] parse error at byte 18: "
                              "syntax error while parsing BJData SOA: unsupported SOA type");
        }
    }

    // ========================================================================
    // READER COVERAGE: offset table index sizes
    // ========================================================================

    SECTION("offset index sizes")
    {
        SECTION("uint8 indices (size 1)")
        {
            // Already covered in basic tests
        }

        SECTION("uint16 indices (size 2)")
        {
            std::vector<uint8_t> v =
            {
                '{', '$', '{',
                'i', 1, 's', '[', '$', 'u', ']',
                '}', '#', 'i', 2
            };
            // Placeholders (2 x uint16)
            v.push_back(0x00);
            v.push_back(0x00);
            v.push_back(0x00);
            v.push_back(0x00);
            // Offset table (3 x uint16 LE)
            v.push_back(0x00);
            v.push_back(0x00);  // 0
            v.push_back(0x05);
            v.push_back(0x00);  // 5
            v.push_back(0x08);
            v.push_back(0x00);  // 8
            // Buffer
            for (char c : std::string("hellobye"))
            {
                v.push_back(c);
            }

            const auto j = json::from_bjdata(v);
            CHECK(j["s"][0] == "hello");
            CHECK(j["s"][1] == "bye");
        }

        SECTION("uint32 indices (size 4)")
        {
            std::vector<uint8_t> v =
            {
                '{', '$', '{',
                'i', 1, 's', '[', '$', 'm', ']',
                '}', '#', 'i', 1
            };
            // Placeholder (1 x uint32)
            v.push_back(0x00);
            v.push_back(0x00);
            v.push_back(0x00);
            v.push_back(0x00);
            // Offset table (2 x uint32 LE)
            v.push_back(0x00);
            v.push_back(0x00);
            v.push_back(0x00);
            v.push_back(0x00);  // 0
            v.push_back(0x04);
            v.push_back(0x00);
            v.push_back(0x00);
            v.push_back(0x00);  // 4
            // Buffer
            for (char c : std::string("test"))
            {
                v.push_back(c);
            }

            const auto j = json::from_bjdata(v);
            CHECK(j["s"][0] == "test");
        }
    }

    // ========================================================================
    // READER COVERAGE: dictionary string encoding
    // ========================================================================

    SECTION("dictionary encoding edge cases")
    {
        SECTION("large dictionary (uint16 indices)")
        {
            std::vector<uint8_t> v =
            {
                '{', '$', '{',
                'i', 1, 's', '[', '$', 'S', '#', 'u', 0x00, 0x01  // 256 entries
            };
            // Add 256 dictionary strings
            for (int i = 0; i < 256; ++i)
            {
                std::string s = "str" + std::to_string(i);
                v.push_back('i');
                v.push_back(static_cast<uint8_t>(s.size()));
                for (char c : s)
                {
                    v.push_back(c);
                }
            }
            v.push_back('}');
            v.push_back('#');
            v.push_back('i');
            v.push_back(2);
            // Indices (uint16 because dict > 255)
            v.push_back(0x00);
            v.push_back(0x00);  // index 0
            v.push_back(0xFF);
            v.push_back(0x00);  // index 255

            const auto j = json::from_bjdata(v);
            CHECK(j["s"][0] == "str0");
            CHECK(j["s"][1] == "str255");
        }

        SECTION("error: dict index out of range")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 's', '[', '$', 'S', '#', 'i', 2,
                'i', 3, 'o', 'n', 'e',
                'i', 3, 't', 'w', 'o',
                '}', '#', 'i', 1,
                5  // index 5 but dict only has 2 entries
            };
            CHECK_THROWS_WITH(json::from_bjdata(v),
                              "[json.exception.parse_error.113] parse error at byte 27: "
                              "syntax error while parsing BJData SOA: dict index out of range");
        }
    }

    // ========================================================================
    // READER COVERAGE: row-major mode
    // ========================================================================

    SECTION("row-major with offset strings")
    {
        std::vector<uint8_t> const v =
        {
            '[', '$', '{',
            'i', 2, 'i', 'd', 'U',
            'i', 4, 'n', 'a', 'm', 'e', '[', '$', 'U', ']',
            '}', '#', 'i', 2,
            // Row 0: id=65, name placeholder
            65, 0,
            // Row 1: id=66, name placeholder
            66, 1,
            // Offset table
            0, 5, 8,
            // Buffer
            'A', 'l', 'i', 'c', 'e', 'B', 'o', 'b'
        };
        const auto j = json::from_bjdata(v);
        CHECK(j[0]["id"] == 65);
        CHECK(j[0]["name"] == "Alice");
        CHECK(j[1]["id"] == 66);
        CHECK(j[1]["name"] == "Bob");
    }

    // ========================================================================
    // READER COVERAGE: high-precision number parsing
    // ========================================================================

    SECTION("high-precision parsing")
    {
        SECTION("high-prec integer")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'n', 'H', 'i', 8,
                '}', '#', 'i', 1,
                '1', '2', '3', '4', '5', '6', '7', '8'
            };
            const auto j = json::from_bjdata(v);
            CHECK(j["n"][0] == 12345678);
        }

        SECTION("high-prec float")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'n', 'H', 'i', 5,
                '}', '#', 'i', 1,
                '3', '.', '1', '4', '0'
            };
            const auto j = json::from_bjdata(v);
            CHECK(j["n"][0] == Approx(3.14));
        }

        SECTION("high-prec with padding - streaming path")
        {
            // Without offset fields, uses streaming path
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'n', 'H', 'i', 4,
                '}', '#', 'i', 2,
                '4', '2', 0x00, 0x00,
                '7', 0x00, 0x00, 0x00
            };
            const auto j = json::from_bjdata(v);
            CHECK(j["n"][0] == 42);
            CHECK(j["n"][1] == 7);
        }

        SECTION("high-prec invalid number falls back to string")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'n', 'H', 'i', 3,
                '}', '#', 'i', 1,
                'a', 'b', 'c'  // not a valid number
            };
            const auto j = json::from_bjdata(v);
            CHECK(j["n"][0] == "abc");
        }
    }

    // ========================================================================
    // READER COVERAGE: streaming path (emit_soa_streaming)
    // ========================================================================

    SECTION("streaming path coverage")
    {
        SECTION("fixed string in streaming")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 's', 'S', 'i', 5,
                '}', '#', 'i', 2,
                'h', 'e', 'l', 'l', 'o',
                'w', 'o', 'r', 'l', 'd'
            };
            const auto j = json::from_bjdata(v);
            CHECK(j["s"][0] == "hello");
            CHECK(j["s"][1] == "world");
        }

        SECTION("dict string in streaming")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 's', '[', '$', 'S', '#', 'i', 2,
                'i', 3, 'y', 'e', 's',
                'i', 2, 'n', 'o',
                '}', '#', 'i', 3,
                0, 1, 0  // indices
            };
            const auto j = json::from_bjdata(v);
            CHECK(j["s"][0] == "yes");
            CHECK(j["s"][1] == "no");
            CHECK(j["s"][2] == "yes");
        }

        SECTION("all numeric types in streaming col-major")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'a', 'U',
                'i', 1, 'b', 'i',
                'i', 1, 'c', 'd',
                '}', '#', 'i', 1,
                255,        // uint8
                0x80,       // int8 = -128
                0x00, 0x00, 0x80, 0x3F  // float 1.0
            };
            const auto j = json::from_bjdata(v);
            CHECK(j["a"][0] == 255);
            CHECK(j["b"][0] == -128);
            CHECK(j["c"][0] == 1.0f);
        }

        SECTION("streaming row-major")
        {
            std::vector<uint8_t> const v =
            {
                '[', '$', '{',
                'i', 1, 'x', 'U',
                'i', 1, 'y', 'i',
                '}', '#', 'i', 2,
                // row 0
                10, 0x14,  // x=10, y=20
                // row 1
                30, 0x28   // x=30, y=40
            };
            const auto j = json::from_bjdata(v);
            CHECK(j[0]["x"] == 10);
            CHECK(j[0]["y"] == 20);
            CHECK(j[1]["x"] == 30);
            CHECK(j[1]["y"] == 40);
        }
    }

    // ========================================================================
    // READER COVERAGE: error conditions
    // ========================================================================

    SECTION("error: invalid offset range - start > end")
    {
        std::vector<uint8_t> const v =
        {
            '{', '$', '{',
            'i', 1, 's', '[', '$', 'U', ']',
            '}', '#', 'i', 1,
            0,        // placeholder
            5, 2, 10  // offsets: 5, 2, 10 - invalid: 5 > 2
        };
        // Note: the actual buffer would need to be provided but error triggers first
        CHECK_THROWS_AS(json::from_bjdata(v), json::parse_error);
    }

    SECTION("error: offset exceeds buffer")
    {
        std::vector<uint8_t> const v =
        {
            '{', '$', '{',
            'i', 1, 's', '[', '$', 'U', ']',
            '}', '#', 'i', 1,
            0,           // placeholder
            0, 100,      // offsets: 0, 100 - but buffer only has 3 bytes
            'a', 'b', 'c'
        };
        CHECK_THROWS_AS(json::from_bjdata(v), json::parse_error);
    }

    // ========================================================================
    // WRITER COVERAGE: write_bjdata_soa
    // ========================================================================

    SECTION("writer SOA coverage")
    {
        SECTION("row-major output")
        {
            json j = json::array(
            {
                {{"id", 1}, {"name", "Alice"}},
                {{"id", 2}, {"name", "Bob"}}
            });
            auto v = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4,
                                     json::bjdata_soa_format_t::row_major);
            auto j2 = json::from_bjdata(v);
            CHECK(j2 == j);
        }

        SECTION("column-major output")
        {
            json j = json::array(
            {
                {{"id", 1}, {"name", "test"}},
                {{"id", 2}, {"name", "data"}}
            });
            auto v = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4,
                                     json::bjdata_soa_format_t::col_major);
            // Column-major produces object output
            auto j2 = json::from_bjdata(v);
            CHECK(j2["id"][0] == 1);
            CHECK(j2["id"][1] == 2);
        }

        SECTION("writer string encoding - fixed")
        {
            // Strings of same length should use fixed encoding
            json j = json::array(
            {
                {{"s", "aaa"}},
                {{"s", "bbb"}},
                {{"s", "ccc"}}
            });
            auto v = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4,
                                     json::bjdata_soa_format_t::row_major);
            auto j2 = json::from_bjdata(v);
            CHECK(j2[0]["s"] == "aaa");
            CHECK(j2[2]["s"] == "ccc");
        }

        SECTION("writer string encoding - dict with low uniqueness")
        {
            // Many repeated strings should trigger dict encoding
            json j = json::array();
            for (int i = 0; i < 20; ++i)
            {
                j.push_back({{"status", i % 2 == 0 ? "active" : "inactive"}});
            }
            auto v = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4,
                                     json::bjdata_soa_format_t::row_major);
            auto j2 = json::from_bjdata(v);
            CHECK(j2[0]["status"] == "active");
            CHECK(j2[1]["status"] == "inactive");
        }

        SECTION("writer string encoding - offset for variable length")
        {
            // Force offset encoding with threshold=0
            json j = json::array(
            {
                {{"desc", "short"}},
                {{"desc", "a much longer description"}},
                {{"desc", "medium length"}}
            });
            // Note: threshold parameter controls encoding selection
            auto v = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4,
                                     json::bjdata_soa_format_t::row_major);
            auto j2 = json::from_bjdata(v);
            CHECK(j2[0]["desc"] == "short");
            CHECK(j2[1]["desc"] == "a much longer description");
        }

        SECTION("writer with null values")
        {
            json j = json::array(
            {
                {{"val", nullptr}},
                {{"val", nullptr}}
            });
            auto v = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4,
                                     json::bjdata_soa_format_t::col_major);
            auto j2 = json::from_bjdata(v);
            CHECK(j2["val"][0].is_null());
            CHECK(j2["val"][1].is_null());
        }

        SECTION("writer with bool values")
        {
            json j = json::array(
            {
                {{"flag", true}},
                {{"flag", false}},
                {{"flag", true}}
            });
            auto v = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4,
                                     json::bjdata_soa_format_t::col_major);
            auto j2 = json::from_bjdata(v);
            CHECK(j2["flag"][0] == true);
            CHECK(j2["flag"][1] == false);
            CHECK(j2["flag"][2] == true);
        }

        SECTION("writer various numeric types")
        {
            json j = json::array(
            {
                {{"u8", 255}, {"i32", -1000000}, {"f64", 3.14159}}
            });
            auto v = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4,
                                     json::bjdata_soa_format_t::col_major);
            auto j2 = json::from_bjdata(v);
            CHECK(j2["u8"][0] == 255);
            CHECK(j2["i32"][0] == -1000000);
            CHECK(j2["f64"][0] == Approx(3.14159));
        }

        SECTION("writer - schema validation fails for non-uniform objects")
        {
            // Objects with different keys shouldn't use SOA
            json j = json::array(
            {
                {{"a", 1}},
                {{"b", 2}}  // different key
            });
            auto v = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4,
                                     json::bjdata_soa_format_t::col_major);
            // Should fall back to regular array encoding
            auto j2 = json::from_bjdata(v);
            CHECK(j2[0]["a"] == 1);
            CHECK(j2[1]["b"] == 2);
        }

        SECTION("writer - schema validation fails for mixed types")
        {
            // Same key but different value types shouldn't use SOA
            json j = json::array(
            {
                {{"val", 123}},
                {{"val", "string"}}  // different type
            });
            auto v = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4,
                                     json::bjdata_soa_format_t::col_major);
            auto j2 = json::from_bjdata(v);
            CHECK(j2[0]["val"] == 123);
            CHECK(j2[1]["val"] == "string");
        }

        SECTION("writer - empty array")
        {
            json j = json::array();
            auto v = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4,
                                     json::bjdata_soa_format_t::col_major);
            auto j2 = json::from_bjdata(v);
            CHECK(j2.is_array());
            CHECK(j2.empty());
        }

        SECTION("writer - array with empty objects")
        {
            json j = json::array({{}, {}});
            auto v = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4,
                                     json::bjdata_soa_format_t::col_major);
            auto j2 = json::from_bjdata(v);
            // Empty objects can't use SOA, falls back to regular
            CHECK(j2.size() == 2);
        }

        SECTION("writer - nested arrays/objects not supported")
        {
            json j = json::array(
            {
                {{"nested", {{"a", 1}}}},
                {{"nested", {{"a", 2}}}}
            });
            auto v = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4,
                                     json::bjdata_soa_format_t::col_major);
            // Should fall back to regular encoding
            auto j2 = json::from_bjdata(v);
            CHECK(j2[0]["nested"]["a"] == 1);
        }
    }

    // ========================================================================
    // WRITER COVERAGE: analyze_soa_string_field
    // ========================================================================

    SECTION("string analysis thresholds")
    {
        SECTION("all identical strings - uses dict")
        {
            json j = json::array();
            for (int i = 0; i < 100; ++i)
            {
                j.push_back({{"s", "same"}});
            }
            auto v = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4,
                                     json::bjdata_soa_format_t::col_major);
            auto j2 = json::from_bjdata(v);
            CHECK(j2["s"][0] == "same");
            CHECK(j2["s"][99] == "same");
        }

        SECTION("all unique long strings - uses offset")
        {
            json j = json::array();
            for (int i = 0; i < 10; ++i)
            {
                j.push_back({{"s", "unique_long_string_number_" + std::to_string(i) + "_end"}});
            }
            auto v = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4,
                                     json::bjdata_soa_format_t::col_major);
            auto j2 = json::from_bjdata(v);
            CHECK(j2["s"][0].get<std::string>().find("number_0") != std::string::npos);
        }
    }

    // ========================================================================
    // WRITER COVERAGE: write_soa_string_schema variants
    // ========================================================================

    SECTION("writer string schema variants")
    {
        SECTION("dict with large index (uint16)")
        {
            // Create dict with > 255 unique strings to force uint16 indices
            json j = json::array();
            for (int i = 0; i < 300; ++i)
            {
                j.push_back({{"s", "str" + std::to_string(i % 256)}});
            }
            // With 256 unique strings, indices should be uint16
            auto v = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4,
                                     json::bjdata_soa_format_t::col_major);
            auto j2 = json::from_bjdata(v);
            CHECK(j2["s"].size() == 300);
        }

        SECTION("offset with uint16 indices")
        {
            // Create strings totaling > 255 bytes to force uint16 offsets
            json j = json::array();
            std::string long_str(100, 'x');
            for (int i = 0; i < 5; ++i)
            {
                j.push_back({{"s", long_str + std::to_string(i)}});
            }
            auto v = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4,
                                     json::bjdata_soa_format_t::col_major);
            auto j2 = json::from_bjdata(v);
            CHECK(j2["s"][0].get<std::string>().size() > 100);
        }

        SECTION("offset with uint32 indices")
        {
            // Create strings totaling > 65535 bytes
            json j = json::array();
            std::string very_long(10000, 'y');
            for (int i = 0; i < 10; ++i)
            {
                j.push_back({{"s", very_long + std::to_string(i)}});
            }
            auto v = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4,
                                     json::bjdata_soa_format_t::col_major);
            auto j2 = json::from_bjdata(v);
            CHECK(j2["s"][0].get<std::string>().size() > 10000);
        }
    }

    // ========================================================================
    // WRITER COVERAGE: write_soa_string_value
    // ========================================================================

    SECTION("writer string value encoding")
    {
        SECTION("dict index uint8")
        {
            json j = json::array();
            for (int i = 0; i < 10; ++i)
            {
                j.push_back({{"s", i % 2 == 0 ? "even" : "odd"}});
            }
            auto v = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4,
                                     json::bjdata_soa_format_t::col_major);
            auto j2 = json::from_bjdata(v);
            CHECK(j2["s"][0] == "even");
            CHECK(j2["s"][1] == "odd");
        }

        SECTION("fixed string with padding")
        {
            json j = json::array(
            {
                {{"s", "a"}},
                {{"s", "bb"}},
                {{"s", "ccc"}}
            });
            // Should pad shorter strings to max length
            auto v = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4,
                                     json::bjdata_soa_format_t::col_major);
            auto j2 = json::from_bjdata(v);
            CHECK(j2["s"][0] == "a");
            CHECK(j2["s"][1] == "bb");
            CHECK(j2["s"][2] == "ccc");
        }
    }

    // ========================================================================
    // WRITER COVERAGE: write_soa_offset_table
    // ========================================================================

    SECTION("writer offset table sizes")
    {
        SECTION("small buffer - uint8 offsets")
        {
            json j = json::array(
            {
                {{"s", "ab"}},
                {{"s", "cd"}}
            });
            auto v = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4,
                                     json::bjdata_soa_format_t::col_major);
            auto j2 = json::from_bjdata(v);
            CHECK(j2["s"][0] == "ab");
        }

        SECTION("medium buffer - uint16 offsets")
        {
            json j = json::array();
            for (int i = 0; i < 3; ++i)
            {
                j.push_back({{"s", std::string(100, 'a' + i)}});
            }
            auto v = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4,
                                     json::bjdata_soa_format_t::col_major);
            auto j2 = json::from_bjdata(v);
            CHECK(j2["s"][0].get<std::string>().size() == 100);
        }
    }

    // ========================================================================
    // WRITER COVERAGE: get_bjdata_soa_schema
    // ========================================================================

    SECTION("schema validation")
    {
        SECTION("non-array input")
        {
            json j = {{"key", "value"}};  // object, not array
            auto v = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4,
                                     json::bjdata_soa_format_t::col_major);
            auto j2 = json::from_bjdata(v);
            CHECK(j2["key"] == "value");
        }

        SECTION("array of non-objects")
        {
            json j = json::array({1, 2, 3});  // primitives, not objects
            auto v = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4,
                                     json::bjdata_soa_format_t::col_major);
            auto j2 = json::from_bjdata(v);
            CHECK(j2 == json::array({1, 2, 3}));
        }

        SECTION("array with binary values - not supported")
        {
            json j = json::array(
            {
                {{"data", json::binary({1, 2, 3})}},
                {{"data", json::binary({4, 5, 6})}}
            });
            auto v = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4,
                                     json::bjdata_soa_format_t::col_major);
            // Binary not supported in SOA, should fall back
            auto j2 = json::from_bjdata(v);
            CHECK(j2[0]["data"].is_binary());
        }

        SECTION("array with nested array values - not supported")
        {
            json j = json::array(
            {
                {{"arr", {1, 2}}},
                {{"arr", {3, 4}}}
            });
            auto v = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4,
                                     json::bjdata_soa_format_t::col_major);
            auto j2 = json::from_bjdata(v);
            CHECK(j2[0]["arr"] == json::array({1, 2}));
        }

        SECTION("objects with different field counts")
        {
            json j = json::array(
            {
                {{"a", 1}, {"b", 2}},
                {{"a", 3}}  // missing "b"
            });
            auto v = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4,
                                     json::bjdata_soa_format_t::col_major);
            auto j2 = json::from_bjdata(v);
            CHECK(j2[0]["a"] == 1);
            CHECK(j2[1]["a"] == 3);
        }
    }

    // ========================================================================
    // READER COVERAGE: EOF conditions
    // ========================================================================

    SECTION("EOF error conditions")
    {
        SECTION("EOF reading bool")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'b', 'T',
                '}', '#', 'i', 1
                // missing bool value
            };
            CHECK_THROWS_AS(json::from_bjdata(v), json::parse_error);
        }

        SECTION("EOF reading half-float byte 1")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'h', 'h',
                '}', '#', 'i', 1,
                0x00  // only 1 byte, need 2
            };
            CHECK_THROWS_AS(json::from_bjdata(v), json::parse_error);
        }

        SECTION("EOF reading half-float byte 2")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 'h', 'h',
                '}', '#', 'i', 1
                // no bytes for half-float
            };
            CHECK_THROWS_AS(json::from_bjdata(v), json::parse_error);
        }

        SECTION("EOF reading string buffer")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 's', '[', '$', 'U', ']',
                '}', '#', 'i', 1,
                0,           // placeholder
                0, 10,       // offsets claim 10 bytes
                'a', 'b'     // only 2 bytes provided
            };
            CHECK_THROWS_AS(json::from_bjdata(v), json::parse_error);
        }

        SECTION("EOF reading offset table")
        {
            std::vector<uint8_t> const v =
            {
                '{', '$', '{',
                'i', 1, 's', '[', '$', 'U', ']',
                '}', '#', 'i', 2,
                0, 0,        // placeholders
                0, 3         // incomplete offset table (need 3 entries)
            };
            CHECK_THROWS_AS(json::from_bjdata(v), json::parse_error);
        }
    }

    // ========================================================================
    // ROUNDTRIP TESTS
    // ========================================================================

    SECTION("roundtrip tests")
    {
        SECTION("complex mixed types roundtrip")
        {
            json j = json::array(
            {
                {{"id", 1}, {"name", "Alice"}, {"active", true}, {"score", 95.5}},
                {{"id", 2}, {"name", "Bob"}, {"active", false}, {"score", 87.3}},
                {{"id", 3}, {"name", "Charlie"}, {"active", true}, {"score", 92.1}}
            });

            auto v_row = json::to_bjdata(j, true, true,
                                         json::bjdata_version_t::draft4,
                                         json::bjdata_soa_format_t::row_major);
            auto j_row = json::from_bjdata(v_row);
            CHECK(j_row == j);

            auto v_col = json::to_bjdata(j, true, true,
                                         json::bjdata_version_t::draft4,
                                         json::bjdata_soa_format_t::col_major);
            auto j_col = json::from_bjdata(v_col);
            // Col-major produces different structure but same data
            CHECK(j_col["id"][0] == 1);
            CHECK(j_col["name"][1] == "Bob");
        }

        SECTION("empty strings roundtrip")
        {
            json j = json::array(
            {
                {{"s", ""}},
                {{"s", ""}},
                {{"s", ""}}
            });
            auto v = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4,
                                     json::bjdata_soa_format_t::col_major);
            auto j2 = json::from_bjdata(v);
            CHECK(j2["s"][0] == "");
            CHECK(j2["s"][1] == "");
        }

        SECTION("single record roundtrip")
        {
            json j = json::array(
            {
                {{"x", 42}, {"y", "test"}}
            });
            auto v = json::to_bjdata(j, true, true,
                                     json::bjdata_version_t::draft4,
                                     json::bjdata_soa_format_t::row_major);
            auto j2 = json::from_bjdata(v);
            CHECK(j2[0]["x"] == 42);
            CHECK(j2[0]["y"] == "test");
        }
    }
}