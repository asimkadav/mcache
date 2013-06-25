/*
 * Copyright (C) ST-Ericsson SA 2012
 * Author: Maxime Coquelin <maxime.coquelin@xxxxxxxxxxxxxx> for ST-Ericsson.
 * License terms: GNU General Public License (GPL), version 2
 */
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/mfd/dbx500-prcmu.h>
#include <linux/pasr.h>
#include <linux/ux500-pasr.h>


static void ux500_pasr_apply_mask(u16 *mem_reg, void *cookie)
{
	printk(KERN_INFO"%s: cookie = %d, mem_reg = 0x%04x\n",
	 __func__, (int)cookie, *mem_reg);
}

static int ux500_pasr_probe(struct platform_device *pdev)
{
	int i;
	struct ux500_pasr_data *pasr_data = dev_get_platdata(&pdev->dev);

	if (!pasr_data)
	 return -ENODEV;

	for (i = 0; pasr_data[i].base_addr != 0xFFFFFFFF; i) {
	 phys_addr_t base = pasr_data[i].base_addr;

	 /*
	 * We don't have specific structure pointer to pass, but only
	 * DDR die channel in PRCMU. This may change in future
	 * version.
	 */
	 void *cookie = (void *)(int)pasr_data[i].mailbox;

	 if (pasr_register_mask_function(base,
	 &ux500_pasr_apply_mask,
	 cookie))
	 printk(KERN_ERR"Pasr register failed\n");
	}

	return 0;
}

static struct platform_driver ux500_pasr_driver = {
	.probe = ux500_pasr_probe,
	.driver = {
	 .name = "ux500-pasr",
	 .owner = THIS_MODULE,
	},
};

static int __init ux500_pasr_init(void)
{
	return platform_driver_register(&ux500_pasr_driver);
}
module_init(ux500_pasr_init);
