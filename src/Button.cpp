//
// (c)2020 by Lucky Resistor. See LICENSE for details.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
#include "Button.hpp"


#include <hal-common/event/Loop.hpp>
#include <hal-arduino-fm0-platform-io/GPIO_Pin_FeatherM0.hpp>

#include <limits>


namespace Button {


/// The type for the button press counter.
///
using ButtonPressCounter = uint8_t;

/// The count limit for long button presses.
///
const ButtonPressCounter cLongButtonPressCount = 40;


/// The pin where the button signal is attached.
///
using gButtonPin = lr::GPIO::Pin5;

/// The callback function.
///
Callback gCallback = nullptr;

/// The counter to the press duration.
///
ButtonPressCounter gButtonPressCounter = 0;

/// The last button state.
///
bool gLastButtonState = false;


void pollState()
{
    const bool buttonState = !(gButtonPin::getInput()); // button low if pressed.
    if (buttonState != gLastButtonState) {
        gLastButtonState = buttonState;
        if (buttonState) {
            gButtonPressCounter = 0;
        } else {
            // Call the events after the button is released.
            if (gCallback) {
                if (gButtonPressCounter < cLongButtonPressCount) {
                    gCallback(Press::Short);
                } else {
                    gCallback(Press::Long);
                }
            }
        }
    }
    // Count while the button is pressed.
    if (gLastButtonState) {
        if (gButtonPressCounter < std::numeric_limits<ButtonPressCounter>::max()) {
            ++gButtonPressCounter;
        }
    }
}


void initialize()
{
    gButtonPin::configureAsInput();
    lr::event::mainLoop().addRepeatedEvent(&pollState, 50_ms);
}


void setCallback(Callback callback)
{
    gCallback = callback;
}


}
