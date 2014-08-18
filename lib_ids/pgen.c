/*
 * Packet Generator
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
 * Manage a list of allocated packets and frames of various types.
 * Create packets and frames based upon caller supplied specifications.
 *
 * Copyright(c) 2009 Anacaz Networks Inc., ALL RIGHTS RESERVED
 *
 * rfa 090608
 */
#include <common.h>
#include <net.h>
#include <malloc.h>

typedef Ethernet_t eth_t;
typedef IP_t ipv4_t;

typedef struct udp
{
	ushort		udp_src;	/* UDP source port		*/
	ushort		udp_dst;	/* UDP destination port		*/
	ushort		udp_len;	/* Length of UDP packet		*/
	ushort		udp_xsum;	/* Checksum			*/
} udp_t;

/*
 * BUG!!!  Not correct.
 */
typedef struct tcp
{
	ushort		tcp_src;	/* TCP source port		*/
	ushort		tcp_dst;	/* TCP destination port		*/
	ushort		tcp_len;	/* Length of TCP packet		*/
	ushort		tcp_xsum;	/* Checksum			*/
} tcp_t;


void *pgen_alloc(int size)
{
	return(malloc(size));
}

void pgen_free(void *frame)
{
	free(frame);
}

eth_t *pgen_eth(void *frame, unsigned char *dest, unsigned char *src)
{
	eth_t *eth = (eth_t *)frame;

	if (!frame)
		return(0);
	memcpy(eth->et_dest, dest, sizeof(eth->et_dest));
	memcpy(eth->et_src, src, sizeof(eth->et_src));
	return(eth);
}

/*
 * ip = pgen_ip(pgen_eth(pgen_alloc(1500), dest, src), src, dst);
 */
ipv4_t *pgen_ip(eth_t *eth, IPaddr_t sip, IPaddr_t dip)
{
	ipv4_t *ip = (ipv4_t *)&eth[1];


	if (!eth)
		return(0);

	eth->et_prot = PROT_IP;

	memcpy(&ip->ip_src, &sip, sizeof(ip->ip_src));
	memcpy(&ip->ip_dst, &dip, sizeof(ip->ip_dst));

	return(ip);
}

/*
 * Usage:
 * udp = pgen_udp(pgen_ip(pgen_eth(pgen_alloc(540),
 *		dmac, smac), sip, dip), sport, dport);
 */
udp_t *pgen_udp(ipv4_t *ip, unsigned short src, unsigned short dst)
{
	udp_t *udp = (udp_t *)&((unsigned char *)ip)[IP_HDR_SIZE_NO_UDP];

	if (!ip)
		return(0);

	memcpy(&udp->udp_src, &src, sizeof(udp->udp_src));
	memcpy(&udp->udp_dst, &dst, sizeof(udp->udp_dst));

	return(udp);
}

tcp_t *pgen_tcp(ipv4_t *ip)
{
	return(0);
}

void *pgen_icmp(void)
{
	return(0);
}

void *pgen_ping(void)
{
	return(0);
}
