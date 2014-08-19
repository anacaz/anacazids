/*
 * Integrated Diagnostics Subsystem
 *
 * Copyright (c) 2006-2014 "Anacaz Networks, Inc."
 * Diagnostics subsystem for U-Boot
 * 
 * This file is part of anacazids.
 * 
 * anacazids is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * rfa - 090610-140815
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

void exit(int);

int main(int argc, char **argv)
{
	int odd, even;
	uint32_t hex;

	odd = even = 0;
	while (--argc)
	{
		if (!strcmp(*++argv, "odd"))
			odd = 1;
		if (!strcmp(*argv, "even"))
			even = 1;
	}
	for (hex = 0; hex < 256; ++hex)
	{
		int count;
		uint32_t bit;

		printf("%02X: ", hex);
		for (count = 0, bit = 0x80; bit; bit >>= 1)
		{
			if (hex & bit)
				printf("1"), ++count;
			else
				printf("0");
		}
		if (odd && count % 2)
			printf(" ODD %d", count);
		if (even && !(count % 2))
			printf(" EVEN %d", count);
		printf("\n");
	}
	exit(0);
}
