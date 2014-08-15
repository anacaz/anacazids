/*
 * Integrated Diagnostics Subsystem
 *
 * rfa - 060719
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
	int idsfd, orgfd, hdrfd;
	char *orgfile;
	static char idsfile[256];
	static char hdrfile[256];
	static char newfile[256];
	static char buffer[0x10000];
	int bytes;
	static test_hdr_t test_hdr;
	int argcount;
	char *cp;

	if (!--argc)
	{
		printf("\n\nUsage: %s binary-file [-o output-filenam[.ids]]"
			" [arg1] [arg2] ... [argn]\n\n", *argv);
		exit(-1);
	}

	orgfile = *++argv, --argc;
	/*
	 * Open the binary file.
	 */
	if ((orgfd = open(orgfile, O_RDONLY)) == -1)
	{
		perror(orgfile);
		exit(-1);
	}
	if (!strcmp(*++argv, "-o"))
	{
		if (!--argc)
		{
			printf("Missing output-filename\n");
			exit(-1);
		}
		sprintf(idsfile, "%s", *++argv);
		if (!strstr(idsfile, ".ids"))
			strcat(idsfile, ".ids");
		++argv, --argc;
	}
	else
	{
		sprintf(idsfile, "%s.ids", orgfile);
	}
	strcpy(newfile, idsfile);
	if ((cp = strstr(newfile, ".ids")))
	{
		*cp = '\0';
	}
	if ((idsfd = open(idsfile, O_RDWR | O_CREAT | O_TRUNC, 0644)) == -1)
	{
		perror(idsfile);
		exit(-1);
	}
	for (cp = strchr(newfile, '-'); cp; cp = strchr(cp, '-'))
		*cp = '_';
	sprintf(hdrfile, "%s.h", newfile);
	if ((hdrfd = open(hdrfile, O_RDWR | O_CREAT | O_TRUNC, 0644)) == -1)
	{
		perror(hdrfile);
		exit(-1);
	}
	sprintf(buffer, "#ifndef _%s_h\n", newfile);
	if (write(hdrfd, buffer, strlen(buffer)) == -1)
	{
		perror(hdrfile);
		exit(-1);
	}
	sprintf(buffer, "#define _%s_h\n", newfile);
	if (write(hdrfd, buffer, strlen(buffer)) == -1)
	{
		perror(hdrfile);
		exit(-1);
	}
	sprintf(buffer,
		"/*\n * WARNING! DO NOT EDIT -- "
		"THIS IS AN AUTO GENERATED FILE\n */\n");
	if (write(hdrfd, buffer, strlen(buffer)) == -1)
	{
		perror(hdrfile);
		exit(-1);
	}
	/*
	 * Build and write the test_hdr.
	 */
	for (argcount = 0; argcount < argc; ++argcount, ++argv)
	{
		char *value, *mask;
		int off;

		if (strlen(*argv) > 31)
		{
			printf("error: %s: %s argument too long!\n",
				idsfile, *argv);
			close(idsfd);
			close(orgfd);
			exit(-1);

		}
		snprintf(&test_hdr.argv[argcount][0], 31, "%s", *argv);
		test_hdr.argv[argcount][31] = '\0';
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
		if (write(hdrfd, buffer, off) == -1)
		{
			perror(hdrfile);
			exit(-1);
		}
		if ((cp = strchr(buffer, '{')))
		{
			value = cp;
			if (write(hdrfd, "_REG\t\t", 6) == -1)
			{
				perror(hdrfile);
				exit(-1);
			}
		}
		else
		{
			if (write(hdrfd, "\t\t", 2) == -1)
			{
				perror(hdrfile);
				exit(-1);
			}
		}
		++value;
		if (write(hdrfd, value, strlen(value)) == -1)
		{
			perror(hdrfile);
			exit(-1);
		}
		if (write(hdrfd, "\n", 1) == -1)
		{
			perror(hdrfile);
			exit(-1);
		}
		if (mask)
		{
			if (write(hdrfd, buffer, off) == -1)
			{
				perror(hdrfile);
				exit(-1);
			}
			if (write(hdrfd, "_MSK\t\t", 6) == -1)
			{
				perror(hdrfile);
				exit(-1);
			}
			if (write(hdrfd, mask, strlen(mask)) == -1)
			{
				perror(hdrfile);
				exit(-1);
			}
			if (write(hdrfd, "\n", 1) == -1)
			{
				perror(hdrfile);
				exit(-1);
			}
		}
	}
	sprintf(test_hdr.argc, "%d", argcount);
	sprintf(test_hdr.name, "%s", newfile);
	if ((ext = strrchr(test_hdr.name, '.')) != 0)
		*ext = '\0';
	sprintf(test_hdr.pass, "next");
	sprintf(test_hdr.fail, "stop");
	sprintf(test_hdr.cntr1, "%d", 0);
	sprintf(test_hdr.cntr2, "%d", 0);
	if (write(idsfd, &test_hdr, sizeof(test_hdr)) == -1)
	{
		perror(idsfile);
		exit(-1);
	}

	/*
	 * Read from the bin file and write to the ids.
	 */
	 for (;;)
	 {
	 	if ((bytes = read(orgfd, buffer, 0x10000)) == -1)
		{
			perror(orgfile);
			break;
		}
		if (!bytes)
			break;
		if (write(idsfd, buffer, bytes) == -1)
		{
			perror(idsfile);
			break;
		}

	}
	sprintf(buffer, "#endif /* !_%s_h */\n", newfile);
	if (write(hdrfd, buffer, strlen(buffer)) == -1)
	{
		perror(hdrfile);
		exit(-1);
	}
	close(hdrfd);
	close(orgfd);
	close(idsfd);
	exit(0);
}
