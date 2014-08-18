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
 * rfa - 061102-090506-15
 * Hardware abstraction layer.
 *
 * Copyright (c)2006-2009 Anacaz Networks, Inc., ALL RIGHTS RESERVED
 *
 * rfa - 061010-17-18-19
 */
#include <common.h>
#include <malloc.h>
#include <idshw.h>

#define HW_TABLE_MAX	256

static hw_t *hw_table[HW_TABLE_MAX];

#define FORWARD	/* */

FORWARD static int hw_lookup(const char *name);

// void example(void)
// {
//	int hd;
//
//	hd = load(hw_create("name", (void *)0x1234, 0, 1024));
// }

/*
 * Null out the hardware device table 'hw_table'.
 */
void hw_table_init(void)
{
	int hd;

	for (hd = 0; hd < HW_TABLE_MAX && hw_table[hd]; ++hd)
		hw_table[hd] = 0;
}

/*
 * The size value 'hwlen' includes the sizeof(hw_t) and the private
 * structure size that includes 'hw_t'.
 *
 * Returns pointer to newly created hardware pointer on success.
 * Returns 0 on failure.
 */
hw_t *hw_create(char *name, void *phy, int (*driver)(int, char **), int hwlen)
{
	hw_t *devp;

	if (!(devp = (hw_t *)malloc(hwlen)))
		return(0);
	devp->flags = HWF_CREATE;	/* This device was malloc'd */
	strcpy(devp->name, name);	/* Name */
	devp->driver = driver;		/* Driver */
	devp->phy = phy;		/* Physical location */
	return(devp);
}

/*
 * Load the device pointer into the hardware table.
 */
int hw_load(hw_t *devp)
{
	int hd;

	if ((hd = hw_lookup(devp->name)) != -1)
		return(-1);			/* Already exists */
	if ((hd = hw_lookup(0)) == -1)
		return(-1);			/* Hw table full */
	hw_table[hd] = devp;
	return(hd);
}

/*
 * Release the hardware table entry.
 */
void hw_terminate(int hd)
{
	if (hw_table[hd])
	{
		if (hw_table[hd]->flags & HWF_CREATE)
			free(hw_table[hd]);
		hw_table[hd] = 0;	/* BUG!!!  Defrag the hw table */
	}
}

/*
 * Attach the caller specified device 'name' to the indexed link 'off'
 * belonging to the hardware referenced by 'master'.
 *
 * Return slave handle on success, -1 on failure.
 */
int hw_attach(int master, int off, const char *name)
{
	int slave;

	if (!hw_table[master])
		return(-1);

	if ((slave = hw_lookup(name)) == -1)
		return(-1);

	switch (off)
	{
	case 0:
		hw_table[master]->ctl = hw_table[slave];
		break;
	case 1:
		hw_table[master]->data = hw_table[slave];
		break;
	case 2:
		hw_table[master]->link = hw_table[slave];
		break;
	default:
		return(-1);
	}

	return(slave);
}

void hw_detach(int master, int off)
{
	if (!hw_table[master])
		return;

	switch (off)
	{
	case 0:
		hw_table[master]->ctl = 0;
		break;
	case 1:
		hw_table[master]->data = 0;
		break;
	case 2:
		hw_table[master]->link = 0;
		break;
	default:
		break;
	}
}

/*
 * This function allows the caller to execute the command line 'cmd' upon
 * the device 'dev' and receive the returned status.
 */
int hw_launcher(char *cmd)
{
	extern int makeargs(char *line, char **argv);

	int argc;
	char *argv[16];
	int status;
	int hd;

	if (!(argc = makeargs(cmd, argv)))
		return(-1);
	if (!(hd = hw_lookup(argv[0])))
		return(-1);
	status = (*hw_table[hd]->driver)(argc, argv);
	return(status);
}

/*
 * If the caller passes a name then return the hardware descriptor 'hd' for
 * that name in the hardware table 'hw_table'.  If the caller passes null
 * for the name then return the hardware descriptor 'hd' for the first
 * available 'null' entry in the hardware table 'hw_table'.
 *
 * Return -1 on failure.
 */
static int hw_lookup(const char *name)
{
	int hd;

	if (!name)
	{
		/*
		 * Look for next available entry.
		 */
		for (hd = 0; hd < HW_TABLE_MAX; ++hd)
			if (!hw_table[hd])
				return(hd);
		return(-1);
	}
	/*
	 * Look for named device.
	 */
	for (hd = 0; hd < HW_TABLE_MAX && hw_table[hd]; ++hd)
		if (!strcmp(hw_table[hd]->name, name))
			return(hd);
	return(-1);
}
