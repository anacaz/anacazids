/*
 * Integrated Diagnostics Subsystem
 *
 * Diagnostic Test Suite - i2c.h
 *
 */

#ifndef _I2C_H_
#define _I2C_H_

#include <io.h>  /* brings in byteorder.h and types.h */

/*
 * WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
 *
 * The implementation MUST NOT use static or global variables if the
 * I2C routines are used to read SDRAM configuration information
 * because this is done before the memories are initialized. Limited
 * use of stack-based variables are OK (the initial stack size is
 * limited).
 *
 * WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
 */

/*
 * Configuration items.
 */
#define I2C_RXTX_LEN	128	/* maximum tx/rx buffer length */

#define CFG_I2C_SPEED	400000  /* I2C speed and slave address  */
#define CFG_I2C_SLAVE	0x7F

#define CFG_HZ  1000            /* decrementer freq: 1 ms ticks */

#define	FUJI_I2C	1	/* 2 address bytes, host:big endian, Fuji:little endian */
#define STD_I2C		0	/* 1 address byte, no swap */

#define FUJI_ADDR_SIZE	2
#define	FUJI_REG_SIZE	4
#define	FUJI_MAX_REG_ADDR	0xFFFC

#define MAX_PIC_REGS	32
#define PIC_REG_ADDR_SIZE  1

#define I2C_BUS0	0
#define	I2C_BUS1	1
#define	I2C_BOTH_BUSES	2

#define I2C_E    0x0000 /* i2c CFG Eeprom bus - offset 0x3000 */
#define I2C_S    0x0100 /* i2c System bus - offset 0x3100 */

#define I2C_READ  1
#define I2C_WRITE 0

#define	SPD_EEPROM_SIZE		256  /* content in 1st 128 bytes, remainder free to use */

#define	XFP_MUX_I2C_DEV		0x70
#define	XFP_I2C_DEV		0x50

#define	SPD_DDR2_SEC_DEV	0x34
#define	PIC_I2C_DEV		0x59

#define	XFP_MEM_SIZE		256

#define	XFP_GEN_CTRL_BYTE	0x6E
#define	XFP_TBL_SEL_BYTE	0x7F

#define	XFP_ENABLE		0
#define	XFP_DISABLE		1

#define	XFP_DISABLE_BIT		0x40

#define	PASS		0
#define	FAIL		1
typedef enum
{
	CHAN_0,		/* 0 */
	CHAN_1,		/* 1 */
	CHAN_2,		/* 2 */
	CHAN_3,		/* 3 */
	ALL_CHANS	/* 4 */
} xfp_chan_t;

typedef enum
{
	CHAN_0_VAL = 4, /* 4 */
	CHAN_1_VAL,	/* 5 */
	CHAN_2_VAL,	/* 6 */
	CHAN_3_VAL	/* 7 */
} xfp_chan_val_t;

/*
 *  * I2C Registers(0x3000-0x4000) bit definitions
 */

/* 0x3000 - I2C Address Register */
#define MPC85xx_I2CADR_MASK     0xFE

/* 0x3004 - I2C Frequency Divider Register */
#define MPC85xx_I2CFDR_MASK     0x3F

/* 0x3008 - I2C Control Register */
#define MPC85xx_I2CCR_MEN       0x80
#define MPC85xx_I2CCR_MIEN      0x40
#define MPC85xx_I2CCR_MSTA      0x20
#define MPC85xx_I2CCR_MTX       0x10
#define MPC85xx_I2CCR_TXAK      0x08
#define MPC85xx_I2CCR_RSTA      0x04
#define MPC85xx_I2CCR_BCST      0x01

/* 0x300c - I2C Status Register */
#define MPC85xx_I2CSR_MCF       0x80
#define MPC85xx_I2CSR_MAAS      0x40
#define MPC85xx_I2CSR_MBB       0x20
#define MPC85xx_I2CSR_MAL       0x10
#define MPC85xx_I2CSR_BCSTM     0x08
#define MPC85xx_I2CSR_SRW       0x04
#define MPC85xx_I2CSR_MIF       0x02
#define MPC85xx_I2CSR_RXAK      0x01

/* 0x3010 - I2C Data Register */
#define MPC85xx_I2CDR_DATA      0xFF

/* 0x3014 - I2C Digital Filtering Sampling Rate Register */
#define MPC85xx_I2CDFSRR        0x3F

/*
 * Initialization, must be called once on start up, may be called
 * repeatedly to change the speed and slave addresses.
 */
void d_i2c_init(int speed, int slaveaddr, int bus, int clkdiv);

/*
 * Checks if specified i2c bus is initialized
 */
int d_i2c_chk_init(int bus);

/*
 * Probe the given I2C chip address.  Returns 0 if a chip responded,
 * not 0 on failure.
 */
int d_i2c_probe(uchar chip, int bus, int delay);

/*
 * Read/Write interface:
 *   chip:    I2C chip address, range 0..127
 *   addr:    Memory (register) address within the chip
 *   alen:    Number of bytes to use for addr (typically 1, 2 for larger
 *              memories, 0 for register type devices with only one
 *              register)
 *   buffer:  Where to read/write the data
 *   len:     How many bytes to read/write
 *
 *   Returns: 0 on success, not 0 on failure
 */
int d_i2c_read(uchar chip, uint addr, int alen, uchar *buffer, int len, int bus, int delay);
int d_i2c_write(uchar chip, uint addr, int alen, uchar *buffer, int len, int bus, int delay);
int d_i2c_16b_line_write (unsigned char dev, unsigned int addr, int alen, unsigned char *data, int length, int bus, int ids_delay);

/*
 * Utility routines to read/write registers.
 */
unsigned char d_i2c_reg_read (uchar chip, uchar reg, int bus, int delay);
void  d_i2c_reg_write(uchar chip, uchar reg, uchar val, int bus, int delay);

#endif	/* _I2C_H_ */
