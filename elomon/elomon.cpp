// elomon.cpp
//
// A Protocol Monitor for ELO Touchscreens.
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
#include <qmessagebox.h>
#include <qpushbutton.h>

#include <elomon.h>

bool exiting=false;

MainWidget::MainWidget(QWidget *parent,const char *name)
  :QWidget(parent,name)
{
  elo_speed=ELO_DEFAULT_SPEED;
  bool elo_workaround=false;

  //
  // Generate Fonts
  //
  QFont header_font("Helvetica",14,QFont::Bold);
  header_font.setPixelSize(12);
  QFont label_font("Helvetica",14,QFont::Bold);
  label_font.setPixelSize(12);

  //
  // Process Command Switches
  //
  if(qApp->argc()<2) {
    QMessageBox::warning(this,"EloMon",ELO_USAGE);
    exiting=true;
    return;
  }
  if(qApp->argc()>2) {
    bool arg_found=false;
    for(int i=1;i<(qApp->argc()-1);i++) {
      if(!strcmp(qApp->argv()[i],"-b")) {  // Baud Rate
	if(sscanf(qApp->argv()[++i],"%d",&elo_speed)!=1) {
	  QMessageBox::warning(this,"EloMon","Invalid TTY speed.");
	  exiting=true;
	  return;
	}
	arg_found=true;
      }
      if(!strcmp(qApp->argv()[i],"-W")) {  // Input Device Bug Workaround
	elo_workaround=true;
	arg_found=true;
      }
      if(!arg_found) {
	QMessageBox::warning(this,"EloMon",ELO_USAGE);
	exiting=true;
	return;
      }
    }
  }

  setCaption("EloMon");

  //
  // Press Section
  //
  QLabel *label=new QLabel("Press",this,"press_label");
  label->setGeometry(10,10,100,20);
  label->setFont(header_font);
  label->setAlignment(AlignLeft|AlignVCenter);

  elo_pressx_edit=new QLineEdit(this,"elo_pressx_edit");
  elo_pressx_edit->setGeometry(40,30,40,14);
  elo_pressx_edit->setReadOnly(true);
  label=new QLabel("X:",this);
  label->setGeometry(15,30,20,14);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter);
  
  elo_pressy_edit=new QLineEdit(this,"elo_pressy_edit");
  elo_pressy_edit->setGeometry(115,30,40,14);
  elo_pressy_edit->setReadOnly(true);
  label=new QLabel("Y:",this);
  label->setGeometry(90,30,20,14);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter);
  
  elo_pressz_edit=new QLineEdit(this,"elo_pressz_edit");
  elo_pressz_edit->setGeometry(190,30,40,14);
  elo_pressz_edit->setReadOnly(true);
  label=new QLabel("Z:",this);
  label->setGeometry(165,30,20,14);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Release Section
  //
  label=new QLabel("Release",this,"release_label");
  label->setGeometry(10,55,100,20);
  label->setFont(header_font);
  label->setAlignment(AlignLeft|AlignVCenter);

  elo_releasex_edit=new QLineEdit(this,"elo_releasex_edit");
  elo_releasex_edit->setGeometry(40,75,40,14);
  elo_releasex_edit->setReadOnly(true);
  label=new QLabel("X:",this);
  label->setGeometry(15,75,20,14);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter);
  
  elo_releasey_edit=new QLineEdit(this,"elo_releasey_edit");
  elo_releasey_edit->setGeometry(115,75,40,14);
  elo_releasey_edit->setReadOnly(true);
  label=new QLabel("Y:",this);
  label->setGeometry(90,75,20,14);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter);
  
  elo_releasez_edit=new QLineEdit(this,"elo_releasez_edit");
  elo_releasez_edit->setGeometry(190,75,40,14);
  elo_releasez_edit->setReadOnly(true);
  label=new QLabel("Z:",this);
  label->setGeometry(165,75,20,14);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Close Button
  //
  QPushButton *button=new QPushButton("&Close",this,"close_button");
  button->setGeometry(sizeHint().width()-70,sizeHint().height()-45,60,35);
  button->setFont(header_font);
  connect(button,SIGNAL(clicked()),qApp,SLOT(quit()));

  //
  // Touch Screen Driver
  //
  elo_driver=new EloDriver(this,"elo_controller");
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
  connect(elo_driver,SIGNAL(touchPressEvent(TouchEvent *)),
	  this,SLOT(touchPressData(TouchEvent *)));
  connect(elo_driver,SIGNAL(touchReleaseEvent(TouchEvent *)),
	  this,SLOT(touchReleaseData(TouchEvent *)));
}


MainWidget::~MainWidget()
{
}


QSize MainWidget::sizeHint() const
{
  return QSize(250,140);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::touchPressData(TouchEvent *e)
{
  elo_pressx_edit->setText(QString().sprintf("%d",e->x()));
  elo_pressy_edit->setText(QString().sprintf("%d",e->y()));
  elo_pressz_edit->setText(QString().sprintf("%d",e->z()));
}


void MainWidget::touchReleaseData(TouchEvent *e)
{
  elo_releasex_edit->setText(QString().sprintf("%d",e->x()));
  elo_releasey_edit->setText(QString().sprintf("%d",e->y()));
  elo_releasez_edit->setText(QString().sprintf("%d",e->z()));
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);
  
  MainWidget *w=new MainWidget(NULL,"main");
  if(exiting) {
    exit(1);
  }
  a.setMainWidget(w);
  w->show();
  return a.exec();
}


