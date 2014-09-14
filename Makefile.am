## automake.am
##
## Top level automake.am for EloCal
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

SUBDIRS = lib doc elomon elocal

ACLOCAL_AMFLAGS = -I m4

maintainer-clean-local:	
	rm -rf m4

rpm:	dist
	mkdir -p $(HOME)/rpmbuild/SOURCES
	cp elocal-$(VERSION).tar.gz $(HOME)/rpmbuild/SOURCES/
	$(RPMBUILD) -ba --target $(ARCH)-$(VENDOR)-linux elocal.spec
	mv $(HOME)/rpmbuild/RPMS/$(ARCH)/elocal-*.rpm $(top_srcdir)/
	mv $(HOME)/rpmbuild/SRPMS/elocal-*.src.rpm $(top_srcdir)/
	rm $(HOME)/rpmbuild/SOURCES/elocal-$(VERSION).tar.gz
	rm -rf $(HOME)/rpmbuild/BUILD/elocal-$(VERSION)

EXTRA_DIST = autogen.sh elocal.spec.in get_target.sh get_distro.sh

CLEANFILES = *~ *.tar.gz *.rpm
DISTCLEANFILES = elocal.spec
MAINTAINERCLEANFILES = *~ Makefile.in configure aclocal.m4 *.tar.gz\
                       config.guess config.status config.sub install-sh\
                       missing mkinstalldirs depcomp ltmain.sh\
                       rivendell.spec
