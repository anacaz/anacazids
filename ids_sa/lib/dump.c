
#ifdef LINUX
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#include <malloc.h>
#else /* LINUX */
#endif /* LINUX */

typedef struct dumpdata dumpdata_t;

struct dumpdata
{
	unsigned long (*dumpchar)(const void *p, int n);
	unsigned long (*dumpshort)(const void *p, int n);
	unsigned long (*dumplong)(const void *p, int n);
};

#define DUMPDATA(base) \
dumpdata_t base##dumpdata = { base##dumpchar, base##dumpshort, base##dumplong }

unsigned long nulldumpchar(const void *p, int n)
{
	printf("error: invalid base ");
	return(0);
}

unsigned long nulldumpshort(const void *p, int n)
{
	printf("error: invalid base ");
	return(0);
}

unsigned long nulldumplong(const void *p, int n)
{
	printf("error: invalid base ");
	return(0);
}

unsigned long hexdumpchar(const void *p, int n)
{
	while (n--)
	{
		printf("%02X ", *(unsigned char *)p);
		p = &((unsigned char *)p)[1];
	}
	return((unsigned long)p);
}

unsigned long hexdumpshort(const void *p, int n)
{
	n /= sizeof(short);

	printf("%04X  ", *(unsigned short *)p);
	return((unsigned long)&((unsigned short *)p)[1]);
}

unsigned long hexdumplong(const void *p, int n)
{
	n /= sizeof(short);

	printf("%08X    ", *p);
}

unsigned long octdumpchar(const void *p) { printf("%03o ", *p); return(0); }
unsigned long octdumpshort(const void *p) { printf("%03o  ", *p); return(0); }
unsigned long octdumplong(const void *p) { printf("%03o    ", *p); return(0); }

unsigned long decdumpchar(const void *p) { printf("%02X ", *p); return(0); }
unsigned long decdumpshort(const void *p) { printf("%04X  ", *p); return(0); }
unsigned long decdumplong(const void *p) { printf("%08X    ", *p); return(0); }

DUMPDATA(null);
DUMPDATA(hex);
DUMPDATA(oct);
DUMPDATA(dec);

void dump(unsigned long addr, int size, int width, int base)
{
	dumpdata_t *dp;
	int cols;

	if (base == 16)
		dp = &hexdumpdata;
	else
		dp = &nulldumpdata;

	if (width == sizeof(long))
	{
		dumplines(dp->dumplong, addr, 
		for (cols = 16; size > 0; size -= cols)
		{
			dumpaddr(addr, base);
			cols = size > 16 ? 16 : size;
			(*dp->dumplong)(&addr, cols / width);
			dumpascii(addr, cols);
		}
	}
	else if (width == sizeof(short))
	{
		cols /= width;
	}
	else /* if (width == sizeof(char)) */
	{
	}
}

void dumplines((*dumpline)(unsigned long, int), unsigned long addr, int len)
{
	int cols;

	for (cols = 16; size > 0; size -= cols)
	{
		dumpaddr(addr, base);
		cols = size > 16 ? 16 : size;
		(*dp->dumplong)(&addr, cols);
		dumpascii(addr, cols);
	}
}

void dumpaddr(unsigned long addr)
{
	printf("08%X ", addr);
}

/*
 * Dump 1 80 column line in the following format(s):
 *
 * HEX: 48 spaces available:
 *
 * FF0055AA 45 34 34 34 34 23 02 02 02 34 34 23 12 23 34 34 ................
 * FF0055AA 4534 3434 3423 0202 0234 3423 1223 3434         ................
 * FF0055AA 45343434 34230202 02343423 12233434             ................
 *
 * OCT: 48 spaces available:
 *
 * FF0055AA 023 023 012 128 101 101 101 101 101 101 101 101 101 200 200 200
 * FF0055AA 453463 343453 342353 020253 023453 342353 122353 343453
 * FF0055AA 45343434342 45343434342 45343434342 45343434342 
 *
 * DEC:
 *
 * FF0055AA 023 023 012 128 101 101 101 101 101 101 101 101  ................
 * FF0055AA 64535 65535 54000 64000 64000 23000  34343 34567 ................
 */
void dumpascii(char *buf, int len)
{
	while (len--)
	{
		printf("%c", isprint(*buf) ? *buf : '.');
		++buf;
	}
	printf("\n");
}
