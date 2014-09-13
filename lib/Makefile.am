## automake.am
##
## Automake.am for elocal/lib
##
## by Fred Gleason <fredg@paravelsystems.com>
##
## Use automake to process this into a Makefile.in
##
##   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
##
##   This program is free software; you can redistribute it and/or modify
##   it under the terms of the GNU General Public License as
##   published by the Free Software Foundation; either version 2 of
##   the License, or (at your option) any later version.
##
##   This program is distributed in the hope that it will be useful,
##   but WITHOUT ANY WARRANTY; without even the implied warranty of
##   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##   GNU General Public License for more details.
##
##   You should have received a copy of the GNU General Public
##   License along with this program; if not, write to the Free Software
##   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
##

AM_CPPFLAGS = -Wall @QT_CXXFLAGS@
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

# instdir = /usr/local/lib

lib_LTLIBRARIES = libelo_driver.la
dist_libelo_driver_la_SOURCES = elo_driver.cpp elo_driver.h\
                                touchevent.cpp touchevent.h\
                                rttydevice.cpp rttydevice.h
nodist_libelo_driver_la_SOURCES = moc_elo_driver.cpp

libelo_driver_la_LDFLAGS = -release $(VERSION)

CLEANFILES = *~ moc_*
MAINTAINERCLEANFILES = *~ Makefile.in configure aclocal.m4 *.tar.gz moc_*
