/*
 * Integrated Diagnostics Subsystem
 *
 * rfa - 060808-21-090505
 */
#include <common.h>
#include <test_api.h>
#include <ids_jmpvec.h>
#include "ram.h"

static char *program;

int readback_test(void *memory, int size, int access, char *type)
{
	pvalue_t generated;
	pmod_t mod;
	int count, overrun;
	int next;

	RAM_TITLE(READBACK);
	if (!access)
		access = sizeof(char);
	count = size / access;
	overrun = count % 8;
	next = 0;
	PG_DATA_TYPE(generated) = access;
	mod = modgen(type);
	switch (access)
	{
	case sizeof(char):
		generated = spatgen(generated, generated, mod);
		while (!TST_PVF(generated,PVF_END))
		{
			pattern.c = CHAR(generated);
			printf("\t[ %s fill:%02X ", program, pattern.c);
			printf("find:%02X ]\n", pattern.c);
			RAM_READBACK(c);
			generated = patgen(generated, NOMASK, mod);
		}
		break;
	case sizeof(short):
		generated = spatgen(generated, generated, mod);
		while (!TST_PVF(generated,PVF_END))
		{
			pattern.s = SHORT(generated);
			printf("\t[ %s fill:%04X ", program, pattern.s);
			printf("find:%04X ]\n", pattern.s);
			RAM_READBACK(s);
			generated = patgen(generated, NOMASK, mod);
		}
		break;
	case sizeof(long):
		generated = spatgen(generated, generated, mod);
		while (!TST_PVF(generated,PVF_END))
		{
			pattern.l = LONG(generated);
			printf("\t[ %s fill:%08X ", program, pattern.l);
			printf("find:%08X ]\n", pattern.l);
			RAM_READBACK(l);
			generated = patgen(generated, NOMASK, mod);
		}
		break;
	default:
		return(-1);
	}
	return(0);
}

int ram_readback_main(int argc, char **argv)
{
	void *memory;
	int len;
	char *type;
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
		printf("%s: Missing memory region length\n", program);
		return(-1);
	}
	len = (int)strtol(*argv++, (char **)0, 0), --argc;
	if (!argc)
	{
		type = "ham";
		access = "char";
	}
	else
		type = *argv++, --argc;
	if (argc)
		access = *argv++, --argc;

	if (!strcmp(access, "char"))
		return(readback_test(memory, len, sizeof(char), type));
	else if (!strcmp(access, "short"))
		return(readback_test(memory, len, sizeof(short), type));
	else if (!strcmp(access, "long"))
		return(readback_test(memory, len, sizeof(long), type));
	else if (!strcmp(access, "longlong"))
		return(readback_test(memory, len, sizeof(long long), type));
	else
		printf("Invalid access %s; use char, short, or long\n");
	return(-1);
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

	if (ram_readback_main(argc, argv))
		++rv;
#ifdef COMMENT
	mb_callback = mb_start(ram_callback);
#endif /* COMMENT */
	return(rv);
}
#endif /* IDS_SA */
