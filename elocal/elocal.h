// elocal.h
//
// A Calibration Utility for ELO Touchscreens
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
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


#ifndef ELOCAL_H
#define ELOCAL_H

#include <qwidget.h>
#include <qlabel.h>

#include <elo_driver.h>

//
// Widget Settings
//
#define ELO_TARGET_OFFSET 50
#define ELO_TARGET_SIDE 50
#define ELO_EVENTS_INTERVAL 100
#define ELO_DEFAULT_SPEED 9600
#define ELO_DEFAULT_CONTROLLER EloDriver::EloAuto
#define ELO_USAGE "USAGE: elocal [-W] [-b <tty-speed>] <tty-dev>"

class MainWidget : public QWidget
{
  Q_OBJECT
 public:
  MainWidget(QWidget *parent=0,const char *name=0);
  ~MainWidget();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void eventsData();
  void touchEventData(TouchEvent *e);
 
 private:
  bool CollectTarget(int xpos,int ypos,int *xpix,int *ypix);
  void ShowTarget(const QPoint pt);
  void HideTarget();
  QLabel *elo_target_label;
  EloDriver *elo_driver;
  QPoint elo_pixel_point[3];
  QPoint elo_touch_point[3];
  int elo_istate;
  int elo_speed;
};


#endif 
