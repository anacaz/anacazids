/*
 * Integrated Diagnostics Subsystem
 *
 * rfa - 090603
 */
#include <common.h>
#include <test_api.h>
#include <ids_jmpvec.h>

static char *program;

#define REG_TEST(type,X) \
do \
{ \
	unsigned type mask; \
\
	for (mask = 1; mask; mask <<= 1) \
	{ \
		if (!(regmask.X & mask)) \
			continue; \
		*(volatile unsigned type *)raddr = mask; \
		if (*(volatile unsigned type *)raddr != mask) \
		{ \
			printf("\t[ %s REGISTER %X BIT %X FAILED ]\n", \
			program, (volatile unsigned type *)raddr, mask); \
			*(volatile unsigned type *)raddr = contents.X; \
			return(-1); \
		} \
	} \
	*(volatile unsigned type *)raddr = contents.X; \
} while (0)

int reg_rw_test(unsigned long raddr, data_types_u regmask, int access)
{
	data_types_u contents;

	if (!access)
		access = sizeof(long);
	switch (access)
	{
	case sizeof(char):
		contents.c = *(volatile unsigned char *)raddr;
		printf("\t[ %s REGISTER %08X&%02X:=%02X ]\n",
			program, raddr, regmask.c, contents.c);
		REG_TEST(char,c);
		break;
	case sizeof(short):
		contents.s = *(volatile unsigned short *)raddr;
		printf("\t[ %s REGISTER %08X&%04X:=%04X ]\n",
			program, raddr, regmask.s, contents.s);
		REG_TEST(short,s);
		break;
	case sizeof(long):
		contents.l = *(volatile unsigned long *)raddr;
		printf("\t[ %s REGISTER %08X&%08X:=%08X ]\n",
			program, raddr, regmask.l, contents.l);
		REG_TEST(long,l);
		break;
	case sizeof(long long):
		contents.ll = *(volatile unsigned long long *)raddr;
		printf("\t[ %s REGISTER %08X&%016X:=%016llX ]\n",
			program, raddr, regmask.ll, contents.ll);
		REG_TEST(long long,ll);
		break;
	default:
		return(-1);
	}
	return(0);
}

int reg_rw_main(int argc, char **argv)
{
	data_types_u regmask;
	unsigned long raddr;
	char *access;
	int size;

	if (!--argc)
	{
		printf("Usage: %s access-type reg1 | reg:mask1\n",
			*argv);
		return(-1);
	}
	program = *argv++;

	if (!--argc)
	{
		printf("Usage: %s access is missing\n", program);
		return(-1);
	}
	access = *argv++;

	if (!strcmp(access, "char"))
		size = sizeof(char);
	else if (!strcmp(access, "short"))
		size = sizeof(short);
	else if (!strcmp(access, "long"))
		size = sizeof(long);
	else if (!strcmp(access, "longlong"))
		size = sizeof(long long);
	else
	{
		printf("Invalid access %s -- char|short|long|longlong\n",
			program);
		return(-1);
	}

	if (argc)
	{
		printf("[ TEST %s : %08X:%08X %d-BIT ACCESS ]\n",
			program, strtol(*argv, (char **)0, 0),
			strtol(argv[argc - 1], (char **)0, 0), size * 8);
	}

	while (argc--)
	{
		char *argp;

		argp = *argv++;
		raddr = strtol(argp, (char **)0, 0);
		while (*argp && *argp != ':')
			++argp;
		if (*argp == ':')
		{
			++argp;
			switch (size)
			{
			case sizeof(char):
				regmask.c = (unsigned char)strtol(argp,
					(char **)0, 0);
				break;
			case sizeof(short):
				regmask.s = (unsigned short)strtol(argp,
					(char **)0, 0);
				break;
			case sizeof(long):
				regmask.l = strtol(argp, (char **)0, 0);
				break;
			case sizeof(long long):
				regmask.ll = (unsigned long long)strtol(argp,
					(char **)0, 0);
				break;
			default:
				break;
			}
		}
		else if (!*argp)
		{
			switch (size)
			{
			case sizeof(char):
				regmask.c = 0xFF;
				break;
			case sizeof(short):
				regmask.s = 0xFFFF;
				break;
			case sizeof(long):
				regmask.l = 0xFFFFFFFFL;
				break;
			case sizeof(long long):
				regmask.ll = 0xFFFFFFFFFFFFFFFFLL;
				break;
			default:
				break;
			}
		}
		else
		{
			printf("%s: Invalid specifier %s\n",
				program, argv[-1]);
			return(-1);
		}
		if (reg_rw_test(raddr, regmask, size) == -1)
			return(-1);
	}

	return(0);
}

#ifdef IDS_SA
int _start(int argc, char **argv)
{
	int status;

	status = 0;
	if (reg_rw_main(argc, argv) != 0)
		++status;
#ifdef COMMENT
	mb_callback = mb_start(lbus_callback);
#endif /* COMMENT */
	return(status);
}
#endif /* IDS_SA */
