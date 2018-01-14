/*
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

#include <actions/removespecialnoteproperty.h>
#include <score/note.h>
#include "actionfixture.h"

TEST_CASE_METHOD(ActionFixture, "Actions/RemoveLeftHandFingering", "")
{
    LeftHandFingering fingering(LeftHandFingering::Finger::Middle,
        LeftHandFingering::DisplayPosition::Below);
    myLocation.getNote()->setLeftHandFingering(fingering);
    
    RemoveLeftHandFingering action(myLocation);
    
    action.redo();
    REQUIRE(!myLocation.getNote()->hasLeftHandFingering());
    
    action.undo();
    REQUIRE(myLocation.getNote()->hasLeftHandFingering());
    REQUIRE(myLocation.getNote()->getLeftHandFingering() == fingering);
}
