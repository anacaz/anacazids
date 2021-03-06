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
 * rfa - 090603
 */
#include <common.h>
#include <test_api.h>
#include <ids_jmpvec.h>

static char *program;

static data_types_u contents;

#define REG_TEST(type,X) \
do \
{ \
	unsigned type mask, value; \
\
	for (mask = 1; mask; mask <<= 1) \
	{ \
		value = *(volatile unsigned type *)raddr; \
		if (value != contents.X) \
			return(-1); \
	} \
} while (0)

int reg_ro_test(unsigned long raddr, int access)
{
	if (!access)
		access = sizeof(long);
	switch (access)
	{
	case sizeof(char):
		contents.c = *(volatile unsigned char *)raddr;
		printf("\t[ %s REGISTER %08X:=%02X ]\n",
			program, raddr, contents.c);
		REG_TEST(char,c);
		break;
	case sizeof(short):
		contents.s = *(volatile unsigned short *)raddr;
		printf("\t[ %s REGISTER %08X:=%04X ]\n",
			program, raddr, contents.s);
		REG_TEST(short,s);
		break;
	case sizeof(long):
		contents.l = *(volatile unsigned long *)raddr;
		printf("\t[ %s REGISTER %08X:=%08X ]\n",
			program, raddr, contents.l);
		REG_TEST(long,l);
		break;
	case sizeof(long long):
		contents.ll = *(volatile unsigned long long *)raddr;
		printf("\t[ %s REGISTER %08X:=%016X ]\n",
			program, raddr, contents.ll);
		REG_TEST(long long,ll);
		break;
	default:
		return(-1);
	}
	return(0);
}

int reg_ro_main(int argc, char **argv)
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
		if (reg_ro_test(strtol(*argv++, (char **)0, 0), bits) == -1)
			return(-1);
	}

	return(0);
}

#ifdef IDS_SA
int _start(int argc, char **argv)
{
	int status;

	status = 0;
	if (reg_ro_main(argc, argv) != 0)
		++status;
#ifdef COMMENT
	mb_callback = mb_start(lbus_callback);
#endif /* COMMENT */
	return(status);
}
#endif /* IDS_SA */
