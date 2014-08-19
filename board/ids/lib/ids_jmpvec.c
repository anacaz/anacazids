/*
 * Integrated Diagnostics Subsystem
 *
 * Copyright (c) 2006-2014 "Anacaz Networks, Inc."
 * Diagnostics subsystem for U-Boot
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
 * rfa - 061102-090506-0722
 */
#include <common.h>
#include <linux/string.h>
#include <ids_jmpvec.h>
#include <test_api.h>


unsigned long get_timer(unsigned long base)
{
	return(GET_TIMER(base));
}

int run_command(const char *s, int x)
{
	return(RUN_COMMAND(s,x));
}

void fileio_tftp_svc(void *p)
{
	FILEIO_TFTP_SVC(p);
}

void setenv (char *varname, char *varvalue)
{
	SETENV(varname, varvalue);
}

char *getenv(char *name)
{
	return(GETENV(name));
}

void *memcpy(void *to, const void *from, __kernel_size_t size)
{
	return(MEMCPY(to, from, size));
}

long simple_strtol(const char *cp, char **endp, unsigned int base)
{
	return(STRTOL(cp, endp, base));
}

int (*mb_start(int (*callback)(mb_op_enum_t, ...)))(mb_op_enum_t, ...)
{
	return(MB_START(callback));
}

pvalue_t spatgen(pvalue_t ivalue, pvalue_t seed, pmod_t modifier)
{
	return(SPATGEN(ivalue, seed, modifier));
}

pvalue_t patgen(pvalue_t curval, pvalue_t mask, pmod_t modifier)
{
	return(PATGEN(curval, mask, modifier));
}

int strcmp(const char *s1, const char *s2)
{
	return(STRCMP(s1, s2));
}

__kernel_size_t strlen(const char *s)
{
	return(STRLEN(s));
}

pmod_t modgen(char *s)
{
	return(MODGEN(s));
}

int ids_peek(int argc, char **argv)
{
	return(IDS_PEEK(argc, argv));
}

void udelay(unsigned long usec)
{
	UDELAY(usec);
}

void i2c_init(int speed, int slaveadd)
{
	I2C_INIT(speed, slaveadd);
}

void i2c_probe(unsigned char chip)
{
	I2C_PROBE(chip);
}

int i2c_set_bus_num(unsigned int bus)
{
	return(I2C_SET_BUS_NUM(bus));
}

unsigned int i2c_get_bus_num(void)
{
	return(I2C_GET_BUS_NUM());
}

int i2c_set_bus_speed(unsigned int speed)
{
	return(I2C_SET_BUS_SPEED(speed));
}

unsigned int i2c_get_bus_speed(void)
{
	return(I2C_GET_BUS_SPEED());
}

unsigned char i2c_reg_read(unsigned char addr, unsigned char reg)
{
	return(I2C_REG_READ(addr, reg));
}

void i2c_reg_write(unsigned char addr, unsigned char reg, unsigned char val)
{
	I2C_REG_WRITE(addr, reg, val);
}

void i2c_read(unsigned char dev, unsigned int addr, int alen, unsigned char *data, int len)
{
	I2C_READ(dev, addr, alen, data, len);
}

void i2c_write(unsigned char dev, unsigned int addr, int alen, unsigned char *data, int len)
{
	I2C_WRITE(dev, addr, alen, data, len);
}

#ifdef COMMENT
unsigned long a6_get(unsigned long base, unsigned long addr)
{
	return(A6_GET(base,addr));
}

void a6_put(unsigned long base, unsigned long addr, unsigned long value)
{
	A6_PUT(base,addr,value);
}

unsigned short mux_get(unsigned long addr)
{
	return(MUX_GET(addr));
}

unsigned short mux_aget(unsigned long addr)
{
	return(MUX_AGET(addr));
}

void mux_put(unsigned long addr, unsigned short value)
{
	MUX_PUT(addr,value);
}

void mux_aput(unsigned long addr, unsigned short value)
{
	
	MUX_APUT(addr,value);
}
#endif /* COMMENT */
