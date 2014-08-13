/*
 * Integrated Diagnostics Subsystem
 *
 * rfa - 060804-0912-1009
 */
#ifdef LINUX
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#else /* LINUX */
//#include <common.h>
#include <linux/ctype.h>
#include <idsio.h>
#include <net.h>
#endif /* LINUX */
#include <idsfs.h>
#include <ids_jmpvec.h>
#include <menu.h>
#include <test_api.h>

#ifdef COMMENT
typedef struct packet
{
	union
	{
		Ethernet_t	*eth;
		VLAN_Ethernet_t	*vlan;
	} l2;
} packet_t;
#endif /* COMMENT */

/*
 * Release ethernet frame previously allocated by a call to pgen_eth()
 */
void pgen_free(Ethernet_t *p)
{
#ifndef LINUX
	extern void free(void *);
#endif /* !LINUX */
	free((unsigned char *)p);
}

/*
 * Allocate an ethernet frame and return a referrence back to the caller.
 */
Ethernet_t *pgen_eth(unsigned char *dest, unsigned char *src,
   unsigned short len)
{
#ifndef LINUX
	extern unsigned char *malloc(int);
#endif /* !LINUX */
	Ethernet_t *packet;
	int n;

	if (!(packet = (Ethernet_t *)malloc(2000)))
		return(0);

	for (n = 0; n < 6; ++n)
	{
		packet->et_dest[n] = dest[n];
		packet->et_src[n]  = src[n];
	}
	if (len)
		packet->et_protlen = len;
	return(packet);
}

VLAN_Ethernet_t *pgen_vlan(unsigned char *dest, unsigned char *src,
   unsigned short tag)
{
	VLAN_Ethernet_t *packet;

	if (!(packet = (VLAN_Ethernet_t *)pgen_eth(dest, src,
	    VLAN_ETHER_HDR_SIZE)))
		return(0);

	packet->vet_tag  = tag;

	return(packet);
}


IP_t *pgen_ip(Ethernet_t *packet, unsigned short prot, unsigned long src,
   unsigned long dest)
{
	IP_t *iphdr;

	if (packet->et_protlen == E802_HDR_SIZE)
	{
		packet->et_prot = PROT_IP;
		iphdr = (IP_t *)&((unsigned char *)packet)[packet->et_protlen];
	}
	else if (packet->et_protlen == VLAN_ETHER_HDR_SIZE)
	{
		((VLAN_Ethernet_t *)packet)->vet_type = PROT_IP;
		iphdr = (IP_t *)&((unsigned char *)packet)[packet->et_protlen];
	}
	else
	{
		packet->et_protlen = PROT_IP;
		iphdr = (IP_t *)&((unsigned char *)packet)[ETHER_HDR_SIZE];
	}

	iphdr->ip_p   = prot;
	iphdr->ip_src = src;
	iphdr->ip_dst = dest;

	return(iphdr);
}

#ifdef COMMENT
UDP_t *pgen_udp(IP_t *iphdr, unsigned short src, unsigned short dest,
   unsigned short len)
{
	UDP_t *udphdr;

	return(udphdr);
}

TCP_t *pgen_tcp(IP_t *iphdr, unsigned short src, unsigned short dest,
   unsigned short len)
{
	TCP_t *tcphdr;

	return(tcphdr);
}
#endif /* COMMENT */

#ifdef COMMENT
void pgen_test()
{
}
#endif /* COMMENT */
