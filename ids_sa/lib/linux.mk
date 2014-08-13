#
# Integrated Diagnostics System
#
# rfa - 090429
#
# include $(TOPDIR)/config.mk

CC=	gcc
AR=	ar

OFILES=	patgen.o \
	pgen.o

#	print_utils.o \
	console.o \
	ctype.o \
	ns16550.o \
	serial.o \
	speed.o \
	string.o \
	utils.o \
	vsprintf.o \
	env.o \
	time.o \
	interrupts.o \
#	ticks.o \
#	cpu.o

CFLAGS=	-I../.. -I../include -I../src -I../hw
DFLAGS=	-DLINUX

all:	libids-platform.a

.c.o:
	$(CC) $(DFLAGS) $(CFLAGS) -c $< -o $@

libids-platform.a: $(OFILES)
	$(AR) crv $@ $(OFILES)

clean:
	rm -rf *.a $(OFILES)
