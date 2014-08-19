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
 * rfa
 */
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

void exit(int);

static unsigned short buffer[0x10000];

int main(int argc, char **argv)
{
	int fd;
	int bytes;
	int column;
	char *filename, *cp;

	if (!--argc)
	{
		printf("Usage: %s filename\n", *argv);
		exit(-1);
	}
	filename = *++argv;
	if ((fd = open(filename, O_RDONLY)) == -1)
	{
		printf("\n\n");
		perror(*argv);
		printf("\n\n");
		exit(-1);
	}
	if ((cp = strchr(filename, '.')))
		*cp = '\0';
	printf("unsigned short %s[] = {\n", filename);
	column = 0;
	do
	{
		int n;

		memset(buffer, 0, 0x10000);
		if ((bytes = read(fd, buffer, 0x10000 * sizeof(short))) == -1)
		{
			printf("\n\n");
			if (cp)
				*cp = '.';
			perror(filename);
			printf("\n\n");
			break;
		}
		for (n = 0; n < (bytes / 2); ++n)
		{
			if (!column)
				printf("\t");
			printf("0x%04X, ", buffer[n]);
			if (++column >= 8)
				printf("\n"), column = 0;
		}
	} while (bytes);

	printf("};\n");
	close(fd);
	exit(0);
}
