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
 * This is the test execution driver.  All test exection, counter manipulation,
 * statistics collection, and fail policy invocation happens here.
 *
 * Copyright (c)2006-2009 Anacaz Networks, Inc., ALL RIGHTS RESERVED
 *
 * rfa - 060718-19-20-0804-15-16-17-18-0926-27-28-29-1025-090514-22-0601-02-03
 * rfa - 090623-23-0720-1105-06
 * rfa - 140815
 */
#include <stdint.h>
#include <common.h>
#include <command.h>
#include <linux/ctype.h>
#include <linux/string.h>
#include <linux/time.h>
#include <rtc.h>
#include <idsio.h>
#include <idsfs.h>
#include <menu.h>
#include <test_api.h>

extern int fileio_create(char *name);
extern void fileio_close(int ld);

#define FORWARD	/**/

FORWARD void test_execution_appl(menu_t *);
FORWARD int do_test_cycles_appl(int, int (*)(menu_t *), menu_t *);
FORWARD int do_test_go_appl(menu_t *);
FORWARD int do_test_appl(menu_t *p, char **action);
FORWARD void test_stats_appl(menu_t *);
FORWARD void get_stats_appl(menu_t *);		/* Display tests stats */
FORWARD time_t gettime(void);
FORWARD int is_control_c(void);
FORWARD void sleep(uint32_t delay);

static int load_args(menu_t *, char *argv[]);


static char *passtext[] =
{
	" _____  _____  _____  _____ ",
	"|_____)|_____|(_____ (_____ ",
	"|      |     | _____) _____)",
	0
};

static char *failtext[] =
{
	" _____  _____  __ __ _      ",
	"|____  |_____|   |   |      ",
	"|      |     | __|__ |_____ ",
	0
};

static char *abortext[] =
{
	" _____  _____  _____  _____ _____",
	"|_____||_____)|     ||_____)  |  ",
	"|     ||_____)|_____||     \\  |  ",
	0
};

void exec_text_init()
{
	passtext[0] = " _____  _____  _____  _____ ";
	passtext[1] = "|_____)|_____|(_____ (_____ ";
	passtext[2] = "|      |     | _____) _____)";
	passtext[3] = 0;

	failtext[0] = " _____  _____  __ __ _      ";
	failtext[1] = "|____  |_____|   |   |      ";
	failtext[2] = "|      |     | __|__ |_____ ";
	failtext[3] = 0;

	abortext[0] = " _____  _____  _____  _____ _____";
	abortext[1] = "|_____||_____)|     ||_____)  |  ";
	abortext[2] = "|     ||_____)|_____||     \\  |  ";
	abortext[3] = 0;
}

/*
 * TEST EXECUTION
 *
 * This is how the execution_menu gets loaded and run when in regress
 * mode.
 *
 * Hooked in from menu.c:menu_insert_run()
 * 
 * From the test menu is how we locate the list and test headers in the
 * memory device.
 */
void test_execution_appl(menu_t *p)
{
	char save_mode[64];

	/*
	 * Save the menu reference for use in the test_go_appl()
	 * application below.
	 */
	sprintf(execution_menu.itemlist[EXEC_NAME].data.cache, "%s", p->name);
	execution_menu.itemlist[EXEC_GO].menu = p; /* Point to the test menu */
	get_ids_mode(save_mode);
	if (!strcmp("autorun", save_mode))
		(void)item_input(&execution_menu, 1);
	else
		menu_input(&execution_menu);
}

/*
 * This is where we call the application run program that handles running tests.
 * The called program, "do_test_go_appl", calls itself recursively to walk the
 * menu tree through: lists, single list tests and individual tests.
 *
 * Added reference to execution_menu.RUN item to the go application.
 */
void test_go_appl(menu_t *p)
{
	char save_mode[64];
	char date[80];
	int cycles;
	char **textp;
	time_t stime, etime;
	time_t elapsed;
	int log;
	int status;

	printf("\n");
	log = -1;
	textp = passtext;		/* Pre-load pass message */
	/*
	 * set cycle count from execution menu
	 */
	cycles = 1;
	get_ids_mode(save_mode);
	if (!strcmp(save_mode, "default") || !strcmp(save_mode, "autorun"))
	{
		char *cp;

		/*
		 * Search the menu list for an ITEM_PARM named "cycles"
		 * and return a pointer to the ascii data value.
		 *
		 * BUG!!! (maybe) Currently if the test has an ITEM_PARM
		 * named 'cycles' then the loop control uses the test's
		 * own cycle count.  If there is no 'cycles' ITEM_PARM then
		 * loop control uses the execution menu's cycle count
		 * 'CYCLES'.  Should these be either or?  Multiply the two
		 * together?
		 */
#ifdef COMMENT
		item_t *itemp;

		if ((itemp = find_item(p->itemlist[EXEC_GO].menu, "cycles")))
			cp = itemp->data.cache;
		else
#endif /* COMMENT */
			cp = execution_menu.itemlist[EXEC_CYCLES].data.cache;
		cycles = strtol(cp, (char **)0, 0);
		if (cycles < 1)
			cycles = 1;
		/*
		 * Create log entry for this test run.
		 */
		if ((log = fileio_create(execution_menu.itemlist[EXEC_NAME].data.cache)) == -1)
		{
			printf("Can not create log file %s\n", execution_menu.itemlist[EXEC_NAME].data.cache);
			// (void)exec_pause();
		}
	}

	stime = gettime();		/* Set the start test time */

	/*
	 * This is the hook to run built-ins from execution control.
	 *
	 * BUG!!!  Can't get the pointer to both the execution menu and
	 * the actual menu at the same time.  Need a method to get to the
	 * test routine the same way we dereference the elf loader for the
	 * .tls downloads.
	 */
	if (!strcmp(save_mode, "debug"))
		status = -1;
	else
		status = do_test_cycles_appl(cycles, do_test_go_appl, p);

	if (status == -1)
		textp = failtext;
	else if (status == -100)
		textp = abortext;
	else
		textp = passtext;
	/*
	 * Get the elapsed test time.
	 */
	etime = gettime();
	elapsed = etime - stime;
	(void)ctime_r(&elapsed, date);
	sprintf(execution_menu.itemlist[EXEC_TIME].data.cache,
		"                    ");
	sprintf(execution_menu.itemlist[EXEC_TIME].data.cache, "%s", &date[11]);
	execution_menu.itemlist[EXEC_TIME].data.cache[8] = '\0';

	/*
	 * Display the pass / fail message
	 */
	lprintf("\n");
	while (*textp)
		lprintf("\t%s\n", *textp++);
	lprintf("\n");
	/*
	 * Check if there is an active log file.
	 * If so then close that log file.
	 */
	if (log != -1 && (!strcmp(save_mode, "default") ||
	    !strcmp(save_mode, "autorun")))
	{
		fileio_close(log);
	}

	printf("\n\n");
	if (strcmp("autorun", save_mode))
		(void)exec_pause();
}

static char results[128] = { "[ CYCLE                           TIME                       **** ------ **** ]" };

/*
 * This routine is the test cycle counter routine.  If cycles is specified
 * as zero (0) then cycle forever or until operator interrupt.
 */
int do_test_cycles_appl(int cycles, int (*test_appl)(menu_t *p), menu_t *p)
{
	char *cycleb = &results[8];
	int error, cycle;
	int status;

	sprintf(cycleb, "                         ");
	for (error = 0, cycle = 0; !cycles || cycle < cycles; ++cycle)
	{
		sprintf(cycleb, "%d:%d", cycle + 1, cycles);
		results[strlen(results)] = ' ';
		if ((status = (*test_appl)(p)) == -1)
			error = -1;
		else if (status == -100)
			return(status);
		if (is_control_c())
			return(-100);
	}
	if (error == -1)
		return(error);
	return(0);
}

/*
 * This is where we load and run individual tests.
 *
 * Added reference to execution_menu.RUN item to the go application.
 */
int do_test_go_appl(menu_t *p)
{
	char save_mode[64];
	menu_t *testmenu, *listmenu;
#if 0
/* SCHEDULED FOR DEPRECATION */
	list_hdr_t *tlsp;
	test_hdr_t *idsp;
#endif
	int testindex, lastindex;
	int rv = 0;
	int spin = 0;
	int retry = 0;
	int loop = 0;
	item_t *timep;
	char *cp;

	timep = find_item(&main_menu, "TIME");
	/*
	 * Get the saved menu reference from test_execution_appl
	 * application above.
	 */
	get_ids_mode(save_mode);
	if (!strcmp(save_mode, "bringup"))			/* BRINGUP */
		listmenu = testmenu = p;			/* GO */
	else
		listmenu = testmenu = p->itemlist[EXEC_GO].menu;/* GO */
	/*
	 * This is an overloaded loop for use by test lists BUT ...
	 * individually executed tests fall into this loop ALSO!
	 *
	 * See note below by the: if (... == ITEM_PARM)
	 */
	spin = 0;
	retry = 0;
	loop = 0;
	lastindex = -1;
	for (testindex = 1; 0 <= testindex && testindex < listmenu->itemcount; )
	{
		char *action;

		if (listmenu->itemlist[1].type == ITEM_LIST)
		{
			/*
			 * Process each list in turn from top to bottom.
			 * Call recursively and suppress the pass/fail
			 * text from printing until the last test list
			 * has executed.
			 */
			rv = do_test_go_appl(listmenu->itemlist[testindex].menu);
			if (rv == -1)
				return(-1);
			lastindex = testindex;
			++testindex;
			continue;
		}

		if (listmenu->itemlist[1].type == ITEM_TEST)
			testmenu = listmenu->itemlist[testindex].menu;
#if 0
/* SCHEDULED FOR DEPRECATION */
		tlsp = testmenu->itemlist[0].data.idsfs.tls; /* RUN */
		idsp = testmenu->itemlist[0].data.idsfs.ids; /* RUN */
#endif

		rv = do_test_appl(testmenu, &action);

		/*
		 * WARNING WARNING WARNING WARNING WARNING WARNING WARNING
		 *
		 * This loop is used for test lists as well as individual
		 * tests.  If the next item in the list menu is an ITEM_PARM
		 * then this was an individual test.  We are done.
		 *
		 * DO NOT MOVE THE FOLLOWING 2 LINES OF CODE!
		 *
		 * rfa - 090603
		 */
		if (listmenu->itemlist[1].type == ITEM_PARM)
			break;
		if (is_control_c())
			return(-100);
		/*
		 * If spin control is active then keep doing the previous
		 * operation (test) until the operator intervenes.
		 *
		 * This is here because regardless of which side initiated
		 * the spin (pass | fail) the spin will continue even if
		 * on a subsequent execution the action changes.
		 *
		 * Example: execution 1 results in a fail and the fail policy
		 *          is set to spin.  The condition below results in a
		 *          continuous spin regardless if execution 2 results
		 *          in a pass.
		 *
		 * BUG!!! need spin=until or spin=count or spin=always
		 *
		 * UPDATE: loop=xxx below will be exported to all conditions.
		 */
		if (spin)
			continue;
		if (!strcmp(action, "retry"))
		{
			if (retry < 3)
			{
				++retry;
				continue;
			}
			break;					/* STOP */
		}
		/*
		 * A retry may have occured but everything is ok now.
		 */
		retry = 0;
		/*
		 * Branch control:
		 *
		 * If the next test is specified along the pass or fail
		 * vectors as numeric data then jump to that test next.
		 *
		 * The validation for the testindex value is in the
		 * conditional for this for loop.
		 */
		if (isdigit(action[0]))
		{
			lastindex = testindex;
			testindex = (int)strtol(action, (char **)0, 0) - 1;
		}
		/*
		 * Blind actions.
		 *
		 * ENHANCEMENT:  Depricate loop and add counters to all
		 * actions.  This clearly indicates that only after n times
		 * through this point will the action execute.  Actual
		 * counters for wait and sleep are exempt.
		 */
		else if (!strcmp(action, "next"))
			lastindex = testindex, ++testindex;	/* NEXT */
		else if (!strcmp(action, "skip"))		/* NEXT + 1 */
			lastindex = testindex, ++testindex, ++testindex;
		else if (!strcmp(action, "stop"))
			break;					/* STOP */
		else if (!strcmp(action, "abort"))
			break;					/* STOP */
		else if (!strcmp(action, "halt"))
			break;					/* STOP */
		else if (!strcmp(action, "spin"))
		{
			++spin;
			continue;
		}
#if 0
/* SCHEDULED FOR DEPRECATION */
		else if (!strcmp(action, "wait") || !strcmp(action, "sleep"))
		{
			sleep(1);				/* DELAY */
			lastindex = testindex, ++testindex;	/* NEXT */
		}
#endif
		else if (!strcmp(action, "pause"))
		{
			(void)exec_pause();			/* INPUT */
			rv = 0;
			lastindex = testindex, ++testindex;	/* NEXT */
		}
		else if (!strcmp(action, "ignore"))
		{
			rv = 0;
			lastindex = testindex, ++testindex;	/* NEXT */
		}
		else if (!strcmp(action, "back"))
		{
			testindex = lastindex;			/* GO BACK */
		}
		/*
		 * Actions with counters and conditions.
		 */
		else if (!strncmp(action, "wait", 4))
		{
			uint32_t usec;

			if ((cp = strchr(action, '=')))
				usec = strtol(++cp, (char **)0, 0);
			else
				usec = 1000;
			usec *= 1000;
			udelay(usec);				/* DELAY */
			lastindex = testindex, ++testindex;	/* NEXT */
		}
		else if (!strncmp(action, "sleep", 5))
		{
			int seconds;

			if ((cp = strchr(action, '=')))
				seconds = strtol(++cp, (char **)0, 0);
			else
				seconds = 1;
			sleep(seconds);				/* DELAY */
			lastindex = testindex, ++testindex;	/* NEXT */
		}
		/*
		 * Actions specifically on the fail track.
		 */
		else if (rv)					/* FAILED */
		{
			if (!strcmp(action, "fail"))
				break;				/* STOP */
			else if (!strncmp(action, "loop", 4))
			{
				if (!loop)
				{
					if ((cp = strchr(action, '=')))
						loop = strtol(++cp, (char **)0, 0);
					else
						loop = 1;
				}
				else if (!--loop)
					break;			/* STOP */
			}
			rv = 0;
			lastindex = testindex, ++testindex;	/* NEXT */
		}
		else
		{
			printf("ERROR: Unknown action %s defaulting to stop\n", action);
			break;					/* STOP */
		}
		if (is_control_c())
			return(-100);
	}
	return(rv);
}

int do_test_appl(menu_t *p, char **action)
{
	char *statusb = &results[66];
	char *timeb = &results[20];
	test_hdr_t *idsp;
	item_t *itemp;
	time_t time;
	int rv = 0;
	int cntr;
	char *cp;
	int cycles, cycle;

	idsp = p->itemlist[0].data.idsfs.ids;			/* RUN */

	/*
	 * Search the menu list for an ITEM_PARM named "cycles"
	 * and return a pointer to the ascii data value.
	 */
	cycles = 1;
	if ((itemp = find_item(p, "cycles")))
	{
		cp = itemp->data.cache;
		cycles = strtol(cp, (char **)0, 0);
	}
	if (cycles < 1)
		cycles = 1;
	for (rv = 0, cycle = 0; cycle < cycles && !rv; ++cycle)
	{
		/* 
		 * EXECUTE THE DOWNLOADED TEST HERE!!!
		 *
		 * This is where we execute the test.
		 *
		 * Execute Test TEST.
		 */
		if ((itemp = find_item(p, "ids-svc")) && itemp->func)
		{
			(*itemp->func)(p);
			rv = 0;
		}
		else
		{
			char *argv[16];
			int argc;
			uint32_t addr;

			/*
			 * Find test.
			 */
			addr = (uint32_t)&idsp[1];
			argc = load_args(p, argv);
			rv = ids_bootelf(addr, argc, argv);
		}

		if (rv)
		{
			cntr = (int)strtol(idsp->cntr2, (char **)0, 0);
			++cntr;
			sprintf(idsp->cntr2, "%d", cntr);
			*action = idsp->fail;
			strncpy(statusb, "FAILED", 6);
		}
		else
		{
			cntr = (int)strtol(idsp->cntr1, (char **)0, 0);
			++cntr;
			sprintf(idsp->cntr1, "%d", cntr);
			*action = idsp->pass;
			strncpy(statusb, "PASSED", 6);
		}
		results[72] = ' ';
		time = gettime();
		(void)ctime_r(&time, timeb);
		if ((cp = strchr(timeb, '\n')))
			*cp = ' ';
		if ((cp = strchr(timeb, '\r')))
			*cp = ' ';
		results[strlen(results)] = ' ';
		lprintf("%s\n", results);
	}
	return(rv);
}

/*
 * 1) total number of cycles
 * 2) total number of faults
 * 3) cumulative test time
 * 4) percent defect free
 * 5) number of logs available
 * 6) number of interruptions
 * 7) number of commands not found
 *
 *  CYCLES PASSED FAULTS  TIME  DEFECT  MTBF  XXXXXX XXXXXX
 * +------+------+------+------+------+------+------+------+
 * |999999|999999|999999|999999|99.99%|999999|999999|999999|
 * +------+------+------+------+------+------+------+------+
 * |999999|999999|999999|999999|999999|999999|999999|999999|
 * +------+------+------+------+------+------+------+------+
 * |999999|999999|999999|999999|999999|999999|999999|999999|
 * +------+------+------+------+------+------+------+------+
 * |999999|999999|999999|999999|999999|999999|999999|999999|
 * +------+------+------+------+------+------+------+------+
 */
// static char *stat_border =
//	"+======+======+======+======+======+======+======+======+";

void get_stats_appl(menu_t *p)
{
	register int count;

	p = &list_menu;
	if (!p->itemcount)
		return;
	printf("\n");
	printf("\tLIST NAME\t TESTS\tPASSED\tFAULTS\t RATIO\n");
	printf("\t---------\t -----\t------\t------\t -----\n");
	for (count = 1; count < p->itemcount; ++count)
	{
		item_t *itemp;
		int item, ratio;
		int passed = 0, faults = 0;

		itemp = &p->itemlist[count];
		if (strlen(itemp->name) < 8)
			printf("\t%s\t\t", itemp->name);
		else
			printf("\t%s\t", itemp->name);
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
		printf("% 6d\t% 6d\t% 6d\t% 5d%%\n",
			itemp->menu->itemcount - 1,
			passed, faults, ratio);
	}
	printf("\n");
	(void)exec_pause();
}

/*
 * Subordinate routines.
 */
static int load_args(menu_t *p, char *argv[])
{
	extern char *argcache[16];
	int argc;

	argv[0] = p->name;
	for (argc = 1; argc < p->itemcount; ++argc)
	{
		item_t *itemp;

		itemp = &p->itemlist[argc];
		if (itemp->type == ITEM_REG)
		{
			/*
			 * The args may come from a register descriptor.
			 */
			sprintf(argcache[argc], "%s:%s",
				itemp->data.reg.addr, itemp->data.reg.mask);
			argv[argc] = argcache[argc];
		}
		else if (itemp->menu && itemp->data.cache[0] == '!')
		{
			item_t *item_svc;

			/*
			 * The args may come from an ids-svc service.
			 */
			if ((item_svc = find_item(p, "svcname")))
			{
				if ((item_svc = find_item(itemp->menu, item_svc->data.cache)))
				{
					argv[argc] = item_svc->data.cache;
					continue;
				}
			}
			if ((item_svc = find_item(itemp->menu, &itemp->data.cache[1])))
			{
				argv[argc] = item_svc->data.cache;
			}
			else
			{
				argv[argc] = itemp->data.cache;
			}
		}
		else
		{
			/*
			 * Or the args may just come from the data cache.
			 */
			argv[argc] = itemp->data.cache;
		}
	}
	return(p->itemcount);
}

#ifdef COMMENT
int atoi(char *s)
{
	int n;

	n = 0;
	while (*s && isdigit(*s)) { n *= 10; n += (*s - '0'); ++s; }
	return(n);
}
#endif /* COMMENT */

static time_t tcmtime = 0;
static time_t starttime = 0;

void settime(time_t time)
{
	tcmtime = 0;
	starttime = gettime();
	tcmtime = time;
}

time_t gettime()
{
#if 1
	uint64_t ticks;
	uint32_t tbclk;
	time_t currtime;

	ticks = get_ticks();
	tbclk = get_tbclk();
	ticks *= 1000LL;
	ticks /= tbclk;
	currtime = (time_t)(ticks / 1000LL);
	return(tcmtime + (currtime - starttime));
#else
	struct rtc_time tmp;

	return((time_t)rtc_get(&tmp));
#endif
}

/*
 * Return 1 if yes and 0 if no.
 */
int exec_ask(char *question)
{
	int c;

	printf("%s [y] ", question ? question : "default");
	c = getchar();
	printf("\n");
	if (c != 'y' && c != 'Y')
		return(0);
	return(1);
}

int exec_pause()
{
	if (is_control_c())
		return(-100);
	printf("Hit any key to continue ...\n");
	(void)getchar();
	return(0);
}

int is_control_c()
{
	// int c;

	if (!(serial_tstc() && serial_getc() == '\003'))
		return(0);
	return(exec_ask("^C Interrupt -- Do you really want to quit?"));
}

void sleep(uint32_t delay)
{
	uint32_t start = get_timer(0);

	delay *= CFG_HZ;
	while (get_timer(start) < delay)
	{
		// if (ctrlc())
		// {
		//	return (-1);
		// }
		udelay (100);
	}
}
