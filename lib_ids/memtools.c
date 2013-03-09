/*
 * Integrated Diagnostics Subsystem
 *
 * rfa - 060825-0925-26-27
 */
#include <common.h>
#include <idsfs.h>
#include <menu.h>
#include <test_api.h>

#define FORWARD /* */

FORWARD void loadascii(char *buf, int off, char *data, int len);
FORWARD void showascii(char *buf);

#define isdigit(c)	('0' <= (c) && (c) <= '9')
#define isascii(c)	(' ' <= (c) && (c) <= '~')

/*
 * In order of appearance.
 */
static char width;		/* 'b' = byte, 'w' = word, 'l' = long word */
static unsigned long memory = 0;
static char output;		/* 'h' = hex, 'd' = decimal, 'o' = octal */
static unsigned int count;

/*
 * peek -b 0x6E00 -h 256
 * peek -bh 0x6E00 256
 * peek -bh 0x6E00 256
 */
int ids_peek(int argc, char **argv)
{
	int runquiet = 0, haveaddr = 0;
	unsigned char byte, *baddr = 0;
	unsigned short word, *saddr = 0;
	unsigned long lword, *laddr = 0;
	int column, display;
	char *addrfmt, *datafmt;
	char asciibuf[24];

	if (!--argc)
	{
		printf("Usage: %s [-bdhloqw] address [count]\n", *argv);
		printf("\t-b\t\tBytewise operation(s)\n");
		printf("\t-w\t\tWordwise operation(s)\n");
		printf("\t-l\t\tLongwordwise operation(s)\n");
		printf("\t-o\t\tOctal output\n");
		printf("\t-d\t\tDecimal output\n");
		printf("\t-h\t\tHexadecimal output\n");
		printf("\t-q\t\tRun quiet, no output\n");
		printf("\taddress:\tMemory location to start poking\n");
		printf("\t  count:\tNumber of consecutive memory locations to peek\n");
		return(-1);
	}

	/*
	 * Set the default parameters and process the options.
	 */
	width = 'b', count = 1, output = 'h';

	while (argc--)
	{
		if (**++argv == '-')
		{
			char *flags = *argv;

			while (*++flags)
			{
				if (*flags == 'b' ||
				    *flags == 'w' ||
				    *flags == 'l')
				{
					width = *flags;
				}
				else if (*flags == 'o' ||
					 *flags == 'h' ||
					 *flags == 'd')
				{
					output = *flags;
				}
				else if (*flags == 'q')
				{
					++runquiet;
				}
				else
				{
					printf("Invalid flag %c\n", *flags);
					return(-1);
				}
			}
		}
		else if (isdigit(**argv))
		{
			if (!haveaddr)
			{
				memory = strtol(*argv, (char **)0, 0);
				++haveaddr;
			}
			else
				count = strtol(*argv, (char **)0, 0);
		}
		else
		{
			printf("(1) argc: %d Unrecognized option %s\n", argc, *argv);
			return(-1);
		}
	}

	if (!haveaddr)
	{
		printf("Missing address\n");
		return(-1);
	}

	switch (width)
	{
	default:
	case 'b': baddr = (unsigned char *)memory; display = 16; break;
	case 'w': saddr = (unsigned short *)memory; display = 8; break;
	case 'l': laddr = (unsigned long *)memory; display = 4; break;
	}

	switch (output)
	{
	default:
	case 'h':
		addrfmt = "0x%08X: ";
		switch (width)
		{
		default:
		case 'b': datafmt = "%02X "; break;
		case 'w': datafmt = "%04X "; break;
		case 'l': datafmt = "%08X "; break;
		}
		break;
	case 'o':
		addrfmt = "%011o: ";
		switch (width)
		{
		default:
		case 'b': datafmt = "%03o "; break;
		case 'w': datafmt = "%06o "; break;
		case 'l': datafmt = "%011o "; break;
		}
		break;
	case 'd':
		addrfmt = "%0x010d: ";
		switch (width)
		{
		default:
		case 'b': datafmt = "%03d "; break;
		case 'w': datafmt = "%05d "; break;
		case 'l': datafmt = "%010d "; break;
		}
		break;
	}

	switch (width)
	{
	default:
	case 'b':
		for (column = 0; count; ++column, --count)
		{
			byte = *baddr++;
			if (runquiet)
				continue;
			if (!(column%display))
			{
				if (column)
					showascii(asciibuf);
				/*
				 * Print the Address.
				 */
				lprintf(addrfmt, &baddr[-1]);
			}
			loadascii(asciibuf, column%display * sizeof(char), (char *)&byte, sizeof(char));
			/*
			 * Print the data.
			 */
			lprintf(datafmt, byte);
		}
		break;
	case 'w':
		for (column = 0; count; ++column, --count)
		{
			word = *saddr++;
			if (runquiet)
				continue;
			if (!(column%display))
			{
				if (column)
					showascii(asciibuf);
				/*
				 * Print the Address.
				 */
				lprintf(addrfmt, &saddr[-1]);
			}
			loadascii(asciibuf, column%display * sizeof(short), (char *)&word, sizeof(short));
			/*
			 * Print the data.
			 */
			lprintf(datafmt, word);
		}
		break;
	case 'l':
		for (column = 0; count; ++column, --count)
		{
			lword = *laddr++;
			if (runquiet)
				continue;
			if (!(column%display))
			{
				if (column)
					showascii(asciibuf);
				/*
				 * Print the Address.
				 */
				lprintf(addrfmt, &laddr[-1]);
			}
			loadascii(asciibuf, column%display * sizeof(long), (char *)&lword, sizeof(long));
			/*
			 * Print the data.
			 */
			lprintf(datafmt, lword);
		}
		break;
	}
	lprintf("\n");

	return(0);
}

void loadascii(char *buf, int off, char *data, int len)
{
	int n;

	for (n = 0; n < len; ++n)
		buf[off + n] = isascii(data[n]) ? data[n] : '.';
}

void showascii(char *buf)
{
	buf[16] = '\0';

	lprintf("   %s\n", buf);

	buf[0]  = ' ';
	buf[1]  = ' ';
	buf[2]  = ' ';
	buf[3]  = ' ';
	buf[4]  = ' ';
	buf[5]  = ' ';
	buf[6]  = ' ';
	buf[7]  = ' ';
	buf[8]  = ' ';
	buf[9]  = ' ';
	buf[10] = ' ';
	buf[11] = ' ';
	buf[12] = ' ';
	buf[13] = ' ';
	buf[14] = ' ';
	buf[15] = ' ';
}

int ids_poke(int argc, char **argv)
{
	static char *peek_argv[25];
	char **pargv;
	int pargc;
	char peek_flags[32];
	int haveaddr = 0, havedata = 0;
	int dopeek = 0;
	unsigned char byte, *baddr = 0;
	unsigned short word, *saddr = 0;
	unsigned long lword, *laddr = 0;
	unsigned int data = 0;

	if (!--argc)
	{
		printf("Usage: %s [-bwlr] address data [count]\n", *argv);
		printf("\t-b\t\tBytewise operation(s) (Default)\n");
		printf("\t-w\t\tWordwise operation(s)\n");
		printf("\t-l\t\tLongwordwise operation(s)\n");
		printf("\t-r\t\tFollow with peek using same arguments\n");
		printf("\taddress:\tMemory location to start poking\n");
		printf("\t   data:\tData to poke into memory\n");
		printf("\t  count:\tNumber of consecutive memory locations to poke with data\n");
		return(-1);
	}

	width = 'b', count = 1;
	pargc = 0;
	pargv = peek_argv;
	*pargv++ = "peek", ++pargc;

	while (argc--)
	{
		if (**++argv == '-')
		{
			char *flags = *argv;
			char *pflags = peek_flags;

			*pflags = *flags;

			while (*++flags)
			{
				if (*flags == 'b' ||
				    *flags == 'w' ||
				    *flags == 'l')
				{
					width = *flags;
					*++pflags = *flags;
				}
				else if (*flags == 'r')
				{
					++dopeek;
				}
				else if (!(*flags == 'o' ||
					   *flags == 'h' ||
					   *flags == 'd'))
				{
					printf("Invalid flag %c\n", *flags);
					return(-1);
				}
				else
					*++pflags = *flags;
			}
			*++pflags = '\0';
			/*
			 * BUG!!!  If another flags arg is entered then
			 * peek_flags will get overloaded and the previous
			 * flags will be erased.
			 */
			*pargv++ = peek_flags, ++pargc;
		}
		else if (isdigit(**argv))
		{
			if (!haveaddr)
			{
				*pargv++ = *argv, ++pargc;
				memory = strtol(*argv, (char **)0, 0);
				++haveaddr;
			}
			else if (!havedata)
			{
				data = strtol(*argv, (char **)0, 0);
				++havedata;
			}
			else
			{
				*pargv++ = *argv, ++pargc;
				count = strtol(*argv, (char **)0, 0);
			}
#ifdef COMMENT
			else
			{
				printf("Too many options %s\n", *argv);
				return(-1);
			}
#endif /* COMMENT */
		}
		else
		{
			printf("(2) Unrecognized option %s\n", *argv);
			return(-1);
		}
	}

	if (!haveaddr)
	{
		printf("Missing address\n");
		return(-1);
	}

	if (!havedata)
	{
		printf("Missing data value\n");
		return(-1);
	}

	switch (width)
	{
	default:
	case 'b':
		baddr = (unsigned char *)memory;
		byte = (unsigned char)data;
		while (count--)
			*baddr++ = byte;
		break;
	case 'w':
		saddr = (unsigned short *)memory;
		word = (unsigned short)data;
		while (count--)
			*saddr++ = word;
		break;
	case 'l':
		laddr = (unsigned long *)memory;
		lword = (unsigned long)data;
		while (count--)
			*laddr++ = lword;
		break;
	}

	if (dopeek)
		return(ids_peek(pargc, peek_argv));

	return(0);
}

int ids_find(int argc, char **argv)
{
	char *memory;
	char *string;
	
	if (!--argc)
	{
		printf("Usage: %s address string\n", *argv);
		return(-1);
	}

	memory = (char *)strtol(*++argv, (char **)0, 0);

	if (!--argc)
	{
		printf("Usage: %s address string\n", *argv);
		return(-1);
	}

	string = *++argv;

	return(0);
}

#ifdef COMMENT
#ifdef IDS_SA
int _start(int argc, char **argv)
{
#else /* IDS_SA */
int main(int argc, char **argv)
{
	++argv, --argc;
#endif /* IDS_SA */

	if (!strcmp("peek", *argv))
		return(ids_peek(argc, argv));
	if (!strcmp("poke", *argv))
		return(ids_poke(argc, argv));
	return(0);
}
#endif /* COMMENT */
