#
# (C) Copyright 2000-2009
#
export BLDDIR	:= $(shell /bin/pwd)
export TOPDIR	:= /$(BLDDIR)/../../../..

CROSS_COMPILE = /home/rgb/tcm/toolchain/usr/local/gcc-4.1.78-eglibc-2.5.78-1/powerpc-e300c3-linux-gnu/bin/powerpc-e300c3-linux-gnu-

export TEXT_BASE := 0x100000
export IDS_SA=1

include $(TOPDIR)/config.mk

IDSTOOLS= $(TOPDIR)/tools/ids
MKTEST	= $(IDSTOOLS)/mktest
MKLIST	= $(IDSTOOLS)/mklist
SHOWLIST= $(IDSTOOLS)/showlist

CFLAGS += -DIDS_SA -ULINUX -fno-builtin
LFLAGS += -nostdlib

all:	ram.tls ram-lite.tls

LIB1=	../lib/ids_jmpvec.o
TARG1=	ram-refresh ram-readback ram-cache ram-addr-lo ram-addr-hi ram-alias

$(TARG1): $(LIB1)
	$(CC) $(CFLAGS) -c $(addsuffix .c,$@)
	$(LD) -Bstatic -Ttext $(TEXT_BASE) $(LFLAGS) \
	$(addsuffix .o,$@) $(LIB1) -o $@

# ram-refresh: ram-refresh.o ../lib/ids_jmpvec.o
#	$(LD) -Bstatic -Ttext $(TEXT_BASE) ram-refresh.o \
#	../lib/ids_jmpvec.o -o ram-refresh $(LFLAGS)

# ram-readback: ram-readback.o ../lib/ids_jmpvec.o
#	$(LD) -Bstatic -Ttext $(TEXT_BASE) ram-readback.o \
#	../lib/ids_jmpvec.o -o ram-readback $(LFLAGS)

# ram-cache: ram-cache.o ../lib/ids_jmpvec.o
#	$(LD) -Bstatic -Ttext $(TEXT_BASE) ram-cache.o \
#	../lib/ids_jmpvec.o -o ram-cache $(LFLAGS)

# ram-addr-lo: ram-addr-lo.o ../lib/ids_jmpvec.o
#	$(LD) -Bstatic -Ttext $(TEXT_BASE) ram-addr-lo.o \
#	../lib/ids_jmpvec.o -o ram-addr-lo $(LFLAGS)

# ram-addr-hi: ram-addr-hi.o ../lib/ids_jmpvec.o
#	$(LD) -Bstatic -Ttext $(TEXT_BASE) ram-addr-hi.o \
#	../lib/ids_jmpvec.o -o ram-addr-hi $(LFLAGS)

# ram-alias: ram-alias.o ../lib/ids_jmpvec.o
#	$(LD) -Bstatic -Ttext $(TEXT_BASE) ram-alias.o \
#	../lib/ids_jmpvec.o -o ram-alias $(LFLAGS)

#
# CP RAM lite test for initial buffer testing.
#
ram-lite.tls: ram-refresh ram-addr-lo ram-addr-hi
	$(MKTEST) ram-addr-lo -o ram-addr-lo-lite \
		memory=0x00400000 len=0x100000 cycles=1
	$(MKTEST) ram-addr-hi -o ram-addr-hi-lite \
		memory=0x00400000 len=0x100000 cycles=1
	$(MKTEST) ram-refresh -o ram-refresh-lite \
		memory=0x00400000 len=0x100000 \
		pattern=ham access=long cycles=1
	$(MKLIST) -o ram-lite.tls \
		ram-addr-lo-lite.ids \
		ram-addr-hi-lite.ids \
		ram-refresh-lite.ids
	$(SHOWLIST) ram-lite.tls

#
#	$(MKTEST) ram-cache -o ram-cache-64 \
#		memory=0x00400000 len=0x10000000 \
#		access=longlong cycles=1
#
#	$(MKTEST) ram-refresh -o ram-refresh-64 \
#		memory=0x00400000 len=0x10000000 \
#		pattern=ham access=longlong cycles=1
#
#	$(MKTEST) ram-readback -o ram-readback-64 \
#		memory=0x00400000 len=0x10000000 \
#		pattern=ham access=longlong cycles=1
#
#	$(MKTEST) ram-cache -o ram-cache-8 \
#		memory=0x00400000 len=0x10000000 \
#		access=char cycles=1
#	$(MKTEST) ram-cache -o ram-cache-16 \
#		memory=0x00400000 len=0x10000000 \
#		access=short cycles=1
#	$(MKTEST) ram-cache -o ram-cache-32 \
#		memory=0x00400000 len=0x10000000 \
#		access=long cycles=1
#
#		ram-cache-8.ids \
#		ram-cache-16.ids \
#		ram-cache-32.ids \
#		ram-cache-64.ids \
#		ram-refresh-64.ids \
#		ram-readback-64.ids

#

ram.tls: ram-refresh ram-readback ram-cache ram-addr-lo ram-addr-hi ram-alias
	$(MKTEST) ram-addr-lo memory=0x00400000 len=0x01000000 cycles=1
	$(MKTEST) ram-addr-hi memory=0x00400000 len=0x01000000 cycles=1
	$(MKTEST) ram-alias memory=0x00400000 len=0x01000000 cycles=1
	$(MKTEST) ram-refresh -o ram-refresh-8 \
		memory=0x00400000 len=0x01000000 \
		pattern=ham access=char cycles=1
	$(MKTEST) ram-refresh -o ram-refresh-16 \
		memory=0x00400000 len=0x01000000 \
		pattern=ham access=short cycles=1
	$(MKTEST) ram-refresh -o ram-refresh-32 \
		memory=0x00400000 len=0x01000000 \
		pattern=ham access=long cycles=1
	$(MKTEST) ram-readback -o ram-readback-8 \
		memory=0x00400000 len=0x01000000 \
		pattern=ham access=char cycles=1
	$(MKTEST) ram-readback -o ram-readback-16 \
		memory=0x00400000 len=0x01000000 \
		pattern=ham access=short cycles=1
	$(MKTEST) ram-readback -o ram-readback-32 \
		memory=0x00400000 len=0x01000000 \
		pattern=ham access=long cycles=1
	$(MKLIST) -o ram.tls \
		ram-addr-lo.ids \
		ram-addr-hi.ids \
		ram-alias.ids \
		ram-refresh-8.ids \
		ram-refresh-16.ids \
		ram-refresh-32.ids \
		ram-readback-8.ids \
		ram-readback-16.ids \
		ram-readback-32.ids
	$(SHOWLIST) ram.tls

%.o:	%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o *.a *.ids *.tls
	rm -rf ram-refresh ram-readback ram-cache
	rm -rf ram-addr-lo ram-addr-hi ram-alias
	rm -rf *-8 *-16 *-32 *-64 ../lib/*.o
	rm -rf ram-*lite
	rm -rf ram_refresh_*.h ram_readback_*.h ram_cache_*.h
	rm -rf ram_addr_*.h ram_alias.h
	rm -rf ram_*.h
