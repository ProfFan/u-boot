/* SPDX-License-Identifier: GPL-2.0 */
/*
 *	Copied from Linux Monitor (LiMon) - Networking.
 *
 *	Copyright 1994 - 2000 Neil Russell.
 *	(See License)
 *	Copyright 2000 Roland Borde
 *	Copyright 2000 Paolo Scaffardi
 *	Copyright 2000-2002 Wolfgang Denk, wd@denx.de
 */

#ifndef __PWN_H__
#define __PWN_H__

#include <common.h>
#include <net.h>

/*
 * Initialize pwn (beginning of netloop)
 */
void pwn_start(void);

/*
 * Deal with the receipt of a pwn packet
 *
 * @param et Ethernet header in packet
 * @param ip IP header in the same packet
 * @param len Packet length
 */
void pwn_receive(struct ethernet_hdr *et, struct ip_udp_hdr *ip, int len);

#endif /* __PWN_H__ */
