/*
 * Integrated Diagnostics Subsystem
 *
 * rfa - 060808-21-090505-31-0601
 */
#include <common.h>
#include <test_api.h>
#include <ids_jmpvec.h>
#include "ram.h"

static char *program;

static pvalue_t seed;
static pvalue_t foreground;
static pvalue_t background;
static unsigned long memend;		/* Region from memory to memend */
static unsigned long start;
static unsigned long stop;		/* Single bit addrs in region */
unsigned long location;
unsigned long mask;

#define FORWARD	/**/

FORWARD static void ram_addr_init(void *memory, int size);
FORWARD int ram_addr_lo_test(void *memory, int size);
FORWARD int ram_addr_lo_main(int argc, char **argv);

/*
 * TEST-1: addr_stuck_lo_test
 *
 * 1) Write all address locations w/background pattern
 * 2) Write base location w/foreground pattern
 * 3) Read all address locations to confirm backgroud pattern
 * 4) FAIL if foreground pattern found
 * 5) FAIL if background pattern not found
 * 6) PASS if background pattern found
 */
int ram_addr_lo_test(void *memory, int size)
{
	static unsigned long content;
	int count, overrun;
	int next;
	int access;

	access = sizeof(long);
	count = size / access;
	overrun = count % 8;
	next = 0;

	RAM_TITLE(ADDR-LO);

	PG_DATA_TYPE(seed) = sizeof(long);
	LONG(seed) = 0x55555555L;

	PG_DATA_TYPE(background) = sizeof(long);
	background = spatgen(background, seed, modgen("fives"));

	PG_DATA_TYPE(foreground) = sizeof(long);
	foreground = spatgen(foreground, seed, modgen("fives"));
	foreground = patgen(foreground, NOMASK, modgen("invert"));
	
	ram_addr_init(memory, size);

	/*
	 * BUG!!!  Handle 8-bit addr 1 and 16-bit addr 2 bits.
	 */
	/*
	 * Write the background pattern to all locations.
	 */
	for (mask = 4L; mask && mask <= stop; mask <<= 1L)
	{
		location = start | mask;
		if (location == start)
			continue;
		*(unsigned long *)location = LONG(background);
		content = *(unsigned long *)location;
		printf("\t[ %s set BG location:%08X = %08X ]\n", program,
			location, content);
	}

	/*
	 * Write the foreground pattern to the base location.
	 */
	*(unsigned long *)start = LONG(foreground);
	printf("\t[ %s set FG location:%08X = %08X ]\n", program,
		start, *(unsigned long *)start);
	/*
	 * If start is non-zero check location 0x00000000 to confirm
	 * that start doesn't have a problem.
	 */
	printf("\t[ %s get FG location:%08X = %08X ]\n", program,
		0, *(unsigned long *)0);
	if (start && *(unsigned long *)0 == LONG(foreground))
		return(-1);

	/*
	 * Confirm the background pattern at all locations except for the base.
	 */
	for (mask = 4L; mask && mask <= stop; mask <<= 1L)
	{
		content = LONG(foreground);
		location = start | mask;
		if (location == start)
			continue;
		content = *(unsigned long *)location;
		printf("\t[ %s get BG location:%08X = %08X ]\n", program,
			location, content);
		if (content != LONG(background))
			return(-1);
	}
	return(0);
}

int ram_addr_lo_main(int argc, char **argv)
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

	if (ram_addr_lo_test(memory, len) == -1)
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

	if (ram_addr_lo_main(argc, argv))
		++rv;
#ifdef COMMENT
	mb_callback = mb_start(ram_callback);
#endif /* COMMENT */
	return(rv);
}
#endif /* IDS_SA */

static void ram_addr_init(void *memory, int size)
{
	if (!memory)
		start = 0L;
	for (start = 1L; start < (unsigned long)memory; start <<= 1L)
		;
	if (start < 4L)
		start = 4L;
	memend = (unsigned long)(memory + size);
	for (stop = start; stop < memend; stop <<= 1L)
		;
	if (!stop)
		stop = 0x80000000L;
	if (stop > memend)
		stop >>= 1L;		/* Adjustment required!!! */
}

