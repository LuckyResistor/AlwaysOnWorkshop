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


#include <hal-common/Duration.hpp>


namespace Configuration {


/// The required stabilizing delay for the used motion sensors.
///
const auto stabilizingDelay = 12_s;

/// The regular duration the light is kept on with no motion detected.
///
const auto normalLightOnDuration = 1_hours;

/// The short duration the light is kept on after the light button is pressed.
///
const auto offRequestLightOnDuration = 30_s;

/// The duration the off-request is kept with motion in the room.
/// After this duration, the system clears the off-request.
///
const auto offRequestDuration = 5_minutes;


}

