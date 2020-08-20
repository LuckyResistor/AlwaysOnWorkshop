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
#include "SerialTrace.hpp"


#include "hal-common/Timer.hpp"
#include "hal-arduino-fm0-platform-io/GPIO_Pin_FeatherM0.hpp"
#include "hal-arduino-platform-io/SerialLine_Arduino1.hpp"


namespace SerialTrace {


/// Wrapper around Serial1.
///
lr::SerialLine_Arduino1 gSerialDevice1;

/// The serial string writer (unbuffered).
///
lr::SerialLineStringWriter gSerial(&gSerialDevice1);

/// The config pin 1
///
using gConfigPin1 = lr::GPIO::Pin10;


/// If event logging shall be activated.
///
bool gEnableEventLogging = false;


void initialize()
{
    gSerialDevice1.initialize(115200);
    gSerial.writeLine("device start");
    gConfigPin1::configureAsInput(lr::GPIO::Pull::Up);
    // Capture the initial configuration.
    lr::Timer::delay(100_ms);
    gEnableEventLogging = !gConfigPin1::getInput();
}


bool isEventLoggingActivated()
{
    return gEnableEventLogging;
}


}

