#
# Integrated Diagnostics Subsystem
#
# Copyright (c) 2006-2014 "Anacaz Networks, Inc."
# Diagnostics subsystem for U-Boot
# 
# This file is part of anacazids.
# 
# anacazids is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# This Makefile is a bit more than it seems.  The underlying source files
# being compiled here are:
# 	reg-rw.c	-- read/write
#	reg-ro.c	-- read only
#	reg-wo.c	-- write only
#	reg-rc.c	-- clear on read
#	reg-sc.c	-- clear on set
# These files contain the tests for testing registers along with their
# idiosyncratic behaviors.
#
# The compiled bin file is then copied to several renamed bin files.
# Each of these seperate bin files are then passed to the 'mktest' tool
# with various arguments of which the first must be
# 'access=<char|short|long|longlong)'.  There must be at least one other
# argument in the form of '<register name>={<register-address>}'.
#
# The results of all this activity is to create one or more IDS menus
# (one for each invocation of mktest) that allows the user to modify
# each named register as a peek and poke operation AND to all tests
# to be run on register list.
#
# Copyright (c)2000-2009 Anacaz Networks, Inc., ALL RIGHTS RESERVED
#
# rfa - 090603-18
#
export BLDDIR	:= $(shell /bin/pwd)
export TOPDIR	:= /$(BLDDIR)/../../../..
export TEXT_BASE := 0x100000
export IDS_SA=1

TOOLS= /home/rgb/tcm/toolchain/usr/local/gcc-4.1.78-eglibc-2.5.78-1/powerpc-e300c3-linux-gnu/
CROSS_COMPILE= $(TOOLS)bin/powerpc-e300c3-linux-gnu-

#include $(TOPDIR)/config.mk

IDSTOOLS= $(TOPDIR)/tools/ids
MKTEST	= $(IDSTOOLS)/mktest
MKLIST	= $(IDSTOOLS)/mklist
SHOWLIST= $(IDSTOOLS)/showlist

CC= $(CROSS_COMPILE)gcc
AS= $(CROSS_COMPILE)as
LD= $(CROSS_COMPILE)ld
SYSTEM= $(TOOLS)lib/gcc/powerpc-e300c3-linux-gnu/4.1.2/include
CFLAGS= -Os -D__KERNEL__ -DTEXT_BASE=$(TEXT_BASE)
CFLAGS+= -DCONFIG_PPC -D__powerpc__
CFLAGS+= -DCONFIG_MPC83XX -DCONFIG_E300 -ffixed-r2 -ffixed-r29 -msoft-float
CFLAGS+= -I$(TOPDIR)/include
CFLAGS+= -I$(TOPDIR)/include/ids
CFLAGS+= -I$(TOPDIR)/board/rgb/ids/include
CFLAGS+= -fno-builtin -ffreestanding -nostdinc
CFLAGS+= -isystem $(SYSTEM)
CFLAGS+= -pipe -Wall -Wstrict-prototypes

CFLAGS+= -DIDS_SA

all:	ppc.tls

reg-rw: ../lib/reg-rw.o ../lib/ids_jmpvec.o
	$(LD) -Bstatic -Ttext $(TEXT_BASE) ../lib/reg-rw.o \
	../lib/ids_jmpvec.o -o reg-rw

reg-ro: ../lib/reg-ro.o ../lib/ids_jmpvec.o
	$(LD) -Bstatic -Ttext $(TEXT_BASE) ../lib/reg-ro.o \
	../lib/ids_jmpvec.o -o reg-ro

ppc.tls: reg-rw reg-ro
	$(MKTEST) reg-ro -o sys-ro-config-32 access=long		\
		immr={0xE0000000:0xFFFF0000}				\
		ddrba0={0xE00000A0:0xFFFFF000}				\
		ddrar0={0xE00000A4:0x8000003F}				\
		ddrba1={0xE00000A8:0xFFFFF000}				\
		ddrar1={0xE00000AC:0x8000003F}
	$(MKTEST) reg-rw -o sys-rw-config-32 access=long		\
		altc={0xE0000008:0xFFF00000}				\
		awbar0={0xE0000020:0xFFF00000}				\
		awar0={0xE0000024:0x8000003F}				\
		awbar1={0xE0000028:0xFFF00000}				\
		awar1={0xE000002C:0x8000003F}				\
		awbar2={0xE0000030:0xFFF00000}				\
		awar2={0xE0000034:0x8000003F}				\
		awbar3={0xE0000038:0xFFF00000}				\
		awar3={0xE000003C:0x8000003F}
	$(MKTEST) reg-rw -o lbus-banks-lo-32 access=long		\
		br0={0xE0005000:0xFFFF9FE1} or0={0xE0005004:0xFFFF9FFF} \
		br1={0xE0005008:0xFFFF9FE1} or1={0xE000500C:0xFFFF9FFF} \
		br2={0xE0005010:0xFFFF9FE1} or2={0xE0005014:0xFFFF9FFF} \
		br3={0xE0005018:0xFFFF9FE1} or3={0xE000501C:0xFFFF9FFF}
	$(MKTEST) reg-rw -o lbus-banks-hi-32 access=long		\
		br4={0xE0005020:0xFFFF9FE1} or4={0xE0005024:0xFFFF9FFF} \
		br5={0xE0005028:0xFFFF9FE1} or5={0xE000502C:0xFFFF9FFF} \
		br6={0xE0005030:0xFFFF9FE1} or6={0xE0005034:0xFFFF9FFF} \
		br7={0xE0005038:0xFFFF9FE1} or7={0xE000503C:0xFFFF9FFF}
	$(MKTEST) reg-ro -o lbus-upm-32 access=long			\
		mar={0xE0005068}					\
		mamr={0xE0005070:0x7FFFFFFF}				\
		mbmr={0xE0005074:0x7FFFFFFF}				\
		mcmr={0xE0005078:0x7FFFFFFF}				\
		mrtpr={0xE0005084:0xFF000000}				\
		mdr={0xE0005088}					\
		lsor={0xE0005090:0x00000007}				\
		lurt={0xE00050A0:0xFF000000}
	$(MKTEST) reg-ro -o lbus-ctrlr-32 access=long			\
		ltesr={0xE00050B0:0xE4000000}				\
		ltedr={0xE00050B4:0xE4000000}				\
		lteir={0xE00050B8:0xE4000000}				\
		lteatr={0xE00050BC:0x101FFFF1}				\
		ltear={0xE00050C0}					\
		lbcr={0xE00050D0:0x80E3FF0F}				\
		lcrr={0xE00050D4:0x8003001F}
	$(MKTEST) reg-rw -o lbus-flash-32 access=long			\
		fmr={0xE00050E0:0x0000F933}				\
		fir={0xE00050E4}					\
		fcr={0xE00050E8}					\
		fbar={0xE00050EC:0x00FFFFFF}				\
		fpar={0xE00050F0:0x00007FFF}				\
		fbcr={0xE00050F4:0x00000FFF}
	$(MKLIST) -o ppc.tls						\
		sys-ro-config-32.ids					\
		sys-rw-config-32.ids					\
		lbus-banks-lo-32.ids					\
		lbus-banks-hi-32.ids					\
		lbus-upm-32.ids						\
		lbus-ctrlr-32.ids					\
		lbus-flash-32.ids
	$(SHOWLIST) ppc.tls

%.o:	%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o *.a *.ids *.tls reg-rw reg-ro *-16 *-32 ../lib/*.o
	rm -rf *_16.h *_32.h
