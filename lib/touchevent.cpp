// touchevent.cpp
//
// A Container Class for Touchscreen Events.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as
//   published by the Free Software Foundation; either version 2 of
//   the License, or (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include <touchevent.h>

TouchEvent::TouchEvent(Type type,int x,int y,int z)
{
  event_type=type;
  elo_x=x;
  elo_y=y;
  elo_z=z;
}


TouchEvent::Type TouchEvent::type() const
{
  return event_type;
}


int TouchEvent::x() const
{
  return elo_x;
}


int TouchEvent::y() const
{
  return elo_y;
}


int TouchEvent::z() const
{
  return elo_z;
}
