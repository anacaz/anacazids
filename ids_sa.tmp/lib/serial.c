#include <common.h>

#ifdef CFG_NS16550_SERIAL

#include <ns16550.h>
#ifdef CFG_NS87308
#include <ns87308.h>
#endif

#if !defined(CONFIG_CONS_INDEX)
#error	"No console index specified."
#elif (CONFIG_CONS_INDEX < 1) || (CONFIG_CONS_INDEX > 4)
#error	"Invalid console index value."
#endif

#if CONFIG_CONS_INDEX == 1 && !defined(CFG_NS16550_COM1)
#error	"Console port 1 defined but not configured."
#elif CONFIG_CONS_INDEX == 2 && !defined(CFG_NS16550_COM2)
#error	"Console port 2 defined but not configured."
#elif CONFIG_CONS_INDEX == 3 && !defined(CFG_NS16550_COM3)
#error	"Console port 3 defined but not configured."
#elif CONFIG_CONS_INDEX == 4 && !defined(CFG_NS16550_COM4)
#error	"Console port 4 defined but not configured."
#endif

/* Note: The port number specified in the functions is 1 based.
 *	 the array is 0 based.
 */
static NS16550_t serial_ports[4] = {
#ifdef CFG_NS16550_COM1
	(NS16550_t)CFG_NS16550_COM1,
#else
	NULL,
#endif
#ifdef CFG_NS16550_COM2
	(NS16550_t)CFG_NS16550_COM2,
#else
	NULL,
#endif
#ifdef CFG_NS16550_COM3
	(NS16550_t)CFG_NS16550_COM3,
#else
	NULL,
#endif
#ifdef CFG_NS16550_COM4
	(NS16550_t)CFG_NS16550_COM4
#else
	NULL
#endif
};

#define PORT	serial_ports[port-1]
#define CONSOLE	(serial_ports[CONFIG_CONS_INDEX-1])

void
_serial_putc(const char c,const int port)
{
	if (c == '\n')
		NS16550_putc(PORT, '\r');

	NS16550_putc(PORT, c);
}

void _serial_putc_raw(const char c,const int port)
{
	NS16550_putc(PORT, c);
}

void _serial_puts(const char *s,const int port)
{
	while (*s) {
		_serial_putc (*s++,port);
	}
}


int _serial_getc(const int port)
{
	return NS16550_getc(PORT);
}

int _serial_tstc(const int port)
{
	return NS16550_tstc(PORT);
}

void
serial_putc(const char c)
{
	_serial_putc(c,CONFIG_CONS_INDEX);
}

void
serial_putc_raw(const char c)
{
	_serial_putc_raw(c,CONFIG_CONS_INDEX);
}

void
serial_puts(const char *s)
{
	_serial_puts(s,CONFIG_CONS_INDEX);
}

int
serial_getc(void)
{
	return _serial_getc(CONFIG_CONS_INDEX);
}

int
serial_tstc(void)
{
	return _serial_tstc(CONFIG_CONS_INDEX);
}

#endif /* CFG_NS16550_SERIAL */
