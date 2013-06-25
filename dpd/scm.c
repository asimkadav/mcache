/*
 * mm/scm.c
 *
 * Written by Sankar
 *
 * Storage class memory		<sankarp@cs.wisc.edu>
 */

#include "scm.h"

/*
 * Variables used to maintain the mapping between virtual memory chunks
 * to physical memory chunks
 */
struct memory_block **mem_block;
unsigned long memblocksz = 0;
unsigned long memblocksz_mb = 0;

/*
 * Persistent page table that is shared across users
 */
ppgtable_user ppgtbl[MAXUSERS];
unsigned long ppgtbl_index = 0;
unsigned long pg_prot_mapsize = 0; // map size in unsigned long
DEFINE_MUTEX(ppgtbl_lock);

extern int sections_per_bloc();

/*
 * Simple lock implementation - This can be modified to a read write lock. This 
 * might be needed if features like reallocating chunks are done where the
 * status of memory sections can change significantly and more functions need to
 * be covered by the locks. But just the allocate_persistent_chunk() is the only
 * function protected by lock. 
*/
DEFINE_MUTEX(scm_lock);

/*
 * OS exposes 128 MB memory blocks (hotplug granularity). So, the bitmap size
 * should be physical memory size / 128 MB. The variable get_max_block_pindex() 
 * tracks the block with the highest numbered physical index. The scm_init() 
 * defines the freememblocks as bitmap. 
 */
#define MEMBLOCKS_BMAPSZ BITS_TO_LONGS(get_max_block_pindex())
#define MAX_MEMBLOCKS_RND MEMBLOCKS_BMAPSZ*sizeof(long)*8
#define MAX_MEMBLOCKS    get_max_block_pindex()+1
#define NOALIGN 0
unsigned long *freememblocks;

//unsigned long get_max_block_pindex() = 0;
extern unsigned long get_max_block_pindex();
static unsigned long max_memory_index = 0;
static unsigned long mcache_offlined_chunks = 0;

/*
 * This was done to avoid introducing a read write lock. Page fault handler 
 * needs to grab read lock in order to avoid any inconsistent views in the 
 * mapping structure due to changes in page table structure. 
 * At any point passive_map1 and passive_map2 should have the same values 
 * except when the chunk allocator is currently working. The active_mapping
 * points to either of these structures and thereby offering a consitent view
 * to the readers (page fault handler). This is similar to RCU mechanism. 
 * The active_mapping pointer can be atomically modified to point to 
 * passive_map1 or passive_map2.
 */
virt_phys_map *active_mapping;
virt_phys_map passive_map1;
virt_phys_map passive_map2;
int current_map;
//extern int sections_per_block;

/*
 * The removable state of the memory regions seem to be changing. So, before 
 * every call to allocate_persistent_chunks, the removable state of all memory 
 * region is checked. Since the chunk allocation is not in the critical path,
 * this does not cause any additional overhead. 
 */
void update_removable_memblocks()
{
	unsigned long startbit = 0, scanbit;
	struct memory_block *mblock;
	unsigned long i;

	for(i = 0;i < MEMBLOCKS_BMAPSZ; i++)
		printk(KERN_ERR"before BITMAP %lx", freememblocks[i]);

	while(startbit < get_max_block_pindex())
	{
		scanbit = bitmap_find_next_zero_area(freememblocks,
					MAX_MEMBLOCKS, startbit, 1, NOALIGN);

		if(scanbit > get_max_block_pindex() || scanbit == (unsigned long)-1)
			break;

		startbit = scanbit+1;
		mblock = mem_block[scanbit];

		if(!is_removable(mblock))
			bitmap_set(freememblocks, scanbit, 1);

		//printk(KERN_ERR"mblock state %d secct %d phys_index %d", mblock->state, mblock->section_count, mblock->start_section_nr / sections_per_block);
	}

	for(i = 0;i < MEMBLOCKS_BMAPSZ; i++)
		printk(KERN_ERR"after BITMAP %lx", freememblocks[i]);
}

/*
 * The initialization of scm related structures go here. This should be invoked
 * once during boot time.
 */
void init_scm(void)
{
	struct memory_block *mblock;
	unsigned long i, phys_index;
	//unsigned long onegb = 1024*1024*1024;

	pg_prot_mapsize = PERS_SPACE;
        pg_prot_mapsize /= EXTENTSIZE;
        pg_prot_mapsize *= 2; // this will be the total bits required
        //unsigned long can accomodate 64 bits
        pg_prot_mapsize /= (8 * sizeof(unsigned long));
        
	printk(KERN_ERR"page protection map size %lx", pg_prot_mapsize);

	printk(KERN_ERR"possible devices %lx.\n", MAX_MEMBLOCKS);
	//pg_prot_mapsize = PERS_SPACE;
	//pg_prot_mapsize /= EXTENTSIZE;
	//pg_prot_mapsize *= 2;
	//pg_prot_mapsize /= sizeof(unsigned long);

	freememblocks = (unsigned long *)kmalloc
		(sizeof(unsigned long) * MEMBLOCKS_BMAPSZ, GFP_KERNEL);
	
	printk ("MEMBLOCKS_BMAPSZ is %lx.\n", MEMBLOCKS_BMAPSZ);

	if (freememblocks == NULL)	{
		printk ("freememblocks:Memory allocation failed.\n");
	}
	
	printk ("MAX_MEMBLOCKS_RND is %lx.\n",MAX_MEMBLOCKS_RND);

	bitmap_fill(freememblocks, MAX_MEMBLOCKS_RND);

	printk ("MAX_MEMBLOCKS_RND is %lx.\n",MAX_MEMBLOCKS_RND);

	 
	mem_block = (struct memory_block **)kmalloc
		(sizeof(struct memory_block *) * MAX_MEMBLOCKS, GFP_KERNEL);

	if (mem_block == NULL)	{
		printk ("mem_block: Memory allocation failed.\n");
	}


	 
	for(i = 0;i < NR_MEM_SECTIONS;i++)
	{
		if(!present_section_nr(i))
			continue;

		mblock = find_memory_block(__nr_to_section(i));
		phys_index = mblock->start_section_nr/sections_per_bloc();
		mem_block[phys_index] = mblock;

		printk(KERN_ERR"mblock state %lx secct %d phys_index %lx phy_dev %d", mblock->state, mblock->section_count, mblock->start_section_nr / sections_per_bloc(), mblock->phys_device);

		if(is_removable(mblock))
			bitmap_clear(freememblocks, phys_index, 1);

		if(phys_index > get_max_block_pindex())
			panic("memblock index count mismatch");
	}



	//for(i = 0;i < MEMBLOCKS_BMAPSZ; i++)
	//	printk(KERN_ERR"BITMAP %lx", freememblocks[i]);

	memblocksz = get_memory_block_size();
	memblocksz_mb = memblocksz / 1024 / 1024;
	printk ("memblocksz_mb is %lx.\n", memblocksz_mb);
	active_mapping = &passive_map1;
	current_map = 1;
	passive_map1.max_virtindex = passive_map2.max_virtindex = -1;
}

/*
 * Allocating persistent chunks - a bitmap of free memory regions is
 * maintained. Consecutive memory regions satisfying the request size
 * will be offlined and sent to the requester.
 */
long allocate_persistent_chunk(unsigned long *p_index, unsigned long size_mb)
{
	//The constraints on the requested size should be checked by the caller
	int memblocks_req = size_mb / memblocksz_mb;
	unsigned long scanbitindex = 0;
	unsigned long freebitindex = 0; 
	unsigned long i, j = 0;
	int error = -EINVAL;

	printk(KERN_ERR"size_mb %lx memblocks requested : %d.\n\n", size_mb, memblocks_req);
	
	while(scanbitindex <= get_max_block_pindex())
	{	
		// Find free bitmaps that cover the requested size
		freebitindex = bitmap_find_next_zero_area(freememblocks, 
						MAX_MEMBLOCKS, scanbitindex, 
						memblocks_req, NOALIGN);
		scanbitindex = freebitindex + 1;
		printk(KERN_ERR"free bit : %lx.\n", freebitindex);

		if(freebitindex > get_max_block_pindex() ||
			freebitindex == (unsigned long)-1)
		{
			printk ("No free contiguous memory.\n");
			return CONTIGERROR;	
		}

		printk ("Will now offline memory.\n");
		// Hotplug-offline the covered memory blocks
		for(i = 0; i < memblocks_req; i++)
		{
			j = freebitindex+i;

			printk ("Offlining memblock %lx.\n", j);

			error = memory_block_change_state(mem_block[j], 
						MEM_OFFLINE, MEM_ONLINE);
			// 0 refers to success
			if(error != 0)
			{
				printk(KERN_ERR"hotplug failed");
				break;
			}
			else	{

				mcache_offlined_chunks++; //ASIM
			}

		}

		max_memory_index = j; //ASIM

		//Hotplug of contiguous memory blocks failed
		//Revert the state of the one's that were offlined  
		if(error != 0)
		{
			printk(KERN_ERR"trying to revert");
			for(i = j-1; i >= freebitindex; i--)
			{
				error = memory_block_change_state(
						mem_block[i], MEM_ONLINE,
						MEM_OFFLINE);
				max_memory_index = i; //ASIM
				printk(KERN_ERR"Reverting for mblock %lx.\n", i);
				if(error == -EINVAL)
				{ /* onlining failed */ }
			}
		}
		else
		{
			bitmap_set(freememblocks, freebitindex, memblocks_req);
			*p_index = freebitindex;
			break;
		}
	}


	if(error == -EINVAL)	
		return HOTPLUGERROR;
	return SUCCESS;
}

long release_persistent_chunk_all()
{
	//The constraints on the requested size should be checked by the caller
	//int memblocks_req = size_mb / memblocksz_mb;
	unsigned long i = 0;
	int error = -EINVAL;

	//printk(KERN_ERR"size_mb %d memblocks requested : %d", size_mb, memblocks_req);

	//Revert the state of the one's that were offlined  
	printk(KERN_ERR"In release_persistent_chunk_all for mcache_offlined_chunks %lx.\n", mcache_offlined_chunks);

	for(i = 0; i < mcache_offlined_chunks; i++)
	{
		if (max_memory_index  < 0)	{
			printk ("Trying to online garbage..\n");
			return EINVAL;
		}
		error = memory_block_change_state(
				mem_block[max_memory_index], MEM_ONLINE,
				MEM_OFFLINE);
		printk(KERN_ERR"Reverting for mblock %lx.\n", max_memory_index);
		max_memory_index--;
		if(error == -EINVAL)
		{ /* onlining failed */ 
			printk ("DRAM onlining failed for mblock[%lx].\n", max_memory_index);
			i--;
		}
	}

	mcache_offlined_chunks = 0;
	if(error == -EINVAL)	
		return HOTPLUGERROR;
	return SUCCESS;
}







/*
 * System call used to allocate persistent chunks to the user. This entry
 * function makes some checks like if the virtual address has already been
 * allocated, request lies in chunk boundary etc.
 */
long alloc_persistent (unsigned long v_addr, unsigned long size_mb)
{
	/*
	input 		: size, virtual address
	output 		: true if allocated else false
	constraints 	: size in granularity of 128
			  virtual address in the persistent region
			  virtual address not already allocated
	*/

	long ret;
	unsigned long p_index;
	long int fill_index, i;
	vpmemblock curmap;
	virt_phys_map *tobeactivemap, *tobelazymap, *tobeprintmap; 
	unsigned long block_sz_mb;
	struct vm_area_struct *vma = NULL;

	update_removable_memblocks();

	/* Check for alignment - allocation done in granularity of block size */
	block_sz_mb = memblocksz/1024/1024;
	if(size_mb & (block_sz_mb-1) != 0)
		return SZALIGNERROR;

	tobeactivemap = active_mapping;
	/* check fo virtual address overlap */
	for(i = tobeactivemap->max_virtindex; i >= 0; i--)
	{
		unsigned long block_start, block_end, size, v_endaddr;

		block_start = tobeactivemap->vpblock[i].v_startaddr;
		size = tobeactivemap->vpblock[i].size;
		size *= (1024*1024);
		block_end = block_start + size - 1;
		v_endaddr = v_addr + size_mb;
	
		if(block_start <= v_addr && block_end >= v_addr)
			return VIRTADDROLAP;

		if(block_start <= v_endaddr && block_end >= v_endaddr)
			return VIRTADDROLAP;
	}

	// Locking to avoid races and corrupting the data structures
	mutex_lock(&scm_lock);

	ret = allocate_persistent_chunk(&p_index, size_mb);

	if(ret != SUCCESS)
		return ret;

	printk(KERN_ERR"phys index returned %lx.\n", p_index);

	if(current_map != 1 && current_map != 2)
		return CURMAPERROR;

	tobeactivemap = &passive_map2;
	tobelazymap = &passive_map1;
	if(current_map == 2)
	{
		tobeactivemap = &passive_map1;
		tobelazymap = &passive_map2;
	}

	curmap.v_startaddr = v_addr;
	curmap.size = size_mb;
	curmap.p_index = p_index;
	curmap.nr_blocks = block_sz_mb; 

	fill_index = tobeactivemap->max_virtindex + 1;
	for(i = tobeactivemap->max_virtindex; i >= 0; i--)
	{
		if(tobeactivemap->vpblock[i].v_startaddr < v_addr)
			break;
		tobeactivemap->vpblock[i+1] =
			tobeactivemap->vpblock[i];
		fill_index = i;
	}
	tobeactivemap->vpblock[fill_index] = curmap;
	tobeactivemap->max_virtindex++;	

	//This is supposed to be an atomic assignment
	active_mapping = tobeactivemap;

	//Update the other list tobelazymap
	fill_index = tobelazymap->max_virtindex + 1;
	for(i = tobelazymap->max_virtindex; i >= 0; i--)
	{
		if(tobelazymap->vpblock[i].v_startaddr < v_addr)
			break;
		tobelazymap->vpblock[i+1] =
			tobelazymap->vpblock[i];
		fill_index = i;
	}
	tobelazymap->vpblock[fill_index] = curmap;
	tobelazymap->max_virtindex++;	

	//Assert to check if both the lists are the same
	tobeprintmap = &passive_map1;
	for(i = tobeprintmap->max_virtindex; i >= 0; i--)
		printk(KERN_ERR"v_addr %lx size %ld p_index %ld nr_blocks %ld",
			tobeprintmap->vpblock[i].v_startaddr,
			tobeprintmap->vpblock[i].size, 
			tobeprintmap->vpblock[i].p_index,
			tobeprintmap->vpblock[i].nr_blocks);
			
	tobeprintmap = &passive_map2;
	for(i = tobeprintmap->max_virtindex; i >= 0; i--)
		printk(KERN_ERR"v_addr %lx size %ld p_index %ld nr_blocks %ld",
			tobeprintmap->vpblock[i].v_startaddr,
			tobeprintmap->vpblock[i].size,
			tobeprintmap->vpblock[i].p_index,
			tobeprintmap->vpblock[i].nr_blocks);

	if (current->mm == NULL)	{
		printk ("kernel pg tables..\n");
	}	

	vma = find_vma(current->mm, v_addr);

	if (vma == NULL)	{
		printk ("No mercurial vma with kernel page tables..\n ");

	}



	
	//unlock
	mutex_unlock(&scm_lock);

	return SUCCESS;
}

/*
 * Changing the protection access rights for extents
 * Iterate through all the pages covered by the extents and change the access
 * rights for all the pages in that extent.
 * Since the VA <-> PA translation could be cached in the TLB, they also need
 * to be flushed 
 */
long mpprotect(void * extseg, void * rights, int count)
{
	extent ext = *(extent *)extseg;
	unsigned long offset = 0;
	unsigned long nr_extents = 0;
	bool read = false, write = false;
	unsigned long *pgprotmap, i, j, address;
	long int oldr = 0, oldw = 0;
	ppgtable_user *p;
	struct mm_struct *d_mm;
	int rw = 0;
	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;
	pte_t *page_table;
        pte_t entry;
        spinlock_t *ptl;
	user_file_rights *uf_rights = (user_file_rights *)rights;

	// check if given extent size is in granularity of extent
	if(ext.base & (EXTENTSIZE-1) || ext.size & (EXTENTSIZE-1))
		return SZALIGNERROR;

	// find the base extents covered by the extent segment sent by user
	// fetch the appropriate extent protection bitmap
	offset = ext.base - PERS_START;
	offset = offset >> EXTENT_SHIFT;	

	nr_extents = ext.size >> EXTENT_SHIFT;	
	
	for(j = 0;j < count; j++)
	{
		uid_t uid = uf_rights[j].uid;
		p = find_shared_ppgtbl_entry(uid, false, false);
		if(!p)
			return NOUSERERROR; 
		// Dummy mm used only for its spinlock
		// other fields of d_mm are not to be used 
		d_mm = &(p->d_mm);
		pgprotmap = p->page_prot_map;

		// apply protection bits to the user based bitmap
		read = write = false;
		rw = uf_rights[j].rw & 0x3;
		if(rw & 0x2)
			read = true;
		if(rw & 0x1)
		{
			read = true;
			write = true;
		}

		for(i = 0; i < nr_extents; i++)
		{
			address = ext.base + (i*EXTENTSIZE);
			oldr = test_bit(((offset+i)*2), pgprotmap); 
			oldw = test_bit(((offset+i)*2)+1, pgprotmap); 

			/* Change the protection bits in the bitmap 
			   maintained in the user shared structure */
			if(read)
				set_bit(((offset+i)*2), pgprotmap); 
			else
				clear_bit(((offset+i)*2), pgprotmap); 
			if(write)
				set_bit(((offset+i)*2)+1, pgprotmap); 
			else
				clear_bit(((offset+i)*2)+1, pgprotmap); 

			/* Check if TLb flush can be avoided by any chance.
			   If the old and the currently applid protection 
			   rights remains the same, then changing the bitmap 
			   is enough. We don't need to modify the pte entries 
			   either if we are not going to flush the TLB. During 
			   the page fault, the handler will refer to the bitmap
			   to find the appropriate rights. So, it's enough if 
			   the bitmap is updated */	
			if(oldr <= 0) oldr = 0;
			if(oldw <= 0) oldw = 0;
		
			//if(!oldr && !oldw)
			//	continue;
			//if((!!oldr == !!(rw&0x2)) && (!!oldw == !!(rw&0x1)))
			//	continue;

			/* Update the pte entires if it exist and flush  TLB 
			   entry */
			pgd = pgd_offset(current->mm, address);	
			if(pgd_none(*pgd))
				continue;
			pud = pud_offset(pgd, address);
			if(pud_none(*pud))
				continue;
			pmd = pmd_offset(pud, address);
			if(pmd_none(*pmd))
				continue;
			pte = pte_offset_map(pmd, address);
			if(pte == NULL)
				continue;
			if(pte_none(*pte))
				continue;
			page_table=pte_offset_map_lock(d_mm, pmd, address, &ptl);
			entry = *pte;
			entry = set_base_pte_flags(entry);
			if(write)
			{ 
				entry = pte_mkwrite(entry);
				entry = pte_mkpresent(entry);
			}
			if(read)
			{
				//if dirty then cache flush something like that
				entry = pte_mkpresent(entry);
				entry = pte_mkclean(entry);
			}
			set_pte_at(d_mm, address, page_table, entry);
			pte_unmap_unlock(page_table, ptl);
			//printk(KERN_ERR"flushing for %lx and entry %lx", address, entry);
			//__flush_tlb_one(address);
			flush_tlb_others(cpu_online_mask, NULL, address);
		}
	}
	return SUCCESS; 
}

long flush_pg(int flush_all)
{
	unsigned long address, i;
	unsigned long start = PERS_START;
	unsigned long end = PERS_START + PERS_SPACE;
	pud_t *pud;
        pmd_t *pmd; 
        pte_t *pte;
        pte_t entry;
	pte_t *page_table;
	spinlock_t *ptl;
	struct mm_struct *d_mm; 

	mutex_lock(&ppgtbl_lock);
	for(i = 0;i < ppgtbl_index; i++)
	{ 
		pud = ppgtbl[i].ppud;
		// dummy mm's spinlock is used
		d_mm = &(ppgtbl[i].d_mm);

		for(address = start; address < end; address += PAGE_SIZE)
		{
			if(pud_none(*pud))
				continue;
			pmd = pmd_offset(pud, address);
			if(pmd_none(*pmd))
				continue;
			pte = pte_offset_map(pmd, address);
			if(pte == NULL)
				continue;
			if(pte_none(*pte))
				continue;

			page_table=pte_offset_map_lock(d_mm, pmd, address, &ptl);
                        entry = *pte;
                        entry = flush_pte(entry);
			set_pte_at(d_mm, address, page_table, entry);
                        pte_unmap_unlock(page_table, ptl);
			//__flush_tlb_one(address);
			flush_tlb_others(cpu_online_mask, NULL, address);
		}
		//bitmap_zero(ppgtbl[i].page_prot_map, pg_prot_mapsize);
		memset(ppgtbl[i].page_prot_map, 0, pg_prot_mapsize*sizeof(unsigned long));
	}	
	mutex_unlock(&ppgtbl_lock);
	return SUCCESS;
}

/*
 * Page fault handler for persistent region
 * 1. Establishes the page table entry
 * 2. Responsible for setting up the protection bits for every page based 
 * on the process
 * 
 * __do_fault() - lies in page fault handling code path - checks if the address
 * that faulted lies in persistent space by checking the persistent field
 * of the vma node. If so, the code path ends up here.
 */
unsigned long pgfault_serviced = 0;
bool ppg_tracker = false;

int do_persistent_fault(struct mm_struct *mm, struct vm_area_struct *vma, 
			unsigned long address, pte_t *pte, pmd_t *pmd,
			unsigned int flags)
{
	int i, ret = 0;
        spinlock_t *ptl;
        pte_t entry;
	pte_t *page_table;
	unsigned long page_nr, va_pg, va_off, p_start, p_w_off;
	virt_phys_map *vpmap = active_mapping;
	bool block_fnd = false;
	struct page* first_page;
	ppgtable_user *p;
	unsigned long *pgprotmap, ee_offset;
	struct mm_struct *d_mm;

	p = find_shared_ppgtbl_entry(current->cred->uid, false, false);
	if(!p)
	{
		printk(KERN_ERR"User shared data structure for user %u is not found", current->cred->uid);
		return EINVAL;	
		//return VM_FAULT_PERS_PROT;	
	}
	// d_mm is only to be used for its spinlock
	d_mm = &(p->d_mm);

	for(i = vpmap->max_virtindex; i >= 0; i--)
	{
		unsigned long saddr, size;
		saddr = vpmap->vpblock[i].v_startaddr;
		size = vpmap->vpblock[i].size * 1024 * 1024;

		if(saddr <= address && address <= saddr+size)
		{
			block_fnd = true;
			break;
		}
	}

	if(block_fnd == true)
	{
		va_pg = address & PAGE_MASK;
		va_off = va_pg - vpmap->vpblock[i].v_startaddr;

		first_page = pfn_to_page(vpmap->vpblock[i].p_index << PFN_SECTION_SHIFT);
		p_start = page_to_pfn(first_page); 
		p_start = p_start << PAGE_SHIFT;
		p_w_off = p_start + va_off;

		page_nr = p_w_off >> PAGE_SHIFT;
		pgfault_serviced++;
		//printk(KERN_ERR"%s: va_pg %lx; va_off %lx; p_start %lx; p_w_off %lx; page_nr %lx;", current->comm, va_pg, va_off, p_start, p_w_off, page_nr);
	}
	else
	{
		printk(KERN_ERR"%lx is not in physical memory backed region", address);
		return EINVAL;	
		//return VM_FAULT_PERS_PROT;	
	}

	page_table = pte_offset_map_lock(d_mm, pmd, address, &ptl);
	entry = pfn_pte(page_nr, vma->vm_page_prot);
	entry = set_base_pte_flags(entry); 

	/* set protection bits */
	pgprotmap = p->page_prot_map;
	ee_offset = address - PERS_START;
	ee_offset = ee_offset >> EXTENT_SHIFT;

	//printk(KERN_ERR"extent offset : %ld", ee_offset);
	
	if(test_bit((ee_offset*2)+1, pgprotmap))
	{
		entry = pte_mkwrite(pte_mkdirty(entry));
		entry = pte_mkpresent(pte_mkdirty(entry));
	}
	else
	{
		if(flags & FAULT_FLAG_WRITE)
		{
			printk(KERN_ERR"user %u does not have write access to address %lx", current->cred->uid, address);
			//entry = clear_pte_flags(entry);
			//ret = VM_FAULT_PERS_PROT;
			ret = EINVAL;
		}	
	}	
	
	if(test_bit((ee_offset*2), pgprotmap))
	{
		entry = pte_mkclean(entry);
		entry = pte_mkpresent(pte_mkdirty(entry));
	}
	else // read access
	{
		// if read bit is not set return error
		// If write bit is enabled read is also enabled
		printk(KERN_ERR"user %u does not have read access to address %lx", current->cred->uid, address);
		//entry = clear_pte_flags(entry);
		ret = EINVAL;
		//ret = VM_FAULT_PERS_PROT;
	}

	//printk(KERN_ERR"VA %lx persistent pte entry %lx", address, entry);
	set_pte_at(d_mm, address, page_table, entry);
	update_mmu_cache(vma, address, page_table);

	pte_unmap_unlock(page_table, ptl);

	return ret;
}

void pud_assign(struct mm_struct *mm, pgd_t *pgd, pud_t *new)
{
	spin_lock(&mm->page_table_lock);
	pgd_populate(mm, pgd, new);
	spin_unlock(&mm->page_table_lock);
}

ppgtable_user *find_shared_ppgtbl_entry(uid_t uid, bool create_new, bool incref)
{
	unsigned long i;
	ppgtable_user *p = NULL;
	pud_t *ppud = NULL, *ppud_ret = NULL;

	if(create_new == true)
		mutex_lock(&ppgtbl_lock);

	for(i = 0;i < ppgtbl_index; i++)
	{
		if(ppgtbl[i].uid == uid)
		{
			p = &(ppgtbl[i]);
			goto ppgd_ret; 
		}
	}

	// Does not support more than MAXUSERS
	if(ppgtbl_index+1 >= MAXUSERS || create_new == false)
		goto ppgd_ret;

	ppg_tracker = true;
	ppgtbl[ppgtbl_index].uid = uid;
	ppud_ret = ppgtbl[ppgtbl_index].ppud = pud_alloc_one(NULL, 0x0);
	//ppgtbl[ppgtbl_index].page_prot_map = (unsigned long *)kmalloc
	//	(sizeof(unsigned long) * pg_prot_mapsize, GFP_KERNEL);
	if(ppgtbl[ppgtbl_index].page_prot_map == NULL)
                printk(KERN_ERR"allocation failed for page protection map");
	//bitmap_zero(ppgtbl[ppgtbl_index].page_prot_map, pg_prot_mapsize);
	printk(KERN_ERR"trying to zero for %lx.\n", pg_prot_mapsize*(sizeof(unsigned long)));	
	memset(ppgtbl[ppgtbl_index].page_prot_map, 0, pg_prot_mapsize*sizeof(unsigned long));
	p = &(ppgtbl[ppgtbl_index]);
	ppgtbl_index++;

	for(i = 0;i < ppgtbl_index; i++)
	{
		ppud = ppgtbl[i].ppud;
		printk(KERN_ERR"USER SHARED %lx.ppgtbl UID %u PUD %p pfn %lx", 
			i, ppgtbl[i].uid, ppud, __pa(ppud));
	}

ppgd_ret:
	if(create_new == true)
		mutex_unlock(&ppgtbl_lock);

	if(ppud_ret != NULL && incref == true)
		get_page(pfn_to_page(__pa(ppud_ret) >> PAGE_SHIFT));

	return p;
}

void clear_ppgd_from_mm(struct mm_struct *mm)
{
	pgd_t *pgd;
	pud_t *pud;
	ppgtable_user *p;

	pgd = pgd_offset(mm, PERS_START);
	pud = pud_offset(pgd, PERS_START);
	p = find_shared_ppgtbl_entry(current->cred->uid, false, false);


	// mm used here is that of the process's real mm
	// The pgd entry is changed which can be seen by the entire process
	// So, it makes sense to take the page table lock rather than the 
	// spinlock in the dummy mm present in user shared structure	

	if(p != NULL && pud == p->ppud)
	{
	//printk(KERN_ERR"REVOKE %s pgd--%lx *pgd--%lx pud--%lx pid--%lx", 
	//		current->comm, pgd, *pgd, pud, current->cred->uid);
		spin_lock(&mm->page_table_lock);
		*(unsigned long *)pgd = 0x0;
		spin_unlock(&mm->page_table_lock);
	}
}

void add_current_pud(void)	
{

	int i, rc;
	unsigned long p_addr = PERS_START;
	unsigned long p_len = PERS_SPACE;
	unsigned long p_flags = MAP_SHARED | MAP_ANONYMOUS | MAP_NORESERVE | MAP_FIXED;
	unsigned long p_prot = PROT_READ | PROT_WRITE;
	unsigned long p_pgoff = 0;
	struct vm_area_struct *vma = NULL;

	//for(i = 0;i < 20; i++)
	//      printk(KERN_ERR"%s virtual hole", current->comm);

	u64 size = p_len;
	u64 addr;
	u64 start;

	struct mm_struct *mm= (struct mm_struct *)__va(read_cr3());

	printk ("kernel page table is at %p.\n", mm);

	vma = find_vma(mm, p_addr);
//	if(vma)
//	{
//		vma->mercurial = true;

		// Initialize shared page table for persistent region
		{
			ppgtable_user *p = find_shared_ppgtbl_entry(
					current->cred->uid, true, true);
			if(p != NULL)
			{
				pud_assign(mm,
						pgd_offset(mm, p_addr),
						p->ppud);
			}
			else
				printk(KERN_ERR"Too many users and shared user data structure cannot be allocated");
		}
//	}
//	else	{
//		printk ("VMA not found..\n");
//	}
	//current->persistent_region_defined = true;

}

/*
 * Associate a virtual page frame with a given physical page frame
 * and protection flags for that frame.

static void set_pte_pfn(unsigned long vaddr, unsigned long pfn, pgprot_t flags)
{
        pgd_t *pgd;
        pud_t *pud;
        pmd_t *pmd;
        pte_t *pte;

        pgd = swapper_pg_dir + pgd_index(vaddr);
        if (pgd_none(*pgd)) {
                BUG();
                return;
        }
        pud = pud_offset(pgd, vaddr);
        if (pud_none(*pud)) {
                BUG();
                return;
        }
        pmd = pmd_offset(pud, vaddr);
        if (pmd_none(*pmd)) {
                BUG();
                return;
        }
        pte = pte_offset_kernel(pmd, vaddr);
        // <pfn,flags> stored as-is, to permit clearing entries 
        set_pte(pte, pfn_pte(pfn, flags));

        
        // It's enough to flush this one mapping.
        // This appears conservative since it is only called
        //  from __set_fixmap.
         
        local_flush_tlb_page(NULL, vaddr, PAGE_SIZE);
}
*/
