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
 * IDS I2C hardware driver and state machine.
 *
 * Copyright (c)2006-2009 Anacaz Networks, Inc., ALL RIGHTS RESERVED
 *
 * rfa - 061023
 */
#include <stdint.h>
#include <common.h>
#include <idssm.h>
#include <idshw.h>

#define FORWARD	/* */

/*
 * I2C Hardware interface description.
 */
typedef struct i2creg i2creg_t;
typedef struct i2cdev i2cdev_t;

typedef struct i2c
{
	hw_t		hw;
	/*
	 * Configuration and initialization information.
	 */
	struct
	{
		uint8_t		dev;
		int		bus;
		int		speed;
		int		slave;
		int		clkdiv;
		int		delay;		/* */
		int		alen;		/* Address length in bytes */
	} 		init;
	/*
	 * Device register access description.
	 */
	struct i2cdev
	{
		struct i2creg
		{
			uint8_t		r;
			uint8_t		_[3];
		} 		ad;
		i2creg_t	fd;
		i2creg_t	cc;
		i2creg_t	cs;
		i2creg_t	cd;
		i2creg_t	dfsr;
	}		*regs;
} i2c_t;

FORWARD int i2c_driver(int argc, char **argv);

#define I2C_DEVICE(HW,dev,bus,speed,slave,clkdiv,delay,alen) \
{ \
	HW, \
	{ (dev), (bus), (speed), (slave), (clkdiv), (delay), (alen) }, \
	0 \
}

i2c_t i2c_device = I2C_DEVICE(HW_DEVICE(i2c,i2c_driver,0,0),1,2,3,4,5,6,7);

/*
 * This function represents the operational method for the I2C device
 * after it has been tested with IDS.  This method can then be used to
 * access I2C hardware for testing attached hardware.
 */
int i2c_driver(int argc, char **argv)
{
	return(0);
}

/*
 * I2C State Machine Arc functions.
 */
FORWARD static void i2c_reset(void *p);			/* 1 */
FORWARD static void i2c_init(void *p);			/* 2 */
FORWARD static void i2c_boot(void *p);			/* 3 */
FORWARD static void i2c_run(void *p);			/* 4 */
FORWARD static void i2c_halt(void *p);			/* 5 */
FORWARD static void i2c_pass(void *p);			/* 6 */
FORWARD static void i2c_fail(void *p);			/* 7 */
FORWARD static void i2c_ignore(void *p);		/* 8 */

ids_sm_t i2c_sm = SM_INSTANCE(i2c);

#ifdef LINUX
int main(int argc, char **argv)
{
	if (!--argc)
	{
		printf("Usage: %s [reset | init]\n", *argv);
		exit(-1);
	}
	setstate(i2c_sm.curr_state, ST_PRES);
	do
	{
		if (!strcmp("run", *++argv))
		{
			sm_show(&i2c_sm, EV_RESET, "old");
			(void)sm_run(&i2c_sm, 0);
			sm_show(&i2c_sm, i2c_sm.last_event, "new");
		}
		else if (!strcmp("reset", *argv))
		{
			sm_show(&i2c_sm, EV_RESET, "old");
			(void)sm_run_raw(&i2c_sm, EV_RESET, 0);
			sm_show(&i2c_sm, EV_RESET, "new");
		}
		else if (!strcmp("init", *argv))
		{
			sm_show(&i2c_sm, EV_INIT, "old");
			(void)sm_run_raw(&i2c_sm, EV_INIT, 0);
			sm_show(&i2c_sm, EV_INIT, "new");
		}
		else if (!strcmp("boot", *argv))
		{
			sm_show(&i2c_sm, EV_BOOT, "old");
			(void)sm_run_raw(&i2c_sm, EV_BOOT, 0);
			sm_show(&i2c_sm, EV_BOOT, "new");
		}
		else
			sm_show(&i2c_sm, EV_NOEVENT, "old");
	} while (--argc);
	exit(0);
}
#endif /* LINUX */

static void i2c_reset(void *p)
{
	setstate(i2c_sm.curr_state, ST_NOINIT);
}

static void i2c_init(void *p)
{
	setstate(i2c_sm.curr_state, ST_NOBOOT);
}

static void i2c_boot(void *p)
{
	setstate(i2c_sm.curr_state, ST_READY);
}

static void i2c_run(void *p)
{
	int status;

	setstate(i2c_sm.curr_state, ST_BUSY);

	status = hw_launcher(p);

	if (status == 0)
		(void)sm_pass(&i2c_sm, p);
	else if (status == -1)
		(void)sm_fail(&i2c_sm, p);
}

static void i2c_halt(void *p)
{
	setstate(i2c_sm.curr_state, ST_NOBOOT);
}

static void i2c_pass(void *p)
{
	setstate(i2c_sm.curr_state, ST_READY);
}

static void i2c_fail(void *p)
{
	setstate(i2c_sm.curr_state, ST_FAULT);
}

static void i2c_ignore(void *p)
{
	setstate(i2c_sm.curr_state, ST_READY);
}
