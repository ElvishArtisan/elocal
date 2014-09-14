// elocal.cpp
//
// A Calibration Utility for ELO Touchscreens.
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

#include <stdlib.h>

#include <qpushbutton.h>
#include <qapplication.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qmessagebox.h>

#include <elocal.h>

bool exiting=false;

MainWidget::MainWidget(QWidget *parent,const char *name)
  :QWidget(parent,name)
{
  elo_speed=ELO_DEFAULT_SPEED;
  bool elo_workaround=false;

  //
  // Process Command Switches
  //
  if(qApp->argc()<2) {
    QMessageBox::warning(this,"EloCal",ELO_USAGE);
    exiting=true;
    return;
  }
  if(qApp->argc()>2) {
    bool arg_found=false;
    for(int i=1;i<(qApp->argc()-1);i++) {
      if(!strcmp(qApp->argv()[i],"-b")) {  // Baud Rate
	if(sscanf(qApp->argv()[++i],"%d",&elo_speed)!=1) {
	  QMessageBox::warning(this,"EloCal","Invalid TTY speed.");
	  exiting=true;
	  return;
	}
	arg_found=true;
      }
      if(!strcmp(qApp->argv()[i],"-W")) {  // Baud Rate
	elo_workaround=true;
	arg_found=true;
      }
      if(!arg_found) {
	QMessageBox::warning(this,"EloCal",ELO_USAGE);
	exiting=true;
	return;
      }
    }
  }

  setCaption("EloCal -- ELO Touch Screen Calibration Utility");

  //
  // Generate Fonts
  //
  QFont button_font("Helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);

  //
  // Generate Target Image
  //
  QPixmap *pix=new QPixmap(ELO_TARGET_SIDE,ELO_TARGET_SIDE);
  QPainter *p=new QPainter(pix);
  p->setPen(QColor(red));
  p->setBrush(QColor(red));
  p->translate(ELO_TARGET_SIDE/2,ELO_TARGET_SIDE/2);
  p->eraseRect(-ELO_TARGET_SIDE/2,-ELO_TARGET_SIDE/2,
	       ELO_TARGET_SIDE,ELO_TARGET_SIDE);
  p->drawPie(-6,-6,12,12,0,5760);
  p->drawArc(-9,-9,18,18,0,5760);
  p->drawArc(-12,-12,24,24,0,5760);
  p->drawArc(-15,-15,30,30,0,5760);
  p->drawArc(-18,-18,36,36,0,5760);
  p->drawArc(-21,-21,42,42,0,5760);
  p->drawArc(-24,-24,48,48,0,5760);
  p->end();
  delete p;
  elo_target_label=new QLabel(this,"elo_target_label");
  elo_target_label->hide();
  elo_target_label->setPixmap(*pix);
  elo_target_label->setAlignment(AlignCenter);

  //
  // Touch Screen Driver
  //
  elo_driver=new EloDriver(this,"elo_driver");
  elo_driver->enableWorkaround(elo_workaround);
  elo_driver->setName(qApp->argv()[qApp->argc()-1]);
  elo_driver->setSpeed(elo_speed);
  switch(elo_driver->open()) {
  case EloDriver::Ok:
    break;

  case EloDriver::NoFile:
    QMessageBox::warning(this,"EloCal","Unable to open specified device!");
    exiting=true;
    return;

  case EloDriver::NoDevice:
    QMessageBox::warning(this,"EloCal","Specified device does not exist!");
    exiting=true;
    return;

  case EloDriver::NotTouchscreen:
    QMessageBox::warning(this,"EloCal",
			 "Specified device is not a touchscreen!");
    exiting=true;
    return;
  }

  //
  // User Instructions
  //
  QMessageBox::information(this,"EloCal","Touch each of the red targets in succession.\nIf nothing happens, simply wait for the program to time out.");

  //
  // Start Event Chain
  //
  elo_istate=0;
  QTimer *timer=new QTimer(this,"event_timer");
  connect(timer,SIGNAL(timeout()),this,SLOT(eventsData()));
  timer->start(ELO_EVENTS_INTERVAL);
}


MainWidget::~MainWidget()
{
}


QSize MainWidget::sizeHint() const
{
  return QSize(280,160);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::eventsData()
{
  static int timeout=0;
  switch(elo_istate) {
      case 0:  // Put up first (dummy) target
	elo_pixel_point[0].setX(frameGeometry().width()/2);
	elo_pixel_point[0].setY(frameGeometry().height()/2);
	elo_pixel_point[1].setX(ELO_TARGET_OFFSET);
	elo_pixel_point[1].setY(ELO_TARGET_OFFSET);
	elo_pixel_point[2].setX(frameGeometry().width()-ELO_TARGET_OFFSET);
	elo_pixel_point[2].setY(frameGeometry().height()-ELO_TARGET_OFFSET);
	connect(elo_driver,SIGNAL(touchReleaseEvent(TouchEvent *)),
		this,SLOT(touchEventData(TouchEvent *)));
	ShowTarget(elo_pixel_point[0]);
	elo_istate=1;
	break;

      case 2:  // Put up first target
	ShowTarget(elo_pixel_point[1]);
	elo_istate=3;
	break;

      case 4:  // Put up second target
	ShowTarget(elo_pixel_point[2]);
	elo_istate=5;
	timeout=0;
	break;

      case 6:  // Display results and exit
	elo_istate=-1;
	HideTarget();
	double xslope,yslope;
	double min_x,max_x;
	double min_y,max_y;

	if(elo_touch_point[1].x()>elo_touch_point[2].x()) {
	  xslope=(double)(elo_touch_point[1].x()-elo_touch_point[2].x())/
	    (double)(elo_pixel_point[1].x()-elo_pixel_point[2].x());
	  min_x=(double)elo_touch_point[1].x()-50.0*xslope;
	  max_x=(double)elo_touch_point[2].x()+50.0*xslope;
	}
	else {
	  xslope=(double)(elo_touch_point[1].x()-elo_touch_point[2].x())/
	    (double)(elo_pixel_point[1].x()-elo_pixel_point[2].x());
	  max_x=(double)elo_touch_point[1].x()-50.0*xslope;
	  min_x=(double)elo_touch_point[2].x()+50.0*xslope;
	}
	yslope=(double)(elo_touch_point[1].y()-elo_touch_point[2].y())/
	    (double)(elo_pixel_point[1].y()-elo_pixel_point[2].y());
	max_y=(double)elo_touch_point[1].y()-50.0*yslope;
	min_y=(double)elo_touch_point[2].y()+50.0*yslope;

	QMessageBox::information(this,"EloCal",QString().
	  sprintf("CALIBRATION VALUES\nMaxX: %d\nMinX: %d\nMaxY: %d\nMinY: %d",
			    (int)max_x,(int)min_x,
			    (int)max_y,(int)min_y));
	exit(0);
	break;
  }

  if((timeout++>(10000/ELO_EVENTS_INTERVAL))&&(elo_istate!=-1)) {
    elo_istate=-1;
    QMessageBox::information(this,"EloCal",
			     "Operation timed out!\nClick OK to exit.");
    exit(1);
  }
}


void MainWidget::touchEventData(TouchEvent *e)
{
  switch(elo_istate) {
      case 1:  // Wait for first target, but throw it away
	elo_istate=2;
	break;

      case 3:  // Wait for first target
	elo_touch_point[1].setX(e->x());
	elo_touch_point[1].setY(e->y());
	elo_istate=4;
	break;

      case 5:  // Wait for second target
	elo_touch_point[2].setX(e->x());
	elo_touch_point[2].setY(e->y());
	elo_istate=6;
	break;
  }
}


void MainWidget::ShowTarget(const QPoint pt)
{
  elo_target_label->setGeometry(pt.x()-ELO_TARGET_SIDE/2,
				pt.y()-ELO_TARGET_SIDE/2,
				ELO_TARGET_SIDE,ELO_TARGET_SIDE);
  elo_target_label->show();
}


void MainWidget::HideTarget()
{
  elo_target_label->hide();
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);
  
  MainWidget *w=new MainWidget(NULL,"main");
  if(exiting) {
    exit(1);
  }
  a.setMainWidget(w);
  w->showMaximized();
  return a.exec();
}


