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
#include "MotionSensors.hpp"


#include "Configuration.hpp"

#include <hal-common/event/Loop.hpp>
#include <hal-arduino-fm0-platform-io/GPIO_Pin_FeatherM0.hpp>


namespace MotionSensors {


/// A sensor state.
///
struct SensorState {
    bool currentState;
    uint32_t counter;
};

/// The possing intervals for the sensors.
///
const auto cSensorPollInterval = 20_ms;

/// The minimum duration a motion signal is held high.
///
/// This is to reduce the number of motion events.
///
const auto cMinimumMotionDuration = 1000_ms;

/// Convert this duration in polling intervals.
///
const uint32_t cMinimumMotionCount = cMinimumMotionDuration.toMilliseconds().ticks() / cSensorPollInterval.toMilliseconds().ticks();


uint32_t gStabilizingDelay = 0; ///< The counter for the initial stabilizing delay.
bool gIsReady = false; ///< The flag if the module is ready.
using gPinSensor1 = lr::GPIO::PinA0; ///< The pin connected to motion sensor 1
using gPinSensor2 = lr::GPIO::PinA3; ///< The pin connected to motion sensor 2
SensorState gSensorState1; ///< The state for motion sensor 1
SensorState gSensorState2; ///< The state for motion sensor 2
bool gCurrentMotionState; ///< The current combined state of detected motion.
MotionCallback gMotionCallback = nullptr; ///< The motion callback.


void onStabilizingCounter()
{
    if (gStabilizingDelay > 0) {
        --gStabilizingDelay;
        if (gStabilizingDelay == 0) {
            gIsReady = true;
        }
    }
}


void initializeSensorState(SensorState &state)
{
    state.currentState = false;
    state.counter = 0;
}


void updateSensorState(SensorState &state, bool input)
{
    if (state.currentState != input) {
        if (input == true) {
            state.currentState = true;
            state.counter = 0;
        } else {
            if (state.counter >= cMinimumMotionCount) {
                state.currentState = false;
                state.counter = 0;
            }
        }
    }
    if (state.counter < cMinimumMotionCount) {
        ++state.counter;
    }
}


void onPollSensors()
{
    bool detectedMotion;
    if (gIsReady) {
        updateSensorState(gSensorState1, gPinSensor1::getInput());
        updateSensorState(gSensorState2, gPinSensor2::getInput());
        detectedMotion = gSensorState1.currentState || gSensorState2.currentState;
    } else {
        detectedMotion = true;
    }
    if (gCurrentMotionState != detectedMotion) {
        gCurrentMotionState = detectedMotion;
        if (gMotionCallback != nullptr) {
            gMotionCallback(gCurrentMotionState);
        }
    }
}


Status initialize()
{
    // Initialize the variables.
    gStabilizingDelay = Configuration::stabilizingDelay.toSeconds().ticks();
    gIsReady = false;
    gCurrentMotionState = true;
    initializeSensorState(gSensorState1);
    initializeSensorState(gSensorState2);

    // Set the sensor pins as inputs.
    gPinSensor1::configureAsInput();
    gPinSensor2::configureAsInput();

    // Add the events.
    lr::event::mainLoop().addRepeatedEvent(&onStabilizingCounter, 1000_ms);
    lr::event::mainLoop().addRepeatedEvent(&onPollSensors, cSensorPollInterval);

    return Status::Success;
}


bool isReady()
{
    return gIsReady;
}


bool isMotionDetected()
{
    return gCurrentMotionState;
}


void setMotionCallback(MotionCallback callBack)
{
    gMotionCallback = callBack;
}


}


