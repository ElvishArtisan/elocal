// touchevent.h
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


#ifndef TOUCHEVENT_H
#define TOUCHEVENT_H


class TouchEvent
{
 public:
  enum Type {TouchPress=1,TouchRelease=2,TouchUnknown=255};
  TouchEvent(Type type,int x,int y,int z);
  TouchEvent::Type type() const;
  int x() const;
  int y() const;
  int z() const;

 private:
  TouchEvent::Type event_type;
  int elo_x;
  int elo_y;
  int elo_z;
};


#endif  // TOUCHEVENT_H
