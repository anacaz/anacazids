/*
 * Integrated Diagnostics Subsystem
 *
 * rfa - 060808-21-090505-18
 */
#include <common.h>
#include <test_api.h>
#include <ids_jmpvec.h>
#include "ram.h"

static char *program;

int cache_fill_test(void *memory, int size, int access)
{
	int count, overrun;
	int next;

	RAM_TITLE(CACHE);
	if (!access)
		access = sizeof(char);
	count = size / access;
	overrun = count % 8;
	next = 0;
	switch (access)
	{
	case sizeof(char):
		printf("\t[ %s: fill / flush char ]\n", program);
		CACHE_FILL(c);
		break;
	case sizeof(short):
		printf("\t[ %s: fill / flush short ]\n", program);
		CACHE_FILL(s);
		break;
	case sizeof(long):
		printf("\t[ %s: fill / flush long ]\n", program);
		CACHE_FILL(l);
		break;
	case sizeof(long long):
		printf("\t[ %s: fill / flush long ]\n", program);
		CACHE_FILL(ll);
		break;
	default:
		return(-1);
	}
	return(0);
}

int ram_cache_main(int argc, char **argv)
{
	void *memory;
	int len;
	char *access;

	if (!--argc)
	{
		printf("Usage: %s address size\n", *argv);
		return(-1);
	}
	program = *argv++;
	if (!argc)
	{
		printf("%s: Missing memory base address\n", program);
		return(-1);
	}
	memory = (void *)strtol(*argv++, (char **)0, 0), --argc;
	if (!argc)
	{
		printf("%s: Missing memory region size in bytes\n", program);
		return(-1);
	}
	len = (int)strtol(*argv++, (char **)0, 0), --argc;
	if (!argc)
	{
		access = "char";
	}
	else
		access = *argv++, --argc;

	if (!strcmp(access, "char"))
		return(cache_fill_test(memory, len, sizeof(char)));
	else if (!strcmp(access, "short"))
		return(cache_fill_test(memory, len, sizeof(short)));
	else if (!strcmp(access, "long"))
		return(cache_fill_test(memory, len, sizeof(long)));
	else if (!strcmp(access, "longlong"))
		return(cache_fill_test(memory, len, sizeof(long long)));
	else
		printf("Invalid access %s; use char, short, or long\n");
	return(-1);
}
