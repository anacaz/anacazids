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
 * This is the menu driver.  All the magic happens here.
 *
 * Copyright (c)2006-2009 Anacaz Networks, Inc., ALL RIGHTS RESERVED
 *
 * rfa - 060718-19-20-0804-15-16-17-18-0926-27-28-29-1025-090514-22-0601-02-03
 * rfa - 090623-24-0701-02-03
 */
#include <stdint.h>
#ifdef LINUX
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#else /* LINUX */
#include <common.h>
#include <command.h>
#include <linux/ctype.h>
#include <linux/string.h>
#include <linux/time.h>
#include <idsio.h>
#endif /* LINUX */
#include <idsfs.h>
#include <menu.h>
#include <test_api.h>

#define FORWARD	/**/

FORWARD static void menu_init(void);
static menu_t *menu_alloc(void);
FORWARD void config_debug_appl(menu_t *p);
FORWARD void config_regress_appl(menu_t *p);
FORWARD void config_fpga_appl(menu_t *p);
FORWARD void menu_list_appl(menu_t *p);
FORWARD void menu_tls_builder(menu_t *top_level, uint8_t *tlsbase);
FORWARD static void menu_insert_run(menu_t *, list_hdr_t *, test_hdr_t *);
FORWARD void menu_add(menu_t *p, item_t *item, menu_t *menu);

#ifdef COMMENT
FORWARD char *get_listname_appl(menu_t *p);	/* Return list name */
FORWARD char *get_testname_appl(menu_t *p);	/* Return test name */
#endif /* COMMENT */

/*
 * DYNAMIC MENU BUILDER
 *
 * This section is where all the builder code lives.
 */
#define MENU_LIMIT	512
static menu_t *menu_pool = 0;
static int menu_index = 0;

static void menu_init(void)
{
	menu_pool = (menu_t *)GET_RAM(MENUPOOL);
	menu_index = 0;
	list_menu.itemcount = 0;
	printf("menu_init: menu_pool=0x%X\n", menu_pool);
}

static menu_t *menu_alloc(void)
{
	if (menu_index >= MENU_LIMIT)
	{
		printf("menu_alloc: PANIC!!! menu_pool[%d] is exhausted!\n",
			menu_index);
		(void)getchar();
		return(0);
	}
	return(&menu_pool[menu_index++]);
}

void menu_list_appl(menu_t *p)
{
	char save_mode[64];
	uint8_t *tlsbase;

	get_ids_mode(save_mode);
	if (!strcmp(save_mode, "debug"))
	{
		/*
		 * Use the static compiled in tests to
		 * build the menus from.
		 */
		menu_input(&debug_list_menu);
		return;
		
	}
	printf(" Searching for TLS headers ...\n"),
	tlsbase = (uint8_t *)DIR_TBL(TLSFILES).load_addr;
	menu_tls_builder(&list_menu, tlsbase);
	if (!strcmp("autorun", save_mode))
		(void)item_input(&list_menu, 1);
	else
		menu_input(&list_menu);
}

/*
 * Build menus and items starting with the 'top_level' menu pointer from the
 * 'tlsbase' buffer downloaded via tftp.
 *
 * BUG!!!  This finally needs to change DRAMATICALLY!!!
 * Needed in order to handle lists within lists.
 */
void menu_tls_builder(menu_t *top_level, uint8_t *tlsbase)
{
	// extern int list_menu_itemcount;
	list_hdr_t *tlsp;
	test_hdr_t *idsp;
	int menucount;
	static menu_t *ids_svc = 0;

	tlsp = (list_hdr_t *)&tlsbase[0];
	idsp = (test_hdr_t *)&tlsp[1];

	menucount = 0;
	printf("Please wait while menus are being built ... ");
	menu_init();
	menu_insert_run(top_level, tlsp, idsp);

	for (; top_level->itemcount < DIR_TBL(TLSFILES).entries + 1; ++top_level->itemcount)
	{
		int testcount;
		int testindex;
		menu_t *list_level;

		/*
		 * TEST LIST MENU
		 *
		 * This builds the items within the Test List Menu.  Each
		 * item is a named test-list.
		 */
		// ids_svc = (menu_t *)0;	/* Clear the service menu */
		/*
		 * Uniqueify the test name.
		 */
		sprintf(top_level->itemlist[top_level->itemcount].name,
			"%s-%d", tlsp->name, top_level->itemcount);
		testcount = (int)strtol(tlsp->tests, (char **)0, 0);
		top_level->itemlist[top_level->itemcount].type = ITEM_LIST;
		if (!(list_level = menu_alloc()))
		{
			printf("menu_tls_builder: Can't alloc menu!\n");
			(void)getchar();
			return;
		}
		++menucount;
		top_level->itemlist[top_level->itemcount].menu = list_level;
		top_level->itemlist[top_level->itemcount].func = list_input;
		sprintf(list_level->name, "%s", top_level->itemlist[top_level->itemcount].name);
		/*
		 * INDIVIDUAL TEST MENUS
		 *
		 * For each test within a test list there is the Test
		 * Menu and then each item is the data pass, fail, args, etc.
		 * that can be set for the test.
		 */
		idsp = (test_hdr_t *)&tlsp[1];
		menu_insert_run(list_level, tlsp, idsp);
		list_level->itemcount += testcount;
		for (testindex = 1; testindex < list_level->itemcount; ++testindex)
		{
			menu_t *test_level;
			int parmc;
			int access = 0;			/* ITEM_REG */

#if 0
			if (!strcmp(((list_hdr_t *)idsp)->magic, "IDS"))
			{
				printf("FOUND TLS ENCAP INT TLS!!!\n");
				(void)getchar();
				continue;
			}
#endif
			/*
			 * Uniqueify the test name.
			 */
			sprintf(list_level->itemlist[testindex].name,
				"%s-%d.%d", idsp->name, top_level->itemcount,
				testindex);
			list_level->itemlist[testindex].type = ITEM_TEST;
			if (!(test_level = menu_alloc()))
			{
				printf("menu_tls_builder: Can't alloc menu!\n");
				return;
			}
			++menucount;
			list_level->itemlist[testindex].menu = test_level;
			list_level->itemlist[testindex].func = menu_input;
			sprintf(test_level->name, "%s",
				list_level->itemlist[testindex].name);
			menu_insert_run(test_level, tlsp, idsp);

			/*
			 * INDIVIDUAL MENU PARMETERS
			 */
			test_level->itemcount +=
				(int)strtol(idsp->argc, (char **)0, 0);
			for (parmc = 1; parmc < test_level->itemcount; ++parmc)
			{
				item_t *itemp;
				char *namep, *cachep;

				itemp = &test_level->itemlist[parmc];
				sprintf(itemp->name, "%s",
					&idsp->argv[parmc - 1][0]);
				/*
				 * Everything has an equal '=' sign!
				 */
				if (!(namep = strchr(itemp->name, '=')))
					continue;		/* ERROR */
				*namep++ = '\0';

				itemp->type = ITEM_PARM;
				itemp->menu = ids_svc;		/* ld any svc */
				itemp->func = 0;

				/*
				 * This is where we trap special data to
				 * resolve.  The '{' and '}' characters
				 * that identify data to be translated.
				 *	mar={0x500000A0}
				 *	2) mar		0x500000A0:=0xCCBEFF00
				 * rfa - 090602
				 *
				 * BUG!!!  Should search for first digit.
				 */
				if (!(cachep = strchr(namep, '{')))
				{
					sprintf(itemp->data.cache, "%s", namep);
					/*
					 * If this is a menu of register items
					 * then there will be an "access" item
					 * in the second position of the menu
					 * and we need to know the access
					 * width to determine the default mask
					 * specifier if one has not been set
					 * in the call to mktest.
					 */
					if (!strcmp("access", itemp->name))
					{
						if (!strcmp("char", itemp->data.cache))
							access = sizeof(char);
						else if (!strcmp("short", itemp->data.cache))
							access = sizeof(short);
						else if (!strcmp("longlong", itemp->data.cache))
							access = sizeof(long long);
						else
							access = sizeof(long);
					}
					/*
					 * IDS SERVICE
					 *
					 * Check for a service.
					 */
					else if (!strcmp("ids-svc", itemp->name))
					{
						itemp->type = ITEM_SVC;
						test_level->itemlist[0].func =
							menu_update_svc(itemp);
						ids_svc = test_level;
					}
					continue;
				}
				sprintf(itemp->data.reg.addr, "%s", &cachep[1]);
				/*
				 * Check for mask
				 *	mar={0x500000A0:0x0000FFFF}
				 * The mask is stored in the data cache
				 * right behind the register address.
				 *	cache: "0x500000A0:0x0000FFFF\0"
				 *	// cache: "0x500000A0\00x0000FFFF\0"
				 * rfa - 090604
				 */
				if (!(cachep = strchr(itemp->data.cache, '}')))
					continue;		/* ITEM_PARM */
				cachep[0] = '\0';
				if ((cachep = strchr(itemp->data.cache, ':')))
				{
					cachep[0] = '\0';
					sprintf(itemp->data.reg.mask, "%s",
						&cachep[1]);
				}
				else
				{
					switch (access)
					{
					case sizeof(char):
						sprintf(itemp->data.reg.mask, "0xFF");
						break;
					case sizeof(short):
						sprintf(itemp->data.reg.mask, "0xFFFF");
						break;
					case sizeof(long long):
						sprintf(itemp->data.reg.mask, "0xFFFFFFFFFFFFFFFF");
						break;
					default:
						sprintf(itemp->data.reg.mask, "0xFFFFFFFF");
						break;
					}
				}
				sprintf(itemp->data.reg.data, "%s", 0);
				itemp->type = ITEM_REG;
			}
			idsp = (test_hdr_t *)&((uint8_t *)idsp)[(int)strtol(idsp->size, (char **)0, 0)];
		}
		tlsp = (list_hdr_t *)idsp;
	}
	printf("done %d menus built.\n", menucount);
}

/*
 * Insert the RUN menu item at the top of a built up test menu.
 *
 * This function also inserts the (see ../include/idsfs.h) pointer to
 * either the "tlsbase" (for the list of test lists menu), the
 * individual .tls header (for the list of tests menu), or the .ids header
 * (for the individual test menu).
 */
static void menu_insert_run(menu_t *p, list_hdr_t *tls, test_hdr_t *ids)
{
	char save_mode[64];

	sprintf(p->itemlist[0].name, "%s", "RUN");
	p->itemlist[0].type = ITEM_APPL;
	p->itemlist[0].menu = p;
	/*
	 * If BRINGUP mode then go directly to test_go_appl.
	 */
	get_ids_mode(save_mode);
	if (!strcmp("bringup", save_mode))
		p->itemlist[0].func = test_go_appl;
	else
		p->itemlist[0].func = test_execution_appl;
	p->itemlist[0].data.idsfs.tls = tls;
	p->itemlist[0].data.idsfs.ids = ids;
	p->itemcount = 1;
}

/*
 * BUILT-IN BUILDER
 *
 * This function is used by the xxx_init() routines found in the
 * ./built-ins/ids_xxx.c files referenced by the MENUTAB_INIT(xxx)
 * macros found in the menutab.c:menutab_init() routine.
 *
 * Each call to menu_add() resultes in another built-in menu being
 * added to the BUILT-IN TESTS menu (item #1) in the MAIN MENU.
 */
void menu_add(menu_t *p, item_t *item, menu_t *menu)
{
	memcpy(&p->itemlist[p->itemcount], item, sizeof(item_t));
	p->itemlist[p->itemcount].menu = menu;
	p->itemlist[p->itemcount].func = menu_input;
	++p->itemcount;
}

/*
 * APPLICATIONS
 *
 * This section is where all the menu driven applications live.
 */

void menu_erase_appl(menu_t *p)
{
	ids_flash_erase();
}

/*
 * SOON TO BE: Application that shows the lists and menus much like 'ls'.
 */
void test_list_appl(menu_t *p)
{
	printf("\n\n");
}

#ifdef COMMENT
char *get_listname_appl(menu_t *p)
{
	list_hdr_t *tls;

	tls = p->itemlist[0].data.idsfs.tls;
	return(tls->name);
}

char *get_testname_appl(menu_t *p)
{
	test_hdr_t *ids;

	ids = p->itemlist[0].data.idsfs.ids;
	return(ids->name);
	return(0);
}

/*
 * Subordinate routines.
 */

int atoi(char *s)
{
	int n;

	n = 0;
	while (*s && isdigit(*s)) { n *= 10; n += (*s - '0'); ++s; }
	return(n);
}
#endif /* COMMENT */
