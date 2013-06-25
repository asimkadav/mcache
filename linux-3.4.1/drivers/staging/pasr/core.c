/*
 * Copyright (C) ST-Ericsson SA 2011
 * Author: Maxime Coquelin <maxime.coquelin@xxxxxxxxxxxxxx> for ST-Ericsson.
 * License terms: GNU General Public License (GPL), version 2
 */

#include <linux/mm.h>
#include <linux/spinlock.h>
#include <linux/pasr.h>
#include <linux/module.h> 

#include "helper.h"

struct pasr_fw {
	struct pasr_map *map;
};

static struct pasr_fw pasr;

void pasr_update_mask(struct pasr_section *section, enum pasr_state state)
{
	struct pasr_die *die = section->die;
	phys_addr_t addr = section->start - die->start;
	u8 bit = addr >> PASR_SECTION_SZ_BITS;

	if (state == PASR_REFRESH)
	 die->mem_reg &= ~(1 << bit);
	else
	 die->mem_reg |= (1 << bit);

	printk("%s(): %s refresh section 0x%08x. Die%d mem_reg = 0x%02x\n"
	 , __func__, state == PASR_REFRESH ? "Start" : "Stop"
	 , section->start, die->idx, die->mem_reg);

	if (die->apply_mask)
	 	die->apply_mask(&die->mem_reg, die->cookie);
	else
		printk ("Applied mask .\n");

	return;
}

void pasr_put(phys_addr_t paddr, unsigned long size)
{
	struct pasr_section *s;
	unsigned long cur_sz;
	unsigned long flags;

	if (!pasr.map) {
	 WARN_ONCE(1, KERN_INFO"%s(): Map not initialized.\n"
	 "\tCommand line parameters missing or incorrect\n"
	 , __func__);
	 goto out;
	}

	printk ("Will disable refresh for %p of size %lu.\n", paddr, size);
	do {
		s = pasr_addr2section(pasr.map, paddr);
		if (!s)
			goto out;

		cur_sz = ((paddr + size) < (s->start + PASR_SECTION_SZ)) ?
			size : s->start + PASR_SECTION_SZ - paddr;

		if (s->lock)
			spin_lock_irqsave(s->lock, flags);

		s->free_size += cur_sz;
		BUG_ON(s->free_size > PASR_SECTION_SZ);

		if (s->free_size < PASR_SECTION_SZ)
			goto unlock;

		if (!s->pair)	{
			pasr_update_mask(s, PASR_NO_REFRESH);
			s->state = PASR_NO_REFRESH;
		}
		else if (s->pair->free_size == PASR_SECTION_SZ) {
			pasr_update_mask(s, PASR_NO_REFRESH);
			s->state = PASR_NO_REFRESH;
			pasr_update_mask(s->pair, PASR_NO_REFRESH);
			s->pair->state = PASR_NO_REFRESH;
		}
unlock:
		if (s->lock)
			spin_unlock_irqrestore(s->lock, flags);

	 paddr += cur_sz;
	 size -= cur_sz;
	} while (size);

out:
	return;
}

void pasr_get(phys_addr_t paddr, unsigned long size)
{
	unsigned long flags;
	unsigned long cur_sz;
	struct pasr_section *s;

	if (!pasr.map) {
	 WARN_ONCE(1, KERN_INFO"%s(): Map not initialized.\n"
	 "\tCommand line parameters missing or incorrect\n"
	 , __func__);
	 return;
	}

	do {
	 s = pasr_addr2section(pasr.map, paddr);
	 if (!s)
		 goto out;

	 cur_sz = ((paddr + size) < (s->start + PASR_SECTION_SZ)) ?
	 size : s->start + PASR_SECTION_SZ - paddr;

	 if (s->lock)
	 spin_lock_irqsave(s->lock, flags);

	 if (s->free_size < PASR_SECTION_SZ)
	 goto unlock;

	 if (!s->pair)	{
		 pasr_update_mask(s, PASR_REFRESH);	
		 s->state = PASR_REFRESH;
	 }
	 else if (s->pair->free_size == PASR_SECTION_SZ) {
		 pasr_update_mask(s, PASR_REFRESH);
		 s->state = PASR_REFRESH;
		 pasr_update_mask(s->pair, PASR_REFRESH);
		 s->pair->state = PASR_REFRESH;
	 }
unlock:
	BUG_ON(cur_sz > s->free_size);
	 s->free_size -= cur_sz;

	 if (s->lock)
	 spin_unlock_irqrestore(s->lock, flags);

	 paddr += cur_sz;
	 size -= cur_sz;
	} while (size);

out:
	return;
}

void show_free_areas(unsigned int);


/* Scan the PASR map and return the total power value.
 * Return negative value on error.
 */
int pasr_power()
{
	int i=0,j=0;
	if (!pasr.map) {
		WARN_ONCE(1, KERN_INFO"%s(): Map not initialized.\n"
				"\tCommand line parameters missing or incorrect\n"
				, __func__);
		return -EINVAL;
	}

	printk ("System memory map..\n");
	show_free_areas(0);

	printk ("DRAM map: Total dies %d.\n", pasr.map->nr_dies);


	for (i=0;i<pasr.map->nr_dies;i++)	{
		printk("Die: %d Sections: %d\n ",pasr.map->die[i].idx, pasr.map->die[i].nr_sections);

		for (j=0;j<pasr.map->die[i].nr_sections;j++)	{
			struct pasr_section *s;			
			s = &pasr.map->die[i].section[j];
			printk ("Section 0x%08x state %s.\n", s->start, s->state == PASR_REFRESH ? "Start" : "Stop");

		}
	}

}


void pasr_memory(int *low, int *high)
{
	int i=0,j=0;
	if (!pasr.map) {
		WARN_ONCE(1, KERN_INFO"%s(): Map not initialized.\n"
				"\tCommand line parameters missing or incorrect\n"
				, __func__);
		return -EINVAL;
	}
    int l=0, h=0;
    BUG_ON(*low !=0);
    BUG_ON(*high !=0);
	//printk ("PASR POWER..\n");
	//show_free_areas(0);

	//printk ("DRAM map: Total dies %d.\n", pasr.map->nr_dies);


	for (i=0;i<pasr.map->nr_dies;i++)	{
		//printk("Die: %d Sections: %d\n ",pasr.map->die[i].idx, pasr.map->die[i].nr_sections);

		for (j=0;j<pasr.map->die[i].nr_sections;j++)	{
			struct pasr_section *s;			
			s = &pasr.map->die[i].section[j];
			//printk ("Section 0x%08x state %s.\n", s->start, s->state == PASR_REFRESH ? "Start" : "Stop");
            if (s->state == PASR_REFRESH)
                h++;
            else l++;
            *low = l;
            *high = h;
		}
    }
    //printk ("(%d)lwo and (%d)high are %d %d", l, h, *low, *high);
}



EXPORT_SYMBOL(pasr_get);
EXPORT_SYMBOL(pasr_put);
EXPORT_SYMBOL(pasr_power);
EXPORT_SYMBOL(pasr_memory);

int pasr_register_mask_function(phys_addr_t addr, void *function, void *cookie)
{
	struct pasr_die *die = pasr_addr2die(pasr.map, addr);

	if (!die) {
	 pr_err("%s: No DDR die corresponding to address 0x%08x\n",
	 __func__, addr);
	 return -EINVAL;
	}

	if (addr != die->start)
	 pr_warning("%s: Addresses mismatch (Die = 0x%08x, addr = 0x%08x\n"
	 , __func__, die->start, addr);

	die->cookie = cookie;
	die->apply_mask = function;

	die->apply_mask(&die->mem_reg, die->cookie);

	return 0;
}

/* The callback which should be invoked by the 
 * DDR2's platform code..\n


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

	for (i = 0; pasr_data[i].base_addr != 0xFFFFFFFF; i++) {
		phys_addr_t base = pasr_data[i].base_addr;

		void *cookie = (void *)(int)pasr_data[i].mailbox;

		if (pasr_register_mask_function(base,
				&ux500_pasr_apply_mask,
				cookie))
			printk(KERN_ERR"Pasr register failed\n");
	}

	return 0;
}

*/




int __init pasr_init_core(struct pasr_map *map)
{
	pasr.map = map;
	return 0;
}


