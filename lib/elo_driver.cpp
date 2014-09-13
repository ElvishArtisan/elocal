// elo_driver.cpp
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


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <sys/utsname.h>

#include <elo_driver.h>

EloDriver::EloDriver(QObject *parent,const char *name)
    : QObject(parent,name)
{
  elo_workaround=false;

  //
  // Scan Timer
  //
  elo_timer=new QTimer(this,"elo_timer");
  connect(elo_timer,SIGNAL(timeout()),this,SLOT(timerData()));

  //
  // TTY Device
  //
  elo_tty=new RTTYDevice();
}


EloDriver::~EloDriver()
{
}


void EloDriver::enableWorkaround(bool state)
{
  elo_workaround=state;
}


void EloDriver::setName(QString dev)
{
  elo_name=dev;
  elo_tty->setName(dev);
}


void EloDriver::setSpeed(int speed)
{
  elo_tty->setSpeed(speed);
}


EloDriver::Error EloDriver::open()
{
  struct termios term;
  unsigned ev_version;
  struct input_id id;
  struct input_absinfo absinfo;
  struct utsname info;
  int count=0;
  int major;
  int minor;
  int offset;

  //
  // What kind of controller are we talking to?
  //
  if((elo_fd=::open((const char *)elo_name,O_RDONLY|O_NONBLOCK))<0) {
    switch(errno) {
	case -ENODEV:
	  return EloDriver::NoDevice;

	default:
	  return EloDriver::NoFile;
    }
  }
  if(tcgetattr(elo_fd,&term)==0) {   // Serial Controller
    elo_controller=EloDriver::Elo2500S;
    ::close(elo_fd);
  }
  else {
    if(ioctl(elo_fd,EVIOCGVERSION,&ev_version)==0) {  // USB Controller
      ioctl(elo_fd,EVIOCGID,&id);
      printf("VendorID: %d  ProductID: %d\n",id.vendor,id.product);
      if((id.bustype!=BUS_USB)||(id.vendor!=ELO_DRIVER_VENDOR_ID)||
	 ((id.product!=ELO_DRIVER_PRODUCT_ID_1)&&
	  (id.product!=ELO_DRIVER_PRODUCT_ID_2)&&
	  (id.product!=ELO_DRIVER_PRODUCT_ID_3))) {
	::close(elo_fd);
	return EloDriver::NotTouchscreen;
      }
      elo_controller=EloDriver::Elo2500U;
    }
    else {
      ::close(elo_fd);
      return EloDriver::NotTouchscreen;
    }
  }
  elo_tty->open(IO_ReadOnly);
  elo_xpos=-1;
  elo_ypos=-1;
  elo_zpos=-1;
  elo_touched=false;
  elo_timer->start(ELO_DRIVER_SCAN_INTERVAL);
  return EloDriver::Ok;
}


EloDriver::Controller EloDriver::controller() const
{
  return elo_controller;
}


void EloDriver::close()
{
  elo_timer->stop();
  elo_tty->close();
}


void EloDriver::timerData()
{
  switch(elo_controller) {
      case EloDriver::Elo2500S:
	ReadTty();
	break;

      case EloDriver::Elo2500U:
	ReadInput();
	break;
  }
}


void EloDriver::ReadTty()
{
  char buf[256];
  int n;
  TouchEvent::Type type;
  int xpos;
  int ypos;
  int zpos;
  static int istate=0;
  TouchEvent *e=NULL;

  while((n=elo_tty->readBlock(buf,255))>0) {
    for(int i=0;i<n;i++) {
#ifdef ELO_DRIVER_RAW_DUMP
      printf("Elo Code: 0x%02X\n",buf[i]&0xff);
#endif  // ELO_DRIVER_RAW_DUMP
      switch(istate) {
	  case 0:
	    if(buf[i]==0x55) {
	      istate=1;
	    }
	    else {
	      istate=0;
	    }
	    break;
	    
	  case 1:
	    if(buf[i]==0x54) {
	      istate=2;
	    }
	    else {
	      istate=0;
	    }
	    break;
	    
	  case 2:   // Touch Type
	    switch(buf[i]&0x06) {
		case 0x02:
		  type=TouchEvent::TouchPress;
		  break;
		  
		case 0x04:
		  type=TouchEvent::TouchRelease;
		  break;
		  
		default:
		  type=TouchEvent::TouchUnknown;
		  break;
	    }
	    istate=3;
	    break;
	    
	  case 3:
	    xpos=0xff&buf[i];
	    istate=4;
	    break;
	    
	  case 4:	      
	    xpos+=(256*(0xff&buf[i]));
	    istate=5;
	    break;
	    
	  case 5:
	    ypos=0xff&buf[i];
	    istate=6;
	    break;
	    
	  case 6:
	    ypos+=(256*(0xff&buf[i]));
	    istate=7;
	    break;
	    
	  case 7:
	    zpos=0xff&buf[i];
	    istate=8;
	    break;

	  case 8:
	    zpos+=(256*(0xff&buf[i]));
	    istate=9;
	    break;
	    
	  case 9:
	    e=new TouchEvent(type,xpos,ypos,zpos);
	    switch(type) {
		case TouchEvent::TouchPress:
		  emit touchPressEvent(e);
		  break;

		case TouchEvent::TouchRelease:
		  emit touchReleaseEvent(e);
		  break;
	    }
	    delete e;
	    e=NULL;
	    istate=0;
	    break;
      }
    }
  }
}


void EloDriver::ReadInput()
{
  TouchEvent *e=NULL;

  struct input_event event;
  while(read(elo_fd,&event,sizeof(struct input_event))>0) {
    switch(event.type) {
	case EV_ABS:
	  if(elo_workaround) {
	    switch(event.code) {
		case ABS_X:
		  break;

		case ABS_Y:
		  elo_xpos=event.value;
		  break;

		case ABS_Z:
		  elo_ypos=event.value;
		  break;

		case 11:
		  elo_zpos=event.value;
		  break;
	    }
	  }
	  else {
	    switch(event.code) {
		case ABS_X:
		  elo_xpos=event.value;
		  break;

		case ABS_Y:
		  elo_ypos=event.value;
		  break;

		case ABS_Z:
                  elo_workaround=true;  // Detect broken HID implementations
		  break;

		case 11:
		case ABS_MISC:
		  elo_zpos=event.value;
		  break;
	    }
	  }
	  break;

	case EV_SYN:
	  if(elo_touched&&(elo_zpos==0)) {  // Touch Release
	    e=new TouchEvent(TouchEvent::TouchRelease,
			     elo_xpos,elo_ypos,elo_zpos);
	    emit touchReleaseEvent(e);
	    delete e;
	    elo_touched=false;
	  }
	  else {
	    if((!elo_touched)&&(elo_zpos>0)) {  // Touch Press
	      e=new TouchEvent(TouchEvent::TouchPress,
			       elo_xpos,elo_ypos,elo_zpos);
	      emit touchPressEvent(e);
	      delete e;
	    }
	    elo_touched=true;
	  }
	  break;
    }
  }
}
