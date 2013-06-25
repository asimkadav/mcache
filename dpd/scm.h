#include <linux/memory.h>
#include <linux/mmzone.h>
#include <linux/mutex.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/cpumask.h>
#include <linux/mm.h>
#include <linux/memblock.h>

#include <asm/mman.h>
#include <asm/pgtable.h>
#include <asm/pgalloc.h>
#include <asm/pgtable_types.h>
#include <asm/tlbflush.h>
#include <asm/bitops.h>

#define PERS_START 0x8000000000
//#define PERS_SPACE (unsigned long)(1024*1024)*(unsigned long)(1024*32)
#define PERS_SPACE (unsigned long)(1024*1024)*(unsigned long)(1024*1)
#define PAGESIZE   4*1024
#define EXTENTSIZE PAGESIZE 
#define EXTENT_SHIFT 12
#define MAXUSERS   8

//pteval_t __supported_pte_mask __read_mostly = ~_PAGE_IOMAP;

#define SUCCESS		0x0
#define VIRTADDROLAP 	-0x1
#define HOTPLUGERROR 	-0x2
#define CURMAPERROR	-0x4 
#define CONTIGERROR	-0x8
#define SZALIGNERROR	-0x16 
#define NOUSERERROR	-0x32
#define MAXVIRTBLOCKS 	128

static pte_t inline clear_pte_flags(pte_t pte)
{
	pteval_t v = native_pte_val(pte);
	return native_make_pte(v & ~0xfff);
}

static pte_t inline flush_pte(pte_t pte)
{
	pteval_t v = native_pte_val(pte);
	return native_make_pte(v & 0x0);
}

static pte_t inline set_base_pte_flags(pte_t pte)
{
	// set user flag alone
	pteval_t v = native_pte_val(pte);
	return native_make_pte((v & ~0xfff) | (v & 0x64));
}

static pte_t inline pte_mkpresent(pte_t pte)
{
	pteval_t v = native_pte_val(pte);
	return native_make_pte(v | 0x1);
}

/* 
Structure to map address between virtual address and physical address 
 
The chunk allocator uses this structure to identify if the virtual address 
with the provided range has already overlapped with previous mapping. 
 
The page fault handler uses this structure to establish a mapping  
 
Linear array is used to maintain the mapping which is sorted. Thus trying to  
optimize lookup during page fault handling. 
*/
typedef struct {
	unsigned long v_startaddr;
	unsigned long size;
	unsigned long p_index;
	unsigned long nr_blocks;	
}vpmemblock;

typedef struct {
	vpmemblock vpblock[MAXVIRTBLOCKS];
	long int max_virtindex;
}virt_phys_map;

typedef struct {
	uid_t uid;
	pud_t *ppud;
	//unsigned long *page_prot_map;
	unsigned long page_prot_map[6*1024*1024];
	struct mm_struct d_mm;
}ppgtable_user;

typedef struct {
	unsigned long base;
	unsigned long size;
}extent;

typedef struct {
	uid_t uid;
	int rw;
}user_file_rights;

extern ppgtable_user ppgtbl[];
extern unsigned long ppgtbl_index;

/* extern functions used by scm.c */
unsigned long get_memory_block_size(void);
int memory_block_change_state(struct memory_block *,
		unsigned long, unsigned long);
struct memory_block *find_memory_block(struct mem_section *);
int is_removable(struct memory_block *);

/* function declaration */

extern void register_persistent_fault_handler(void * (*odft_fn) (struct mm_struct *mm, struct vm_area_struct *vma,
                        unsigned long address, pte_t *pte, pmd_t *pmd,
                        unsigned int flags));


extern unsigned long do_mmap_pgoff(struct file *file, unsigned long addr,
        unsigned long len, unsigned long prot,
        unsigned long flag, unsigned long pgoff);





void init_scm(void);
long allocate_persistent_chunk(unsigned long *, unsigned long);
long alloc_persistent (unsigned long v_addr, unsigned long size_mb);
int do_persistent_fault(struct mm_struct *, struct vm_area_struct *,
                        unsigned long, pte_t *, pmd_t *, unsigned int);
void pud_assign(struct mm_struct *, pgd_t *, pud_t *);
ppgtable_user *find_shared_ppgtbl_entry(uid_t, bool, bool);
void clear_ppgd_from_mm(struct mm_struct *);
long release_persistent_chunk_all(void);
void update_removable_memblocks(void);
void add_current_pud(void);
