/**********************************************************************
 *
 * File:    	print_utils.c
 *
 * Description: common print utilities used across all test directories
 *
 **********************************************************************/
#ifdef LINUX
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#include <malloc.h>
#else /* LINUX */
#include <common.h>
#endif /* LINUX */
#include <ids_types.h>

void print_buf(unsigned char *cp, int bOffset, int count, int type, int print_as_flag)
{
	unsigned int items_per_line = 0;
	unsigned int i, j;
	unsigned long paddr = 0;
	unsigned long pi = 0;
	unsigned short *sp = 0;
	unsigned long *lp = 0;

	switch(type)
	{
	case TYPE_CHAR: items_per_line = 16; pi = 1; break;
	case TYPE_SHORT: items_per_line = 8; sp = (unsigned short *)cp; count /= 2; pi = 2; break;
	case TYPE_LONG: items_per_line = 4; lp = (unsigned long *)cp; count /= 4; pi = 4; break;
	}

	switch (print_as_flag)
	{
	case PRINT_AS_OFFSET: paddr = (unsigned long)bOffset; break;
	case PRINT_AS_ADDRESS: paddr = (unsigned long)cp; break;
	}

	printf("\n");
	switch (type)
	{
	case TYPE_CHAR:
		for (i=0; count > 0;)
		{
			printf("0x%08x: ", paddr+(i*pi));
			for(j=0; (j<items_per_line) && (count > 0); j++)
			{
      				printf("%02X ", *(cp+i));
				i++;
				count--;
			}
			printf("\n");
		}
		break;
	case TYPE_SHORT:
		for (i=0; count > 0;)
		{
			printf("0x%08x: ", paddr+(i*pi));
			for(j=0; (j<items_per_line) && (count > 0); j++)
			{
      				printf("%04X ", *(sp+i));
				i++;
				count--;
			}
			printf("\n");
		}
		break;
	case TYPE_LONG:
		for (i=0; count > 0;)
		{
			printf("0x%08x: ", paddr+(i*pi));
			for(j=0; (j<items_per_line) && (count > 0); j++)
			{
      				printf("%08X ", *(lp+i));
				i++;
				count--;
			}
			printf("\n");
		}
		break;
	}

    	printf("\n");
}

