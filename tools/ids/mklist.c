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
 * rfa - 060719-24-25-26-27
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <idsfs.h>

void exit(int);

#define FORWARD	/**/

FORWARD void set_test_hdr(test_hdr_t *, char *, int , char *, char *);

static int tlssize = 0;

int main(int argc, char **argv)
{
	char **p, *ext;
	struct stat sb;
	int tlsfd, idsfd;
	char *file;
	static char tlsfile[256];
	static char idsfile[256];
	static char buffer[0x10000];
	int bytes;
	static list_hdr_t list_hdr;
	int ids_file_count;
	static test_hdr_t test_hdr;
	static char systemcmd[256];
	char *pass, *fail;

	if (!--argc)
	{
		printf("\n\nUsage: %s -o output-filename[.tls]"
			" file1.ids [file2.ids] ... [filen.ids]\n\n", *argv);
		exit(-1);
	}

	++argv;
	if (*argv[0] != '-')
	{
		printf("Missing flags %s\n", *argv);
		exit(-1);
	}

	if (strcmp(*argv, "-o"))
	{
		printf("Unrecognized option %s\n", *argv);
		exit(-1);
	}

	++argv;
	if (!--argc)
	{
		printf("Missing output-filename\n");
		exit(-1);
	}
	if (!--argc)
	{
		printf("Missing input-filename\n");
		exit(-1);
	}

	tlssize = 0;
	sprintf(tlsfile, "%s", *argv);
	if (!strstr(tlsfile, ".tls"))
		strcat(tlsfile, ".tls");
	if ((tlsfd = open(tlsfile, O_RDWR | O_CREAT | O_TRUNC, 0644)) == -1)
		exit(-1);

	/*
	 * Build and write the list_hdr.
	 */
	ext = strrchr(tlsfile, '.');
	*ext = '\0';
	sprintf(list_hdr.name, "%s", tlsfile);
	*ext = '.';
	if (write(tlsfd, &list_hdr, sizeof(list_hdr)) == -1)
		exit(-1);
	ids_file_count = 0;
	pass = "next";
	fail = "stop";
	while (argc--)
	{
		++argv;
		if (!strncmp("-pass=", *argv, 6))
		{
			pass = *argv + 6;
			continue;
		}
		else if (!strncmp("-fail=", *argv, 6))
		{
			fail = *argv + 6;
			continue;
		}
		sprintf(idsfile, "%s", *argv);
		if (!strstr(idsfile, ".ids"))
			strcat(idsfile, ".ids");
		/*
		 * Open the .ids file.
		 */
		if ((idsfd = open(idsfile, O_RDONLY)) == -1)
		{

			perror(idsfile);
			close(tlsfd);
			sprintf(systemcmd, "rm %s", tlsfile);
			printf("%s\n", systemcmd);
			system(systemcmd);
			exit(-1);
		}
		if (fstat(idsfd, &sb) == -1)
		{
			perror(idsfile);
			close(tlsfd);
			sprintf(systemcmd, "rm %s", tlsfile);
			printf("%s\n", systemcmd);
			system(systemcmd);
			exit(-1);
		}
		/*
		 * Read in IDS test header and add update header elements
		 * here.
		 */
		if ((bytes = read(idsfd, buffer, sizeof(test_hdr_t))) == -1)
		{
			perror(tlsfile);
			close(tlsfd);
			sprintf(systemcmd, "rm %s", tlsfile);
			printf("%s\n", systemcmd);
			system(systemcmd);
			exit(-1);
		}
		/*
		 * BUG!!!
		 *
		 * Need to expand args to specify pass and fail options
		 * on the command line.
		 */
		set_test_hdr((test_hdr_t *)buffer, idsfile, sb.st_size, pass, fail);
		pass = "next";
		fail = "stop";
		if (write(tlsfd, buffer, bytes) == -1)
		{
			perror(tlsfile);
			close(tlsfd);
			sprintf(systemcmd, "rm %s", tlsfile);
			printf("%s\n", systemcmd);
			system(systemcmd);
			exit(-1);
		}

		/*
		 * Read from the bin file and write to the ids.
		 */
		 for (;;)
		 {
			if ((bytes = read(idsfd, buffer, 0x10000)) == -1)
			{
				perror(tlsfile);
				break;
			}
			if (!bytes)
				break;
			if (write(tlsfd, buffer, bytes) == -1)
			{
				perror(tlsfile);
				break;
			}

		}
		close(idsfd);
		++ids_file_count;
	}
	if (lseek(tlsfd, 0, SEEK_SET) == -1)
	{
		perror(tlsfile);
		close(tlsfd);
		exit(-1);
	}

	sprintf(list_hdr.size, "%d", tlssize);
	sprintf(list_hdr.tests, "%d", ids_file_count);
	sprintf(list_hdr.magic, "IDS");
	sprintf(list_hdr.cntr, "%d", 0);
	if (write(tlsfd, &list_hdr, sizeof(list_hdr)) == -1)
	{
		perror(tlsfile);
		exit(-1);
	}
	close(tlsfd);
	exit(0);
}

void set_test_hdr(test_hdr_t *thp, char *filename, int size,
   char *pass, char *fail)
{
	char *ext;

	tlssize += size;
	if ((ext = strrchr(filename, '.')) != 0)
		*ext = '\0';
	sprintf(thp->name, "%s", filename);
	sprintf(thp->pass, "%s", pass);
	sprintf(thp->fail, "%s", fail);
	sprintf(thp->size, "%d", size);
	if (ext != 0)
		*ext = '.';
}
