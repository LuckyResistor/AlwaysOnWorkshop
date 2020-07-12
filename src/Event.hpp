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


#include <Arduino.h>


/// Represets a single event.
///
class Event
{
public:
    /// The event function.
    ///
    typedef void (*Function)();

public:
    /// Create a new empty event.
    ///
    Event();

    /// Create a new delayed event.
    ///
    Event(Function call, uint32_t next);

    /// Copy an event.
    ///
    Event(const Event&) = default;

    /// Assign an event.
    ///
    Event& operator=(const Event&) = default;

public:
    /// Check if the event is invalid/empty.
    ///
    bool isValid() const;

    /// Check if the event is ready to be executed.
    ///
    bool isReady(uint32_t currentTime) const;

    /// Get the call function for this event.
    ///
    Function getCall() const;

    /// Clear this event.
    ///
    void clear();

private:
    Function _call; ///< The call.
    uint32_t _next; ///< The next timestamp for this event.
};

