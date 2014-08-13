#ifndef _lbus_h
#define	_lbus_h
/*
 *
 */
/*
 * LBC Base Registers 0 - 7 bit definitions as left shifts
 * 	0x0_5000, 0x0_5008, 0x0_5010, 0x0_5018,
 * 	0x0_5020, 0x0_5028, 0x0_5030, 0x0_5038
 */
#define	LBC_BR_BA_LSHFT		31
#define	LBC_BR_XBA_LSHFT	14
#define	LBC_BR_PS_LSHFT		12
#define	LBC_BR_DECC_LSHFT	10
#define	LBC_BR_WP_LSHFT		8
#define	LBC_BR_MSEL_LSHFT	7
#define	LBC_BR_ATOM_LSHFT	3

#define	LBC_BR_VALID_BIT	1

/*
 * LBC Option Registers 0 - 7 bit definitions as left shifts
 * 	0x0_5004, 0x0_500C, 0x0_5014, 0x0_501C,
 * 	0x0_5024, 0x0_502C, 0x0_5034, 0x0_503C
 */
#define	LBC_OR_AM_LSHFT		31
#define	LBC_OR_XAM_LSHFT	14
#define	LBC_OR_BCTLD_LSHFT	12
#define LBC_OR_CSNT_LSHFT	11
#define LBC_OR_ACS_LSHFT	10
#define LBC_OR_XACS_LSHFT	8
#define LBC_OR_SCY_LSHFT	7
#define LBC_OR_SETA_LSHFT	3
#define LBC_OR_TRLX_LSHFT	2
#define LBC_OR_EHTR_LSHFT	1

#define LBC_OR_EAD_BIT		1

/*
 * LBC Configuration Register bit definitions as left shifts
 * 	0x0_50D0
 */
#define	LBC_LBCR_LDIS_LSHFT	31
#define	LBC_LBCR_BTCLC_LSHFT	23
#define	LBC_LBCR_AHD_LSHFT	21
#define	LBC_LBCR_LBPSE_LSHFT	17
#define	LBC_LBCR_EPAR_LSHFT	16
#define	LBC_LBCR_BMT_LSHFT	15

/*
 * LBC Clock Ratio Register bit definitions as left shifts
 * 	0x0_50D4
 */
#define	LBC_LCRR_PBYP_LSHFT	31
#define	LBC_LCRR_BUFCMDC_LSHFT	29
#define	LBC_LCRR_ECL_LSHFT	25
#define	LBC_LCRR_EADC_LSHFT	17
#define	LBC_LCRR_CLKDIV_LSHFT	3

#define LBUS_PAGE_SIZE		0x1000
#define LBUS_START(X)		(0x80000000 + (LBUS_PAGE_SIZE * (X)))
#define LBUS_MUX_START()	LBUS_START(0)
#define LBUS_HUB_START()	LBUS_START(1)
#define LBUS_CPLD_START()	LBUS_START(2)
#define LBUS_A6_START()		LBUS_START(3)
#define LBUS_A6_SIZE		0x10
#define LBUS_A6(X)		(LBUS_A6_START() + (LBUS_A6_SIZE * (X)))

typedef struct a6_hif
{

#define A6_AUTO_INCR	0x00000001	/* Addr autoinc 0: no inc, 1: inc */
#define A6_RDY_DELAY	0x00000002	/* Delay hif_rdy 1 hclk read data */
#define A6_ERROR_STAT	0x00000004	/* Error flag when accessing prot */
#define A6_BURST_DIS	0x00000008	/* burst_disable Avoid to prefetch */
#define A6_HIF_GPR	0x00000FF0	/* HIF general purpose bits */
#define A6_ARM_INT2	0x00001000	/* ARM interrupt #2 */
#define A6_ARM_INT1	0x00002000	/* ARM interrupt #1 */
#define A6_ARM_RESET	0x00004000	/* ARM reset 1: reset, 0: none */
#define A6_CHIP_RESET	0x00008000	/* Full chip reset 1: reset, 0: none */
	unsigned long	hif_ctl;
	unsigned long	hif_addr;	/* HIF addr reg LSB [1:0] read only */
	unsigned long	hif_data;	/* HIF data register */
} a6_hif_t;

#endif /* !_lbus_h */
