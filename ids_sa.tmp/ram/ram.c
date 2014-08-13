/*
 * Integrated Diagnostics Subsystem
 *
 * rfa - 060808-21-090505
 */
#include <idsfs.h>
#include <menu.h>
#include <test_api.h>
#include <ids_jmpvec.h>
#ifndef IDS_SA
#include <common.h>
#endif /* !IDS_SA */

typedef union
{
	unsigned char		*c;
	unsigned short		*s;
	unsigned long		*l;
	unsigned long long	*ll;
} ref_types_u;

typedef union
{
	unsigned char		c;
	unsigned short		s;
	unsigned long		l;
	unsigned long long	ll;
} data_types_u;

ref_types_u ram;
data_types_u data;
data_types_u pattern;

static unsigned char ham_c[] =
{
	0x00, 0xFF, 0xF0, 0xCC, 0xAA, 0x55
};

static unsigned short ham_s[] =
{
	0x0000, 0xFFFF, 0xFF00, 0xF0F0, 0xCCCC, 0xAAAA, 0x5555
};

static unsigned long ham_l[] =
{
	0x00000000, 0xFFFFFFFF, 0xFFFF0000, 0xFF00FF00,
	0xF0F0F0F0, 0xCCCCCCCC, 0xAAAAAAAA, 0x55555555
};

static unsigned long long ham_ll[] =
{
	0x0000000000000000LL, 0xFFFFFFFFFFFFFFFFLL,
	0xFFFFFFFF00000000LL, 0xFFFF0000FFFF0000LL,
	0xFF00FF00FF00FF00LL, 0xF0F0F0F0F0F0F0F0LL,
	0xCCCCCCCCCCCCCCCCLL, 0xAAAAAAAAAAAAAAAALL,
	0x5555555555555555LL
};

#define RAM_FILL(X) \
do \
{ \
	ram.X = memory; \
	index = 0; \
	switch (overrun) \
	while (index < count) \
	{ \
	case 0: ram.X[index++] = pattern.X; \
	case 7: ram.X[index++] = pattern.X; \
	case 6: ram.X[index++] = pattern.X; \
	case 5: ram.X[index++] = pattern.X; \
	case 4: ram.X[index++] = pattern.X; \
	case 3: ram.X[index++] = pattern.X; \
	case 2: ram.X[index++] = pattern.X; \
	case 1: ram.X[index++] = pattern.X; \
	} \
} while (0)

#define CACHE_FILL(X) \
do \
{ \
	ram.X = memory; \
	index = 0; \
	switch (overrun) \
	while (index < count) \
	{ \
	case 0: data.X = ram.X[index++]; \
	case 7: data.X = ram.X[index++]; \
	case 6: data.X = ram.X[index++]; \
	case 5: data.X = ram.X[index++]; \
	case 4: data.X = ram.X[index++]; \
	case 3: data.X = ram.X[index++]; \
	case 2: data.X = ram.X[index++]; \
	case 1: data.X = ram.X[index++]; \
	} \
} while (0)

#define RAM_FIND(X) \
do \
{ \
	ram.X = memory; \
	index = 0; \
	switch (overrun) \
	while (index < count) \
	{ \
	case 0: \
		if (ram.X[index] != pattern.X) \
		{ \
			printf("\t\t\t\t\t\t\t       **** FAILED ****\n"); \
			GTEST_FAIL(RAM_REFRESH_FAIL); \
		} \
		++index; \
	case 7: \
		if (ram.X[index] != pattern.X) \
		{ \
			printf("\t\t\t\t\t\t\t       **** FAILED ****\n"); \
			GTEST_FAIL(RAM_REFRESH_FAIL); \
		} \
		++index; \
	case 6: \
		if (ram.X[index] != pattern.X) \
		{ \
			printf("\t\t\t\t\t\t\t       **** FAILED ****\n"); \
			GTEST_FAIL(RAM_REFRESH_FAIL); \
		} \
		++index; \
	case 5: \
		if (ram.X[index] != pattern.X) \
		{ \
			printf("\t\t\t\t\t\t\t       **** FAILED ****\n"); \
			GTEST_FAIL(RAM_REFRESH_FAIL); \
		} \
		++index; \
	case 4: \
		if (ram.X[index] != pattern.X) \
		{ \
			printf("\t\t\t\t\t\t\t       **** FAILED ****\n"); \
			GTEST_FAIL(RAM_REFRESH_FAIL); \
		} \
		++index; \
	case 3: \
		if (ram.X[index] != pattern.X) \
		{ \
			printf("\t\t\t\t\t\t\t       **** FAILED ****\n"); \
			GTEST_FAIL(RAM_REFRESH_FAIL); \
		} \
		++index; \
	case 2: \
		if (ram.X[index] != pattern.X) \
		{ \
			printf("\t\t\t\t\t\t\t       **** FAILED ****\n"); \
			GTEST_FAIL(RAM_REFRESH_FAIL); \
		} \
		++index; \
	case 1: \
		if (ram.X[index] != pattern.X) \
		{ \
			printf("\t\t\t\t\t\t\t       **** FAILED ****\n"); \
			GTEST_FAIL(RAM_REFRESH_FAIL); \
		} \
		++index; \
	} \
} while (0)

#define RAM_READBACK(X) \
do \
{ \
	ram.X = memory; \
	index = 0; \
	switch (overrun) \
	while (index < count) \
	{ \
	case 0: ram.X[index] = pattern.X; data.X = ram.X[index]; \
		if (data.X != pattern.X)			 \
		{ \
			printf("\t\t\t\t\t\t\t       **** FAILED ****\n"); \
			GTEST_FAIL(RAM_INTEGRITY_FAIL); \
		} \
		++index; \
	case 7: ram.X[index] = pattern.X; data.X = ram.X[index]; \
		if (data.X != pattern.X)			 \
		{ \
			printf("\t\t\t\t\t\t\t       **** FAILED ****\n"); \
			GTEST_FAIL(RAM_INTEGRITY_FAIL); \
		} \
		++index; \
	case 6: ram.X[index] = pattern.X; data.X = ram.X[index]; \
		if (data.X != pattern.X)			 \
		{ \
			printf("\t\t\t\t\t\t\t       **** FAILED ****\n"); \
			GTEST_FAIL(RAM_INTEGRITY_FAIL); \
		} \
		++index; \
	case 5: ram.X[index] = pattern.X; data.X = ram.X[index]; \
		if (data.X != pattern.X)			 \
		{ \
			printf("\t\t\t\t\t\t\t       **** FAILED ****\n"); \
			GTEST_FAIL(RAM_INTEGRITY_FAIL); \
		} \
		++index; \
	case 4: ram.X[index] = pattern.X; data.X = ram.X[index]; \
		if (data.X != pattern.X)			 \
		{ \
			printf("\t\t\t\t\t\t\t       **** FAILED ****\n"); \
			GTEST_FAIL(RAM_INTEGRITY_FAIL); \
		} \
		++index; \
	case 3: ram.X[index] = pattern.X; data.X = ram.X[index]; \
		if (data.X != pattern.X)			 \
		{ \
			printf("\t\t\t\t\t\t\t       **** FAILED ****\n"); \
			GTEST_FAIL(RAM_INTEGRITY_FAIL); \
		} \
		++index; \
	case 2: ram.X[index] = pattern.X; data.X = ram.X[index]; \
		if (data.X != pattern.X)			 \
		{ \
			printf("\t\t\t\t\t\t\t       **** FAILED ****\n"); \
			GTEST_FAIL(RAM_INTEGRITY_FAIL); \
		} \
		++index; \
	case 1: ram.X[index] = pattern.X; data.X = ram.X[index]; \
		if (data.X != pattern.X)			 \
		{ \
			printf("\t\t\t\t\t\t\t       **** FAILED ****\n"); \
			GTEST_FAIL(RAM_INTEGRITY_FAIL); \
		} \
		++index; \
	} \
} while (0)

int refresh_test(void *memory, int size, int access)
{
	int index, count, overrun;
	int next, limit;

	printf("[ TEST RAM REFRESH: 0x%08X:%d %d BIT ACCESS ]\n",
		memory, size, access * 8);
	if (access)
	{
		count = size / access;
		overrun = size % 8;
		next = 0;
	}
	switch (access)
	{
	case sizeof(char):
		limit = sizeof(ham_c) / access;
		while (next < limit)
		{
			pattern.c = ham_c[next++];
			printf("\t[ FILL:PATTERN=0x%02X ", pattern.c);
			RAM_FILL(c);
			printf("FIND:PATTERN=0x%02X ]\n", pattern.c);
			RAM_FIND(c);
		}
		break;
	case sizeof(short):
		limit = sizeof(ham_s) / access;
		while (next < limit)
		{
			pattern.s = ham_s[next++];
			printf("\t[ FILL:PATTERN=0x%04X ", pattern.s);
			RAM_FILL(s);
			printf("FIND:PATTERN=0x%04X ]\n", pattern.s);
			RAM_FIND(s);
		}
		break;
	case sizeof(long):
		limit = sizeof(ham_l) / access;
		while (next < limit)
		{
			pattern.l = ham_l[next++];
			printf("\t[ FILL:PATTERN=0x%08X ", pattern.l);
			RAM_FILL(l);
			printf("FIND:PATTERN=0x%08X ]\n", pattern.l);
			RAM_FIND(l);
		}
		break;
	case 64:
		limit = sizeof(ham_ll) / access;
		while (next < limit)
		{
			pattern.ll = ham_ll[next++];
			printf("\t[ FILL:PATTERN=0x%08X%08X ",
				(unsigned long)(pattern.ll >> 32),
				(unsigned long)(pattern.ll & 0xFFFFFFFF));
			RAM_FILL(ll);
			printf("FIND:PATTERN=0x%08X%08X ]\n",
				(unsigned long)(pattern.ll >> 32),
				(unsigned long)(pattern.ll & 0xFFFFFFFF));
			RAM_FIND(ll);
		}
		break;
	default:
		printf("\t\t\t\t\t\t\t       **** INVALID ****\n");
		GTEST_FAIL(RAM_REFRESH_FAIL);
	}
	printf("\t\t\t\t\t\t\t       **** PASSED ****\n");
	GTEST_PASS(RAM_REFRESH_PASS);
	return(0);					/* BUG!!! */
}

int integrity_test(void *memory, int size, int access)
{
	int index, count, overrun;
	int next, limit;

	printf("[ TEST RAM INTEGRITY: 0x%08X:%d %d BIT ACCESS ]\n",
		memory, size, access * 8);
	if (access)
	{
		count = size / access;
		overrun = size % 8;
		index = 0;
		next = 0;
	}
	switch (access)
	{
	case sizeof(char):
		limit = sizeof(ham_c) / access;
		while (next < limit)
		{
			pattern.c = ham_c[next++];
			printf("\t[ READBACK:PATTERN=0x%02X ]\n", pattern.c);
			RAM_READBACK(c);
		}
		break;
	case sizeof(short):
		limit = sizeof(ham_s) / access;
		while (next < limit)
		{
			pattern.s = ham_s[next++];
			printf("\t[ READBACK:PATTERN=0x%04X ]\n", pattern.s);
		RAM_READBACK(s);
		}
		break;
	case sizeof(long):
		limit = sizeof(ham_l) / access;
		while (next < limit)
		{
			pattern.l = ham_l[next++];
			printf("\t[ READBACK:PATTERN=0x%08X ]\n", pattern.l);
			RAM_READBACK(l);
		}
		break;
#ifdef COMMENT
	case sizeof(long long):
		limit = sizeof(ham_ll) / access;
		while (next < limit)
		{
			pattern.ll = ham_ll[next++];
			printf("\t[ READBACK:PATTERN=0x%08X%08X ]\n",
				pattern.ll);
			RAM_READBACK(ll);
		}
		break;
#endif /* COMMENT */
	default:
		printf("\t\t\t\t\t\t\t       **** INVALID ****\n");
		GTEST_FAIL(RAM_INTEGRITY_FAIL);
	}
	printf("\t\t\t\t\t\t\t       **** PASSED ****\n");
	GTEST_PASS(RAM_INTEGRITY_PASS);
	return(0);
}

int cache_fill_test(void *memory, int size, int access)
{
	int index, count, overrun;
	int next, limit;

	printf("[ TEST CACHE FILL: 0x%08X:%d %d BIT ACCESS ]\n",
		memory, size, access * 8);
	if (!access)
	{
		printf("\t\t\t\t\t\t\t       **** FAILED ****\n");
		GTEST_FAIL(NULL_CNTR);
	}
	count = size / access;
	overrun = size % 8;
	index = 0;
	next = 0;
	switch (access)
	{
	case sizeof(char):
		limit = sizeof(ham_c) / access;
		while (next < limit)
		{
			pattern.c = ham_c[next++];
			printf("\t[ CACHE-FILL:PATTERN=0x%02X ]\n", pattern.c);
			CACHE_FILL(c);
		}
		break;
	case sizeof(short):
		limit = sizeof(ham_s) / access;
		while (next < limit)
		{
			pattern.s = ham_s[next++];
			printf("\t[ CACHE-FILL:PATTERN=0x%04X ]\n", pattern.s);
			CACHE_FILL(s);
		}
		break;
	case sizeof(long):
		limit = sizeof(ham_l) / access;
		while (next < limit)
		{
			pattern.l = ham_l[next++];
			printf("\t[ CACHE-FILL:PATTERN=0x%08X ]\n", pattern.l);
			CACHE_FILL(l);
		}
		break;
#ifdef COMMENT
	case sizeof(long long):
		limit = sizeof(ham_ll) / access;
		while (next < limit)
		{
			pattern.ll = ham_ll[next++];
			printf("\t[ CACHE-FILL:PATTERN=0x%08X%08X ]\n",
				pattern.ll);
			CACHE_FILL(ll);
		}
		break;
#endif /* COMMENT */
	default:
		printf("\t\t\t\t\t\t\t       **** INVALID ****\n");
		GTEST_FAIL(RAM_CACHE_FAIL);
	}
	printf("\t\t\t\t\t\t\t       **** PASSED ****\n");
	GTEST_PASS(RAM_CACHE_PASS);
	return(0);
}

int ram_refresh_main(int argc, char **argv)
{
	void *memory;
	int len;
	static char ram_refresh_test[21] = { "RAM Refresh Test    " };

	if (GTEST_FLAGS(RAM_REFRESH_PASS) == CNTR_INACTIVE)
	{
		GTEST_NEW_CNTR(RAM_REFRESH_PASS, CNTR_PASS,
			0, ram_refresh_test, sizeof(ram_refresh_test));
		GTEST_NEW_CNTR(RAM_REFRESH_FAIL, CNTR_FAIL,
			0, ram_refresh_test, sizeof(ram_refresh_test));
	}

	if (!--argc)
	{
		printf("Usage: %s start-address byte-count\n", *argv);
		return(-1);
	}
	memory = (void *)strtol(argv[1], (char **)0, 0);
	len = (int)strtol(argv[2], (char **)0, 0);

	refresh_test(memory, len, sizeof(char));
	refresh_test(memory, len, sizeof(short));
	refresh_test(memory, len, sizeof(long));
	refresh_test(memory, len, sizeof(long long));
	return(0);
}

int ram_integrity_main(int argc, char **argv)
{
	void *memory;
	int len;
	static char ram_integrity_test[21] = { "RAM Integrity Test  " };

	if (GTEST_FLAGS(RAM_INTEGRITY_PASS) == CNTR_INACTIVE)
	{
		GTEST_NEW_CNTR(RAM_INTEGRITY_PASS, CNTR_PASS,
			0, ram_integrity_test, sizeof(ram_integrity_test));
		GTEST_NEW_CNTR(RAM_INTEGRITY_FAIL, CNTR_FAIL,
			0, ram_integrity_test, sizeof(ram_integrity_test));
	}

printf("ram_integrity_main: %s %s %s\n", argv[0], argv[1], argv[2]);

	if (!--argc)
	{
		printf("Usage: %s start-address byte-count\n", *argv);
		return(-1);
	}
	memory = (void *)strtol(argv[1], (char **)0, 0);
	len = (int)strtol(argv[2], (char **)0, 0);

	integrity_test(memory, len, sizeof(char));
	integrity_test(memory, len, sizeof(short));
	integrity_test(memory, len, sizeof(long));
	integrity_test(memory, len, sizeof(long long));
	return(0);
}

int ram_cache_main(int argc, char **argv)
{
	void *memory;
	int len;
	static char ram_cache_test[21] = { "RAM Cache Test      " };

	if (GTEST_FLAGS(RAM_CACHE_PASS) == CNTR_INACTIVE)
	{
		GTEST_NEW_CNTR(RAM_CACHE_PASS, CNTR_PASS,
			0, ram_cache_test, sizeof(ram_cache_test));
		GTEST_NEW_CNTR(RAM_CACHE_FAIL, CNTR_FAIL,
			0, ram_cache_test, sizeof(ram_cache_test));
	}

	if (!--argc)
	{
		printf("Usage: %s start-address byte-count\n", *argv);
		return(-1);
	}
	memory = (void *)strtol(argv[1], (char **)0, 0);
	len = (int)strtol(argv[2], (char **)0, 0);

	cache_fill_test(memory, len, sizeof(char));
	cache_fill_test(memory, len, sizeof(short));
	cache_fill_test(memory, len, sizeof(long));
	cache_fill_test(memory, len, sizeof(long long));
	return(0);
}

#ifdef IDS_SA
int _start(int argc, char **argv)
{
#ifdef COMMENT
	pvalue_t x;
#endif /* COMMENT */
	int rv;

	rv = 0;
#ifdef COMMENT
	x = patgen(x, x, XOR);
	printf("patgen()=0x%X\n", PG_REFCHAR(x));
#endif /* COMMENT */
	printf("_start:\targv[0]=%s\n", argv[0]);
	printf("_start:\targv[1]=%s\n", argv[1]);
	printf("_start:\targv[2]=%s\n", argv[2]);
	if (ram_refresh_main(argc, argv))
		++rv;
	if (ram_integrity_main(argc, argv))
		++rv;
	if (ram_cache_main(argc, argv))
		++rv;
	
	printf("\n[ System RAM Refresh Test:]\n");
	printf("\t[ PASS Count: %d ]\n", GTEST_GET_CNTR(RAM_REFRESH_PASS));
	printf("\t[ FAIL Count: %d ]\n\n", GTEST_GET_CNTR(RAM_REFRESH_FAIL));

	printf("\n[ System RAM Integrity Test:]\n");
	printf("\t[ PASS Count: %d ]\n", GTEST_GET_CNTR(RAM_INTEGRITY_PASS));
	printf("\t[ FAIL Count: %d ]\n\n", GTEST_GET_CNTR(RAM_INTEGRITY_FAIL));

	printf("\n[ System RAM Cache Test:]\n");
	printf("\t[ PASS Count: %d ]\n", GTEST_GET_CNTR(RAM_CACHE_PASS));
	printf("\t[ FAIL Count: %d ]\n\n", GTEST_GET_CNTR(RAM_CACHE_FAIL));

	return(rv);
}
#endif /* IDS_SA */
