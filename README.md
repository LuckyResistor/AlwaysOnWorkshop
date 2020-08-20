Always-On Workshop
==================

This is a very simple (demo) implementation for the Always-On platform. The idea is to use it in an Office or Workshop with no Windows, where light needs to be turned on as soon any motion is detected.

It is meant as a starting point to develop own implementations, which make use of more sensors and/or Feather boards with more (network) capabilities.

You find complete circuit diagrams for the setup in the project description. These diagrams show all possible chips and connections and desribe the bus. 

Setup For This Project
----------------------

- Always-On Board with:
  - Feather M0 Basic Proto
  - Electronic relay connected to the workshop lights
  - Optocopupler connected to the light switch in push-button configuration.
  - FRAM and RTC omitted
- 2x Motion Sensor Board with:
  - Panasonic PIR sensor.
  - Connected as S1 and S2
- 1x Status display board.
  - With four LEDs
  - Connected to SDA/SCL.

Mode of Operation
-----------------

### Startup

On initial boot, the firmware assumes the reset was caused because of a power loss.

All attached LEDs are flashed on for 200ms to allow visually testing them.

The used PIR sensors require a warm-up period before they provide a reliable reading. Depending on the sensor this can be anything between 10 seconds and 210 seconds. The `Sensor` module has a `isReady()` method, to indicate if the readings are reliable. 

Therefore, after a short delay of 2-3 seconds, the lights are turned on - until the readings from the sensors stabilize. The reasons to turn the lights initially on are:

- After the power is coming back, emegency lights are usually turned off - therefore any person in the room wouldn't like to be in the dark.
- If no light is required, or no person is present, it will automatically turned off, after the configured delay.
- A power loss should be a rare event.

The delay to turn the lights on is there, to prevent adding to the usual power-surge, caused by various other devices attached to the same phase.

There is a small downside to this behaviour, in the case the attached light caused the power loss through a malfunction. In this rare situation, it will make the process of restoring the power more difficult. But, in this situation, repairing the light installation should be prioritized anyway. 

On startup, while waiting for the sensors to stabilize, the green (first) LED is blinking to indicate this state. As soon the normal operation is reached, the green LED is on permanently.

### Normal Operation

In normal operation, any detected movements by the sensors will turn the lights on. If the light button is pressed, while the lights are turned off, this has the same effect as motion is detected.

If no movement is detected anymore, the lights are turned off after a long delay. This delay is set to at least, one full hour to prevent the lights from turned off if a person sits almost motionless in the room. 

If the lights are turned on, and the light button is pressed, the delay for turning the lights off is reduced to one minute. This state is indicated by the orange LED on the display. After the lights are turned off, this state is reset and the LED is turned off.

### Failures

If there is a component failure while initializing the device. The status LED on the Feather board is blinking.

Any other failure states are indicated by blinking the red (fourth) LED. In this version of the firmware, there is no failure possible, to indicate this situation.

Suggestions for Extensions
--------------------------

- Add a ambient light sensor to use it in rooms with windows. The example board for motion sensors already has pads for a good one.
- Use a RTC to turn on light only after dawn, using a calculated light level value from date and time.
- Add a RTC to log any motion events. Into FRAM accessible over the serial line, or to a SD-card.
- Connect the serial input/output (RX/TX) to e.g. an Raspberry-Pi for event logging and/or more control over the configuration (board layout change required).
- Connect sensor line `S3` with a security cameras in the room, either as output or input.
- Use sensor line `S3` (with an optocoupler for safety) to connect multiple Always-On boards in larger rooms/houses/installations.
- Add a door sensor and connect it to sensor line `S3`.
- Use a Bluetooth baord to turn the lights on/off, based on detected presence Bluetooth beacons.

Note about Ambient Light Sensors
--------------------------------

If the ambient light sensor is affected by the in-room-light, the difficult part is to implement the following two events:

- Simple: Motion started in the evening, and the light fades, so at one point the lights need to be turned on because of low light.
- Tricky: Motion started in the morning, light was turned on, but the ambient light from outside would be enough to allow turning the light off.

The simplest solution is to mount the ambient light sensor at a place where it isn't affected by the in-room-lights.

Also thunderstorms etc. may lead to fast changes in ambient light levels, which shouldn't lead to fast in-room-light flickering.

Copyright & License
-------------------

Copyright (C)2020 Lucky Resistor

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
