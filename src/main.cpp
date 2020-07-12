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
#include "EventLoop.hpp"
#include "Display.hpp"

#include <Arduino.h>
#include <Wire.h>


/// Setup the device.
///
void setup()
{
    Wire.begin();
    EventLoop::initialize();
    Button::initialize();
    Display::initialize();

    Display::setState(Display::State::Stabilizing);
}

/// The main loop of the devive.
///
void loop()
{
    EventLoop::loop();
    delay(1);
}

