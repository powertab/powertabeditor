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

TEST_CASE("App/SettingsManager", "")
{
    SettingsManager manager;

    int count = 0;
    manager.subscribeToChanges([&]() { ++count; });

    {
        auto settings = manager.getWriteHandle();
        settings->set("foo", 42);
        settings->set("bar", "asdf");
    }

    {
        auto settings = manager.getReadHandle();
        REQUIRE(settings->get<int>("foo") == 42);
    }

    REQUIRE(count == 1);
}
