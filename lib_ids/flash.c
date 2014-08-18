/*
 * Integrated Diagnostics Subsystem
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
 * u-boot jump vector interface file.
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
