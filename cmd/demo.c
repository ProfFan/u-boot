// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2013 Google, Inc
 *
 * (C) Copyright 2012
 * Pavel Herrmann <morpheus.ibis@gmail.com>
 */

#include <common.h>
#include <dm-demo.h>
#include <mapmem.h>
#include <asm/io.h>
#include <net.h>

struct udevice *demo_dev;

static int do_demo_hello(cmd_tbl_t *cmdtp, int flag, int argc,
			 char * const argv[])
{
	int ch = 0;

	if (argc)
		ch = *argv[0];

	return demo_hello(demo_dev, ch);
}

static int do_demo_status(cmd_tbl_t *cmdtp, int flag, int argc,
			  char * const argv[])
{
	int status;
	int ret;

	ret = demo_status(demo_dev, &status);
	if (ret)
		return ret;

	printf("Status: %d\n", status);

	return 0;
}

static int do_demo_light(cmd_tbl_t *cmdtp, int flag, int argc,
			 char * const argv[])
{
	int light;
	int ret;

	if (argc) {
		light = simple_strtoul(argv[0], NULL, 16);
		ret = demo_set_light(demo_dev, light);
	} else {
		ret = demo_get_light(demo_dev);
		if (ret >= 0) {
			printf("Light: %x\n", ret);
			ret = 0;
		}
	}

	return ret;
}

int do_demo_list(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct udevice *dev;
	int i, ret;

	puts("Demo uclass entries:\n");

	for (i = 0, ret = uclass_first_device(UCLASS_DEMO, &dev);
	     dev;
	     ret = uclass_next_device(&dev)) {
		printf("entry %d - instance %08x, ops %08x, platdata %08x\n",
		       i++, (uint)map_to_sysmem(dev),
		       (uint)map_to_sysmem(dev->driver->ops),
		       (uint)map_to_sysmem(dev_get_platdata(dev)));
	}

	return cmd_process_error(cmdtp, ret);
}

int do_demo_net(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct udevice *dev;
	int i, ret;

	puts("VIRTIO uclass entries:\n");
	int devnum = 0;
	struct udevice *net_dev;
	ret = uclass_get_device(UCLASS_VIRTIO, devnum, &net_dev);
	printf("getting devices %d\n", ret);
	for (i = 0, ret = uclass_first_device(UCLASS_VIRTIO, &dev);
	     dev;
	     ret = uclass_next_device(&dev)) {
		printf("entry %d - instance %08x, ops %08x, platdata %08x\n",
		       i++, (uint)map_to_sysmem(dev),
		       (uint)map_to_sysmem(dev->driver->ops),
		       (uint)map_to_sysmem(dev_get_platdata(dev)));
	}

	if (ret == 0) {
		ret = uclass_get_device(UCLASS_ETH, devnum, &net_dev);

		printf("getting devices %d\n", ret);
		for (i = 0, ret = uclass_first_device(UCLASS_ETH, &dev);
			dev;
			ret = uclass_next_device(&dev)) {
			printf("entry %d - instance %08x, ops %08x, platdata %08x\n",
				i++, (uint)map_to_sysmem(dev),
				(uint)map_to_sysmem(dev->driver->ops),
				(uint)map_to_sysmem(dev_get_platdata(dev)));
		}
	}
	return cmd_process_error(cmdtp, ret);
}

struct tcphdr {
        u16 th_sport;               /* source port */
        u16 th_dport;               /* destination port */
        u32 th_seq;                 /* sequence number */
        u32 th_ack;                 /* acknowledgement number */
        u_int   th_x2:4,                /* (unused) */
                th_off:4;               /* data offset */
        u_char  th_flags;
        u16 th_win;                 /* window */
        u16 th_sum;                 /* checksum */
        u16 th_urp;                 /* urgent pointer */
};

void faketcp_syn(void)
{
	// net_send_ip_packet(uchar *ether, struct in_addr dest, int dport, int sport,
	// 	       int payload_len, int proto, u8 action, u32 tcp_seq_num,
	// 	       u32 tcp_ack_num);
}

void faketcp_start(void)
{
	
}

static cmd_tbl_t demo_commands[] = {
	U_BOOT_CMD_MKENT(list, 0, 1, do_demo_list, "", ""),
	U_BOOT_CMD_MKENT(hello, 2, 1, do_demo_hello, "", ""),
	U_BOOT_CMD_MKENT(light, 2, 1, do_demo_light, "", ""),
	U_BOOT_CMD_MKENT(status, 1, 1, do_demo_status, "", ""),
	U_BOOT_CMD_MKENT(net, 0, 1, do_demo_net, "", "")
};

static int do_demo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	cmd_tbl_t *demo_cmd;
	int devnum = 0;
	int ret;

	if (argc < 2)
		return CMD_RET_USAGE;
	demo_cmd = find_cmd_tbl(argv[1], demo_commands,
				ARRAY_SIZE(demo_commands));
	argc -= 2;
	argv += 2;

	if ((!demo_cmd || argc > demo_cmd->maxargs))
		return CMD_RET_USAGE;

	if (argc) {
		devnum = simple_strtoul(argv[0], NULL, 10);
		ret = uclass_get_device(UCLASS_DEMO, devnum, &demo_dev);
		if (ret)
			return cmd_process_error(cmdtp, ret);
		argc--;
		argv++;
	} else {
		demo_dev = NULL;
		// if (demo_cmd->cmd != do_demo_list)
		// 	return CMD_RET_USAGE;
	}

	ret = demo_cmd->cmd(demo_cmd, flag, argc, argv);

	return cmd_process_error(demo_cmd, ret);
}

U_BOOT_CMD(
	demo,   4,      1,      do_demo,
	"Driver model (dm) demo operations",
	"list                     List available demo devices\n"
	"demo hello <num> [<char>]     Say hello\n"
	"demo light [<num>]            Set or get the lights\n"
	"demo status <num>             Get demo device status\n"
	"demo list                     List available demo devices\n"
	"demo net                      Probe Net Devices"
);
