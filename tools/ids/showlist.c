/*
 * Integrated Diagnostics Subsystem
 *
 * rfa - 060725
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

FORWARD void set_test_hdr(test_hdr_t *thp, char *filename, int size, char *pass, char *fail);

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

	if (!--argc)
	{
		printf("\n\nUsage: %s testlist-filename[.tls]\n\n", *argv);
		exit(-1);
	}
	while (argc--)
	{
		++argv;
		sprintf(tlsfile, "%s", *argv);
		if (!strstr(tlsfile, ".tls"))
			strcat(tlsfile, ".tls");
		if ((tlsfd = open(tlsfile, O_RDONLY)) == -1)
		{
			perror(tlsfile);
			exit(-1);
		}

		if (read(tlsfd, &list_hdr, sizeof(list_hdr)) == -1)
		{
			perror(tlsfile);
			exit(-1);
		}
		printf("%s: TLS Test List Header\n", tlsfile);
		printf("\t");
		printf("test-count=%s ", list_hdr.tests);
		printf("size=%s ", list_hdr.size);
		printf("magic=%s ", list_hdr.magic);
		printf("\n");

		for (ids_file_count = (int)strtol(list_hdr.tests, (char **)0, 0); ids_file_count; --ids_file_count)
		{
			int argcount, limit;

			if (read(tlsfd, &test_hdr, sizeof(test_hdr)) == -1)
			{
				perror(tlsfile);
				exit(-1);
			}
			printf("%s: IDS Test Header\n", test_hdr.name);
			printf("\t");
			printf("pass=%s ", test_hdr.pass);
			printf("fail=%s ", test_hdr.fail);
			printf("size=%s ", test_hdr.size);
			// printf("appl=%s ", test_hdr.appl);
			printf("argc=%s\n", test_hdr.argc);

			limit = (int)strtol(test_hdr.argc, (char **)0, 0);
			printf("\t");
			for (argcount = 0; argcount < limit; ++argcount)
			{
				printf("%s ", &test_hdr.argv[argcount][0]);
			}
			printf("\n");
			if (lseek(tlsfd, strtol(test_hdr.size, (char **)0, 0) - sizeof(test_hdr_t), SEEK_CUR) == -1)
			{
				perror(tlsfile);
				close(tlsfd);
				exit(-1);
			}
		}

		close(tlsfd);
	}
	exit(0);
}

void set_test_hdr(test_hdr_t *thp, char *filename, int size,
   char *pass, char *fail)
{
	sprintf(thp->name, "%s", filename);
	sprintf(thp->pass, "%s", pass);
	sprintf(thp->fail, "%s", fail);
	sprintf(thp->size, "%d", size);
}
