/*
  * Copyright (C) 2015 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
  
#include <catch.hpp>

#include <app/settingsmanager.h>

static const std::string theKey = "foo/bar";

TEST_CASE("App/SettingsManager/Int")
{
    SettingsManager settings;

    REQUIRE(settings.get(theKey, -1) == -1);
    settings.set(theKey, 1);
    REQUIRE(settings.get(theKey, -1) == 1);
    settings.set(theKey, 2);
    REQUIRE(settings.get(theKey, -1) == 2);
    REQUIRE_THROWS(settings.get<std::string>(theKey));
}

TEST_CASE("App/SettingsManager/String")
{
    SettingsManager settings;

    settings.set(theKey, "asdf");
    REQUIRE(settings.get<std::string>(theKey) == "asdf");
    REQUIRE_THROWS(settings.get<int>(theKey, -1));
}

TEST_CASE("App/SettingsManager/StringList")
{
    SettingsManager settings;

    std::vector<std::string> strings = {"abc", "def"};

    for (int i = 0; i < 3; ++i)
        settings.setList(theKey, strings);

    REQUIRE(settings.getList<std::string>(theKey) == strings);
}

TEST_CASE("App/SettingsManager/IntList")
{
    SettingsManager settings;

    std::vector<int> vals = {1, 2, 3};
    settings.setList(theKey, vals);

    REQUIRE(settings.getList<int>(theKey) == vals);
}

TEST_CASE("App/SettingsManager/Paths")
{
    SettingsManager settings;

    settings.set("foo", 1);
    REQUIRE(settings.get("foo", -1) == 1);
    settings.set("foo/bar", 2);
    settings.set("foo/baz", "asdf");
    REQUIRE(settings.get("foo/bar", -1) == 2);
    REQUIRE(settings.get<std::string>("foo/baz") == "asdf");
    REQUIRE_THROWS(settings.get("foo", -1));
}

TEST_CASE("App/SettingsManager/JSON")
{
    SettingsManager settings;

    settings.set("key_a", -123);
    settings.setList("key_b", std::vector<int>({1, 2, 3}));
    settings.setList(
        "key_c", std::vector<std::string>({ "string1", "string2", "string3" }));
    settings.set("parent/child_a", 234);
    settings.set("parent/child_b", "asdf");

    std::ostringstream output;
    settings.save(output);
    REQUIRE(output.str() ==
R"({
    "key_a": -123,
    "key_b": [
        1,
        2,
        3
    ],
    "key_c": [
        "string1",
        "string2",
        "string3"
    ],
    "parent": {
        "child_a": 234,
        "child_b": "asdf"
    }
})");
}
