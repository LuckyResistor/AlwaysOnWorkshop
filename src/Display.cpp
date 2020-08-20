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
#include "Display.hpp"


#include "hal-common/event/Loop.hpp"


using namespace lr;


const auto cLed1 = lr::TCA9534::Port0;
const auto cLed2 = lr::TCA9534::Port1;
const auto cLed3 = lr::TCA9534::Port2;
const auto cLed4 = lr::TCA9534::Port3;


Display *gInstance = nullptr;


Display::Display(WireMaster *bus)
:
    _io(bus, TCA9534::Address0),
    _state(OperationState::Off),
    _blinkCounter(0),
    _offRequestEnabled(false)
{
    gInstance = this;
}


Display::Status Display::initialize()
{
    if (hasError(_io.initialize())) {
        return Status::Error;
    }
    event::mainLoop().addRepeatedEvent([]{
        gInstance->onBlinkEvent();
    }, 750_ms);
    return Status::Success;
}


void Display::setOperationState(OperationState state)
{
    if (_state != state) {
        _state = state;
        switch (_state) {
        case OperationState::Off:
            _io.setMode(cLed1, TCA9534::Mode::HighZ);
            _io.setMode(cLed2, TCA9534::Mode::HighZ);
            _io.setMode(cLed3, TCA9534::Mode::HighZ);
            _io.setMode(cLed4, TCA9534::Mode::HighZ);
            break;
        case OperationState::Test:
            _io.setMode(cLed1, TCA9534::Mode::Output_Low);
            _io.setMode(cLed2, TCA9534::Mode::Output_Low);
            _io.setMode(cLed3, TCA9534::Mode::Output_Low);
            _io.setMode(cLed4, TCA9534::Mode::Output_Low);
            break;
        case OperationState::Stabilizing:
            _io.setMode(cLed1, TCA9534::Mode::Output_Low);
            _io.setMode(cLed2, TCA9534::Mode::HighZ);
            _io.setMode(cLed3, TCA9534::Mode::HighZ);
            _io.setMode(cLed4, TCA9534::Mode::HighZ);
            break;
        case OperationState::Running:
            _io.setMode(cLed1, TCA9534::Mode::Output_Low);
            break;
        case OperationState::Failure:
            _io.setMode(cLed4, TCA9534::Mode::Output_Low);
            break;
        }
    }
}


void Display::setOffRequest(bool enabled)
{
    if (_offRequestEnabled != enabled) {
        _offRequestEnabled = enabled;
        if (enabled) {
            _io.setMode(cLed2, TCA9534::Mode::Output_Low);
        } else {
            _io.setMode(cLed2, TCA9534::Mode::HighZ);
        }
    }
}


void Display::onBlinkEvent()
{
    ++_blinkCounter;
    if (_state == OperationState::Stabilizing) {
        if ((_blinkCounter & 1) == 0) {
            _io.setMode(cLed1, TCA9534::Mode::HighZ);
        } else {
            _io.setMode(cLed1, TCA9534::Mode::Output_Low);
        }
    } else if (_state == OperationState::Failure) {
        if ((_blinkCounter & 1) == 0) {
            _io.setMode(cLed4, TCA9534::Mode::HighZ);
        } else {
            _io.setMode(cLed4, TCA9534::Mode::Output_Low);
        }
    }
    if (_offRequestEnabled) {
        if ((_blinkCounter & 1) == 0) {
            _io.setMode(cLed2, TCA9534::Mode::HighZ);
        } else {
            _io.setMode(cLed2, TCA9534::Mode::Output_Low);
        }
    }
}

