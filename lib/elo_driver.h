// elo_driver.h
//
// A Calibration Utility for ELO Touchscreens
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


#ifndef ELO_DRIVER_H
#define ELO_DRIVER_H

#include <linux/input.h>

#include <qobject.h>
#include <qtimer.h>

#include <rttydevice.h>

#include <touchevent.h>

#define ELO_DRIVER_SCAN_INTERVAL 100
//#define ELO_DRIVER_RAW_DUMP
#define ELO_DRIVER_VENDOR_ID 1255
#define ELO_DRIVER_PRODUCT_ID_1 7
#define ELO_DRIVER_PRODUCT_ID_2 8
#define ELO_DRIVER_PRODUCT_ID_3 32

class EloDriver : public QObject
{
  Q_OBJECT
 public:
  enum Controller {Elo2500S=0,Elo2500U=1,EloAuto=255};
  enum Error {Ok=0,NoFile=1,NoDevice=2,NotTouchscreen=3};
  EloDriver(QObject *parent=0,const char *name=0);
  ~EloDriver();
  void enableWorkaround(bool state);
  void setName(QString dev);
  void setSpeed(int speed);
  EloDriver::Controller controller() const;
  EloDriver::Error open();
  void close();

 private slots:
  void timerData();

 signals:
  void touchPressEvent(TouchEvent *e);
  void touchReleaseEvent(TouchEvent *e);

 private:
  void ReadTty();
  void ReadInput();
  EloDriver::Controller elo_controller;
  RTTYDevice *elo_tty;
  QTimer *elo_timer;
  int elo_fd;
  QString elo_name;
  int elo_xpos;
  int elo_ypos;
  int elo_zpos;
  bool elo_touched;
  bool elo_workaround;
};


#endif  // ELO_DRIVER_H
