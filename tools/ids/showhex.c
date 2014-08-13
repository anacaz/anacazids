/*
 * Integrated Diagnostics Subsystem
 *
 * rfa - 090610
 */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

void exit(int);

int main(int argc, char **argv)
{
	int odd, even;
	unsigned long hex;

	odd = even = 0;
	while (--argc)
	{
		if (!strcmp(*++argv, "odd"))
			odd = 1;
		if (!strcmp(*argv, "even"))
			even = 1;
	}
	for (hex = 0; hex < 256; ++hex)
	{
		int count;
		unsigned long bit;

		printf("%02X: ", hex);
		for (count = 0, bit = 0x80; bit; bit >>= 1)
		{
			if (hex & bit)
				printf("1"), ++count;
			else
				printf("0");
		}
		if (odd && count % 2)
			printf(" ODD %d", count);
		if (even && !(count % 2))
			printf(" EVEN %d", count);
		printf("\n");
	}
	exit(0);
}
