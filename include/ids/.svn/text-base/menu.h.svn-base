#ifndef _menu_h
#define _menu_h
/*
 * Integrated Diagnostics Subsystem
 *
 * rfa - 060718-19-20-24-25-26-27-090518-31-0617-0701
 */

void exit(int);
#ifndef LINUX
#define strtol		simple_strtol
#endif /* !LINUX */

/*
 * Defined struct item.type types.
 */
#define ITEM_MENU	1	/* Item is another menu */
#define ITEM_LIST	2	/* Item is a .tls test list */
#define ITEM_EXIT	3	/* Item exits */
#define ITEM_APPL	4	/* Item is an applicaiton */
#define ITEM_TEST	5	/* Item is a .ids test */
#define ITEM_PARM	6	/* Item is an ascii parameter */
#define ITEM_CNTR	7	/* Item is a counter */
#define ITEM_TIME	8	/* Item is a clock or timer */
#define ITEM_NAME	9	/* Item is a name */
#define ITEM_LOG	11	/* Item is a log */
#define ITEM_REG	12	/* Item is an addressable hw register */
#define ITEM_CTL	13	/* Item causes a change in control */
#define ITEM_SVC	14	/* Item is an IDS SERVICE */
#define ITEM_DATE	15	/* Item is a date */

#define INST_BLANK(nam) { #nam, 0 }
#define INST_MENU(nam,m,help) \
{ #nam, ITEM_MENU, (m), menu_input, { "\0" }, { #help } }
#define INST_LIST(nam,m,help) \
{ #nam, ITEM_LIST, (m), menu_input, { "\0" }, { #help } }
#define INST_EXIT(nam) \
{ #nam, ITEM_EXIT, 0, (void (*)(menu_t *))exit }
#define INST_APPL(nam,appl,help) \
{ #nam, ITEM_APPL, 0, (void (*)(menu_t *))(appl), { "\0" }, { #help } }
#define INST_DBG_APPL(nam,m,appl,help) \
{ #nam, ITEM_APPL, (m), (void (*)(menu_t *))(appl), { "\0" }, { #help } }
/*
 * ITEM_TEST can only be created by the dynamic menu builder.  This macro
 * is not used for compile time instantiation but rather for documention
 * purposes only.
#define INST_TEST(nam,header) \
{ #nam, ITEM_TEST, 0, (void (*)(menu_t *))(header) }
 */
#define INST_PARM(nam,value,help) \
{ #nam, ITEM_PARM, 0, (void (*)(menu_t *))0, { #value }, { #help } }
#define INST_CNTR(nam,value) \
{ #nam, ITEM_CNTR, 0, (void (*)(menu_t *))0, { #value } }
#define INST_TIME(nam,help) \
{ #nam, ITEM_TIME, 0, (void (*)(menu_t *))0, { "\0" }, { #help } }
#define INST_NAME(nam) \
{ #nam, ITEM_NAME, 0, (void (*)(menu_t *))0 }
#define INST_REG(nam,help) \
{ #nam, ITEM_REG,  0, (void (*)(menu_t *))0, { "\0" }, { #help } }
#define INST_CTL(nam,m,appl,value,help) \
{ #nam, ITEM_CTL, (m), (void (*)(menu_t *))(appl), { #value }, { #help } }
#define INST_SVC(nam,m,appl,help) \
{ #nam, ITEM_SVC, (m), (void (*)(menu_t *))(appl), { "\0" }, { #help } }
#define INST_DATE(nam,help) \
{ #nam, ITEM_DATE, 0, (void (*)(menu_t *))0, { "\0" }, { #help } }

/*
 * PROPOSED:
 *
 * Ram carve menu indicies and macros.
 * 00000000 - 1FFFFFFF -- u-boot
 * 00200000 - 00200FFF -- u-boot-vectors
 * 00201000 - 0023FFFF -- menupool
 * 00240000 - 002FFFFF -- log-files
 * 00300000 - 15FFFFFF -- scratch-ram
 * 16000000 - 16FFFFDF -- tls-download
 * 16FFFFE0 - 1FFFFFFF -- u-boot-stack
 */
#define RC_UBOOT_INDEX		0
#define RC_MENUPOOL_INDEX	1
#define RC_LOGFILES_INDEX	2
#define RC_MEMORY_INDEX		3
#define RC_TLSFILES_INDEX	4

#define RC(name)	RC_##name##_INDEX
#define GET_RAM(name) (unsigned long)strtol(ram_carve_menu.itemlist[RC_##name##_INDEX].data.cache, (char **)0, 0)
#define DIR_TBL(name)	dir_tbl[RC(name)]

/*
 * Pointers into the TLS download region.
 */
typedef struct idsfs
{
	list_hdr_t	*tls;
	test_hdr_t	*ids;
} idsfs_t;

typedef struct hwreg
{
	char		addr	[32];
	char		mask	[32];
	char		data	[32];
} hwreg_t;

typedef struct idstime
{
	char		month	[4];
	char		day	[4];
	char		year	[8];
	char		hour	[4];
	char		minute	[4];
	char		second	[4];
	time_t		lasttime;
	time_t		currtime;
} idstime_t;

/*
 * This structure describes one item within the a list of items that
 * comprise a menu.
 */
typedef struct item
{
	char		name[32];		/* Item display name */
	int		type;			/* Item type */
	struct menu	*menu;
	void		(*func)(struct menu *);
	union
	{
		char		cache[128];	/* Item data */
		idsfs_t		idsfs;		/* Overloaded: tls/ids ptrs */
		hwreg_t		reg;		/* Register item data */
		idstime_t	time;		/* IDS time */
	} data;
	char		help[128];		/* Help message */
	// unsigned long		eol;		/* 0x34567890 */
} item_t;

/*
 * This structure describs one menu within the IDS framework.
 */
typedef struct menu
{
	char		name[32];
	int		itemcount;
	// item_t		itemlist;
	item_t		itemlist[16];
} menu_t;

#define MENU_START(name,count)	{ #name, (count), {
#define MENU_END()		} }

extern menu_t mode_menu;
extern menu_t main_menu;
extern menu_t execution_menu;
extern menu_t file_mgr_menu;
extern menu_t file_mgr_config_menu;
extern menu_t file_mgr_edvt_menu;
extern menu_t file_mgr_a6ddr_menu;
extern menu_t file_mgr_fpga_menu;
extern menu_t file_mgr_uqam_menu;
extern menu_t file_mgr_a6debug_menu;
extern menu_t file_mgr_chan_menu;
extern menu_t tftp_menu;
extern menu_t xmodem_menu;
// extern menu_t target_menu;
extern menu_t upload_menu;
extern menu_t log_menu;
extern menu_t send_logs_menu;
extern menu_t list_menu;
extern menu_t ram_carve_menu;
extern menu_t debug_list_menu;

extern menu_t *this_menu;

/*
 * EXECUTION MENU: menutab.c
 */
#define EXEC_GO			0
#define EXEC_SHOW		1
#define EXEC_NAME		2
#define EXEC_CYCLES		3
#define EXEC_TIME		4
#define EXEC_VIEW		5
#define EXEC_CLEAR		6

/*
 * FILE MGR Menu: fileio.c
 */
#ifdef COMMENT
#define FILEIO_GET		0
#define FILEIO_PUT		1
#define FILEIO_ELF		2
#define FILEIO_VIEW		3
#define FILEIO_SHOW		4
#define FILEIO_FILENAME		5
#define FILEIO_LOADADDR		6
#endif /* COMMENT */

/*
 * TFTP Menu: fileio.c
 */
#define FILEIO_GO		0
#define FILEIO_CTL		1	/* Formerly FILEIO_GET up above */
#define FILEIO_GATEWAY		2
#define FILEIO_NETMASK		3
#define FILEIO_HOSTNAME		4
#define FILEIO_ROOTPATH		5
#define FILEIO_IPADDR		6
#define FILEIO_SERVER		7
#define FILEIO_DNS		8
#define FILEIO_DOMAIN		9

void menu_add(menu_t *p, item_t *item, menu_t *menu);

/*
 * MENU menu.c
 */
int set_ids_mode(const char *string);
void get_ids_mode(char *buffer);

int menu_show_reg(menu_t *menu);
void menu_input(menu_t *p);			/* MENU main */
void list_input(menu_t *p);			/* LIST main */

/*
 * vector related defines
 *
 * individual vectors are defined in u-boot/diags/tests/include/ids_jmvvec.h
 *
 */
void menu_input(menu_t *p);
menu_t *item_input(menu_t *p, int index);
void download_appl(menu_t *p);
void download_run_appl(menu_t *p);
/*
 * UQAM Applicaitons.
 */
void regress_uqam_appl(menu_t *p);

/*
 * TCM Applications.
 */
void regress_run_appl(menu_t *p);
void config_debug_appl(menu_t *p);
void config_download_appl(menu_t *p);
void config_only_appl(menu_t *p);
void config_regress_appl(menu_t *p);
void config_fpga_appl(menu_t *p);
void config_uqam_appl(menu_t *p);
void config_a6debug_appl(menu_t *p);
void config_chan_appl(menu_t *p);
void config_edvt_appl(menu_t *p);
void config_a6ddr_appl(menu_t *p);

void config_manufac_appl(menu_t *p);
void config_field_appl(menu_t *p);
void menu_list_appl(menu_t *p);
void menu_erase_appl(menu_t *p);
void menu_show_appl(menu_t *p);
item_t *find_item(menu_t *p, const char *s);
void menu_del_appl(menu_t *p);
void test_execution_appl(menu_t *p);
void test_go_appl(menu_t *p);
void test_list_appl(menu_t *p);
char *get_listname_appl(menu_t *p);
char *get_testname_appl(menu_t *p);
void prog_bar_appl(menu_t *p);
void menu_add_appl(menu_t *menup, item_t *itemp);
void download_appl(menu_t *p);
void config_debug_appl(menu_t *p);
void menu_list_appl(menu_t *p);
void get_stats_appl(menu_t *p);			/* Display tests stats */
int exec_pause(void);
void (*menu_update_svc(item_t *itemp))(menu_t *);
int menu_update_time(menu_t *);

/*
 * From fileio.c
 */
void fileio_tftp(menu_t *p);
void fileio_xmodem(menu_t *p);
void fileio_upload(menu_t *p);
void fileio_tftp_svc(menu_t *p);
void fileio_spi_svc(menu_t *p);

/*
 * From flash.c
 */
void ids_flash_tls_load(menu_t *p);

#endif /* !_menu_h */
