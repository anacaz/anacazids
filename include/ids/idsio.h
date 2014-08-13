#ifndef _idsio_h
#define _idsio_h
/*
 * Integrated Diagnostics Subsystem
 *
 * rfa - 060807-090624
 */

#define getchar		serial_getc
#define putchar		serial_putc
#define isavail		serial_tstc

#define GETCHAR		0
#define PUTCHAR		1

// #define jmp_vector	0x00180000

#endif /* !_idsio_h */
