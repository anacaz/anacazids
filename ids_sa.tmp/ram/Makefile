#
# (C) Copyright 2000-2005
#
export BLDDIR	:= home/${USER}/p4ws
export TOPDIR	:= /$(BLDDIR)/uboot

CROSS_COMPILE = /home/rgb/tcm/toolchain/usr/local/gcc-4.1.78-eglibc-2.5.78-1/powerpc-e300c3-linux-gnu/bin/powerpc-e300c3-linux-gnu-

export TEXT_BASE := 0x100000

export IDS_SA=1

include $(TOPDIR)/config.mk

CFLAGS += -DIDS_SA

OBJS	= ram.o
TARGETS	= ram.tls

ALL=	$(TARGETS)

all:	$(ALL)

ram: ram.o ../lib/ids_jmpvec.o
	$(LD) -Bstatic -Ttext $(TEXT_BASE) ram.o ../lib/ids_jmpvec.o -o ram

#	mktest ram memory=0xB0000 len=0x1000
ram.tls: ram
	mktest ram memory=0x00200000 len=0x002FFF10
	mklist -o ram.tls ram.ids

%.o:	%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o *.a $(TARGETS) ram ram.ids ../lib/*.o
