/*   rttydevice.cpp
 *
 *   A Qt driver for tty ports on Linux.
 *
 *   (C) Copyright 2002 Fred Gleason <fredg@paravelsystems.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation; either version 2 of
 *   the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <qiodevice.h>
#include <rttydevice.h>


RTTYDevice::RTTYDevice() : QIODevice()
{
  Init();
}

RTTYDevice::~RTTYDevice()
{
  if(tty_open) {
    close();
  }
}

bool RTTYDevice::open(int mode)
{
  int flags=O_NONBLOCK|O_NOCTTY;
  struct termios term;

  tty_mode=mode;
  if((mode&IO_ReadWrite)==IO_ReadWrite) {
    flags|=O_RDWR;
  }
  else {
    if(((mode&IO_WriteOnly)!=0)) {
      flags|=O_WRONLY;
    }
    if(((mode&IO_ReadOnly)!=0)) {
      flags|=O_RDONLY;
    }
  }
  if((mode&IO_Append)!=0) {
    flags|=O_APPEND;
  }
  if((mode&IO_Truncate)!=0) {
    flags|=O_TRUNC;
  }

  if((tty_fd=::open((const char *)tty_name,flags))<0) {
    tty_status=IO_OpenError;
    return false;
  }
  tty_open=true;
  tty_status=IO_Ok;

  tcgetattr(tty_fd,&term);
  cfsetispeed(&term,B0);
  cfsetospeed(&term,tty_speed);
  cfmakeraw(&term);
  term.c_iflag |= IGNBRK; 
  switch(tty_parity) {
  case RTTYDevice::None:
    term.c_iflag |= IGNPAR;
    break;

  case RTTYDevice::Even:
    term.c_cflag |= PARENB;
    break;

  case RTTYDevice::Odd:
    term.c_cflag |= PARENB|PARODD;
    break;
  }
  tcsetattr(tty_fd,TCSADRAIN,&term);

  return true;
}


void RTTYDevice::close()
{
  if(tty_open) {
    ::close(tty_fd);
  }
  tty_open=false;
}


void RTTYDevice::flush()
{
}


Q_LONG RTTYDevice::readBlock(char *data,Q_ULONG maxlen)
{
  Q_LONG n;

  if((n=read(tty_fd,data,(size_t)maxlen))<0) {
    if(errno!=EAGAIN) {
      tty_status=IO_ReadError;
      return -1;
    }
    return 0;
  }
  tty_status=IO_Ok;
  return n;
}


Q_LONG RTTYDevice::writeBlock(const char *data,Q_ULONG len)
{
  Q_LONG n;

  if((n=write(tty_fd,data,(size_t)len))<0) {
    tty_status=IO_WriteError;
    return n;
  }
  tty_status=IO_Ok;
  return n;
}


int RTTYDevice::getch()
{
  char c;
  int n;

  if((n=readBlock(&c,1))<0) {
    tty_status=IO_ReadError;
    return -1;
  }
  return (int)c;
}


int RTTYDevice::putch(int ch)
{
  char c;
  int n;

  c=(char)ch;
  if((n=writeBlock(&c,1))<0) {
    tty_status=IO_WriteError;
    return -1;
  }
  return ch;
}


int RTTYDevice::ungetch(int ch)
{
  tty_status=IO_WriteError;
  return -1;
}


QIODevice::Offset RTTYDevice::size() const
{
  return 0;
}


int RTTYDevice::flags() const
{
  return tty_mode|state();
}


int RTTYDevice::mode() const
{
  return tty_mode;
}


int RTTYDevice::state() const
{
  if(tty_open) {
    return IO_Open;
  }
  return 0;
}


bool RTTYDevice::isDirectAccess() const
{
  return false;
}


bool RTTYDevice::isSequentialAccess() const
{
  return true;
}


bool RTTYDevice::isCombinedAccess() const
{
  return false;
}


bool RTTYDevice::isBuffered() const
{
  return false;
}


bool RTTYDevice::isRaw() const
{
  return true;
}


bool RTTYDevice::isSynchronous() const
{
  return true;
}


bool RTTYDevice::isAsynchronous() const
{
  return false;
}


bool RTTYDevice::isTranslated() const
{
  return false;
}


bool RTTYDevice::isReadable() const
{
  if(((tty_mode&IO_ReadOnly)!=0)||((tty_mode&IO_ReadWrite)!=0)) {
    return true;
  }
  return false;
}


bool RTTYDevice::isWritable() const
{
  if(((tty_mode&IO_WriteOnly)!=0)||((tty_mode&IO_ReadWrite)!=0)) {
    return true;
  }
  return false;
}


bool RTTYDevice::isReadWrite() const
{
  if((tty_mode&IO_ReadWrite)!=0) {
    return true;
  }
  return false;

}


bool RTTYDevice::isInactive() const
{
  if(!tty_open) {
    return true;
  }
  return false;
}


bool RTTYDevice::isOpen() const
{
  if(tty_open) {
    return true;
  }
  return false;
}


int RTTYDevice::status() const
{
  return tty_status;
}


void RTTYDevice::resetStatus()
{
  tty_status=IO_Ok;
}


void RTTYDevice::setName(QString name)
{
  tty_name=name;
}


int RTTYDevice::speed() const
{
  switch(tty_speed) {
  case B0:
    return 0;
    break;

  case B50:
    return 50;
    break;

  case B75:
    return 75;
    break;

  case B110:
    return 110;
    break;

  case B134:
    return 134;
    break;

  case B150:
    return 150;
    break;

  case B200:
    return 200;
    break;

  case B300:
    return 300;
    break;

  case B600:
    return 600;
    break;

  case B1200:
    return 1200;
    break;

  case B1800:
    return 1800;
    break;

  case B2400:
    return 2400;
    break;

  case B4800:
    return 4800;
    break;

  case B9600:
    return 9600;
    break;

  case B19200:
    return 19200;
    break;

  case B38400:
    return 38400;
    break;

  case B57600:
    return 57600;
    break;

  case B115200:
    return 115200;
    break;

  case B230400:
    return 230400;
    break;
  }
  return 0;
}


void RTTYDevice::setSpeed(int speed)
{
  switch(speed) {
  case 0:
    tty_speed=B0;
    break;

  case 50:
    tty_speed=B50;
    break;

  case 75:
    tty_speed=B75;
    break;

  case 110:
    tty_speed=B110;
    break;

  case 134:
    tty_speed=B134;
    break;

  case 150:
    tty_speed=B150;
    break;

  case 200:
    tty_speed=B200;
    break;

  case 300:
    tty_speed=B300;
    break;

  case 600:
    tty_speed=B600;
    break;

  case 1200:
    tty_speed=B1200;
    break;

  case 1800:
    tty_speed=B1800;
    break;

  case 2400:
    tty_speed=B2400;
    break;

  case 4800:
    tty_speed=B4800;
    break;

  case 9600:
    tty_speed=B9600;
    break;

  case 19200:
    tty_speed=B19200;
    break;

  case 38400:
    tty_speed=B38400;
    break;

  case 57600:
    tty_speed=B57600;
    break;

  case 115200:
    tty_speed=B115200;
    break;

  case 230400:
    tty_speed=B230400;
    break;

  default:
    tty_speed=B9600;
    break;
  }
   

}


int RTTYDevice::wordLength() const
{
  switch(tty_length) {
  case CS5:
    return 5;
    break;

  case CS6:
    return 6;
    break;

  case CS7:
    return 7;
    break;

  case CS8:
    return 8;
    break;
  }
  return 0;
}


void RTTYDevice::setWordLength(int length)
{
  switch(length) {
  case 5:
    tty_length=CS5;
    break;

  case 6:
    tty_length=CS6;
    break;

  case 7:
    tty_length=CS7;
    break;

  case 8:
    tty_length=CS8;
    break;

  default:
    tty_length=CS8;
    break;
  }
}


RTTYDevice::Parity RTTYDevice::parity() const
{
  return tty_parity;
}


void RTTYDevice::setParity(Parity parity)
{
  tty_parity=parity;
}


void RTTYDevice::Init()
{
  tty_speed=9600;
  tty_length=8;
  tty_parity=RTTYDevice::None;
  tty_open=false;
}
