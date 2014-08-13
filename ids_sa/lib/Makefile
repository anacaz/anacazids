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

BLDDIR	:= home/${USER}/p4ws
TOPDIR	:= /$(BLDDIR)/uboot

export	BLDDIR
export	TOPDIR

include $(TOPDIR)/config.mk

LIB	= libids_sa.a

AOBJS	=
ifdef IDS_SA
AOBJS	+= ticks.o
endif

COBJS	= patgen.o
#COBJS	= patgen.o pgen.o print_utils.o
#COBJS	+= console.o ctype.o ns16550.o serial.o string.o utils.o \
	  vsprintf.o env.o time.o interrupts.o
# cpu.o
# speed.o

OBJS	= $(AOBJS) $(COBJS)

CPPFLAGS += -I.. -I$(TOPDIR)/include -I../../tests/include

all:	$(LIB) $(OBJS)

$(LIB): .depend $(OBJS)
	$(AR) crv $@ $(OBJS)

#########################################################################

.depend:	Makefile $(AOBJS:.o=.S) $(COBJS:.o=.c)
		$(CC) -M $(CFLAGS) $(AOBJS:.o=.S) $(COBJS:.o=.c) > $@

sinclude .depend

#########################################################################

clean:
	rm -rf *.o *.a
