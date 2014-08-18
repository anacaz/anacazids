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
 * rfa - 060725-1003-05-06
 */
#include <common.h>
#include <idsfs.h>
#include <idsio.h>
#include <menu.h>

#define BARS		72

#define MAPTOBARS(bars,value)	(((bars * 100) * value) / 10000)

char progress_bar[BARS + 1];

#define FORWARD	/**/

FORWARD void prog_bar_appl(menu_t *p);
FORWARD int prog_bar_main(int argc, char **argv);
FORWARD void showprogress(char *name, int progress, int ticks,
	int fail, int pass);

void prog_bar_appl(menu_t *p)
{
	int ticks;
	char *argv[10];
	char testname[32];
	char progress[32];

	argv[0] = "p-name";
	sprintf(testname, "%s", p->name);
	argv[1] = testname;
	argv[2] = progress;
	for (ticks = 0; ticks <= 100; ++ticks)
	{
		volatile int t;

		sprintf(progress, "%d", ticks);
		prog_bar_main(3, argv);
		for (t = 500; t; --t) ;
	}
(void)getchar();
}

int prog_bar_main(int argc, char **argv)
{
	int progress;
	int ticks;
	char *testname;
	int n, limit;

	if (!--argc)
	{
		printf("Usage: %s test-name [progress] [ticks]\n", *argv);
		return(-1);
	}

	/*
	 * Retrieve the test name.
	 */
	testname = *++argv;

	/*
	 * Retrieve the progress point.
	 */
	progress = 0;
	if (--argc)
		progress = (int)strtol(*++argv, (char **)0, 0);

	limit = MAPTOBARS(BARS, progress) - 1;
	for (n = 0; n < limit; ++n)
		progress_bar[n] = '=';
	while (n < BARS)
		progress_bar[n++] = '.';
	progress_bar[n] = '\0';

	ticks = 0;
	if (--argc)
		ticks = (int)strtol(*++argv, (char **)0, 0);

	showprogress(testname, progress, ticks, 0, 0);
	return(0);
}

/*
     test-name                      | ticks: 37620 | fail:    0 | pass:    0  
11% [=========...............................................................]
 *
 *	:ho=\E[2;1H
 * fs = return from status line.
 *	:fs=\E8
 * ds = disable status line.
 *	:ds=\E7\E[1;24r\E8
 * ts = move cursor to column %1 of status line.
 *	:ts=\E7\E[1;%dH\E[1K
 *	:is=\E7\E[2;24r\E8
 */

void progressrun(void *p)
{
}

/*
 * Progress is a whole number between 0 and 100 (inclusive)
 * which represents a percentage of completion.
 * Arguments:
 * 	name		- Test name currently running
 * 	progress	- Percentage of progress complete
 * 	ticks		- CPU tick cycles
 * 	fail		- Number of failures
 * 	pass		- Number of successes
 */
void showprogress(char *name, int progress, int ticks, int fail, int pass)
{
	int bar, dots;
	char percent[8];

	printf("\033[H");
	printf("      %-31s| ticks: %6d | fail: %4d | pass: %4d\n",
		name, ticks, fail, pass);
	sprintf(percent, "%d%%", progress);
	printf(" %-4s[", percent);

	if (progress < 0)
		progress = 0;
	if (progress > 100)
		progress = 100;

	if (progress == 0)
		printf("........................................................................]");
	else if (progress == 1)
		printf("=.......................................................................]");
	else if (progress == 99)
		printf("=======================================================================.]");
	else if (progress == 100)
		printf("========================================================================]");
	else
	{
		bar = MAPTOBARS(BARS, progress);
		dots = BARS - bar;

		progress_bar[bar - 1] = '=';
		printf("%s]", progress_bar);
	}
	printf("\n");
}
