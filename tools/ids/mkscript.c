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
 * Using elements of mktest and showlist build a .c file that contains
 * an initialization sequence complete with delays and in line verification
 * check points.
 *
 * Elements of mktest make up the core processor for converting
 * {address[:mask]} with =data to produce register and memory write
 * activities.  $(var)={address[:mask]} to produce register and memory
 * read activities.
 *
 *	mkscript cpld-init-16 access=short \
 *		{0x80002000:0x0FFF}=0x0FFF \
 *		{0x80002000:0x0FFF}=0x0000
 *
 * Elements of showlist make up the symbolic conversion to the form above.
 * Scanning the .tls file for the tag an intermediate representation is
 * built up to resemble the nomenclature above.  In this way then only
 * the Makefile needs to change when register and mask updates occur.
 *
 *	mkscript cpld-init-16 access=short \
 *		cpld-rw-16:enc-reset=0x0FFF \
 *		cpld-rw-16:enc-reset=0x0000
 *
 * Copyright (c)2009 Anacaz Networks, Inc., ALL RIGHTS RESERVED
 *
 * rfa - 090623
 */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <idsfs.h>

void exit(int);

int main(int argc, char **argv)
{
	char **p, *ext;
	struct stat sb;
	int orgfd, srcfd;
	char *orgfile;
	static char srcfile[256];
	static char newfile[256];
	static char buffer[0x10000];
	int bytes;
	int argcount;
	char *cp;

	if (!--argc)
	{
		printf("\n\nUsage: %s binary-file"
			" [arg1] [arg2] ... [argn]\n\n", *argv);
		exit(-1);
	}

	orgfile = *++argv;
	strcpy(newfile, orgfile);
	/*
	 * Open the binary file.
	 */
	if ((orgfd = open(orgfile, O_RDONLY)) == -1)
	{
		perror(orgfile);
		exit(-1);
	}
	for (cp = strchr(newfile, '-'); cp; cp = strchr(cp, '-'))
		*cp = '_';
	sprintf(srcfile, "%s__script.c", newfile);
	if ((srcfd = open(srcfile, O_RDWR | O_CREAT | O_TRUNC, 0644)) == -1)
	{
		perror(srcfile);
		exit(-1);
	}
	sprintf(buffer,
		"/*\n * WARNING! DO NOT EDIT -- "
		"THIS IS AN AUTO GENERATED FILE\n */\n");
	if (write(srcfd, buffer, strlen(buffer)) == -1)
	{
		perror(srcfile);
		exit(-1);
	}
	/*
	 * Build and write the test_hdr.
	 */
	for (argcount = 0, --argc; argcount < argc; ++argcount)
	{
		char *value, *mask;
		int off;

		++argv;
		if (strlen(*argv) > 31)
		{
			printf("error: %s: %s argument too long!\n",
				srcfile, *argv);
			close(orgfd);
			exit(-1);

		}
		sprintf(buffer, "#define %s", *argv);
		while ((cp = strchr(buffer, '-')))
		{
			*cp = '_';
		}
		value = strchr(buffer, '=');
		if ((cp = strchr(buffer, '}')))
			*cp = '\0';
		cp = value;
		if ((mask = strchr(buffer, ':')))
			*mask++ = '\0';
		while (!isspace(*cp))
		{
			if (islower(*cp))
				*cp = toupper(*cp);
			--cp;
		}
		off = value - buffer;
		if (write(srcfd, buffer, off) == -1)
		{
			perror(srcfile);
			exit(-1);
		}
		if ((cp = strchr(buffer, '{')))
		{
			value = cp;
			if (write(srcfd, "_REG\t\t", 6) == -1)
			{
				perror(srcfile);
				exit(-1);
			}
		}
		else
		{
			if (write(srcfd, "\t\t", 2) == -1)
			{
				perror(srcfile);
				exit(-1);
			}
		}
		++value;
		if (write(srcfd, value, strlen(value)) == -1)
		{
			perror(srcfile);
			exit(-1);
		}
		if (write(srcfd, "\n", 1) == -1)
		{
			perror(srcfile);
			exit(-1);
		}
		if (mask)
		{
			if (write(srcfd, buffer, off) == -1)
			{
				perror(srcfile);
				exit(-1);
			}
			if (write(srcfd, "_MSK\t\t", 6) == -1)
			{
				perror(srcfile);
				exit(-1);
			}
			if (write(srcfd, mask, strlen(mask)) == -1)
			{
				perror(srcfile);
				exit(-1);
			}
			if (write(srcfd, "\n", 1) == -1)
			{
				perror(srcfile);
				exit(-1);
			}
		}
	}

	close(srcfd);
	close(orgfd);
	exit(0);
}
