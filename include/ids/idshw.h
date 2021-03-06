#ifndef _ids_hw_h
#define _ids_hw_h
/*
 * Integrated Diagnostics Subsystem
 *
 * Copyright (c) 2006-2014 "Anacaz Networks, Inc."
 * Diagnostics subsystem for U-Boot
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
 * Hardware abstraction layer.
 *
 * rfa - 061010-090708
 */
#include <idshw_rgb.h>

/*
 * Public Device Information:
 *
 *  ctl <----> device <----> data
 *               |
 *               +---------> link
 *
 * This structure is instantiated as the first element of a private
 * device structure.  (This is why the pointer 'desc' has been deprecated.)
 * The device routines treat this structure as a dev_t element and the
 * dev_t pointer is cast to the private device data sturcture to retrieve
 * the private device information.
 *
 *	HW MODULE OPERATIONS
 *	--------------------
 *	1.) create
 *	2.) attach
 *	3.) detach
 *	4.) terminate
 */

/*
 * Defined values returned by the driver.
 */
#define STAT_NOPRES	0
#define STAT_NORESET	1
#define STAT_NOINIT	2
#define STAT_NOBOOT	3
#define STAT_READY	4
#define STAT_BUSY	5
#define STAT_FAULT	6

#define HWF_CREATE		0x0001

typedef struct hw
{
	unsigned short	flags;
	char		name[32];
	int		(*driver)(int argc, char **argv);
	void		*phy;		/* Base physical location in HW */
	struct hw	*ctl;		/* Fixed control plane I/O */
	struct hw	*data;		/* Attached data plane I/O */
	struct hw	*link;		/* Linked down stream I/O */
} hw_t;

#define HW_DEVICE(name,driver,phy,ctl) \
{ \
	0, #name, (driver), (phy), (ctl), 0, 0 \
}

/*
 * Hardware manipulation functions.
 */
hw_t *hw_create(char *name, void *phy, int (*driver)(int, char **), int hwlen);
int hw_load(hw_t *devp);
void hw_terminate(int hd);
int hw_attach(int master, int off, const char *name);
void hw_detach(int master, int off);
int hw_launcher(char *cmd);

#endif /* !_ids_hw_h */
