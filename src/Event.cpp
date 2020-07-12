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
#include "Event.hpp"


Event::Event()
    : _call(nullptr), _next(0)
{
}


Event::Event(Function call, uint32_t next)
    : _call(call), _next(next)
{   
}


bool Event::isValid() const
{
    return _call != nullptr;
}


bool Event::isReady(uint32_t currentTime) const
{
    const auto delta = static_cast<int32_t>(_next - currentTime);
    return delta <= 0;  
}


Event::Function Event::getCall() const
{
    return _call;
}


void Event::clear()
{
    _call = nullptr;
}
