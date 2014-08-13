/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * IDS Integrated Diagnostcis System
 */

#include <common.h>
#include <command.h>

#ifdef	CMD_IDS_DEBUG
#define	PRINTF(fmt,args...)	printf (fmt ,##args)
#else
#define PRINTF(fmt,args...)
#endif

#if defined(CONFIG_CMD_IDS)
/* Integrated Diagnostics System
 *
 * Syntax:
 *	ids
 */
int do_ids(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	extern int ids_main(int argc, char **argv);

	return(ids_main(argc, argv));
}

/**************************************************/
U_BOOT_CMD(
	ids,     4,     0,      do_ids,
	"ids     - integrated diagnostics system\n",
	""
);

#endif
