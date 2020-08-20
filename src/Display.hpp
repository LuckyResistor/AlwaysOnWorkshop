#pragma once
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


#include "hal-tca9534/TCA9534.hpp"


/// The display module
///
class Display
{
public:
    /// The main operation state of the device.
    ///
    enum class OperationState {
        Off,             ///< Initial state with all lights off.
        Test,            ///< Initial test if all LEDs. 
        Stabilizing,     ///< The system is running, but the motion sensor is warming up.
        Running,         ///< The system is running and ready.
        Failure          ///< A fatal system failure.
    };

    /// The call status
    ///
    using Status = lr::CallStatus;

public:
    /// Create a new display module instance.
    ///
    Display(lr::WireMaster *bus);

    /// Initialize the display interface.
    ///
    Status initialize();

    /// Set the displayed state.
    ///
    void setOperationState(OperationState state);

    /// Set the off request indicator.
    ///
    void setOffRequest(bool enabled);

public:
    /// Event handler for blinking LEDs.
    ///
    void onBlinkEvent();    

private:
    lr::TCA9534 _io; ///< The interface for the IO chip.
    OperationState _state; ///< The current operation state.
    uint8_t _blinkCounter; ///< The blink counter.
    bool _offRequestEnabled; ///< The off request indicator.
};

