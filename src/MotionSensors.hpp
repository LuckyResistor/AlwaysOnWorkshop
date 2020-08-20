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


#include <hal-common/StatusTools.hpp>


namespace MotionSensors {


/// The status of function calls.
///
using Status = lr::CallStatus;

/// The callback function to receive motion events.
///
using MotionCallback = void(*)(bool motionDetected);


/// Initialize the motion sensors module.
///
Status initialize();

/// Check if the motion sensors are ready and return reliable readings.
///
bool isReady();

/// Check if there is motion.
///
bool isMotionDetected();

/// Register a callback for motion events.
///
void setMotionCallback(MotionCallback callBack);


}

