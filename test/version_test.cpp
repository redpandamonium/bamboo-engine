// Bamboo Engine, a 3D game engine.
// Copyright (C) 2020 Leon Suchy
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// Created by Leon Suchy on 04.08.20.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <bamboo_engine/util/version.hpp>

using namespace bbge;

TEST(version, parse_empty) {

    version v = version::from_string("");
    ASSERT_EQ(v.get_major(), 0);
    ASSERT_TRUE(v.has_minor());
    ASSERT_EQ(v.get_minor(), 0);
    ASSERT_FALSE(v.has_patch());
}

TEST(version, parse_no_patch) {

    version v = version::from_string("1.2");
    ASSERT_EQ(v.get_major(), 1);
    ASSERT_TRUE(v.has_minor());
    ASSERT_EQ(v.get_minor(), 2);
    ASSERT_FALSE(v.has_patch());
}

TEST(version, parse_patch) {

    version v = version::from_string("1.2.3");
    ASSERT_EQ(v.get_major(), 1);
    ASSERT_TRUE(v.has_minor());
    ASSERT_EQ(v.get_minor(), 2);
    ASSERT_TRUE(v.has_patch());
    ASSERT_EQ(v.get_patch(), 3);
}

TEST(version, trailing_dot) {
    ASSERT_THROW(version::from_string("1.2."), std::invalid_argument);
}

TEST(version, too_many_components) {
    ASSERT_THROW(version::from_string("1.2.3.4"), std::invalid_argument);
}

TEST(version, empty_components) {
    ASSERT_THROW(version::from_string(".."), std::invalid_argument);
}

TEST(version, constexpr_ctor) {

    // these only have to compile.
    constexpr const version v1 (1, 2, 3);
    constexpr const version v2;
    constexpr const version v3(1, 2);
}