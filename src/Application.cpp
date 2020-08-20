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
#include "Application.hpp"


#include "Button.hpp"
#include "Configuration.hpp"
#include "Display.hpp"
#include "MotionSensors.hpp"
#include "PowerRelay.hpp"
#include "SerialTrace.hpp"

#include "hal-common/Timer.hpp"
#include "hal-common/event/Loop.hpp"
#include "hal-common/Watchdog.hpp"
#include "hal-arduino-fm0-platform-io/WireMaster_FeatherM0.hpp"
#include "hal-arduino-fm0-platform-io/GPIO_Pin_FeatherM0.hpp"

#include <Arduino.h>


namespace Application {


/// The application state.
///
enum class State {
    Stabilizing, ///< Waiting for the motion sensor to stabilize.
    NoMotion, ///< Running, but no motion detected, light is off.
    Motion, ///< Running, motion was detected, light is on.
    MotionOffRequest, ///< The user requested to turn the light off.
};


/// The main loop for this application.
///
lr::event::BasicLoop<lr::event::StaticStorage<16>> gLoop;

/// The main wire connection.
///
lr::WireMaster_FeatherM0 gMainWire;

/// The display module.
///
Display gDisplay(&gMainWire);

/// The pin with the status LED.
///
using gPinStatusLed = lr::GPIO::Pin13;


/// The application state.
///
State gState;

/// A counter for the delay until the power is switched on.
///
uint8_t gPowerOnDelay;

/// A counter to count seconds until the light is turned off.
///
uint32_t gPowerOffCounter;

/// A counter to limit the duration of the off request.
///
uint32_t gOffRequestCounter;


/// Reset the motion counter
///
void resetMotionCounter()
{
    if (SerialTrace::isEventLoggingActivated()) {
        gSerial.writeLine("reset motion counter");
    }
    if (gState == State::MotionOffRequest) {
        gPowerOffCounter = Configuration::offRequestLightOnDuration.toSeconds().ticks();
    } else {
        gPowerOffCounter = Configuration::normalLightOnDuration.toSeconds().ticks();
    }
}

/// Turn the light on because of motion or button press.
///
void turnLightOn()
{
    if (SerialTrace::isEventLoggingActivated()) {
        gSerial.writeLine("turn light on");
    }
    gState = State::Motion;
    resetMotionCounter();
    PowerRelay::setState(PowerRelay::State::On);
}

/// Turn the light off.
///
void turnLightOff()
{
    if (SerialTrace::isEventLoggingActivated()) {
        gSerial.writeLine("turn light off");
    }
    gState = State::NoMotion;
    PowerRelay::setState(PowerRelay::State::Off);
    gDisplay.setOffRequest(false);
}

/// Request to turn the lights off.
///
void requestLightOff()
{
    if (SerialTrace::isEventLoggingActivated()) {
        gSerial.writeLine("request light off");
    }
    gState = State::MotionOffRequest;
    resetMotionCounter();
    gOffRequestCounter = Configuration::offRequestDuration.toSeconds().ticks();
    gDisplay.setOffRequest(true);
}

/// Failure handling.
///
void onFailure()
{
    while (true) {
        gPinStatusLed::setOutputHigh();
        lr::Timer::delay(750_ms);
        gPinStatusLed::setOutputLow();
        lr::Timer::delay(750_ms);
    }
}

/// Handling a motion event.
///
void onMotion(bool hasMotion)
{
    if (hasMotion) {
        if (SerialTrace::isEventLoggingActivated()) {
            gSerial.writeLine("motion detected");
        }
        if (gState == State::NoMotion) {
            turnLightOn();
        } else if (gState == State::Motion || gState == State::MotionOffRequest) {
            resetMotionCounter();
        }
    }
}

/// Handling a button press.
///
void onButtonPress(Button::Press buttonPress)
{
    if (buttonPress == Button::Press::Short) {
        if (SerialTrace::isEventLoggingActivated()) {
            gSerial.writeLine("short button press");
        }
        if (gState == State::NoMotion) {
            turnLightOn();
        } else if (gState == State::Motion) {
            requestLightOff();
        }
    }
}


/// Wait for stabilizing.
///
void onWaitForStabilizing()
{
    if (gPowerOnDelay > 0) {
        --gPowerOnDelay;
        if (gPowerOnDelay == 0) {
            PowerRelay::setState(PowerRelay::State::On);
            if (SerialTrace::isEventLoggingActivated()) {
                gSerial.writeLine("turn light on after delay");
            }
        }
    } else if (MotionSensors::isReady()) {
        if (SerialTrace::isEventLoggingActivated()) {
            gSerial.writeLine("motion sensor ready");
        }
        gDisplay.setOperationState(Display::OperationState::Running);
        turnLightOn();
        return; // Do not repeat this event.
    }

    gLoop.addDelayedEvent(&onWaitForStabilizing, 200_ms, false);
}


/// Regular ticks while the device is in running state.
///
void onRunningTick()
{
    // clear the watchdog.
    lr::Watchdog::clearWatchdog();

    // Wait for no motion.
    if (gState == State::Motion || gState == State::MotionOffRequest) {
        if (MotionSensors::isMotionDetected()) {
            resetMotionCounter();
        } else {
            --gPowerOffCounter;
            if (SerialTrace::isEventLoggingActivated() && (gPowerOffCounter & 0x000f) == 0) {
                gSerial.write("power off in ");
                gSerial.write(lr::String::number(gPowerOffCounter));
                gSerial.writeLine(" seconds");
            }
            if (gPowerOffCounter == 0) {
                turnLightOff();
            }
        }
    }

    // Limit the duration of the off request.
    if (gState == State::MotionOffRequest)  {
        --gOffRequestCounter;
        if (SerialTrace::isEventLoggingActivated() && (gOffRequestCounter & 0x000f) == 0) {
            gSerial.write("off request ends in ");
            gSerial.write(lr::String::number(gOffRequestCounter));
            gSerial.writeLine(" seconds");
        }
        if (gOffRequestCounter == 0) {
            gState = State::Motion;
            resetMotionCounter();
            gDisplay.setOffRequest(false);
        }
    }
}


void initialize()
{
    // Initialize the globals.
    gState = State::Stabilizing;
    gPowerOnDelay = 20;
    gPowerOffCounter = 0;
    gOffRequestCounter = 0;

    // Initialize all modules.
    lr::Watchdog::initialize();
    SerialTrace::initialize();
    gPinStatusLed::configureAsOutput();
    gPinStatusLed::setOutputLow();
    if (hasError(gMainWire.initialize())) {
        gSerial.writeLine("failed initialize i2c");
        onFailure();
    }
    lr::Watchdog::clearWatchdog();
    Button::initialize();
    if (hasError(gDisplay.initialize())) {
        gSerial.writeLine("failed initialize display");
        onFailure();
    };
    lr::Watchdog::clearWatchdog();
    PowerRelay::initialize();
    if (hasError(MotionSensors::initialize())) {
        gSerial.writeLine("failed initialize motion");
        onFailure();
    }

    // Blink the LEDs for testing.
    lr::Watchdog::clearWatchdog();
    gPinStatusLed::setOutputHigh();
    gDisplay.setOperationState(Display::OperationState::Test);
    lr::Timer::delay(200_ms);
    gPinStatusLed::setOutputLow();
    gDisplay.setOperationState(Display::OperationState::Stabilizing);

    // Go into stabilizing state and start all events and callbacks.
    lr::Watchdog::clearWatchdog();
    MotionSensors::setMotionCallback(&onMotion);
    Button::setCallback(&onButtonPress);
    gLoop.addRepeatedEvent(&onRunningTick, 1000_ms);
    gLoop.addDelayedEvent(&onWaitForStabilizing, 200_ms, false);
}


void loop()
{
    gLoop.loopOnce();
}


}


