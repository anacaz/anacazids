/*
 * Integrated Diagnostics Subsystem
 *
 * rfa - 060718-20-0804-07
 */
#ifdef LINUX
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#else /* LINUX */
#include <common.h>
#include <linux/ctype.h>
#include <linux/string.h>
#include <idsio.h>
#include <net.h>
#include <i2c.h>
#endif /* LINUX */
#include <idsfs.h>
#include <ids_jmpvec.h>
#include <menu.h>
#include <test_api.h>
#include <idshw_rgb.h>

extern int ids_peek(int argc, char **argv);

#define FORWARD	/* */

#ifdef LINUX
struct termios stdinold, stdinnew;
#endif /* LINUX */

FORWARD void ids_start(void);
FORWARD void ids_exit(int errno);
FORWARD void load_u_boot_vectors(void);

static char arg0[128];
static char arg1[128];
static char arg2[128];
static char arg3[128];
static char arg4[128];
static char arg5[128];
static char arg6[128];
static char arg7[128];
static char arg8[128];
static char arg9[128];
static char arg10[128];
static char arg11[128];
static char arg12[128];
static char arg13[128];
static char arg14[128];
static char arg15[128];

char *argcache[16] =
{
	arg0, arg1, arg2, arg3,
	arg4, arg5, arg6, arg7,
	arg8, arg9, arg10, arg11,
	arg12, arg13, arg14, arg15
};

#ifdef LINUX
int main(int arc, char **argv)
#else /* LINUX */
int ids_main(int arc, char **argv)
#endif /* LINUX */
{
	argcache[0] = arg0;
	argcache[1] = arg1;
	argcache[2] = arg2;
	argcache[3] = arg3;
	argcache[4] = arg4;
	argcache[5] = arg5;
	argcache[6] = arg6;
	argcache[7] = arg7;
	argcache[8] = arg8;
	argcache[9] = arg9;
	argcache[10] = arg10;
	argcache[11] = arg11;
	argcache[12] = arg12;
	argcache[13] = arg13;
	argcache[14] = arg14;
	argcache[15] = arg15;

	ids_start();
	for (;;)
	{
		set_ids_mode("main");
		main_menu.itemlist[1].data.time.lasttime = gettime();
		(void)menu_update_time(&main_menu);
		menu_input(&main_menu);
		printf("Do you really want to quit? ");
		if (getchar() == 'y')
			break;
	}
	ids_exit(0);
#ifdef LINUX
	exit(0);
#else /* LINUX */
	return(0);
#endif /* LINUX */
}

/*
 * IDS systems startup code.
 */
void ids_start(void)
{
	extern void menutab_init(void);
	extern void fileio_init(void);
	extern void cmd_tbl_init(void);
	extern void exec_text_init(void);

#ifdef LINUX
	tcgetattr(0, &stdinold);
	tcgetattr(0, &stdinnew);
	cfmakeraw(&stdinnew);
	tcsetattr(0, TCSANOW, &stdinnew);
#endif /* LINUX */

	menutab_init();			/* Also loads BUILT-IN menu */
	fileio_init();
	cmd_tbl_init();
	load_u_boot_vectors();
	exec_text_init();
}

void ids_exit(int errno)
{
#ifdef LINUX
	tcsetattr(0, TCSANOW, &stdinold);
#endif /* LINUX */
	printf("Goodbye!\n");
}

#ifndef LINUX
/*
 * U-Boot functions are loaded into a common vector table for use by
 * downloaded tests and test lists.
 */
extern void NetSendPacket(volatile uchar * pkt, int len);
extern void NetSetHandler(rxhand_f *f);
extern struct eth_device *eth_get_dev(void);
extern int eth_rx(void);
extern void eth_set_current(void);
extern void eth_halt(void);
extern int eth_init(bd_t *bis);
extern unsigned long get_tbclk(void);
extern void udelay(unsigned long);
extern unsigned long get_timer(unsigned long);
extern char *getenv(char *);
extern void setenv(char *, char *);
extern void wait_ticks(unsigned long);
extern int run_command(const char *, int);

void load_u_boot_vectors(void)
{
	u_boot_vectors[0]  = (void (*)(void))NetSendPacket;
	u_boot_vectors[1]  = (void (*)(void))NetSetHandler;
	u_boot_vectors[2]  = (void (*)(void))eth_get_dev;
	u_boot_vectors[3]  = (void (*)(void))eth_rx;
	u_boot_vectors[4]  = (void (*)(void))eth_set_current;
	u_boot_vectors[5]  = (void (*)(void))eth_halt;
	u_boot_vectors[6]  = (void (*)(void))eth_init;
	u_boot_vectors[7]  = (void (*)(void))get_tbclk;
	u_boot_vectors[8]  = (void (*)(void))udelay;
	u_boot_vectors[9]  = (void (*)(void))get_timer;
	u_boot_vectors[10] = (void (*)(void))getenv;
	u_boot_vectors[11] = (void (*)(void))setenv;
	u_boot_vectors[12] = (void (*)(void))wait_ticks;
#ifdef CONFIG_MPC8378TCM	
	u_boot_vectors[13] = (void (*)(void))a6_get;
	u_boot_vectors[14] = (void (*)(void))a6_put;
	u_boot_vectors[15] = (void (*)(void))mux_get;
	u_boot_vectors[16] = (void (*)(void))mux_aget;
	u_boot_vectors[17] = (void (*)(void))mux_put;
	u_boot_vectors[18] = (void (*)(void))mux_aput;
#endif
#ifdef CONFIG_SERIAL_SOFTWARE_FIFO
	u_boot_vectors[20] = (void (*)(void))serial_buffered_init;
	u_boot_vectors[21] = (void (*)(void))serial_buffered_putc;
	u_boot_vectors[22] = (void (*)(void))serial_buffered_puts;
	u_boot_vectors[23] = (void (*)(void))serial_buffered_getc;
	u_boot_vectors[24] = (void (*)(void))serial_buffered_tstc;
#endif /* CONFIG_SERIAL_SOFTWARE_FIFO */

	u_boot_vectors[30] = (void (*)(void))hang;
	u_boot_vectors[31] = (void (*)(void))initdram;
	u_boot_vectors[32] = (void (*)(void))display_options;
	u_boot_vectors[33] = (void (*)(void))print_size;
	u_boot_vectors[34] = (void (*)(void))print_buffer;

	u_boot_vectors[40] = (void (*)(void))main_loop;
	u_boot_vectors[41] = (void (*)(void))run_command;
	u_boot_vectors[42] = (void (*)(void))readline;
#ifdef CONFIG_BOOT_RETRY_TIME
	u_boot_vectors[43] = (void (*)(void))init_cmd_timeout;
	u_boot_vectors[44] = (void (*)(void))reset_cmd_timeout;
#endif	/* CONFIG_BOOT_RETRY_TIME */

	u_boot_vectors[50] = (void (*)(void))board_init_f;
	u_boot_vectors[51] = (void (*)(void))board_init_r;
	u_boot_vectors[52] = (void (*)(void))checkboard;
//	u_boot_vectors[53] = (void (*)(void))checkflash;
//	u_boot_vectors[54] = (void (*)(void))checkdram;
	u_boot_vectors[55] = (void (*)(void))strmhz;
//	u_boot_vectors[56] = (void (*)(void))last_stage_init;
#ifdef CFG_ID_EEPROM
	u_boot_vectors[57] = (void (*)(void))mac_read_from_eeprom;
#endif /* CFG_IDS_EEPROM */

#ifndef CFG_NO_FLASH
	u_boot_vectors[60] = (void (*)(void))flash_perror;
#endif 
	u_boot_vectors[61] = (void (*)(void))autoscript;
	u_boot_vectors[62] = (void (*)(void))print_image_hdr;
	// u_boot_vectors[63] = (void (*)(void))printf;
	u_boot_vectors[63] = (void (*)(void))lprintf;
	u_boot_vectors[64] = (void (*)(void))memcpy;
	u_boot_vectors[65] = (void (*)(void))simple_strtol;
	u_boot_vectors[66] = (void (*)(void))spatgen;
	u_boot_vectors[67] = (void (*)(void))patgen;
	u_boot_vectors[68] = (void (*)(void))strcmp;
	u_boot_vectors[69] = (void (*)(void))modgen;
	u_boot_vectors[70] = (void (*)(void))ids_peek;
	u_boot_vectors[71] = (void (*)(void))sprintf;
	u_boot_vectors[72] = (void (*)(void))strcmp;
	u_boot_vectors[73] = (void (*)(void))mb_start;
	u_boot_vectors[74] = (void (*)(void))fileio_tftp_svc;

	/*
	 * I2C vectors.
	 */
	u_boot_vectors[80] = (void (*)(void))i2c_init;
	u_boot_vectors[81] = (void (*)(void))i2c_probe;
	u_boot_vectors[82] = (void (*)(void))i2c_set_bus_num;
	u_boot_vectors[83] = (void (*)(void))i2c_get_bus_num;
	u_boot_vectors[84] = (void (*)(void))i2c_set_bus_speed;
	u_boot_vectors[85] = (void (*)(void))i2c_get_bus_speed;
	u_boot_vectors[86] = (void (*)(void))i2c_reg_read;
	u_boot_vectors[87] = (void (*)(void))i2c_reg_write;
	u_boot_vectors[88] = (void (*)(void))i2c_read;
	u_boot_vectors[89] = (void (*)(void))i2c_write;
}
#endif /* !LINUX */
