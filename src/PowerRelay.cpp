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
#include "PowerRelay.hpp"



#include <hal-arduino-fm0-platform-io/GPIO_Pin_FeatherM0.hpp>


namespace PowerRelay {


/// The pin to control the power relay.
///
using gPowerRelayPin = lr::GPIO::Pin6;

/// The current state of the relay.
///
State gState;


void initialize()
{
    gPowerRelayPin::setOutputLow();
    gPowerRelayPin::configureAsOutput();
    gState = State::Off;
}


void setState(State state)
{
    if (state != gState) {
        gState = state;
        if (state == State::On) {
            gPowerRelayPin::setOutputHigh();
        } else {
            gPowerRelayPin::setOutputLow();
        }
    }
}


}

