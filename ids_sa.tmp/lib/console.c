/*
 */

#ifdef LINUX
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#else /* LINUX */
#include <common.h>
#endif /* LINUX */

#ifdef CONFIG_AMIGAONEG3SE
int console_changed = 0;
#endif

#ifdef CFG_CONSOLE_IS_IN_ENV
/*
 * if overwrite_console returns 1, the stdin, stderr and stdout
 * are switched to the serial port, else the settings in the
 * environment are used
 */
#ifdef CFG_CONSOLE_OVERWRITE_ROUTINE
extern int overwrite_console (void);
#define OVERWRITE_CONSOLE overwrite_console ()
#else
#define OVERWRITE_CONSOLE 0
#endif /* CFG_CONSOLE_OVERWRITE_ROUTINE */

#endif /* CFG_CONSOLE_IS_IN_ENV */

void serial_printf (const char *fmt, ...)
{
	va_list args;
	uint i;
	char printbuffer[CFG_PBSIZE];

	va_start (args, fmt);

	/* For this to work, printbuffer must be larger than
	 * anything we ever want to print.
	 */
	i = vsprintf (printbuffer, fmt, args);
	va_end (args);

	serial_puts(printbuffer);
}

int is_control_c(void)
{
	return(serial_tstc() && serial_getc() == '\003');
}

void putc(const char c)
{
	serial_putc(c);
}

void puts(const char *s)
{
	serial_puts(s);
}

void printf (const char *fmt, ...)
{
	va_list args;
	uint i;
	char printbuffer[CFG_PBSIZE];

	va_start (args, fmt);

	/* For this to work, printbuffer must be larger than
	 * anything we ever want to print.
	 */
	i = vsprintf (printbuffer, fmt, args);
	va_end (args);

	/* Print the string */
	serial_puts(printbuffer);
}

void vprintf (const char *fmt, va_list args)
{
	uint i;
	char printbuffer[CFG_PBSIZE];

	/* For this to work, printbuffer must be larger than
	 * anything we ever want to print.
	 */
	i = vsprintf (printbuffer, fmt, args);

	/* Print the string */
	serial_puts(printbuffer);
}

/* test if ctrl-c was pressed */
static int ctrlc_disabled = 0;	/* see disable_ctrl() */
static int ctrlc_was_pressed = 0;
int ctrlc (void)
{
	if (serial_tstc())
	{
		switch (serial_getc())
		{
		case 0x03:		/* ^C - Control C */
			ctrlc_was_pressed = 1;
			return 1;
		default:
			break;
		}
	}
	return 0;
}
