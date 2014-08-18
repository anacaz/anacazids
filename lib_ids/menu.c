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
 * rfa - 090623-24-0709
 * rfa - 140815
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

menu_t *this_menu = (menu_t *)0;

#define FORWARD	/**/

FORWARD int menu_show(menu_t *p);
FORWARD int menu_show_reg(menu_t *menu);
FORWARD int menu_update_time(menu_t *menu);
FORWARD int ui(menu_t *p, char *prompt, void (*hotkey)(menu_t *, int));
FORWARD static int ui_getkey(void);
FORWARD void menu_input_hotkey(menu_t *p, int c);
FORWARD void menu_help(menu_t *p);
FORWARD void menu_input(menu_t *p);			/* MENU main */
FORWARD void list_input(menu_t *p);			/* LIST main */
FORWARD menu_t *item_input(menu_t *p, int index);
static void get_parm_input(char *prompt, char *value);
static void get_reg_input(hwreg_t *reg, char *access);
static void get_time_input(char *prompt, idstime_t *time);
static void get_date_input(char *prompt, idstime_t *time);
FORWARD void list_input_hotkey(menu_t *p, int c);	/* LIST processing */
FORWARD void menu_pass(menu_t *p);			/* PASS editor */
FORWARD void menu_fail(menu_t *p);			/* FAIL editor */
FORWARD void download_appl(menu_t *p);

static char ids_mode[64];

int set_ids_mode(const char *string)
{
	int truncated;

	truncated = 0;
	if (strlen(string) > 63)
	{
		((volatile char *)string)[63] = '\0';
		truncated = -1;
	}
	sprintf(ids_mode, "%s", string);
	return(truncated);
}

void get_ids_mode(char *buffer)
{
	strcpy(buffer, (const char *)ids_mode);
}

/*
 * VT52 termcap
 *
 * xterm-vt52|XFree86 xterm emulating dec vt52:\
 *	:co#80:it#8:li#24:\
 *	:ac=``aaffggjjkkllmmnnooppqqrrssttuuvvwwxxyyzz{{||}}~~:\
 *	:ae=\EG:as=\EF:bl=^G:cd=\EJ:ce=\EK:cl=\EH\EJ:cm=\EY%+ %+ :\
 *	:cr=^M:do=\EB:ho=\EH:kb=^H:kd=\EB:kl=\ED:kr=\EC:ku=\EA:\
 *	:le=\ED:nd=\EC:nw=^M^J:sf=^J:sr=\EI:ta=^I:up=\EA:
 */
#define CLREOS	"\033[J"		/* cd=\EJ: clear to end of screen */
#define CLREOL	"\033[K"		/* ce=\EK: clear to end of line */
#define CLRSCR	"\033[H\033[J"		/* cl=\EH\EJ: clear screen cur home */
#define CURPOS	"\033Y%%%+ d%%%+ d"	/* cm=\EY%+ %+ : movcur row,col */
#define DOWN	"\033[B"		/* do=\EB: cursor down 1 line */
#define HOME	"\033[H"		/* ho=\EH: home cursor */
#define LEFT	"\033[D"		/* kl=\ED: cursor left 1 column */
#define RIGHT	"\033[C"		/* kr=\EC: cursor right 1 column */
#define UP	"\033[A"		/* ku=\EA: cursor up 1 line */

#define DISPLAY(X)		printf(X)
#define CURSOR(R,C)		printf(CURPOS, (R), (C))

/*
 * USER INTERFACE:
 *
 *	This section is where all the UI code lives.
 */
int menu_show(menu_t *p)
{
	static char notify1[256] = { "**      ANACAZ IDS                                                           **" };
	static char notify2[256] = { "**                                                                           **" };
	static char banner[256]  = { "*******************************************************************************" };
	static char border[256]  = { "**                                                                           **" };
	static char guide[256]   = { "** ESC/u = <back>   ^N = HELP   ^L = TLS DNLD CTL   ^T = RAM CARVE           **" };
	static char idstag[256]  = { "** IDS(tm) Integrated Diagnostics Subsystem                                  **" };
	static char title[256];

	register int count;

	DISPLAY(HOME);
	DISPLAY(CLREOS);
	// printf("\033[H\033[J");
	printf("\n%s\n", banner);
	printf("%s\n", border);
	printf("%s\n", idstag);

	printf("%s\n", border);
	printf("%s\n", notify1);
	printf("%s\n", notify2);
	printf("%s\n", border);

	sprintf(title, "%s", border);
	sprintf(title, "** [ %s MENU ] [ %s MODE ]", p->name, ids_mode);
	title[strlen(title)] = ' ';
	printf("%s\n", title);

	printf("%s\n", border);
	printf("%s\n", guide);
	printf("%s\n", banner);

	for (count = 0; count < p->itemcount; ++count)
	{
		static char spad[] = "                                ";
		item_t *itemp;

		itemp = &p->itemlist[count];

		if (count < 9)
			printf("\t %d) ", count + 1);
		else
			printf("\t%d) ", count + 1);

		printf("%s%s", itemp->name, &spad[strlen(itemp->name)]);

		if (itemp->type == ITEM_LOG)
		{
			int size;

			/*
			 * Elapsed TIME.
			 */
			printf("%s", itemp->data.cache);
			/*
			 * Log SIZE.
			 */
			if (count == p->itemcount - 1)
				size = log_dir.load_addr - (uint32_t)itemp->menu;
			else
				size = (uint32_t)itemp[1].menu - (uint32_t)itemp->menu;
			printf("\t%8d", size);
		}

		/*
		 * Print PASS / FAIL header for a list of tests.
		 */
		if (!count && p->itemcount > 1)
		{
			/*
			 * Print the PASS and FAIL title headers.
			 */
			if (itemp->type == ITEM_APPL &&
			    itemp[1].type == ITEM_TEST &&
			    !strcmp(itemp->name, "RUN"))
			{
				printf("  PASS\t  FAIL\tCYCLES\tFAULTS");
			}
			else if (itemp->type == ITEM_APPL &&
			    itemp[1].type == ITEM_LIST &&
			    !strcmp(itemp->name, "RUN"))
			{
				printf(" TESTS\tPASSED\tFAULTS\t RATIO");
			}
		}

		/*
		 * WARNING WARNING WARNING WARNING WARNING WARNING WARNING
		 *
		 * DO NOT ADD AN 'else' IN FRONT OF THIS 'if'!!!
		 */
		if (itemp->type == ITEM_TEST)
		{
			test_hdr_t *idsp;
			int pass_cntr, fail_cntr;

			idsp = itemp->menu->itemlist[0].data.idsfs.ids;

			printf("%6s\t%6s", idsp->pass, idsp->fail);
			/*
			 * Display the cycle counter
			 */
			fail_cntr = (int)strtol(idsp->cntr2, (char **)0, 0);
			pass_cntr = (int)strtol(idsp->cntr1, (char **)0, 0);
			printf("\t% 6d", pass_cntr + fail_cntr);
			printf("\t% 6d", fail_cntr);
		}
		else if (itemp->type == ITEM_LIST)
		{
			int item, ratio;
			int passed = 0, faults = 0;

			for (item = 1; item < itemp->menu->itemcount; ++item)
			{
				test_hdr_t *idsp;
				int fail_cntr;

				idsp = itemp->menu->itemlist[item].menu->itemlist[0].data.idsfs.ids;
				fail_cntr = (int)strtol(idsp->cntr2,
					(char **)0, 0);
				faults += fail_cntr;
				passed += (int)strtol(idsp->cntr1,
					(char **)0, 0);
			}
			ratio = (passed * 100) / (passed + faults);
			printf("% 6d\t% 6d\t% 6d\t% 5d%%",
				itemp->menu->itemcount - 1,
				passed, faults, ratio);
		}
		else if (itemp->type == ITEM_PARM || itemp->type == ITEM_SVC)
		{
			printf("    =\t%s", itemp->data.cache);
			if (p == &ram_carve_menu)
			{
				printf(" % 6d:%d", dir_tbl[count].load_addr - 
					dir_tbl[count].base_addr,
					dir_tbl[count].entries);
			}
		}
		else if (itemp->type == ITEM_CTL)
			printf("    =\t%s", itemp->data.cache);
		else if (itemp->type == ITEM_REG)
		{
			printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b%s:%s =\t%s",
				itemp->data.reg.addr,
				itemp->data.reg.mask,
				itemp->data.reg.data);
		}
		else if (itemp->type == ITEM_CNTR)
			printf("\t:\t%s", itemp->data.cache);
		else if (itemp->type == ITEM_TIME)
		{
			(void)menu_update_time(p);

			printf("\t:\t%s:%s:%s",
				itemp->data.time.hour,
				itemp->data.time.minute,
				itemp->data.time.second);
		}
		else if (itemp->type == ITEM_DATE)
		{
			item_t *timep;

			(void)menu_update_time(p);

			if (!(timep = find_item(p, "TIME")))
				timep = itemp;
			printf("\t:\t%s/%s/%s",
				timep->data.time.month,
				timep->data.time.day,
				timep->data.time.year);
		}
		else if (itemp->type == ITEM_NAME)
		{
			if (itemp->data.cache[0])
				printf("    =\t%s", itemp->data.cache);
			else
				printf("    =\t%s", "<Unknown>");
		}
		printf("\n");
	}
	return(count);
}

int menu_update_time(menu_t *menu)
{
	item_t *timep/*, *datep*/;
	int hour, minute, second;
	int day, month, year;

	if (!(timep = find_item(menu, "TIME")))
		return(0);

	timep->data.time.lasttime = timep->data.time.currtime;
	timep->data.time.currtime = gettime();

	second = timep->data.time.currtime % 60;
	minute = (timep->data.time.currtime / 60) % 60;
	hour = ((timep->data.time.currtime / 60) / 60) % 24;

	day = (timep->data.time.currtime / (24 * 60 * 60)) % 365;
	month = 8;
	year = 1970 + (day / 365);

	sprintf(timep->data.time.second, "%02d", second);
	sprintf(timep->data.time.minute, "%02d", minute);
	sprintf(timep->data.time.hour, "%02d", hour);
	sprintf(timep->data.time.day, "%02d", day);
	sprintf(timep->data.time.month, "%02d", month);
	sprintf(timep->data.time.year, "%04d", year);

	return((int)timep->data.time.currtime);
}

int menu_show_reg(menu_t *menu)
{
	item_t *itemp;
	int access;
	int count, regcount;

	if (!(itemp = find_item(menu, "access")))
		return(0);

	if (!strcmp("char", itemp->data.cache))
		access = sizeof(uint8_t);
	else if (!strcmp("short", itemp->data.cache))
		access = sizeof(uint16_t);
	else if (!strcmp("long", itemp->data.cache))
		access = sizeof(uint32_t);
	else if (!strcmp("longlong", itemp->data.cache))
		access = sizeof(uint64_t);
	else
		access = sizeof(uint32_t);

	regcount = 0;
	for (count = 0; count < menu->itemcount; ++count)
	{
		uint32_t raddr;

		itemp = &menu->itemlist[count];
		if (itemp->type != ITEM_REG)
			continue;
		++regcount;
		raddr = strtol(itemp->data.reg.addr, (char **)0, 0);
#ifdef COMMENT
		/*
		 * This is an attempt at figuring out the bit width
		 * for accesses from the number of hex characters in
		 * the data? (mask) specifier.
		 */
		if (!strncmp("0x", itemp->data.reg.data, 2))
			access = strlen(itemp->data.reg.data) - 2;
#endif /* COMMENT */
		switch (access)
		{
		case sizeof(uint8_t):
		{
			uint32_t data, mask;

			mask = (uint32_t)strtol(itemp->data.reg.mask, (char **)0, 0);
			data = *((volatile uint8_t *)raddr) & mask;
			sprintf(itemp->data.reg.data, "0x%02X", data);
			break;
		}
		case sizeof(uint16_t):
		{
			uint16_t data, mask;

			mask = (uint16_t)strtol(itemp->data.reg.mask, (char **)0, 0);
			data = *((volatile uint16_t *)raddr) & mask;
			sprintf(itemp->data.reg.data, "0x%04X", data);
			break;
		}
		case sizeof(uint32_t):
		{
			uint32_t data, mask;

			mask = (uint32_t)strtol(itemp->data.reg.mask, (char **)0, 0);
			data = *((volatile uint32_t *)raddr) & mask;
			sprintf(itemp->data.reg.data, "0x%08X", data);
			break;
		}
		case sizeof(uint64_t):
		{
			uint64_t data, mask;

			mask = (uint64_t)strtol(itemp->data.reg.mask, (char **)0, 0);
			data = *((volatile uint64_t *)raddr) & mask;
			sprintf(itemp->data.reg.data, "0x%016X", data);
			break;
		}
		default:
			break;
		}
	}
	return(regcount);
}

/*
 * MAIN UI FUNCTION
 *
 * Receive user key strokes until there is a <cr> or <nl> or 4 keys have
 * been received and convert the numeric input into an index to be used
 * to index the current menu itemlist to select the item within the
 * current menu.
 */
int ui(menu_t *p, char *prompt, void (*hotkey)(menu_t *, int))
{
	char key_strokes[16];
	int key_count;
	int item_count = 0;

	int index;			/* Menu item index */
	int c;

	if (!p)
	{
		printf("ui: ERROR: p=0x%X\n", p);
		return(-1);
	}

	for (key_count = 0;;)
	{
		if (!key_count)
		{
			item_count = menu_show(p);
			printf("%s", prompt);
		}
		c = getchar();		/* BUG !!! replace with ui_getkey() */
		switch (c)
		{
		case '\b':
			if (key_count > 0)
			{
				printf("\b \b");
				--key_count;	/* Back space */
			}
			break;
		case '\022':				/* ^R */
#ifdef COMMENT
			if ((index = ui(p, "ids> ", menu_input_hotkey)) == -1)
#endif /* COMMENT */
			break;
		case '\014':				/* ^L */
			/*
			 * Allow access from file_mgr_menu only.
			 */
			if (p == &file_mgr_menu)
				menu_input(tftp_menu.itemlist[1].menu);
			break;
		case 'u':
		case 'U':
		case '\033':
			return(-1);
		case '0':
			if (!key_count)
				break;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			putchar(c);
			key_strokes[key_count++] = (char)c;
			break;
		case '\n':
		case '\r':
			key_strokes[key_count] = '\0';
			/*
			 * Select the menu item.  Check to see that
			 * the menu item is between 1 and item_count.
			 */
			index = (int)strtol(key_strokes, (char **)0, 0);
			if (1 <= index && index <= item_count)
				return(index);
			key_count = 0;
			break;
		default:
			if (!key_count)
			{
				if (' ' <= c && c <= '~')
					(void)cli(c);
				else
				{
					/*
					 * Pass to hot-key processing and
					 * then reset key_count and start
					 * over.
					 */
					if (hotkey)
						(*hotkey)(p, c);
				}
			}
			break;
		}
	}
	return(-1);
}

/*
 * BUG!!! In progress.  To allow "1 1 1<cr>" type input.
 *
 * Intercept key processing by calling this function instead of getchar()
 * directly.  Allow buffered input to be returned until circular buffer
 * is exhausted.  
 */
static int ui_getkey(void)
{
	static char key_buf[256];
	static int key_in = 0;
	static int key_out = 0;

	int c;

	if (key_out != key_in)
	{
		c = key_buf[key_out];
		if (++key_out >= 256)
			key_out = 0;
		return(c);
	}

	for (;;)
	{
		if (key_in >= 256)
			key_in = 0;
		c = getchar();
		switch(c)
		{
		case '\033':
			/*
			 * Flush the input and return the ESC key.
			 */
			key_out = key_in = 0;
			return(c);
		case '\n':
		case '\r':
			/*
			 * Add the line terminator to the buffer
			 * and return the first key to the caller.
			 */
			key_buf[key_in++] = c;
			return(ui_getkey());
		default:
			key_buf[key_in++] = c;
			break;
		}
	}
}

/*
 * MENU HELP
 *
 * Standard menu input control "hot" key processing function.  Currenly
 * only processes requests for help by allowing user to type ^N and then
 * the menu number.  ESC returns user to normal operation.
 */
void menu_input_hotkey(menu_t *p, int c)
{
	switch (c)
	{
	case '\016':				/* ^N */
		menu_help(p);
		break;
	case '\024':				/* ^T */
		if (p != &ram_carve_menu)	/* Already here? */
			menu_input(&ram_carve_menu);
		break;
	default:
		break;
	}
}

/*
 * The menu_help() function will print the help message for
 * each menu item instead of executing the actual menu item.
 */
void menu_help(menu_t *p)
{
	char save_mode[64];
	int index;

	/*
	 * If not in DEBUG mode then return.
	 */
	get_ids_mode(save_mode);
	if (strcmp(save_mode, "debug"))
		return;
	for (;;)
	{
		char *help;

		if ((index = ui(p, "help> ", 0)) == -1)
			return;
		help = p->itemlist[index - 1].help;
		if (help[0] == '\0')
			printf("No help is available\n");
		else 
			printf(" %s", p->itemlist[index - 1].help);
		(void)getchar();
	}
}

/*
 * This is the main menu processing routine.  The menu referrence is
 * passed by the caller and the menu input is translated in to action.
 */
void menu_input(menu_t *p)
{
	int index;

	for (;;)
	{
		this_menu = p;
		if ((index = ui(p, "ids> ", menu_input_hotkey)) == -1)
			return;
		p = item_input(p, index);
	}
}

void list_input(menu_t *p)
{
	int index;

	for (;;)
	{
		this_menu = p;
		if ((index = ui(p, "ids> ", list_input_hotkey)) == -1)
			return;
		p = item_input(p, index);
	}
}

/*
 * Convert the named service into the actual function that performs the
 * service.  If the item is not of type ITEM_SVC or if the named service
 * is not valid then return null.
 */
void (*menu_update_svc(item_t *itemp))(menu_t *)
{
	if (itemp->type == ITEM_SVC)
	{
		/*
		 * Add services here.
		 */
		if (!strcmp("tftp", itemp->data.cache))		/* TFTP */
			itemp->func = fileio_tftp_svc;
	}
	return(itemp->func);
}

/*
 * This is the common item input routine that serves both menu processing
 * and list processing.  This function was created to put the processing in
 * one place only.  The menu_input() and list_input() functions above call
 * the generic_input() routine.
 */
menu_t *item_input(menu_t *p, int index)
{
	item_t *itemp;

	if (!index)
		return(0);
	/*
	 * Reference the item using the returned index.
	 */
	itemp = &p->itemlist[index - 1];

	/*
	 * This is where we go!
	 */
	switch (itemp->type)
	{
	case ITEM_LOG:
	case ITEM_LIST:
	case ITEM_MENU:
	case ITEM_TEST:
		if (itemp->func)
			(*itemp->func)(itemp->menu);
		break;
	case ITEM_APPL:
		if (itemp->func)
			(*itemp->func)(p);
		break;
	case ITEM_CTL:
		get_parm_input(itemp->name, itemp->data.cache);
		if (itemp->func)
			(*itemp->func)(p);
		p = itemp->menu;			/* MAGIC !!! */
		break;
	case ITEM_SVC:
		get_parm_input(itemp->name, itemp->data.cache);
		p->itemlist[0].func = menu_update_svc(itemp);
		break;
	case ITEM_PARM:
		get_parm_input(itemp->name, itemp->data.cache);
		break;
	case ITEM_REG:
	{
		item_t *access;

		if (!(access = find_item(p, "access")))
			get_reg_input(&itemp->data.reg, "long");
		else
			get_reg_input(&itemp->data.reg, access->data.cache);
		break;
	}
	case ITEM_EXIT:
		printf("Goodbye!\n");
	case ITEM_CNTR:
		{
			if (exec_ask(" Do you want to reset the %s counter?"))
			{
				char *cp;
				int special = 0;

				for (cp = itemp->data.cache; *cp; ++cp)
				{
					if (isxdigit(*cp))
						*cp = '0';
					else
						++special;
				}
				if (!special)
					sprintf(itemp->data.cache, "0");
			}
			break;
		}
	case ITEM_TIME:
		get_time_input(itemp->name, &itemp->data.time);
		break;
	case ITEM_DATE:
	{
		item_t *timep;

		if (!(timep = find_item(p, "TIME")))
			break;
		get_date_input(itemp->name, &timep->data.time);
		break;
	}
	default:
		break;
	}
	return(p);
}

/*
 * ITEM_PARM
 *
 * Prompt user for input and place the keyed input into the value buffer
 * if the user does not abort the update.
 */
static void get_parm_input(char *prompt, char *value)
{
	static char input[256];
	int n, c, vlen;

	printf(" %s = ", prompt);
	printf("%s", value);

	vlen = strlen(value);
	for (n = vlen; n; --n)
		putchar('\b');
	/*
	 * Potential BUG here!!!  Some of the value pointers point
	 * to arrays that are only 64 bytes in size.
	 */
	for (n = 0; n < 127; ++n)
	{
		c = getchar();
		/*
		 * Abort without changing the input buffer.
		 */
		if (c == '\033')
			return;
		/*
		 * Done, accept input.
		 */
		if (c == '\r' || c == '\n')
			break;
		if (c < ' ')
		{
			if (c == '\b' && n > 0)
			{
				if (--n < vlen) /* Back space */
					printf("\b%c\b", value[n]);
				else
					printf("\b \b");
			}
			--n;			/* Ignore character */
		}
		else
		{
			input[n] = (char)c;
			putchar(c);
		}
	}
	input[n] = '\0';
	sprintf(value, "%s", input);
}

/*
 * ITEM_REG
 *
 * Prompt user for input and place the keyed input into the value buffer
 * if the user does not abort the update.
 */
static void get_reg_input(hwreg_t *reg, char *access)
{
	uint32_t raddr, data;
	int n, c;
	int size;

	if (!strcmp("char", access))
		size = sizeof(uint8_t);
	else if (!strcmp("short", access))
		size = sizeof(uint16_t);
	else if (!strcmp("longlong", access))
		size = sizeof(uint64_t);
	else
		size = sizeof(uint32_t);

	raddr = strtol(reg->addr, (char **)0, 0);

	switch (size)
	{
	case sizeof(uint8_t):
		printf("\t%s=%s\b\b\b\b", reg->addr, reg->data);
		break;
	case sizeof(uint16_t):
		printf("\t%s=%s\b\b\b\b\b\b", reg->addr, reg->data);
		break;
	case sizeof(uint32_t):
		printf("\t%s=%s\b\b\b\b\b\b\b\b\b\b", reg->addr, reg->data);
		break;
	case sizeof(uint64_t):
		printf("\t%s=%s\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b",
			reg->addr, reg->data);
		break;
	default:
		return;
	}

	for (n = 0; n < 31; ++n)
	{
		c = getchar();
		/*
		 * Abort without changing the input buffer.
		 */
		if (c == '\033')
			return;
		/*
		 * Done, accept input.
		 */
		if (c == '\r' || c == '\n')
			break;
		if (c < ' ')
		{
			if (c == '\b' && n > 0)
			{
				printf("\b \b");
				--n;		/* Back space */
			}
			--n;			/* Ignore character */
		}
		else
		{
			reg->data[n] = (char)c;
			putchar(c);
		}
	}
	reg->data[n] = '\0';

	data = strtol(reg->data, (char **)0, 0);
	switch (size)
	{
	case sizeof(char):
		*(volatile uint8_t *)raddr = (uint8_t)data;
		sprintf(reg->data, "0x%02X", (uint8_t)data);
		break;
	case sizeof(short):
		*(volatile uint16_t *)raddr = (uint16_t)data;
		sprintf(reg->data, "0x%04X", (uint16_t)data);
		break;
	case sizeof(long):
		*(volatile uint32_t *)raddr = data;
		sprintf(reg->data, "0x%08X", (uint32_t)data);
		break;
	case sizeof(long long):
		*(volatile uint64_t *)raddr = (uint64_t)data; /* BUG !!! */
		sprintf(reg->data, "0x%016X", (uint64_t)data);
		break;
	default:
		break;
	}
}

/*
 * ITEM_TIME
 *
 * Prompt user for input and place the keyed input into the value buffer
 * if the user does not abort the update.
 */
static void get_time_input(char *prompt, idstime_t *time)
{
	int element;
	int n, c;
	char input[8];
	time_t hour = 0, min = 0, sec = 0;

	printf(" %s = ", prompt);
	printf("%s:%s:%s\b\b\b\b\b\b\b\b",
		time->hour, time->minute, time->second);
	for (element = 0; element < 3; ++element)
	{
		for (n = 0; n < 2; ++n)
		{
			c = getchar();
			/*
			 * Abort without changing the input buffer.
			 */
			if (c == '\033')
				return;
			/*
			 * Done, accept input.
			 */
			if (c == '\r' || c == '\n')
				break;
			if (c < ' ')
			{
				if (c == '\b' && n > 0)
				{
					printf("\b \b");
					--n;		/* Back space */
				}
				--n;			/* Ignore character */
			}
			else if (!isdigit(c))
				break;
			else
			{
				input[n] = (char)c;
				putchar(c);
			}
		}
		input[n] = '\0';
		if (element == 0)
		{
			hour = strtol(input, (char **)0, 0);
			while (n--)
				printf("\b");
			printf("%02d:", hour);
		}
		else if (element == 1)
		{
			min = strtol(input, (char **)0, 0);
			while (n--)
				printf("\b");
			printf("%02d:", min);
		}
		else
		{
			sec = strtol(input, (char **)0, 0);
			while (n--)
				printf("\b");
			printf("%02d", sec);
		}

	}
	time->currtime += (hour % 24) * 60 * 60;
	time->currtime += (min % 60) * 60;
	time->currtime += (sec % 60);

	settime(time->currtime);

	sprintf(time->hour, "%02d", hour);
	sprintf(time->minute, "%02d", min);
	sprintf(time->second, "%02d", sec);
}

/*
 * ITEM_DATE
 *
 * Prompt user for input and place the keyed input into the value buffer
 * if the user does not abort the update.
 */
static void get_date_input(char *prompt, idstime_t *time)
{
	int element;
	int n, c;
	char input[8];
	// static int months[] =
	//	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	time_t year = 0, month = 0, day = 0;
	// time_t days;

return;
	printf(" %s = ", prompt);
	printf("%s/%s/%s\b\b\b\b\b\b\b\b\b\b",
		time->month, time->day, time->year);
	for (element = 0; element < 3; ++element)
	{
		for (n = 0; n < (element == 2 ? 4 : 2); ++n)
		{
			c = getchar();
			/*
			 * Abort without changing the input buffer.
			 */
			if (c == '\033')
				return;
			/*
			 * Done, accept input.
			 */
			if (c == '\r' || c == '\n')
				break;
			if (c < ' ')
			{
				if (c == '\b' && n > 0)
				{
					printf("\b \b");
					--n;		/* Back space */
				}
				--n;			/* Ignore character */
			}
			else if (!isdigit(c))
				break;
			else
			{
				input[n] = (char)c;
				putchar(c);
			}
		}
		input[n] = '\0';
		if (element == 0)
		{
			month = strtol(input, (char **)0, 0);
			while (n--)
				printf("\b");
			printf("%02d/", month);
		}
		else if (element == 1)
		{
			day = strtol(input, (char **)0, 0);
			while (n--)
				printf("\b");
			printf("%02d/", day);
		}
		else
		{
			year = strtol(input, (char **)0, 0);
			while (n--)
				printf("\b");
			printf("%04d", year);
		}

	}
	sprintf(time->day, "%02d", day);
	sprintf(time->month, "%02d", month);
	sprintf(time->year, "%04d", year);

	time->currtime = ((year + 2000) - 1970) * 365;
	settime(time->currtime);
}

/*
 * LIST PROCESSING
 *
 * This routine intercepts the user selected control keys and invokes the
 * pass and fail editors.  These editors operate in a menu/cli mode to 
 * change the pass transfer or fail policy control vectors that appear in
 * a list menu.
 */
void list_input_hotkey(menu_t *p, int c)
{
	switch (c)
	{
	case '\016':				/* ^N */
		menu_help(p);
		break;
	case '\006':				/* ^F */
		menu_fail(p);
		break;
	case '\020':				/* ^P */
		menu_pass(p);
		break;
	case '\024':				/* ^T */
		if (p != &ram_carve_menu)	/* Already here? */
			menu_input(&ram_carve_menu);
		break;
	default:
		break;
	}
}

/*
 * This is the list pass vector editor.
 */
void menu_pass(menu_t *p)
{
	int index;
	item_t *itemp;

	for (;;)
	{
		if ((index = ui(p, "pass> ", 0)) == -1)
			return;
		if (index == 1)
			continue;
		itemp = &p->itemlist[index - 1];
		itemp = &itemp->menu->itemlist[0];
		get_parm_input("PASS", itemp->data.idsfs.ids->pass);
	}
}

void menu_fail(menu_t *p)
{
	int index;
	item_t *itemp;

	for (;;)
	{
		if ((index = ui(p, "fail> ", 0)) == -1)
			return;
		if (index == 1)
			continue;
		itemp = &p->itemlist[index - 1];
		itemp = &itemp->menu->itemlist[0];
		get_parm_input("FAIL", itemp->data.idsfs.ids->fail);
	}
}

/*
 * APPLICATIONS
 *
 * This section is where all the menu driven applications live.
 *
 * This application matches the parm input to find the new menu to 
 * change control to and updates the ITEM_CTL for all menus under it's
 * control to the new method of choice.  This is really twisted.
 */
void download_appl(menu_t *p)
{
	if (!strcmp("tftp", p->itemlist[1].data.cache))
		p->itemlist[1].menu = &tftp_menu;
	else if (!strcmp("xmodem", p->itemlist[1].data.cache))
		p->itemlist[1].menu = &xmodem_menu;
	else if (!strcmp("ymodem", p->itemlist[1].data.cache))
		p->itemlist[1].menu = &xmodem_menu;
	else if (!strcmp("zmodem", p->itemlist[1].data.cache))
		p->itemlist[1].menu = &xmodem_menu;
#ifdef COMMENT
	else if (!strcmp("kermit", p->itemlist[1].data.cache))
		p->itemlist[1].menu = &kermit_menu;
#endif /* COMMENT */
	tftp_menu.itemlist[1].menu = p->itemlist[1].menu;
	strcpy(tftp_menu.itemlist[1].data.cache, p->itemlist[1].data.cache);
	xmodem_menu.itemlist[1].menu = p->itemlist[1].menu;
	strcpy(xmodem_menu.itemlist[1].data.cache, p->itemlist[1].data.cache);
}

/*
 * This application is called with a pointer to the "file_mgr_menu".  Stashing
 * this menu pointer in the communications' unused application menu pointer
 * is a way to relieve the fileio_tftp routine from using a fixed menu
 * reference.
 */
void download_run_appl(menu_t *p)
{
	menu_t *comm;

	comm = tftp_menu.itemlist[1].menu;
	/*
	 * Pass "file_mgr_menu" to tftp service routine.
	 */
	comm->itemlist[0].menu = p;
	menu_input(comm);
}

/*
 * Need to creat an autorun mode that does not wait for operator
 * input when downloading and executing tls lists.
 *
 *	if (itemp->func)
 *		(*itemp->func)(p);
 */
void regress_run_appl(menu_t *p)
{
	char save_mode[64];
	menu_t *comm;

	get_ids_mode(save_mode);
	set_ids_mode("autorun");

	comm = tftp_menu.itemlist[1].menu;
	/*
	 * Pass "file_mgr_menu" to tftp service routine.
	 */
	comm->itemlist[0].menu = &file_mgr_a6ddr_menu;
	(void)item_input(comm, 1);
	menu_list_appl(p);

	comm->itemlist[0].menu = &file_mgr_fpga_menu;
	(void)item_input(comm, 1);
	menu_list_appl(p);

	comm->itemlist[0].menu = &file_mgr_chan_menu;
	(void)item_input(comm, 1);
	menu_list_appl(p);

	set_ids_mode(save_mode);
	(void)exec_pause();
}

void regress_uqam_appl(menu_t *p)
{
	char save_mode[64];
	menu_t *comm;

	get_ids_mode(save_mode);
	set_ids_mode("autorun");

	set_ids_mode(save_mode);
	(void)exec_pause();
}

/*
 * Find the named item within the specified menu.
 */
item_t *find_item(menu_t *p, const char *s)
{
	int count;

	for (count = 0; count < p->itemcount; ++count)
	{
		if (!strcmp(p->itemlist[count].name, s))
			return(&p->itemlist[count]);
	}
	return(0);
}

/*
 * This is the application that will force the visible refresh of the
 * ITEM_REG values within the current menu referenced by this_menu.
 */
int menu_show_main(int argc, char **argv)
{
	if (!this_menu)
	{
		printf("Uninitialized menu pointer!\n");
		return(-1);
	}
	return(menu_show_reg(this_menu));
}

