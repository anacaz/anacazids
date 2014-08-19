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

CC=	gcc
CFILES=	
OFILES=	$(CFILES:%.c=%.o)
CFLAGS=	-I. -I../../include -I../../include/ids

TARGETS	= mklist mktest showlist bin2ascii showhex

all: clean $(TARGETS)

bin2ascii: bin2ascii.o
	$(CC) -o $@ bin2ascii.o

mklist: mklist.o
	$(CC) -o $@ mklist.o

showlist: showlist.o
	$(CC) -o $@ showlist.o

mktest: mktest.o
	$(CC) -o $@ mktest.o

showhex: showhex.o
	$(CC) -o $@ showhex.o

.c.o: 
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o $(TARGETS)

