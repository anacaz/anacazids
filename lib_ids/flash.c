/*
 * Integrated Diagnostics Subsystem
 *
 * rfa - 060724
 */
#ifdef LINUX
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#else /* LINUX */
#include <common.h>
#include <linux/ctype.h>
#include <idsio.h>
#endif /* LINUX */
#include <idsfs.h>
#include <menu.h>

#ifdef LINUX
#define FLASHSIZE	0x800000
static unsigned char flashdevice[FLASHSIZE];
#else /* LINUX */
#define FLASHSIZE	0x100
static unsigned char flashdevice[FLASHSIZE];
#endif /* LINUX */
static int flashindex = 0;

void ids_flash_list(char *results)
{
}

unsigned char *ids_flash_getdevice(void)
{
	return(flashdevice);
}

void ids_flash_store(unsigned char *buffer, int count)
{
	int n;

	printf("ids_flash_store(0x%X, %d)\n", buffer, count);
	for (n = 0; n < count; ++n)
		flashdevice[flashindex++] = buffer[n];
}

void ids_flash_erase()
{
	printf("Erasing flash ...\n");
	for (flashindex = 0; flashindex < FLASHSIZE; ++flashindex)
	{
		flashdevice[flashindex] = 0;
	}
	printf("Flash erased!\n");
	flashindex = 0;
}

void ids_flash_add(menu_t *p)
{
}
