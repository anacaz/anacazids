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
		*(volatile unsigned type *)raddr = mask; \
	} \
} while (0)

int reg_wo_test(unsigned long raddr, int access)
{
	if (!access)
		access = sizeof(long);
	switch (access)
	{
	case sizeof(char):
		printf("\t[ %s REGISTER %08X: ]\n", program, raddr);
		REG_TEST(char,c);
		break;
	case sizeof(short):
		printf("\t[ %s REGISTER %08X: ]\n", program, raddr);
		REG_TEST(short,s);
		break;
	case sizeof(long):
		printf("\t[ %s REGISTER %08X: ]\n", program, raddr);
		REG_TEST(long,l);
		break;
	case sizeof(long long):
		printf("\t[ %s REGISTER %08X: ]\n", program, raddr);
		REG_TEST(long long,ll);
		break;
	default:
		return(-1);
	}
	return(0);
}

int reg_wo_main(int argc, char **argv)
{
	char *access;
	int bits;

	if (!--argc)
	{
		printf("Usage: %s access reg-addrs\n", *argv);
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
		bits = sizeof(char);
	else if (!strcmp(access, "short"))
		bits = sizeof(short);
	else if (!strcmp(access, "long"))
		bits = sizeof(long);
	else if (!strcmp(access, "longlong"))
		bits = sizeof(long long);
	else
	{
		printf("Invalid access %s; use char, short, or long\n", program);
		return(-1);
	}

	if (argc)
	{
		printf("[ TEST %s : %08X:%08X %d-BIT ACCESS ]\n",
			program, strtol(*argv, (char **)0, 0),
			strtol(argv[argc - 1], (char **)0, 0), bits * 8);
	}

	while (argc--)
	{
		if (reg_wo_test(strtol(*argv++, (char **)0, 0), bits) == -1)
			return(-1);
	}

	return(0);
}

#ifdef IDS_SA
int _start(int argc, char **argv)
{
	int status;

	status = 0;
	if (reg_wo_main(argc, argv) != 0)
		++status;
#ifdef COMMENT
	mb_callback = mb_start(lbus_callback);
#endif /* COMMENT */
	return(status);
}
#endif /* IDS_SA */
