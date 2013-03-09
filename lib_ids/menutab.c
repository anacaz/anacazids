/*
 * Integrated Diagnostics Subsystem
 *
 * Copyright (c)2006-2009 Anacaz Networks, Inc., ALL RIGHTS RESERVED
 *
 * Internal IDS menu structure.
 *
 * rfa - 060718-19-20-24-25-26-27-0804-07-08-090522-0602-23-24-0803-04-11-12
 * rfa - 090813-17-1002-1119-20-100210
 */
#ifdef LINUX
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#else /* LINUX */
#include <common.h>
#include <idsio.h>
#include <rtc.h>
#endif /* LINUX */
#include <idsfs.h>
#include <menu.h>
#include <test_api.h>

#define FORWARD	/* */

FORWARD void rtc_appl(menu_t *p);
FORWARD void console_appl(menu_t *p);

/*
 * INST_APPL(DOWNLOAD, config_download_appl, bringup mode),
 *
 * INST_APPL(MANUFAC, config_manufac_appl, manufac mode),
 * INST_APPL(FIELD, config_field_appl, field mode),
 *
 * TODO:
 *
 * Extend this to insert up to 10 INSTALLED TESTS items similar to the
 * one below.  Replace the mode_menu name with the main_menu name.  Add
 * an insert_main_menu function and add a menu-name to add downloaded
 * files to from the ^L download function.
 */
#ifdef CONFIG_MPC8378TCM
menu_t mode_menu =
	MENU_START(MAIN, 13)
	INST_APPL(AUTORUN,	   regress_run_appl,	download appl),
	INST_APPL(DEBUG,	   config_debug_appl,	debug mode),
	INST_APPL(CONFIG ONLY,	   config_only_appl,	regress mode),
	INST_APPL(INSTALLED TESTS, config_regress_appl,	regress mode),
	INST_APPL(BRINGUP DVT,	   config_regress_appl,	regress mode),
	INST_APPL(ENVIRON DVT,	   config_edvt_appl,	regress mode),
	INST_APPL(A6 INTR TEST,	   config_a6ddr_appl,	regress mode),
	INST_APPL(TCM START-UP,	   config_fpga_appl,	regress mode),
	INST_APPL(TCM CHAN RESET,  config_chan_appl,	regress mode),
	INST_APPL(A6 DEBUG,	   config_a6debug_appl,	regress mode),
	INST_APPL(PROGRESS BAR,	   prog_bar_appl,	progress bar),
	INST_APPL(RTC TEST,	   rtc_appl,		real time clock test),
	INST_APPL(master-list MENU,console_appl,	menu list),
	MENU_END();
#elif CONFIG_MPC8378UQAM
menu_t mode_menu =
	MENU_START(MAIN, 6)
	INST_APPL(AUTORUN,	   regress_uqam_appl,	download appl),
	INST_APPL(DEBUG,	   config_debug_appl,	debug mode),
	INST_APPL(CONFIG ONLY,	   config_only_appl,	regress mode),
	INST_APPL(UQAM START-UP,   config_uqam_appl,	regress mode),
	INST_APPL(PROGRESS BAR,	   prog_bar_appl,	progress bar),
	INST_APPL(RTC TEST,	   rtc_appl,		real time clock test),
	MENU_END();
#endif /* CONFIG_MPC8378XXXX */

/*
	INST_APPL(INSTALLED TESTS, menu_list_appl, regress mode),
 */
menu_t main_menu =
	MENU_START(Main, 3)
	INST_DATE(DATE, Set date: mm/dd/yyyy),
	INST_TIME(TIME, Set time: hh:mm:ss),
	INST_MENU(IDS, &mode_menu, Run IDS),
	MENU_END();

/*
 * INST_APPL(VIEW, menu_show_appl, Show logs),
 * INST_MENU(UPLOAD, &send_logs_menu, Tftp upload logs to server),
 * INST_APPL(CLEAR, menu_clear_appl, Clear all logs),
 */
menu_t log_menu =
	MENU_START(LOG FILES, 0)
	MENU_END();
/*
 * stop, pause, wait, ignore, spin 
 *
 * Add test list and test names here.
 *	INST_LIST_NAME(LIST NAME), was in position 1
 */
menu_t execution_menu =
	MENU_START(EXECUTION, 8)
	INST_APPL(GO, test_go_appl, Execute),
	INST_APPL(SHOWLIST, test_list_appl, List),
	INST_NAME(NAME),
	INST_PARM(CYCLES, 1, Number of exection cycles),
	INST_CNTR(ELAPSED TIME,	00:00:00),
	INST_MENU(VIEW LOGS, &log_menu, view execution logs),
	INST_APPL(CLEAR LOGS, menu_del_appl, clear execution logs),
	INST_APPL(STATISTICS, get_stats_appl, test statistics),
	MENU_END();
/*
 * NOTE:  When changing this list don't forget to modify the RC_ constants
 * in ../include/ids/menu.h that are used as indicies into this list!
 */
menu_t ram_carve_menu =
	MENU_START(RAM LAYOUT, 6)
	INST_PARM(U-BOOT,    0x00200000, U-Boot memory region),
	INST_PARM(MENU POOL, 0x00201000, Menu Pool memory region),
	INST_PARM(LOG FILES, 0x00300000, Log Files memory region),
	INST_PARM(RAM,       0x00400000, Scratch RAM memory region),
	INST_PARM(TLS FILES, 0x10000000, .tls download memory region),
	INST_PARM(SYSTEM,    0x17000000, U-Boot upper memory region),
	MENU_END();

/*
 * FILEIO fileio.c
 *
 * The order of these parameters are aligned with the FILEIO_ constants
 * in the menu.h header file.
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(NAME, tcm-dvt, top level menu name),
	INST_PARM(tls file, tcm/ram.tls, .tls file to download),
	INST_PARM(tls file, tcm/mux.tls, .tls file to download),
	INST_PARM(tls file, tcm/cpld.tls, .tls file to download),
	INST_PARM(tls file, tcm/hub.tls, .tls file to download),
	INST_PARM(tls file, tcm/a6-regs.tls, .tls file to download),
 */
menu_t file_mgr_menu =
	MENU_START(FILE MGR, 16)
	INST_APPL(RUN, download_run_appl, tls file download application),
	INST_PARM(tls file, tcm/ppc.tls, .tls file to download),
	INST_PARM(tls file, tcm/ram-lite.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	MENU_END();

menu_t file_mgr_config_menu =
	MENU_START(FILE MGR, 16)
	INST_APPL(RUN, download_run_appl, tls file download application),
	INST_PARM(tls file, tcm/a600-only.tls, .tls file to download),
	INST_PARM(tls file, tcm/a601-only.tls, .tls file to download),
	INST_PARM(tls file, tcm/a602-only.tls, .tls file to download),
	INST_PARM(tls file, tcm/a603-only.tls, .tls file to download),
	INST_PARM(tls file, tcm/a604-only.tls, .tls file to download),
	INST_PARM(tls file, tcm/a605-only.tls, .tls file to download),
	INST_PARM(tls file, tcm/a606-only.tls, .tls file to download),
	INST_PARM(tls file, tcm/a607-only.tls, .tls file to download),
	INST_PARM(tls file, tcm/a608-only.tls, .tls file to download),
	INST_PARM(tls file, tcm/a609-only.tls, .tls file to download),
	INST_PARM(tls file, tcm/a610-only.tls, .tls file to download),
	INST_PARM(tls file, tcm/a611-only.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	MENU_END();

menu_t file_mgr_edvt_menu =
	MENU_START(FILE MGR, 16)
	INST_APPL(RUN, download_run_appl, tls file download application),
	INST_PARM(tls file, tcm/a6-stress-1.tls, .tls file to download),
	INST_PARM(tls file, tcm/a6-stress-2.tls, .tls file to download),
	INST_PARM(tls file, tcm/mux.tls, .tls file to download),
	INST_PARM(tls file, tcm/hub.tls, .tls file to download),
	INST_PARM(tls file, tcm/a6-runchk.tls, .tls file to download),
	INST_PARM(tls file, tcm/ram.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	MENU_END();

menu_t file_mgr_a6ddr_menu =
	MENU_START(FILE MGR, 16)
	INST_APPL(RUN, download_run_appl, tls file download application),
	INST_PARM(tls file, tcm/tcm-reset.tls, .tls file to download),
	INST_PARM(tls file, tcm/a6-regs.tls, .tls file to download),
	INST_PARM(tls file, tcm/a600-intr.tls, .tls file to download),
	INST_PARM(tls file, tcm/a601-intr.tls, .tls file to download),
	INST_PARM(tls file, tcm/a602-intr.tls, .tls file to download),
	INST_PARM(tls file, tcm/a603-intr.tls, .tls file to download),
	INST_PARM(tls file, tcm/a604-intr.tls, .tls file to download),
	INST_PARM(tls file, tcm/a605-intr.tls, .tls file to download),
	INST_PARM(tls file, tcm/a606-intr.tls, .tls file to download),
	INST_PARM(tls file, tcm/a607-intr.tls, .tls file to download),
	INST_PARM(tls file, tcm/a608-intr.tls, .tls file to download),
	INST_PARM(tls file, tcm/a609-intr.tls, .tls file to download),
	INST_PARM(tls file, tcm/a610-intr.tls, .tls file to download),
	INST_PARM(tls file, tcm/a611-intr.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	MENU_END();

/*
	INST_PARM(tls file, default.tls, .tls file to download),
 */
menu_t file_mgr_fpga_menu =
	MENU_START(FILE MGR, 16)
	INST_APPL(RUN, download_run_appl, tls file download application),
	INST_PARM(tls file, tcm/ppc.tls, .tls file to download),
	INST_PARM(tls file, tcm/ram-lite.tls, .tls file to download),
	INST_PARM(tls file, tcm/i2c-init.tls, .tls file to download),
	INST_PARM(tls file, tcm/led-all.tls, .tls file to download),
	INST_PARM(tls file, tcm/tcm-reset.tls, .tls file to download),
	INST_PARM(tls file, tcm/fpga-init.tls, .tls file to download),
	INST_PARM(tls file, tcm/a600-tcmverify.tls, .tls file to download),
	INST_PARM(tls file, tcm/a601-tcmverify.tls, .tls file to download),
	INST_PARM(tls file, tcm/a602-tcmapp.tls, .tls file to download),
	INST_PARM(tls file, tcm/a604-tcmapp.tls, .tls file to download),
	INST_PARM(tls file, tcm/a606-tcmapp.tls, .tls file to download),
	INST_PARM(tls file, tcm/a608-tcmapp.tls, .tls file to download),
	INST_PARM(tls file, tcm/a610-tcmapp.tls, .tls file to download),
	INST_PARM(tls file, tcm/a6-dump.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	MENU_END();

menu_t file_mgr_uqam_menu =
	MENU_START(FILE MGR, 16)
	INST_APPL(RUN, download_run_appl, tls file download application),
	INST_PARM(tls file, tcm/ppc.tls, .tls file to download),
	INST_PARM(tls file, tcm/ram.tls, .tls file to download),
	INST_PARM(tls file, tcm/i2c-init.tls, .tls file to download),
	INST_PARM(tls file, tcm/led-all.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	MENU_END();

menu_t file_mgr_a6debug_menu =
	MENU_START(FILE MGR, 16)
	INST_APPL(RUN, download_run_appl, tls file download application),
	INST_PARM(tls file, tcm/i2c-init.tls, .tls file to download),
	INST_PARM(tls file, tcm/fpga-init.tls, .tls file to download),
	INST_PARM(tls file, tcm/a6-config-283.tls, .tls file to download),
	INST_PARM(tls file, tcm/a6-tcmapp-rev2.tls, .tls file to download),
	INST_PARM(tls file, tcm/a6-tcmapp-rev2.tls, .tls file to download),
	INST_PARM(tls file, tcm/i2c-init.tls, .tls file to download),
	INST_PARM(tls file, tcm/a6-config-283.tls, .tls file to download),
	INST_PARM(tls file, tcm/a6-tcmapp-rev2.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	MENU_END();

/*
 */
menu_t file_mgr_chan_menu =
	MENU_START(FILE MGR, 16)
	INST_APPL(RUN, download_run_appl, tls file download application),
	INST_PARM(tls file, tcm/ch00-reset.tls, .tls file to download),
	INST_PARM(tls file, tcm/ch01-reset.tls, .tls file to download),
	INST_PARM(tls file, tcm/ch02-reset.tls, .tls file to download),
	INST_PARM(tls file, tcm/ch03-reset.tls, .tls file to download),
	INST_PARM(tls file, tcm/ch04-reset.tls, .tls file to download),
	INST_PARM(tls file, tcm/ch05-reset.tls, .tls file to download),
	INST_PARM(tls file, tcm/ch06-reset.tls, .tls file to download),
	INST_PARM(tls file, tcm/ch07-reset.tls, .tls file to download),
	INST_PARM(tls file, tcm/ch08-reset.tls, .tls file to download),
	INST_PARM(tls file, tcm/ch09-reset.tls, .tls file to download),
	INST_PARM(tls file, tcm/ch10-reset.tls, .tls file to download),
	INST_PARM(tls file, tcm/ch11-reset.tls, .tls file to download),
	INST_PARM(tls file, tcm/ram.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	INST_PARM(tls file, default.tls, .tls file to download),
	MENU_END();

/*
 * CHANGE IN CONTROL MENU:
 *
 * This is the first change-in-control menu implementation.  The first and
 * second items in each of the menus have to be identical.  The ITEM_CTL
 * type is used to display only one of these menus.  They are together
 * mutually exclusive.
 */
menu_t tftp_menu =
	MENU_START(TFTP File, 10)
	INST_APPL(GO, fileio_tftp, Retrieve the tls files),
	INST_CTL(METHOD, &tftp_menu, download_appl, tftp, tftp xmodem kermit spi),
	INST_PARM(GATEWAY IP, 192.168.1.6, Specify gateway IP address),
	INST_PARM(NETMASK, 255.255.255.0, Specify netmask),
	INST_PARM(HOSTNAME, maple, Specify hostname),
	INST_PARM(ROOT PATH, /tftpboot, Specify tls file root path),
	INST_PARM(IP ADDR, 192.168.1.8, Specify host IP address),
	INST_PARM(SERVER IP, 192.168.1.6, Tftp server IP address),
	INST_PARM(DNS IP, 192.168.1.6, Specify DNS IP address),
	INST_PARM(NIS DOMAIN, Unknown, Specify NIS domain),
	MENU_END();

menu_t xmodem_menu =
	MENU_START(XMODEM File, 6)
	INST_APPL(GO, fileio_xmodem, Retrieve the tls files),
	INST_CTL(METHOD, &xmodem_menu, download_appl, tftp, tftp xmodem kermit spi),
	INST_PARM(MODE, xmodem, specify xmodem or ymodem),
	INST_PARM(CHAN, 0, specify channel),
	INST_PARM(OFFSET, 0, specify offset),
	INST_PARM(BAUD RATE, 0, specify baud rate),
	MENU_END();

/*
 * menu_t send_logs_menu =
 *	MENU_START(Send Logs, 3)
 *	INST_APPL(SEND FILE, fileio_upload, transmit the specified log files),
 *	INST_PARM(SERVER NAME, maple, specify server),
 *	INST_PARM(FILE PATH, /tftpboot/idslogs, specify path),
 *	MENU_END();
 */

/*
 * Limited to 16 test lists.
 */
menu_t list_menu =
	MENU_START(master-list, 0)
	MENU_END();

/*
 * Only use menu_add() to add items to the debug_list_menu.
 */
menu_t debug_list_menu =
	MENU_START(debug-list, 0)
	MENU_END();


#define MENUTAB_INIT(name) { extern void name##_init(void); name##_init(); }

/*
 * Runtime initialization of tables and pointers.
 *
 * For applications set the func pointer to the application routine.
 *
 * For menus set the func pointer to 'menu_input' and the menu pointer
 * to the appropriate menu.
 *
 * NOTE: When adding an INST_APPL up above don't forget to set function
 * pointer for the run time initialization below.
 */
void menutab_init(void)
{
#ifdef CONFIG_MPC8378TCM
	mode_menu.itemlist[0].func		= regress_run_appl;
	mode_menu.itemlist[1].func		= config_debug_appl;
	mode_menu.itemlist[2].func		= config_only_appl;
	mode_menu.itemlist[3].func		= config_regress_appl;
	mode_menu.itemlist[4].func		= config_regress_appl;
	mode_menu.itemlist[5].func		= config_edvt_appl;
	mode_menu.itemlist[6].func		= config_a6ddr_appl;
	mode_menu.itemlist[7].func		= config_fpga_appl;
	mode_menu.itemlist[8].func		= config_chan_appl;
	mode_menu.itemlist[9].func		= config_a6debug_appl;

	mode_menu.itemlist[10].func		= prog_bar_appl;
	mode_menu.itemlist[11].func		= rtc_appl;
	mode_menu.itemlist[12].func		= console_appl;
#elif CONFIG_MPC8378UQAM
	mode_menu.itemlist[0].func		= regress_uqam_appl;
	mode_menu.itemlist[1].func		= config_debug_appl;
	mode_menu.itemlist[2].func		= config_only_appl;
	mode_menu.itemlist[3].func		= config_uqam_appl;

	mode_menu.itemlist[4].func		= prog_bar_appl;
	mode_menu.itemlist[5].func		= rtc_appl;
#endif /* CONFIG_MPC8378XXXX */

	main_menu.itemlist[2].menu		= &mode_menu;
	main_menu.itemlist[2].func		= menu_input;

	execution_menu.itemlist[0].func		= test_go_appl;
	execution_menu.itemlist[1].func		= test_list_appl;
	execution_menu.itemlist[5].func		= menu_input;
	execution_menu.itemlist[5].menu		= &log_menu;
	execution_menu.itemlist[6].func		= menu_input;
	execution_menu.itemlist[6].func		= menu_del_appl;
	execution_menu.itemlist[7].func		= get_stats_appl;
	
	file_mgr_menu.itemlist[0].func		= download_run_appl;
	file_mgr_config_menu.itemlist[0].func	= download_run_appl;
	file_mgr_edvt_menu.itemlist[0].func	= download_run_appl;
	file_mgr_a6ddr_menu.itemlist[0].func	= download_run_appl;
	file_mgr_fpga_menu.itemlist[0].func	= download_run_appl;
	file_mgr_chan_menu.itemlist[0].func	= download_run_appl;
	file_mgr_a6debug_menu.itemlist[0].func	= download_run_appl;
	file_mgr_uqam_menu.itemlist[0].func	= download_run_appl;

	tftp_menu.itemlist[FILEIO_GO].func	= fileio_tftp;
	tftp_menu.itemlist[FILEIO_CTL].func	= download_appl;
	tftp_menu.itemlist[FILEIO_CTL].menu	= &tftp_menu;

	xmodem_menu.itemlist[FILEIO_GO].func	= fileio_xmodem;
	xmodem_menu.itemlist[FILEIO_CTL].func	= download_appl;
	xmodem_menu.itemlist[FILEIO_CTL].menu	= &xmodem_menu;

#ifdef COMMENT
	target_menu.itemlist[0].func		= menu_input;
	target_menu.itemlist[0].menu		= &tftp_menu;

	send_logs_menu.itemlist[0].func		= fileio_upload;
	send_logs_menu.itemlist[4].func		= fileio_dir;
#endif /* COMMENT */

	/*
	 * This is where static items are added to the
	 * BUILT-IN (formerly DEBUG) menu.
	 */
	MENUTAB_INIT(ids_nand);
 	MENUTAB_INIT(ram);
#ifdef COMMENT
#ifdef CONFIG_MPC8378TCM  
 	MENUTAB_INIT(tcm);
	MENUTAB_INIT(ids_nand);
#endif /* CONFIG_MPC8378TCM */
 	MENUTAB_INIT(ppc);
 	MENUTAB_INIT(i2c);
#endif /* COMMENT */
}

/*
 * From menutab.c: mode_menu.itemlist[4] ...
 *
 * INST_APPL(BUILT-IN TESTS, config_debug_appl, debug mode),
 *
 * menu_list_appl() will execute menu_input(&debug_list_menu) to execute
 * MENUTAB_INIT(tcm); macro added to the debug_list_menu.  This is a static
 * compile time menu.
 * 
 */
void config_debug_appl(menu_t *p)
{
	set_ids_mode("debug");
	menu_list_appl(p);		/* menu_input(&debug_list_menu); */
}

/*
 * From menutab.c: mode_menu.itemlist[4] ...
 *
 * INST_APPL(INSTALLED TESTS, config_regress_appl, regress mode),
 *
 * menu_list_appl() will execute menu_input(&list_menu) to execute
 * tftp downloaded .tls files converted into IDS menus.
 */
void config_only_appl(menu_t *p)
{
	set_ids_mode("config");
	menu_input(&file_mgr_config_menu);
	menu_list_appl(p);			/* menu_input(&list_menu); */
}

void config_regress_appl(menu_t *p)
{
	set_ids_mode("default");
	menu_input(&file_mgr_menu);
	menu_list_appl(p);			/* menu_input(&list_menu); */
}

void config_edvt_appl(menu_t *p)
{
	set_ids_mode("default");
	menu_input(&file_mgr_edvt_menu);
	menu_list_appl(p);
}

void config_fpga_appl(menu_t *p)
{
	set_ids_mode("default");
	menu_input(&file_mgr_fpga_menu);
	menu_list_appl(p);			/* menu_input(&list_menu); */
}

void config_uqam_appl(menu_t *p)
{
	set_ids_mode("default");
	menu_input(&file_mgr_uqam_menu);
	menu_list_appl(p);			/* menu_input(&list_menu); */
}

void config_chan_appl(menu_t *p)
{
	set_ids_mode("default");
	menu_input(&file_mgr_chan_menu);
	menu_list_appl(p);			/* menu_input(&list_menu); */
}

void config_a6ddr_appl(menu_t *p)
{
	set_ids_mode("default");
	menu_input(&file_mgr_a6ddr_menu);
	menu_list_appl(p);
}

void config_a6debug_appl(menu_t *p)
{
	set_ids_mode("default");
	menu_input(&file_mgr_a6debug_menu);
	menu_list_appl(p);			/* menu_input(&list_menu); */
}

#ifdef COMMENT
void config_download_appl(menu_t *p)
{
	set_ids_mode("default");
	menu_input(&main_menu);
}

void config_manufac_appl(menu_t *p)
{
	set_ids_mode("default");
	menu_input(&main_menu);
}

void config_field_appl(menu_t *p)
{
	set_ids_mode("default");
	menu_input(&main_menu);
}
#endif /* COMMENT */

void rtc_appl(menu_t *p)
{
#if defined(CONFIG_MPC8378TCM)
	struct rtc_time tmp;

	rtc_get(&tmp);
#endif
	(void)getchar();
}

void console_appl(menu_t *p)
{
	set_ids_mode("default");
	menu_list_appl(p);			/* menu_input(&list_menu); */
}
