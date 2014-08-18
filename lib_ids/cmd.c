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
 * This is the cli command table.  New commands get added here along
 * with the cli-ification of menu items.
 *
 * The command structure is intentionally not exported.  All functions
 * that need to operate on the command structure need to in this file
 * and and api function exported beyond this file.
 *
 * Copyright (c)2006-2009 Anacaz Networks, Inc., ALL RIGHTS RESERVED
 *
 * rfa - 090623-24
 */
#include <common.h>
#include <linux/ctype.h>
#include <linux/string.h>
#include <idsio.h>
#include <test_api.h>

#define FORWARD	/* */

/*
 * IDS Command Table
 *
 * Add new cli commands right here.
 */
FORWARD int cli(int c);
FORWARD cmd_t *findcmd(char *name);
extern int menu_show_main(int, char **);

FORWARD static int help_main(int, char **);

extern int ids_poke(int, char **);
extern int ids_peek(int, char **);
extern int ram_addr_hi_main(int, char **);
extern int ram_addr_lo_main(int, char **);
extern int ram_alias_main(int, char **);
extern int ram_cache_main(int, char **);
extern int ram_refresh_main(int, char **);
extern int ram_readback_main(int, char **);
extern int reg_rw_main(int, char **);
extern int reg_ro_main(int, char **);
extern int reg_wo_main(int, char **);

cmd_t cmd_tbl[] =
{
    { 1, "help",        help_main,	  "Display command list"	   },
    { 1, "poke",        ids_poke,	  "[-bwlr] address data [count]"   },
    { 1, "peek",        ids_peek,	  "[-bdhloqw] address [count]"     },
    { 1, "ram-addr-hi", ram_addr_hi_main,  "address size"	           },
    { 1, "ram-addr-lo", ram_addr_lo_main,  "address size"		   },
    { 1, "ram-alias",   ram_alias_main,    "address size"		   },
    { 1, "ram-cache",   ram_cache_main,    "address size"		   },
    { 1, "ram-refresh", ram_refresh_main,  "address size"		   },
    { 1, "ram-readback",ram_readback_main, "address size"		   },
    { 1, "reg-rw",      reg_rw_main, "access addr1 [mask1] [reg2] [mask2]" },
    { 1, "reg-ro",      reg_ro_main, "access addr1 [mask1] [reg2] [mask2]" },
    { 1, "reg-wo",      reg_wo_main, "access addr1 [mask1] [reg2] [mask2]" },
    { 0, "show",        menu_show_main,    "update menu hw register items" },
    { 0, 0 }
};

void cmd_tbl_init(void)
{
	cmd_tbl[0].command  = help_main;
	cmd_tbl[1].command  = ids_poke;
	cmd_tbl[2].command  = ids_peek;
	cmd_tbl[3].command  = ram_addr_hi_main;
	cmd_tbl[4].command  = ram_addr_lo_main;
	cmd_tbl[5].command  = ram_alias_main;
	cmd_tbl[6].command  = ram_cache_main;
	cmd_tbl[7].command  = ram_refresh_main;
	cmd_tbl[8].command  = ram_readback_main;
	cmd_tbl[9].command  = reg_rw_main;
	cmd_tbl[10].command = reg_ro_main;
	cmd_tbl[11].command = reg_wo_main;
	cmd_tbl[12].command = menu_show_main;
	cmd_tbl[13].name    = (char *)0;		/* cmd_tbl end */
}

/*
 * Command Line Interface (CLI)
 *
 * This section is where all the cli code lives.
 */
int cli(int c)
{
	char *argv[16];
	int argc;
	char key_cache[256];
	int key_count;
	int status;
	cmd_t *cmd = 0;

	for (key_count = 0; key_count < 255; c = getchar())
	{
		if (c == '\b')
		{
			if (key_count > 0)
			{
				printf("\b \b");
				/*
				 * When the key count returns to zero
				 * then return back to the ui() routine.
				 */
				if (!--key_count)
					return(c);
			}
		}
		else if (c == '\n' || c == '\r')
		{
			putchar('\n');
			break;
		}
		else
		{
			putchar(c);
			key_cache[key_count++] = (char)c;
		}
	}
	key_cache[key_count] = '\0';

	/*
	 * This is where makeargs() use to be.
	 *
	 * argc = makeargs(key_cache, argv);
	 */
	{
		int n;

		for (argc = n = 0; n < key_count; ++argc)
		{
			while (n < key_count && isspace(key_cache[n]))
				++n;
			argv[argc] = &key_cache[n];
			while (n < key_count && !isspace(key_cache[n]))
				++n;
			key_cache[n++] = '\0';
		}
	}

	if (!argc)
		return(-1);

	/*
	 * BUG!!!  Add find device here!!!
	 */

	/*
	 * Look up the command.
	 */
	if (!(cmd = findcmd(argv[0])))
	{
		printf("Command %s not found\n", argv[0]);
		(void)exec_pause();			/* BUG!!! */
		return(-1);
	}

	/*
	 * Jump to the command and pass the argument vector.
	 */
	if ((status = (*cmd->command)(argc, argv)) == -1)
		printf("[ **** FAILED **** ] ");
	else
		printf("[ **** PASSED **** ] ");

	if (cmd->type)
	{
		if (exec_pause() == -1)
			printf("\n");
	}

	return(status);
}

cmd_t *findcmd(char *name)
{
	int n;

	for (n = 0; cmd_tbl[n].name; ++n)
	{
		if (!strcmp(name, cmd_tbl[n].name))
			return(&cmd_tbl[n]);
	}
	return(0);
}

static int help_main(int argc, char **argv)
{
	int n;

	printf("\n");
	for (n = 1; cmd_tbl[n].name; ++n)
	{
		printf("\t%s", cmd_tbl[n].name);
		if (cmd_tbl[n].usage)
		{
			if (strlen(cmd_tbl[n].name) < 8)
				printf("\t");
			printf("\t-- %s", cmd_tbl[n].usage);
		}
		printf("\n");
	}
	return(0);
}
