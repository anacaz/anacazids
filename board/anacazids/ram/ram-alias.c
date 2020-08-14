/*
 * Integrated Diagnostics Subsystem
 *
 * rfa - 060808-21-090505-31-0601
 */
#include <common.h>
#include <test_api.h>
#include <ids_jmpvec.h>
#include "ram.h"

extern int ids_peek(int argc, char **argv);

static char *program;

static pvalue_t seed;
static pvalue_t ascend;

#define FORWARD	/**/

FORWARD int ram_alias_test(void *memory, int size);
FORWARD int ram_alias_main(int argc, char **argv);

/*
 */
int ram_alias_test(void *memory, int size)
{
	static char peek_addr[32];
	static char *argv[4] =
	{
		"peek", "-l", peek_addr, "100"
	};
	int count, overrun;
	int next;
	int access;

	access = sizeof(long);
	count = size / access;
	overrun = count % 8;
	next = 0;

	RAM_TITLE(ALIAS);

	PG_DATA_TYPE(seed) = sizeof(long);
	LONG(seed) = (unsigned long)memory;

	PG_DATA_TYPE(ascend) = sizeof(long);
	ascend = spatgen(ascend, seed, modgen("ascend"));

	/*
	 * FORWARD: Write the location address to each location.
	 */
	printf("\t[ %s fill:%08X-%08X ]\n", program, memory, memory + size - 4);
	{
		int _index = 0;

		ram.l = memory;
		switch (overrun)
		while (_index < count)
		{
		case 0: ram.l[_index] = (unsigned long)&ram.l[_index], ++_index;
		case 7: ram.l[_index] = (unsigned long)&ram.l[_index], ++_index;
		case 6: ram.l[_index] = (unsigned long)&ram.l[_index], ++_index;
		case 5: ram.l[_index] = (unsigned long)&ram.l[_index], ++_index;
		case 4: ram.l[_index] = (unsigned long)&ram.l[_index], ++_index;
		case 3: ram.l[_index] = (unsigned long)&ram.l[_index], ++_index;
		case 2: ram.l[_index] = (unsigned long)&ram.l[_index], ++_index;
		case 1: ram.l[_index] = (unsigned long)&ram.l[_index], ++_index;
		}
	}
	sprintf(peek_addr, "0x%X", memory - 64);
	ids_peek(4, argv);
	printf("\n");
	printf("\t[ %s find:%08X-%08X ]\n", program, memory, memory + size - 4);
	{
		int _index;

		ram.l = memory;
		_index = 0;
		switch (overrun)
		while (_index < count)
		{
		case 0: if (ram.l[_index] != (unsigned long)&ram.l[_index])
			{ return(-1); }
			++_index;
		case 7: if (ram.l[_index] != (unsigned long)&ram.l[_index])
			{ return(-1); }
			++_index;
		case 6: if (ram.l[_index] != (unsigned long)&ram.l[_index])
			{ return(-1); }
			++_index;
		case 5: if (ram.l[_index] != (unsigned long)&ram.l[_index])
			{ return(-1); }
			++_index;
		case 4: if (ram.l[_index] != (unsigned long)&ram.l[_index])
			{ return(-1); }
			++_index;
		case 3: if (ram.l[_index] != (unsigned long)&ram.l[_index])
			{ return(-1); }
			++_index;
		case 2: if (ram.l[_index] != (unsigned long)&ram.l[_index])
			{ return(-1); }
			++_index;
		case 1: if (ram.l[_index] != (unsigned long)&ram.l[_index])
			{ return(-1); }
			++_index;
		}
	}
	sprintf(peek_addr, "0x%X", memory + size - 64);
	ids_peek(4, argv);
	printf("\n");

	pattern.l = 0;
	printf("\t[ %s fill:%08X ]\n", program, pattern.l);
	RAM_FILL(l);

	sprintf(peek_addr, "0x%X", memory - 64);
	ids_peek(4, argv);
	printf("\n");
	/*
	 * BACKWARD: Write the location address to each location.
	 */
	printf("\t[ %s fill:%08X-%08X ]\n", program, memory + size - 4, memory);
	{
		int _index = count;

		ram.l = memory;
		switch (overrun)
		while (_index)
		{
		case 0: --_index;
			ram.l[_index] = (unsigned long)&ram.l[_index];
		case 7: --_index;
			ram.l[_index] = (unsigned long)&ram.l[_index];
		case 6: --_index;
			ram.l[_index] = (unsigned long)&ram.l[_index];
		case 5: --_index;
			ram.l[_index] = (unsigned long)&ram.l[_index];
		case 4: --_index;
			ram.l[_index] = (unsigned long)&ram.l[_index];
		case 3: --_index;
			ram.l[_index] = (unsigned long)&ram.l[_index];
		case 2: --_index;
			ram.l[_index] = (unsigned long)&ram.l[_index];
		case 1: --_index;
			ram.l[_index] = (unsigned long)&ram.l[_index];
		}
	}
	sprintf(peek_addr, "0x%X", memory - 64);
	ids_peek(4, argv);
	printf("\n");
	printf("\t[ %s find:%08X-%08X ]\n", program, memory, memory + size - 4);
	{
		int _index;

		ram.l = memory;
		_index = 0;
		switch (overrun)
		while (_index < count)
		{
		case 0: if (ram.l[_index] != (unsigned long)&ram.l[_index])
			{ return(-1); }
			++_index;
		case 7: if (ram.l[_index] != (unsigned long)&ram.l[_index])
			{ return(-1); }
			++_index;
		case 6: if (ram.l[_index] != (unsigned long)&ram.l[_index])
			{ return(-1); }
			++_index;
		case 5: if (ram.l[_index] != (unsigned long)&ram.l[_index])
			{ return(-1); }
			++_index;
		case 4: if (ram.l[_index] != (unsigned long)&ram.l[_index])
			{ return(-1); }
			++_index;
		case 3: if (ram.l[_index] != (unsigned long)&ram.l[_index])
			{ return(-1); }
			++_index;
		case 2: if (ram.l[_index] != (unsigned long)&ram.l[_index])
			{ return(-1); }
			++_index;
		case 1: if (ram.l[_index] != (unsigned long)&ram.l[_index])
			{ return(-1); }
			++_index;
		}
	}
	sprintf(peek_addr, "0x%X", memory + size - 64);
	ids_peek(4, argv);
	printf("\n");
	return(0);
}

int ram_alias_main(int argc, char **argv)
{
	void *memory;
	int len;

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
		printf("%s: Missing memory region length\n", program);
		return(-1);
	}
	len = (int)strtol(*argv++, (char **)0, 0), --argc;

	if (ram_alias_test(memory, len) == -1)
		return(-1);
	return(0);
}

#ifdef IDS_SA
static int (*mb_callback)(mb_op_enum_t, ...) = 0;

int ram_callback(mb_op_enum_t op, ...)
{
	static int ack = 0;		/* The mailbox acknowledge */
	int status = 0;

	va_list args;

	va_start (args, op);

	if (!mb_callback)
		return(-1);

	switch (op)
	{
	case MB_ACK:
		++ack;			/* IDS has been ack'd */
	case MB_NOP:
		status = ack;		/* Return the ack count */
		break;
	case MB_PING:
		status = (*mb_callback)(MB_ACK);
		break;
	case MB_RELOC:
		if ((status = (*mb_callback)(op, u_boot_vectors)) == -1)
			break;
		break;
	case MB_GETFILE:
	{
		int arg;

		/*
		 * Grab the do_tftp stuff from the SR release and put it here.
		 */
		arg = va_arg(args, int);	/* Example */
		break;
	}
	case MB_PGMFILE:
		break;
	case MB_PUTFILE:
		break;
	case MB_END:
		status = (*mb_callback)(MB_END);
		ack = 0;
		mb_callback = (int (*)(mb_op_enum_t, ...))0;
		break;
	default:
		status = 0;
		break;
	}
	va_end (args);
	return(status);
}

int _start(int argc, char **argv)
{
	int rv = 0;

	if (ram_alias_main(argc, argv))
		++rv;
#ifdef COMMENT
	mb_callback = mb_start(ram_callback);
#endif /* COMMENT */
	return(rv);
}
#endif /* IDS_SA */
