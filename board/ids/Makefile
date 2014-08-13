#
# (C) Copyright 2004
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

export BLDDIR	:= $(shell /bin/pwd)
export TOPDIR	:= /$(BLDDIR)/../../..

include $(TOPDIR)/config.mk

SUBDIRS	= ram ppc tcm

OBJS	= $(AOBJS) $(COBJS)

all:
	for x in $(SUBDIRS); do $(MAKE) -C $$x || exit 1; done

# $(LIB): .depend $(OBJS)
#	echo $(CROSS_COMPILE)
#	$(AR) crv $@ $(OBJS)

clean: 
	for x in $(SUBDIRS); do $(MAKE) CFLAGS="$(CFLAGS)" -C $$x $@ ||exit 1; done
