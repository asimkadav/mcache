#define SpinVersion	"Spin Version 5.2.0 -- 2 May 2009"
#define PanSource	"hanoi.4.pm"

#ifdef WIN64
#define ONE_L	((unsigned long) 1)
#define long	long long
#else
#define ONE_L	(1L)
#endif
char *TrailFile = PanSource; /* default */
char *trailfilename;
#if defined(BFS)
#ifndef SAFETY
#define SAFETY
#endif
#ifndef XUSAFE
#define XUSAFE
#endif
#endif
#ifndef uchar
#define uchar	unsigned char
#endif
#ifndef uint
#define uint	unsigned int
#endif
#ifndef HASH32
#define HASH64
#endif
#define DELTA	500
#ifdef MA
	#if NCORE>1 && !defined(SEP_STATE)
	#define SEP_STATE
	#endif
#if MA==1
#undef MA
#define MA	100
#endif
#endif
#ifdef W_XPT
#if W_XPT==1
#undef W_XPT
#define W_XPT 1000000
#endif
#endif
#ifndef NFAIR
#define NFAIR	2	/* must be >= 2 */
#endif
#define HAS_CODE
#define MERGED	1
#if !defined(BCS) && defined(SCHED)
	#define BCS	1
#endif
#if !defined(HAS_LAST) && defined(BCS)
	#define HAS_LAST	1
/*	#define NOREDUCE	1	*/
#endif
#ifdef NP	/* includes np_ demon */
#define HAS_NP	2
#define VERI	7
#define endclaim	3 /* none */
#endif
typedef struct S_F_MAP {
	char *fnm; int from; int upto;
} S_F_MAP;

#define nstates6	11	/* CtoB */
#define endstate6	10
short src_ln6 [] = {
	  0,   0,   0,   0,   0,   0,  64,  64, 
	 63,  67,  67,   0, };
S_F_MAP src_file6 [] = {
	{ "-", 0, 5 },
	{ "hanoi.4.pm", 6, 10 },
	{ "-", 11, 12 }
};
uchar reached6 [] = {
	  0,   0,   1,   1,   1,   1,   1,   1, 
	  0,   1,   0,   0, };
uchar *loopstate6;

#define nstates5	11	/* CtoA */
#define endstate5	10
short src_ln5 [] = {
	  0,   0,   0,   0,   0,   0,  56,  56, 
	 55,  59,  59,   0, };
S_F_MAP src_file5 [] = {
	{ "-", 0, 5 },
	{ "hanoi.4.pm", 6, 10 },
	{ "-", 11, 12 }
};
uchar reached5 [] = {
	  0,   0,   1,   1,   1,   1,   1,   1, 
	  0,   1,   0,   0, };
uchar *loopstate5;

#define nstates4	11	/* BtoC */
#define endstate4	10
short src_ln4 [] = {
	  0,   0,   0,   0,   0,   0,  48,  48, 
	 47,  51,  51,   0, };
S_F_MAP src_file4 [] = {
	{ "-", 0, 5 },
	{ "hanoi.4.pm", 6, 10 },
	{ "-", 11, 12 }
};
uchar reached4 [] = {
	  0,   0,   1,   1,   1,   1,   1,   1, 
	  0,   1,   0,   0, };
uchar *loopstate4;

#define nstates3	11	/* BtoA */
#define endstate3	10
short src_ln3 [] = {
	  0,   0,   0,   0,   0,   0,  40,  40, 
	 39,  43,  43,   0, };
S_F_MAP src_file3 [] = {
	{ "-", 0, 5 },
	{ "hanoi.4.pm", 6, 10 },
	{ "-", 11, 12 }
};
uchar reached3 [] = {
	  0,   0,   1,   1,   1,   1,   1,   1, 
	  0,   1,   0,   0, };
uchar *loopstate3;

#define nstates2	11	/* AtoC */
#define endstate2	10
short src_ln2 [] = {
	  0,   0,   0,   0,   0,   0,  32,  32, 
	 31,  35,  35,   0, };
S_F_MAP src_file2 [] = {
	{ "-", 0, 5 },
	{ "hanoi.4.pm", 6, 10 },
	{ "-", 11, 12 }
};
uchar reached2 [] = {
	  0,   0,   1,   1,   1,   1,   1,   1, 
	  0,   1,   0,   0, };
uchar *loopstate2;

#define nstates1	11	/* AtoB */
#define endstate1	10
short src_ln1 [] = {
	  0,   0,   0,   0,   0,   0,  24,  24, 
	 23,  27,  27,   0, };
S_F_MAP src_file1 [] = {
	{ "-", 0, 5 },
	{ "hanoi.4.pm", 6, 10 },
	{ "-", 11, 12 }
};
uchar reached1 [] = {
	  0,   0,   1,   1,   1,   1,   1,   1, 
	  0,   1,   0,   0, };
uchar *loopstate1;

#define nstates0	64	/* :init: */
#define endstate0	63
short src_ln0 [] = {
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,  10, 
	 13,  14,  15,  16,  17,  18,  12,  19, 
	  0, };
S_F_MAP src_file0 [] = {
	{ "-", 0, 54 },
	{ "hanoi.4.pm", 55, 63 },
	{ "-", 64, 65 }
};
uchar reached0 [] = {
	  0,   0,   1,   1,   1,   1,   1,   1, 
	  1,   1,   1,   1,   1,   1,   1,   1, 
	  1,   1,   1,   1,   1,   1,   1,   1, 
	  1,   1,   1,   1,   1,   1,   1,   1, 
	  1,   1,   1,   1,   1,   1,   1,   1, 
	  1,   1,   1,   1,   1,   1,   1,   1, 
	  1,   1,   1,   1,   1,   1,   1,   0, 
	  1,   0,   0,   0,   0,   0,   0,   0, 
	  0, };
uchar *loopstate0;
struct {
	int tp; short *src;
} src_all[] = {
	{ 6, &src_ln6[0] },
	{ 5, &src_ln5[0] },
	{ 4, &src_ln4[0] },
	{ 3, &src_ln3[0] },
	{ 2, &src_ln2[0] },
	{ 1, &src_ln1[0] },
	{ 0, &src_ln0[0] },
	{ 0, (short *) 0 }
};
short *frm_st0;
struct {
	char *c; char *t;
} code_lookup[] = {
	{ (char *) 0, "" }
};
#define _T5	23
#define _T2	24
#define T_ID	unsigned char
#define SYNC	0
#define ASYNC	0

#ifndef NCORE
	#ifdef DUAL_CORE
		#define NCORE	2
	#elif QUAD_CORE
		#define NCORE	4
	#else
		#define NCORE	1
	#endif
#endif
char *procname[] = {
   ":init:",
   "AtoB",
   "AtoC",
   "BtoA",
   "BtoC",
   "CtoA",
   "CtoB",
   ":np_:",
};

#define PCtoB	((P6 *)this)
typedef struct P6 { /* CtoB */
	unsigned _pid : 8;  /* 0..255 */
	unsigned _t   : 4; /* proctype */
	unsigned _p   : 7; /* state    */
} P6;
#define Air6	(sizeof(P6) - 3)
#define PCtoA	((P5 *)this)
typedef struct P5 { /* CtoA */
	unsigned _pid : 8;  /* 0..255 */
	unsigned _t   : 4; /* proctype */
	unsigned _p   : 7; /* state    */
} P5;
#define Air5	(sizeof(P5) - 3)
#define PBtoC	((P4 *)this)
typedef struct P4 { /* BtoC */
	unsigned _pid : 8;  /* 0..255 */
	unsigned _t   : 4; /* proctype */
	unsigned _p   : 7; /* state    */
} P4;
#define Air4	(sizeof(P4) - 3)
#define PBtoA	((P3 *)this)
typedef struct P3 { /* BtoA */
	unsigned _pid : 8;  /* 0..255 */
	unsigned _t   : 4; /* proctype */
	unsigned _p   : 7; /* state    */
} P3;
#define Air3	(sizeof(P3) - 3)
#define PAtoC	((P2 *)this)
typedef struct P2 { /* AtoC */
	unsigned _pid : 8;  /* 0..255 */
	unsigned _t   : 4; /* proctype */
	unsigned _p   : 7; /* state    */
} P2;
#define Air2	(sizeof(P2) - 3)
#define PAtoB	((P1 *)this)
typedef struct P1 { /* AtoB */
	unsigned _pid : 8;  /* 0..255 */
	unsigned _t   : 4; /* proctype */
	unsigned _p   : 7; /* state    */
} P1;
#define Air1	(sizeof(P1) - 3)
#define Pinit	((P0 *)this)
typedef struct P0 { /* :init: */
	unsigned _pid : 8;  /* 0..255 */
	unsigned _t   : 4; /* proctype */
	unsigned _p   : 7; /* state    */
} P0;
#define Air0	(sizeof(P0) - 3)
typedef struct P7 { /* np_ */
	unsigned _pid : 8;  /* 0..255 */
	unsigned _t   : 4; /* proctype */
	unsigned _p   : 7; /* state    */
} P7;
#define Air7	(sizeof(P7) - 3)
#if defined(BFS) && defined(REACH)
#undef REACH
#endif
#ifdef VERI
#define BASE	1
#else
#define BASE	0
#endif
typedef struct Trans {
	short atom;	/* if &2 = atomic trans; if &8 local */
#ifdef HAS_UNLESS
	short escp[HAS_UNLESS];	/* lists the escape states */
	short e_trans;	/* if set, this is an escp-trans */
#endif
	short tpe[2];	/* class of operation (for reduction) */
	short qu[6];	/* for conditional selections: qid's  */
	uchar ty[6];	/* ditto: type's */
#ifdef NIBIS
	short om;	/* completion status of preselects */
#endif
	char *tp;	/* src txt of statement */
	int st;		/* the nextstate */
	int t_id;	/* transition id, unique within proc */
	int forw;	/* index forward transition */
	int back;	/* index return  transition */
	struct Trans *nxt;
} Trans;

#define qptr(x)	(((uchar *)&now)+(int)q_offset[x])
#define pptr(x)	(((uchar *)&now)+(int)proc_offset[x])
extern uchar *Pptr(int);
#define q_sz(x)	(((Q0 *)qptr(x))->Qlen)

#ifndef VECTORSZ
#define VECTORSZ	1024           /* sv   size in bytes */
#endif

#ifdef VERBOSE
#ifndef CHECK
#define CHECK
#endif
#ifndef DEBUG
#define DEBUG
#endif
#endif
#ifdef SAFETY
#ifndef NOFAIR
#define NOFAIR
#endif
#endif
#ifdef NOREDUCE
#ifndef XUSAFE
#define XUSAFE
#endif
#if !defined(SAFETY) && !defined(MA)
#define FULLSTACK
#endif
#else
#ifdef BITSTATE
#if defined(SAFETY) && !defined(HASH64)
#define CNTRSTACK
#else
#define FULLSTACK
#endif
#else
#define FULLSTACK
#endif
#endif
#ifdef BITSTATE
#ifndef NOCOMP
#define NOCOMP
#endif
#if !defined(LC) && defined(SC)
#define LC
#endif
#endif
#if defined(COLLAPSE2) || defined(COLLAPSE3) || defined(COLLAPSE4)
/* accept the above for backward compatibility */
#define COLLAPSE
#endif
#ifdef HC
#undef HC
#define HC4
#endif
#ifdef HC0
#define HC	0
#endif
#ifdef HC1
#define HC	1
#endif
#ifdef HC2
#define HC	2
#endif
#ifdef HC3
#define HC	3
#endif
#ifdef HC4
#define HC	4
#endif
#ifdef COLLAPSE
#if NCORE>1 && !defined(SEP_STATE)
unsigned long *ncomps;	/* in shared memory */
#else
unsigned long ncomps[256+2];
#endif
#endif
#define MAXQ   	255
#define MAXPROC	255
#define WS		sizeof(void *) /* word size in bytes */
typedef struct Stack  {	 /* for queues and processes */
#if VECTORSZ>32000
	int o_delta;
	int o_offset;
	int o_skip;
	int o_delqs;
#else
	short o_delta;
	short o_offset;
	short o_skip;
	short o_delqs;
#endif
	short o_boq;
#ifndef XUSAFE
	char *o_name;
#endif
	char *body;
	struct Stack *nxt;
	struct Stack *lst;
} Stack;

typedef struct Svtack { /* for complete state vector */
#if VECTORSZ>32000
	int o_delta;
	int m_delta;
#else
	short o_delta;	 /* current size of frame */
	short m_delta;	 /* maximum size of frame */
#endif
#if SYNC
	short o_boq;
#endif
#define StackSize	(WS)
	char *body;
	struct Svtack *nxt;
	struct Svtack *lst;
} Svtack;

Trans ***trans;	/* 1 ptr per state per proctype */

struct H_el *Lstate;
int depthfound = -1;	/* loop detection */
#if VECTORSZ>32000
int proc_offset[MAXPROC];
int q_offset[MAXQ];
#else
short proc_offset[MAXPROC];
short q_offset[MAXQ];
#endif
uchar proc_skip[MAXPROC];
uchar q_skip[MAXQ];
unsigned long  vsize;	/* vector size in bytes */
#ifdef SVDUMP
int vprefix=0, svfd;		/* runtime option -pN */
#endif
char *tprefix = "trail";	/* runtime option -tsuffix */
short boq = -1;		/* blocked_on_queue status */
typedef struct State {
	uchar _nr_pr;
	uchar _nr_qs;
	uchar   _a_t;	/* cycle detection */
#ifndef NOFAIR
	uchar   _cnt[NFAIR];	/* counters, weak fairness */
#endif
#ifndef NOVSZ
#if VECTORSZ<65536
	unsigned short _vsz;
#else
	unsigned long  _vsz;
#endif
#endif
#ifdef HAS_LAST
	uchar  _last;	/* pid executed in last step */
#endif
#ifdef EVENT_TRACE
#if nstates_event<256
	uchar _event;
#else
	unsigned short _event;
#endif
#endif
	uchar a[18];
	uchar a_act;
	uchar b[18];
	uchar b_act;
	uchar c[18];
	uchar c_act;
	uchar sv[VECTORSZ];
} State;

#define HAS_TRACK	0
int _; /* a predefined write-only variable */

#define FORWARD_MOVES	"pan.m"
#define REVERSE_MOVES	"pan.b"
#define TRANSITIONS	"pan.t"
#define _NP_	7
uchar reached7[3];  /* np_ */
uchar *loopstate7;  /* np_ */
#define nstates7	3 /* np_ */
#define endstate7	2 /* np_ */

#define start7	0 /* np_ */
#define start6	8
#define start5	8
#define start4	8
#define start3	8
#define start2	8
#define start1	8
#define start0	55
#ifdef NP
	#define ACCEPT_LAB	1 /* at least 1 in np_ */
#else
	#define ACCEPT_LAB	0 /* user-defined accept labels */
#endif
#ifdef MEMCNT
	#ifdef MEMLIM
		#warning -DMEMLIM takes precedence over -DMEMCNT
		#undef MEMCNT
	#else
		#if MEMCNT<20
			#warning using minimal value -DMEMCNT=20 (=1MB)
			#define MEMLIM	(1)
			#undef MEMCNT
		#else
			#if MEMCNT==20
				#define MEMLIM	(1)
				#undef MEMCNT
			#else
			 #if MEMCNT>=50
			  #error excessive value for MEMCNT
			 #else
				#define MEMLIM	(1<<(MEMCNT-20))
			 #endif
			#endif
		#endif
	#endif
#endif
#if NCORE>1 && !defined(MEMLIM)
	#define MEMLIM	(2048)	/* need a default, using 2 GB */
#endif
#define PROG_LAB	0 /* progress labels */
uchar *accpstate[8];
uchar *progstate[8];
uchar *loopstate[8];
uchar *reached[8];
uchar *stopstate[8];
uchar *visstate[8];
short *mapstate[8];
#ifdef HAS_CODE
int NrStates[8];
#endif
#define NQS	0
short q_flds[1];
short q_max[1];
typedef struct Q0 {	/* generic q */
	uchar Qlen;	/* q_size */
	uchar _t;
} Q0;

/** function prototypes **/
char *emalloc(unsigned long);
char *Malloc(unsigned long);
int Boundcheck(int, int, int, int, Trans *);
int addqueue(int, int);
/* int atoi(char *); */
/* int abort(void); */
int close(int);
int delproc(int, int);
int endstate(void);
int hstore(char *, int);
#ifdef MA
int gstore(char *, int, uchar);
#endif
int q_cond(short, Trans *);
int q_full(int);
int q_len(int);
int q_zero(int);
int qrecv(int, int, int, int);
int unsend(int);
/* void *sbrk(int); */
void Uerror(char *);
void assert(int, char *, int, int, Trans *);
void c_chandump(int);
void c_globals(void);
void c_locals(int, int);
void checkcycles(void);
void crack(int, int, Trans *, short *);
void d_sfh(const char *, int);
void sfh(const char *, int);
void d_hash(uchar *, int);
void s_hash(uchar *, int);
void r_hash(uchar *, int);
void delq(int);
void do_reach(void);
void pan_exit(int);
void exit(int);
void hinit(void);
void imed(Trans *, int, int, int);
void new_state(void);
void p_restor(int);
void putpeg(int, int);
void putrail(void);
void q_restor(void);
void retrans(int, int, int, short *, uchar *, uchar *);
void settable(void);
void setq_claim(int, int, char *, int, char *);
void sv_restor(void);
void sv_save(void);
void tagtable(int, int, int, short *, uchar *);
void do_dfs(int, int, int, short *, uchar *, uchar *);
void uerror(char *);
void unrecv(int, int, int, int, int);
void usage(FILE *);
void wrap_stats(void);
#if defined(FULLSTACK) && defined(BITSTATE)
int  onstack_now(void);
void onstack_init(void);
void onstack_put(void);
void onstack_zap(void);
#endif
#ifndef XUSAFE
int q_S_check(int, int);
int q_R_check(int, int);
uchar q_claim[MAXQ+1];
char *q_name[MAXQ+1];
char *p_name[MAXPROC+1];
#endif
void qsend(int, int, int);
#define Addproc(x)	addproc(x)
#define LOCAL	1
#define Q_FULL_F	2
#define Q_EMPT_F	3
#define Q_EMPT_T	4
#define Q_FULL_T	5
#define TIMEOUT_F	6
#define GLOBAL	7
#define BAD	8
#define ALPHA_F	9
#define NTRANS	25
#ifdef PEG
long peg[NTRANS];
#endif
