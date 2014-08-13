#ifndef _ids_jmpvec_h
#define _ids_jmpvec_h
/*
 * Integrated Diagnostics Subsystem
 *
 * u-boot jump vector interface file.
 *
 * rfa - 061102-090506-15
 */

/*
 * vector related defines
 */
#define u_boot_vectors	((void (**)(void))0x00200000)	/* ram carved address */

/*
 * This is included for information.  Not actually used in IDS...only in tests
 */
#define NETSENDPACKET(pkt,len)	(*((void (**)(volatile uchar *, int))u_boot_vectors)[0])(pkt,len)
#define NETSETHANDLER(f)	(*((void (**)(rxhand_f *))u_boot_vectors)[1])(f)
#define ETH_GET_DEV()		(*((struct eth_dev *(**)(void))u_boot_vectors)[2])()
#define ETH_RX()		(*((int (**)(void))u_boot_vectors)[3])()
#define ETH_SET_CURRENT()	(*((void (**)(void))u_boot_vectors)[4])()
#define ETH_HALT()		(*((void (**)(void))u_boot_vectors)[5])()
#define ETH_INIT(bis)		(*((int (**)(bd_t *))u_boot_vectors)[6])(bis)
#define GET_TBCLK()		(*((unsigned long (**)(void))u_boot_vectors)[7])()
#define UDELAY(usec)		(*((void (**)(unsigned long))u_boot_vectors)[8])(usec)
#define GET_TIMER(base)		(*((unsigned long (**)(unsigned long))u_boot_vectors)[9])(base)
#define GETENV(name)		(*((char * (**)(char *))u_boot_vectors)[10])(name)
#define SETENV(vname,value)	(*((void (**)(char *, char *))u_boot_vectors)[11])(vname,value)
#define WAIT_TICKS(ticks)	(*((void (**)(unsigned long))u_boot_vectors)[12])(ticks)
#define A6_GET(base,addr)	(*((unsigned long (**)(unsigned long, unsigned long))u_boot_vectors)[13])(base,addr)
#define A6_PUT(base,addr,value)	(*((void (**)(unsigned long, unsigned long, unsigned long))u_boot_vectors)[14])(base,addr,value)
#define MUX_GET(addr)		(*((unsigned long (**)(unsigned long))u_boot_vectors)[15])(addr)
#define MUX_AGET(addr)		(*((unsigned long (**)(unsigned long))u_boot_vectors)[16])(addr)
#define MUX_PUT(addr,value)	(*((void (**)(unsigned long, unsigned long))u_boot_vectors)[17])(addr,value)
#define MUX_APUT(addr,value)	(*((void (**)(unsigned long, unsigned long))u_boot_vectors)[18])(addr,value)

/*
 * Function Prototypes
 */
#ifdef CONFIG_SERIAL_SOFTWARE_FIFO
#define SERIAL_BUFFERED_INIT()	(*((void (**)(void))u_boot_vectors)[20])(void)
#define SERIAL_BUFFERED_PUTC(x) (*((void (**)(const char))u_boot_vectors)[21])(x)
#define SERIAL_BUFFERED_PUTS(x) (*((void (**)(const char *))u_boot_vectors)[22])(x)
#define SERIAL_BUFFERED_GETC()	(*((int (**)(void))u_boot_vectors)[23])()
#define SERIAL_BUFFERED_TSTC()	(*((int (**)(void))u_boot_vectors)[24])()
#endif /* CONFIG_SERIAL_SOFTWARE_FIFO */

#define HANG()			(*((void (**)(void))u_boot_vectors)[30])()
#define INITDRAM(x)		(*((long int (**)(int))u_boot_vectors)[31])(x)
#define DISPLAY_OPTIONS()	(*((int (**)(void))u_boot_vectors)[32])()
#define PRINT_SIZE(x,z)		(*((void (**)(ulong, const char *))u_boot_vectors)[33])(x,z)
#define PRINT_BUFFER(a,d,w,c,l) (*((int (**)(ulong, void *, uint, uint, uint))u_boot_vectors)[34])(a,d,w,c,l)

/* common/main.c */
#define MAIN_LOOP() \
(*((void (**)(void))u_boot_vectors)[40])(void)
#define RUN_COMMAND(x,z) \
(*((int (**)(const char *, int))u_boot_vectors)[41])(x,z)
#define READLINE(x) \
(*((int (**)(const char *const))u_boot_vectors)[42])(x)
#define INIT_CMD_TIMEOUT() \
(*((void (**)(void))u_boot_vectors)[43])()
#define RESET_CMD_TIMEOUT() \
(*((void (**)(void))u_boot_vectors)[44])()

/* lib_$(ARCH)/board.c */
#define BOARD_INIT_F(x) \
(*((void (**)(ulong))u_boot_vectors)[50])(x)
#define BOARD_INIT_R(g,x) \
(*((void (**)(gd_t *, ulong))u_boot_vectors)[51])(g,x)
#define CHECKBOARD() \
(*((int (**)(void))u_boot_vectors)[52])()
#define CHECKFLASH() \
(*((int (**)(void))u_boot_vectors)[53])()
#define CHECKDRAM() \
(*((int (**)(void))u_boot_vectors)[54])()
#define STRMHZ(x,z) \
(*((int (**)(char *, long))u_boot_vectors)[55])(x,z)
#define LAST_STAGE_INIT() \
(*((int (**)(void))u_boot_vectors)[56])()
#ifdef CFG_ID_EEPROM
#define MAC_READ_FROM_EEPROM() \
(*((int (**)(void))u_boot_vectors)[57])()
#endif

/* common/flash.c */
#define FLASH_PERROR(x) \
(*((void (**)(int))u_boot_vectors)[60])(x)

/* common/cmd_autoscript.c */
#define AUTOSCRIPT(x) \
(*((int (**)(ulong))u_boot_vectors)[61])(x)

/* common/cmd_bootm.c */
#define PRINT_IMAGE_HDR(x) \
(*((void (**)(image_header_t *))u_boot_vectors)[62])(x)

//#define PRINTF(fmt, args...)
#ifdef IDS_SA
#define printf \
(*((void (**)(char *, ...))u_boot_vectors)[63])
#define sprintf \
(*((void (**)(char *, char *, ...))u_boot_vectors)[71])
#endif /* IDS_SA */

#define MB_START \
(*((int (*(**)(int (*)(mb_op_enum_t, ...)))(mb_op_enum_t, ...))u_boot_vectors)[73])

#define MEMCPY(t,f,z) \
(*((void *(**)(void *, const void *, __kernel_size_t))u_boot_vectors)[64])(t,f,z)
#define STRTOL(t,f,z) \
(*((long (**)(const char *, char **, unsigned int))u_boot_vectors)[65])(t,f,z)

#define SPATGEN(x,y,z) \
(*((pvalue_t (**)(pvalue_t, pvalue_t, pmod_t))u_boot_vectors)[66])(x,y,z)

#define PATGEN(x,y,z) \
(*((pvalue_t (**)(pvalue_t, pvalue_t, pmod_t))u_boot_vectors)[67])(x,y,z)

#define STRCMP(s1,s2) \
(*((int (**)(const char *, const char *))u_boot_vectors)[68])(s1, s2)

#define MODGEN(x) \
(*((pmod_t (**)(char *))u_boot_vectors)[69])(x)

#define IDS_PEEK(n,v) \
(*((int (**)(int, char **))u_boot_vectors)[70])(n,v)

#define STRLEN(s) \
(*((int (**)(const char *))u_boot_vectors)[72])(s)

#define FILEIO_TFTP_SVC(p) \
(*((void (**)(void *))u_boot_vectors)[74])(p)

#define I2C_INIT(speed,slaveadd) \
(*((void (**)(int,int))u_boot_vectors)[80])(speed,slaveadd)

#define I2C_PROBE(chip) \
(*((void (**)(unsigned char))u_boot_vectors)[81])(chip)

#define I2C_SET_BUS_NUM(bus) \
(*((int (**)(unsigned int))u_boot_vectors)[82])(bus)

#define I2C_GET_BUS_NUM() \
(*((unsigned int (**)(void))u_boot_vectors)[83])()

#define I2C_SET_BUS_SPEED(speed) \
(*((int (**)(unsigned int))u_boot_vectors)[84])(speed)

#define I2C_GET_BUS_SPEED() \
(*((unsigned int (**)(void))u_boot_vectors)[85])()

#define I2C_REG_READ(a,r) \
(*((unsigned char (**)(unsigned char, unsigned char))u_boot_vectors)[86])(a,r)

#define I2C_REG_WRITE(a,r,v) \
(*((void (**)(unsigned char, unsigned char, unsigned char))u_boot_vectors)[87])(a,r,v)

#define I2C_READ(d,a,l,b,z) \
(*((int (**)(unsigned char, unsigned int, int, unsigned char *, int))u_boot_vectors)[88])(d,a,l,b,z)

#define I2C_WRITE(d,a,l,b,z) \
(*((int (**)(unsigned char, unsigned int, int, unsigned char *, int))u_boot_vectors)[89])(d,a,l,b,z)

#ifndef LINUX
#define strtol          simple_strtol
#endif /* !LINUX */

#endif /* !_ids_jmpvec_h */
