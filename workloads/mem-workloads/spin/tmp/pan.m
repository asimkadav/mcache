#define rand	pan_rand
#if defined(HAS_CODE) && defined(VERBOSE)
	cpu_printf("Pr: %d Tr: %d\n", II, t->forw);
#endif
	switch (t->forw) {
	default: Uerror("bad forward move");
	case 0:	/* if without executable clauses */
		continue;
	case 1: /* generic 'goto' or 'skip' */
		IfNotBlocked
		_m = 3; goto P999;
	case 2: /* generic 'else' */
		IfNotBlocked
		if (trpt->o_pm&1) continue;
		_m = 3; goto P999;

		 /* PROC P_4 */
	case 3: /* STATE 4 - line 138 "test-models/bakery.8.pm" - [D_STEP] */
		IfNotBlocked
		sv_save();
		reached[4][4] = 1;
		reached[4][t->st] = 1;
		reached[4][tt] = 1;
S_164_0: /* 2 */
		now.choosing[4] = 1;
#ifdef VAR_RANGES
		logval("choosing[4]", ((int)now.choosing[4]));
#endif
		;
S_165_0: /* 2 */
		((P4 *)this)->j = 0;
#ifdef VAR_RANGES
		logval("P_4:j", ((int)((P4 *)this)->j));
#endif
		;
S_166_0: /* 2 */
		((P4 *)this)->max = 0;
#ifdef VAR_RANGES
		logval("P_4:max", ((int)((P4 *)this)->max));
#endif
		;
		goto S_168_0;
S_168_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 4: /* STATE 11 - line 142 "test-models/bakery.8.pm" - [D_STEP] */
		if (!(boq == -1 && ((((int)((P4 *)this)->j)<5)&&(((int)now.number[ Index(((int)((P4 *)this)->j), 5) ])>((int)((P4 *)this)->max)))))
			continue;
		sv_save();
		reached[4][11] = 1;
		reached[4][t->st] = 1;
		reached[4][tt] = 1;
S_171_0: /* 2 */
		if (!(((((int)((P4 *)this)->j)<5)&&(((int)now.number[ Index(((int)((P4 *)this)->j), 5) ])>((int)((P4 *)this)->max)))))
			Uerror("block in step seq");
S_172_0: /* 2 */
		((P4 *)this)->max = ((int)now.number[ Index(((int)((P4 *)this)->j), 5) ]);
#ifdef VAR_RANGES
		logval("P_4:max", ((int)((P4 *)this)->max));
#endif
		;
S_173_0: /* 2 */
		((P4 *)this)->j = (((int)((P4 *)this)->j)+1);
#ifdef VAR_RANGES
		logval("P_4:j", ((int)((P4 *)this)->j));
#endif
		;
		goto S_175_0;
S_175_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 5: /* STATE 15 - line 144 "test-models/bakery.8.pm" - [D_STEP] */
		if (!(boq == -1 && ((((int)((P4 *)this)->j)<5)&&(((int)now.number[ Index(((int)((P4 *)this)->j), 5) ])<=((int)((P4 *)this)->max)))))
			continue;
		sv_save();
		reached[4][15] = 1;
		reached[4][t->st] = 1;
		reached[4][tt] = 1;
S_176_0: /* 2 */
		if (!(((((int)((P4 *)this)->j)<5)&&(((int)now.number[ Index(((int)((P4 *)this)->j), 5) ])<=((int)((P4 *)this)->max)))))
			Uerror("block in step seq");
S_177_0: /* 2 */
		((P4 *)this)->j = (((int)((P4 *)this)->j)+1);
#ifdef VAR_RANGES
		logval("P_4:j", ((int)((P4 *)this)->j));
#endif
		;
		goto S_179_0;
S_179_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 6: /* STATE 21 - line 146 "test-models/bakery.8.pm" - [D_STEP] */
		if (!(boq == -1 && ((((int)((P4 *)this)->j)==5)&&(((int)((P4 *)this)->max)<5))))
			continue;
		sv_save();
		reached[4][21] = 1;
		reached[4][t->st] = 1;
		reached[4][tt] = 1;
S_180_0: /* 2 */
		if (!(((((int)((P4 *)this)->j)==5)&&(((int)((P4 *)this)->max)<5))))
			Uerror("block in step seq");
S_181_0: /* 2 */
		now.number[4] = (((int)((P4 *)this)->max)+1);
#ifdef VAR_RANGES
		logval("number[4]", ((int)now.number[4]));
#endif
		;
S_182_0: /* 2 */
		((P4 *)this)->j = 0;
#ifdef VAR_RANGES
		logval("P_4:j", ((int)((P4 *)this)->j));
#endif
		;
S_183_0: /* 2 */
		now.choosing[4] = 0;
#ifdef VAR_RANGES
		logval("choosing[4]", ((int)now.choosing[4]));
#endif
		;
		goto S_185_0;
S_185_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 7: /* STATE 25 - line 150 "test-models/bakery.8.pm" - [(((j<5)&&(choosing[j]==0)))] (0:0:0 - 1) */
		IfNotBlocked
		reached[4][25] = 1;
		if (!(((((int)((P4 *)this)->j)<5)&&(((int)now.choosing[ Index(((int)((P4 *)this)->j), 5) ])==0))))
			continue;
		_m = 3; goto P999; /* 0 */
	case 8: /* STATE 27 - line 152 "test-models/bakery.8.pm" - [((j==5))] (0:0:1 - 1) */
		IfNotBlocked
		reached[4][27] = 1;
		if (!((((int)((P4 *)this)->j)==5)))
			continue;
		/* dead 1: j */  (trpt+1)->bup.oval = ((P4 *)this)->j;
#ifdef HAS_CODE
		if (!readtrail)
#endif
			((P4 *)this)->j = 0;
		_m = 3; goto P999; /* 0 */
	case 9: /* STATE 33 - line 156 "test-models/bakery.8.pm" - [D_STEP] */
		if (!(boq == -1 && (((((int)now.number[ Index(((int)((P4 *)this)->j), 5) ])==0)||(((int)now.number[ Index(((int)((P4 *)this)->j), 5) ])>((int)now.number[4])))||((((int)now.number[ Index(((int)((P4 *)this)->j), 5) ])==((int)now.number[4]))&&(4<=((int)((P4 *)this)->j))))))
			continue;
		sv_save();
		reached[4][33] = 1;
		reached[4][t->st] = 1;
		reached[4][tt] = 1;
S_194_0: /* 2 */
		if (!((((((int)now.number[ Index(((int)((P4 *)this)->j), 5) ])==0)||(((int)now.number[ Index(((int)((P4 *)this)->j), 5) ])>((int)now.number[4])))||((((int)now.number[ Index(((int)((P4 *)this)->j), 5) ])==((int)now.number[4]))&&(4<=((int)((P4 *)this)->j))))))
			Uerror("block in step seq");
S_195_0: /* 2 */
		((P4 *)this)->j = (((int)((P4 *)this)->j)+1);
#ifdef VAR_RANGES
		logval("P_4:j", ((int)((P4 *)this)->j));
#endif
		;
		goto S_197_0;
S_197_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 10: /* STATE 37 - line 160 "test-models/bakery.8.pm" - [number[4] = 0] (0:0:1 - 1) */
		IfNotBlocked
		reached[4][37] = 1;
		(trpt+1)->bup.oval = ((int)now.number[4]);
		now.number[4] = 0;
#ifdef VAR_RANGES
		logval("number[4]", ((int)now.number[4]));
#endif
		;
		_m = 3; goto P999; /* 0 */
	case 11: /* STATE 41 - line 163 "test-models/bakery.8.pm" - [-end-] (0:0:0 - 1) */
		IfNotBlocked
		reached[4][41] = 1;
		if (!delproc(1, II)) continue;
		_m = 3; goto P999; /* 0 */

		 /* PROC P_3 */
	case 12: /* STATE 4 - line 106 "test-models/bakery.8.pm" - [D_STEP] */
		IfNotBlocked
		sv_save();
		reached[3][4] = 1;
		reached[3][t->st] = 1;
		reached[3][tt] = 1;
S_123_0: /* 2 */
		now.choosing[3] = 1;
#ifdef VAR_RANGES
		logval("choosing[3]", ((int)now.choosing[3]));
#endif
		;
S_124_0: /* 2 */
		((P3 *)this)->j = 0;
#ifdef VAR_RANGES
		logval("P_3:j", ((int)((P3 *)this)->j));
#endif
		;
S_125_0: /* 2 */
		((P3 *)this)->max = 0;
#ifdef VAR_RANGES
		logval("P_3:max", ((int)((P3 *)this)->max));
#endif
		;
		goto S_127_0;
S_127_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 13: /* STATE 11 - line 110 "test-models/bakery.8.pm" - [D_STEP] */
		if (!(boq == -1 && ((((int)((P3 *)this)->j)<5)&&(((int)now.number[ Index(((int)((P3 *)this)->j), 5) ])>((int)((P3 *)this)->max)))))
			continue;
		sv_save();
		reached[3][11] = 1;
		reached[3][t->st] = 1;
		reached[3][tt] = 1;
S_130_0: /* 2 */
		if (!(((((int)((P3 *)this)->j)<5)&&(((int)now.number[ Index(((int)((P3 *)this)->j), 5) ])>((int)((P3 *)this)->max)))))
			Uerror("block in step seq");
S_131_0: /* 2 */
		((P3 *)this)->max = ((int)now.number[ Index(((int)((P3 *)this)->j), 5) ]);
#ifdef VAR_RANGES
		logval("P_3:max", ((int)((P3 *)this)->max));
#endif
		;
S_132_0: /* 2 */
		((P3 *)this)->j = (((int)((P3 *)this)->j)+1);
#ifdef VAR_RANGES
		logval("P_3:j", ((int)((P3 *)this)->j));
#endif
		;
		goto S_134_0;
S_134_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 14: /* STATE 15 - line 112 "test-models/bakery.8.pm" - [D_STEP] */
		if (!(boq == -1 && ((((int)((P3 *)this)->j)<5)&&(((int)now.number[ Index(((int)((P3 *)this)->j), 5) ])<=((int)((P3 *)this)->max)))))
			continue;
		sv_save();
		reached[3][15] = 1;
		reached[3][t->st] = 1;
		reached[3][tt] = 1;
S_135_0: /* 2 */
		if (!(((((int)((P3 *)this)->j)<5)&&(((int)now.number[ Index(((int)((P3 *)this)->j), 5) ])<=((int)((P3 *)this)->max)))))
			Uerror("block in step seq");
S_136_0: /* 2 */
		((P3 *)this)->j = (((int)((P3 *)this)->j)+1);
#ifdef VAR_RANGES
		logval("P_3:j", ((int)((P3 *)this)->j));
#endif
		;
		goto S_138_0;
S_138_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 15: /* STATE 21 - line 114 "test-models/bakery.8.pm" - [D_STEP] */
		if (!(boq == -1 && ((((int)((P3 *)this)->j)==5)&&(((int)((P3 *)this)->max)<5))))
			continue;
		sv_save();
		reached[3][21] = 1;
		reached[3][t->st] = 1;
		reached[3][tt] = 1;
S_139_0: /* 2 */
		if (!(((((int)((P3 *)this)->j)==5)&&(((int)((P3 *)this)->max)<5))))
			Uerror("block in step seq");
S_140_0: /* 2 */
		now.number[3] = (((int)((P3 *)this)->max)+1);
#ifdef VAR_RANGES
		logval("number[3]", ((int)now.number[3]));
#endif
		;
S_141_0: /* 2 */
		((P3 *)this)->j = 0;
#ifdef VAR_RANGES
		logval("P_3:j", ((int)((P3 *)this)->j));
#endif
		;
S_142_0: /* 2 */
		now.choosing[3] = 0;
#ifdef VAR_RANGES
		logval("choosing[3]", ((int)now.choosing[3]));
#endif
		;
		goto S_144_0;
S_144_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 16: /* STATE 25 - line 118 "test-models/bakery.8.pm" - [(((j<5)&&(choosing[j]==0)))] (0:0:0 - 1) */
		IfNotBlocked
		reached[3][25] = 1;
		if (!(((((int)((P3 *)this)->j)<5)&&(((int)now.choosing[ Index(((int)((P3 *)this)->j), 5) ])==0))))
			continue;
		_m = 3; goto P999; /* 0 */
	case 17: /* STATE 27 - line 120 "test-models/bakery.8.pm" - [((j==5))] (0:0:1 - 1) */
		IfNotBlocked
		reached[3][27] = 1;
		if (!((((int)((P3 *)this)->j)==5)))
			continue;
		/* dead 1: j */  (trpt+1)->bup.oval = ((P3 *)this)->j;
#ifdef HAS_CODE
		if (!readtrail)
#endif
			((P3 *)this)->j = 0;
		_m = 3; goto P999; /* 0 */
	case 18: /* STATE 33 - line 124 "test-models/bakery.8.pm" - [D_STEP] */
		if (!(boq == -1 && (((((int)now.number[ Index(((int)((P3 *)this)->j), 5) ])==0)||(((int)now.number[ Index(((int)((P3 *)this)->j), 5) ])>((int)now.number[3])))||((((int)now.number[ Index(((int)((P3 *)this)->j), 5) ])==((int)now.number[3]))&&(3<=((int)((P3 *)this)->j))))))
			continue;
		sv_save();
		reached[3][33] = 1;
		reached[3][t->st] = 1;
		reached[3][tt] = 1;
S_153_0: /* 2 */
		if (!((((((int)now.number[ Index(((int)((P3 *)this)->j), 5) ])==0)||(((int)now.number[ Index(((int)((P3 *)this)->j), 5) ])>((int)now.number[3])))||((((int)now.number[ Index(((int)((P3 *)this)->j), 5) ])==((int)now.number[3]))&&(3<=((int)((P3 *)this)->j))))))
			Uerror("block in step seq");
S_154_0: /* 2 */
		((P3 *)this)->j = (((int)((P3 *)this)->j)+1);
#ifdef VAR_RANGES
		logval("P_3:j", ((int)((P3 *)this)->j));
#endif
		;
		goto S_156_0;
S_156_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 19: /* STATE 37 - line 128 "test-models/bakery.8.pm" - [number[3] = 0] (0:0:1 - 1) */
		IfNotBlocked
		reached[3][37] = 1;
		(trpt+1)->bup.oval = ((int)now.number[3]);
		now.number[3] = 0;
#ifdef VAR_RANGES
		logval("number[3]", ((int)now.number[3]));
#endif
		;
		_m = 3; goto P999; /* 0 */
	case 20: /* STATE 41 - line 131 "test-models/bakery.8.pm" - [-end-] (0:0:0 - 1) */
		IfNotBlocked
		reached[3][41] = 1;
		if (!delproc(1, II)) continue;
		_m = 3; goto P999; /* 0 */

		 /* PROC P_2 */
	case 21: /* STATE 4 - line 74 "test-models/bakery.8.pm" - [D_STEP] */
		IfNotBlocked
		sv_save();
		reached[2][4] = 1;
		reached[2][t->st] = 1;
		reached[2][tt] = 1;
S_082_0: /* 2 */
		now.choosing[2] = 1;
#ifdef VAR_RANGES
		logval("choosing[2]", ((int)now.choosing[2]));
#endif
		;
S_083_0: /* 2 */
		((P2 *)this)->j = 0;
#ifdef VAR_RANGES
		logval("P_2:j", ((int)((P2 *)this)->j));
#endif
		;
S_084_0: /* 2 */
		((P2 *)this)->max = 0;
#ifdef VAR_RANGES
		logval("P_2:max", ((int)((P2 *)this)->max));
#endif
		;
		goto S_086_0;
S_086_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 22: /* STATE 11 - line 78 "test-models/bakery.8.pm" - [D_STEP] */
		if (!(boq == -1 && ((((int)((P2 *)this)->j)<5)&&(((int)now.number[ Index(((int)((P2 *)this)->j), 5) ])>((int)((P2 *)this)->max)))))
			continue;
		sv_save();
		reached[2][11] = 1;
		reached[2][t->st] = 1;
		reached[2][tt] = 1;
S_089_0: /* 2 */
		if (!(((((int)((P2 *)this)->j)<5)&&(((int)now.number[ Index(((int)((P2 *)this)->j), 5) ])>((int)((P2 *)this)->max)))))
			Uerror("block in step seq");
S_090_0: /* 2 */
		((P2 *)this)->max = ((int)now.number[ Index(((int)((P2 *)this)->j), 5) ]);
#ifdef VAR_RANGES
		logval("P_2:max", ((int)((P2 *)this)->max));
#endif
		;
S_091_0: /* 2 */
		((P2 *)this)->j = (((int)((P2 *)this)->j)+1);
#ifdef VAR_RANGES
		logval("P_2:j", ((int)((P2 *)this)->j));
#endif
		;
		goto S_093_0;
S_093_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 23: /* STATE 15 - line 80 "test-models/bakery.8.pm" - [D_STEP] */
		if (!(boq == -1 && ((((int)((P2 *)this)->j)<5)&&(((int)now.number[ Index(((int)((P2 *)this)->j), 5) ])<=((int)((P2 *)this)->max)))))
			continue;
		sv_save();
		reached[2][15] = 1;
		reached[2][t->st] = 1;
		reached[2][tt] = 1;
S_094_0: /* 2 */
		if (!(((((int)((P2 *)this)->j)<5)&&(((int)now.number[ Index(((int)((P2 *)this)->j), 5) ])<=((int)((P2 *)this)->max)))))
			Uerror("block in step seq");
S_095_0: /* 2 */
		((P2 *)this)->j = (((int)((P2 *)this)->j)+1);
#ifdef VAR_RANGES
		logval("P_2:j", ((int)((P2 *)this)->j));
#endif
		;
		goto S_097_0;
S_097_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 24: /* STATE 21 - line 82 "test-models/bakery.8.pm" - [D_STEP] */
		if (!(boq == -1 && ((((int)((P2 *)this)->j)==5)&&(((int)((P2 *)this)->max)<5))))
			continue;
		sv_save();
		reached[2][21] = 1;
		reached[2][t->st] = 1;
		reached[2][tt] = 1;
S_098_0: /* 2 */
		if (!(((((int)((P2 *)this)->j)==5)&&(((int)((P2 *)this)->max)<5))))
			Uerror("block in step seq");
S_099_0: /* 2 */
		now.number[2] = (((int)((P2 *)this)->max)+1);
#ifdef VAR_RANGES
		logval("number[2]", ((int)now.number[2]));
#endif
		;
S_100_0: /* 2 */
		((P2 *)this)->j = 0;
#ifdef VAR_RANGES
		logval("P_2:j", ((int)((P2 *)this)->j));
#endif
		;
S_101_0: /* 2 */
		now.choosing[2] = 0;
#ifdef VAR_RANGES
		logval("choosing[2]", ((int)now.choosing[2]));
#endif
		;
		goto S_103_0;
S_103_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 25: /* STATE 25 - line 86 "test-models/bakery.8.pm" - [(((j<5)&&(choosing[j]==0)))] (0:0:0 - 1) */
		IfNotBlocked
		reached[2][25] = 1;
		if (!(((((int)((P2 *)this)->j)<5)&&(((int)now.choosing[ Index(((int)((P2 *)this)->j), 5) ])==0))))
			continue;
		_m = 3; goto P999; /* 0 */
	case 26: /* STATE 27 - line 88 "test-models/bakery.8.pm" - [((j==5))] (0:0:1 - 1) */
		IfNotBlocked
		reached[2][27] = 1;
		if (!((((int)((P2 *)this)->j)==5)))
			continue;
		/* dead 1: j */  (trpt+1)->bup.oval = ((P2 *)this)->j;
#ifdef HAS_CODE
		if (!readtrail)
#endif
			((P2 *)this)->j = 0;
		_m = 3; goto P999; /* 0 */
	case 27: /* STATE 33 - line 92 "test-models/bakery.8.pm" - [D_STEP] */
		if (!(boq == -1 && (((((int)now.number[ Index(((int)((P2 *)this)->j), 5) ])==0)||(((int)now.number[ Index(((int)((P2 *)this)->j), 5) ])>((int)now.number[2])))||((((int)now.number[ Index(((int)((P2 *)this)->j), 5) ])==((int)now.number[2]))&&(2<=((int)((P2 *)this)->j))))))
			continue;
		sv_save();
		reached[2][33] = 1;
		reached[2][t->st] = 1;
		reached[2][tt] = 1;
S_112_0: /* 2 */
		if (!((((((int)now.number[ Index(((int)((P2 *)this)->j), 5) ])==0)||(((int)now.number[ Index(((int)((P2 *)this)->j), 5) ])>((int)now.number[2])))||((((int)now.number[ Index(((int)((P2 *)this)->j), 5) ])==((int)now.number[2]))&&(2<=((int)((P2 *)this)->j))))))
			Uerror("block in step seq");
S_113_0: /* 2 */
		((P2 *)this)->j = (((int)((P2 *)this)->j)+1);
#ifdef VAR_RANGES
		logval("P_2:j", ((int)((P2 *)this)->j));
#endif
		;
		goto S_115_0;
S_115_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 28: /* STATE 37 - line 96 "test-models/bakery.8.pm" - [number[2] = 0] (0:0:1 - 1) */
		IfNotBlocked
		reached[2][37] = 1;
		(trpt+1)->bup.oval = ((int)now.number[2]);
		now.number[2] = 0;
#ifdef VAR_RANGES
		logval("number[2]", ((int)now.number[2]));
#endif
		;
		_m = 3; goto P999; /* 0 */
	case 29: /* STATE 41 - line 99 "test-models/bakery.8.pm" - [-end-] (0:0:0 - 1) */
		IfNotBlocked
		reached[2][41] = 1;
		if (!delproc(1, II)) continue;
		_m = 3; goto P999; /* 0 */

		 /* PROC P_1 */
	case 30: /* STATE 4 - line 42 "test-models/bakery.8.pm" - [D_STEP] */
		IfNotBlocked
		sv_save();
		reached[1][4] = 1;
		reached[1][t->st] = 1;
		reached[1][tt] = 1;
S_041_0: /* 2 */
		now.choosing[1] = 1;
#ifdef VAR_RANGES
		logval("choosing[1]", ((int)now.choosing[1]));
#endif
		;
S_042_0: /* 2 */
		((P1 *)this)->j = 0;
#ifdef VAR_RANGES
		logval("P_1:j", ((int)((P1 *)this)->j));
#endif
		;
S_043_0: /* 2 */
		((P1 *)this)->max = 0;
#ifdef VAR_RANGES
		logval("P_1:max", ((int)((P1 *)this)->max));
#endif
		;
		goto S_045_0;
S_045_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 31: /* STATE 11 - line 46 "test-models/bakery.8.pm" - [D_STEP] */
		if (!(boq == -1 && ((((int)((P1 *)this)->j)<5)&&(((int)now.number[ Index(((int)((P1 *)this)->j), 5) ])>((int)((P1 *)this)->max)))))
			continue;
		sv_save();
		reached[1][11] = 1;
		reached[1][t->st] = 1;
		reached[1][tt] = 1;
S_048_0: /* 2 */
		if (!(((((int)((P1 *)this)->j)<5)&&(((int)now.number[ Index(((int)((P1 *)this)->j), 5) ])>((int)((P1 *)this)->max)))))
			Uerror("block in step seq");
S_049_0: /* 2 */
		((P1 *)this)->max = ((int)now.number[ Index(((int)((P1 *)this)->j), 5) ]);
#ifdef VAR_RANGES
		logval("P_1:max", ((int)((P1 *)this)->max));
#endif
		;
S_050_0: /* 2 */
		((P1 *)this)->j = (((int)((P1 *)this)->j)+1);
#ifdef VAR_RANGES
		logval("P_1:j", ((int)((P1 *)this)->j));
#endif
		;
		goto S_052_0;
S_052_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 32: /* STATE 15 - line 48 "test-models/bakery.8.pm" - [D_STEP] */
		if (!(boq == -1 && ((((int)((P1 *)this)->j)<5)&&(((int)now.number[ Index(((int)((P1 *)this)->j), 5) ])<=((int)((P1 *)this)->max)))))
			continue;
		sv_save();
		reached[1][15] = 1;
		reached[1][t->st] = 1;
		reached[1][tt] = 1;
S_053_0: /* 2 */
		if (!(((((int)((P1 *)this)->j)<5)&&(((int)now.number[ Index(((int)((P1 *)this)->j), 5) ])<=((int)((P1 *)this)->max)))))
			Uerror("block in step seq");
S_054_0: /* 2 */
		((P1 *)this)->j = (((int)((P1 *)this)->j)+1);
#ifdef VAR_RANGES
		logval("P_1:j", ((int)((P1 *)this)->j));
#endif
		;
		goto S_056_0;
S_056_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 33: /* STATE 21 - line 50 "test-models/bakery.8.pm" - [D_STEP] */
		if (!(boq == -1 && ((((int)((P1 *)this)->j)==5)&&(((int)((P1 *)this)->max)<5))))
			continue;
		sv_save();
		reached[1][21] = 1;
		reached[1][t->st] = 1;
		reached[1][tt] = 1;
S_057_0: /* 2 */
		if (!(((((int)((P1 *)this)->j)==5)&&(((int)((P1 *)this)->max)<5))))
			Uerror("block in step seq");
S_058_0: /* 2 */
		now.number[1] = (((int)((P1 *)this)->max)+1);
#ifdef VAR_RANGES
		logval("number[1]", ((int)now.number[1]));
#endif
		;
S_059_0: /* 2 */
		((P1 *)this)->j = 0;
#ifdef VAR_RANGES
		logval("P_1:j", ((int)((P1 *)this)->j));
#endif
		;
S_060_0: /* 2 */
		now.choosing[1] = 0;
#ifdef VAR_RANGES
		logval("choosing[1]", ((int)now.choosing[1]));
#endif
		;
		goto S_062_0;
S_062_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 34: /* STATE 25 - line 54 "test-models/bakery.8.pm" - [(((j<5)&&(choosing[j]==0)))] (0:0:0 - 1) */
		IfNotBlocked
		reached[1][25] = 1;
		if (!(((((int)((P1 *)this)->j)<5)&&(((int)now.choosing[ Index(((int)((P1 *)this)->j), 5) ])==0))))
			continue;
		_m = 3; goto P999; /* 0 */
	case 35: /* STATE 27 - line 56 "test-models/bakery.8.pm" - [((j==5))] (0:0:1 - 1) */
		IfNotBlocked
		reached[1][27] = 1;
		if (!((((int)((P1 *)this)->j)==5)))
			continue;
		/* dead 1: j */  (trpt+1)->bup.oval = ((P1 *)this)->j;
#ifdef HAS_CODE
		if (!readtrail)
#endif
			((P1 *)this)->j = 0;
		_m = 3; goto P999; /* 0 */
	case 36: /* STATE 33 - line 60 "test-models/bakery.8.pm" - [D_STEP] */
		if (!(boq == -1 && (((((int)now.number[ Index(((int)((P1 *)this)->j), 5) ])==0)||(((int)now.number[ Index(((int)((P1 *)this)->j), 5) ])>((int)now.number[1])))||((((int)now.number[ Index(((int)((P1 *)this)->j), 5) ])==((int)now.number[1]))&&(1<=((int)((P1 *)this)->j))))))
			continue;
		sv_save();
		reached[1][33] = 1;
		reached[1][t->st] = 1;
		reached[1][tt] = 1;
S_071_0: /* 2 */
		if (!((((((int)now.number[ Index(((int)((P1 *)this)->j), 5) ])==0)||(((int)now.number[ Index(((int)((P1 *)this)->j), 5) ])>((int)now.number[1])))||((((int)now.number[ Index(((int)((P1 *)this)->j), 5) ])==((int)now.number[1]))&&(1<=((int)((P1 *)this)->j))))))
			Uerror("block in step seq");
S_072_0: /* 2 */
		((P1 *)this)->j = (((int)((P1 *)this)->j)+1);
#ifdef VAR_RANGES
		logval("P_1:j", ((int)((P1 *)this)->j));
#endif
		;
		goto S_074_0;
S_074_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 37: /* STATE 37 - line 64 "test-models/bakery.8.pm" - [number[1] = 0] (0:0:1 - 1) */
		IfNotBlocked
		reached[1][37] = 1;
		(trpt+1)->bup.oval = ((int)now.number[1]);
		now.number[1] = 0;
#ifdef VAR_RANGES
		logval("number[1]", ((int)now.number[1]));
#endif
		;
		_m = 3; goto P999; /* 0 */
	case 38: /* STATE 41 - line 67 "test-models/bakery.8.pm" - [-end-] (0:0:0 - 1) */
		IfNotBlocked
		reached[1][41] = 1;
		if (!delproc(1, II)) continue;
		_m = 3; goto P999; /* 0 */

		 /* PROC P_0 */
	case 39: /* STATE 4 - line 10 "test-models/bakery.8.pm" - [D_STEP] */
		IfNotBlocked
		sv_save();
		reached[0][4] = 1;
		reached[0][t->st] = 1;
		reached[0][tt] = 1;
S_000_0: /* 2 */
		now.choosing[0] = 1;
#ifdef VAR_RANGES
		logval("choosing[0]", ((int)now.choosing[0]));
#endif
		;
S_001_0: /* 2 */
		((P0 *)this)->j = 0;
#ifdef VAR_RANGES
		logval("P_0:j", ((int)((P0 *)this)->j));
#endif
		;
S_002_0: /* 2 */
		((P0 *)this)->max = 0;
#ifdef VAR_RANGES
		logval("P_0:max", ((int)((P0 *)this)->max));
#endif
		;
		goto S_004_0;
S_004_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 40: /* STATE 11 - line 14 "test-models/bakery.8.pm" - [D_STEP] */
		if (!(boq == -1 && ((((int)((P0 *)this)->j)<5)&&(((int)now.number[ Index(((int)((P0 *)this)->j), 5) ])>((int)((P0 *)this)->max)))))
			continue;
		sv_save();
		reached[0][11] = 1;
		reached[0][t->st] = 1;
		reached[0][tt] = 1;
S_007_0: /* 2 */
		if (!(((((int)((P0 *)this)->j)<5)&&(((int)now.number[ Index(((int)((P0 *)this)->j), 5) ])>((int)((P0 *)this)->max)))))
			Uerror("block in step seq");
S_008_0: /* 2 */
		((P0 *)this)->max = ((int)now.number[ Index(((int)((P0 *)this)->j), 5) ]);
#ifdef VAR_RANGES
		logval("P_0:max", ((int)((P0 *)this)->max));
#endif
		;
S_009_0: /* 2 */
		((P0 *)this)->j = (((int)((P0 *)this)->j)+1);
#ifdef VAR_RANGES
		logval("P_0:j", ((int)((P0 *)this)->j));
#endif
		;
		goto S_011_0;
S_011_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 41: /* STATE 15 - line 16 "test-models/bakery.8.pm" - [D_STEP] */
		if (!(boq == -1 && ((((int)((P0 *)this)->j)<5)&&(((int)now.number[ Index(((int)((P0 *)this)->j), 5) ])<=((int)((P0 *)this)->max)))))
			continue;
		sv_save();
		reached[0][15] = 1;
		reached[0][t->st] = 1;
		reached[0][tt] = 1;
S_012_0: /* 2 */
		if (!(((((int)((P0 *)this)->j)<5)&&(((int)now.number[ Index(((int)((P0 *)this)->j), 5) ])<=((int)((P0 *)this)->max)))))
			Uerror("block in step seq");
S_013_0: /* 2 */
		((P0 *)this)->j = (((int)((P0 *)this)->j)+1);
#ifdef VAR_RANGES
		logval("P_0:j", ((int)((P0 *)this)->j));
#endif
		;
		goto S_015_0;
S_015_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 42: /* STATE 21 - line 18 "test-models/bakery.8.pm" - [D_STEP] */
		if (!(boq == -1 && ((((int)((P0 *)this)->j)==5)&&(((int)((P0 *)this)->max)<5))))
			continue;
		sv_save();
		reached[0][21] = 1;
		reached[0][t->st] = 1;
		reached[0][tt] = 1;
S_016_0: /* 2 */
		if (!(((((int)((P0 *)this)->j)==5)&&(((int)((P0 *)this)->max)<5))))
			Uerror("block in step seq");
S_017_0: /* 2 */
		now.number[0] = (((int)((P0 *)this)->max)+1);
#ifdef VAR_RANGES
		logval("number[0]", ((int)now.number[0]));
#endif
		;
S_018_0: /* 2 */
		((P0 *)this)->j = 0;
#ifdef VAR_RANGES
		logval("P_0:j", ((int)((P0 *)this)->j));
#endif
		;
S_019_0: /* 2 */
		now.choosing[0] = 0;
#ifdef VAR_RANGES
		logval("choosing[0]", ((int)now.choosing[0]));
#endif
		;
		goto S_021_0;
S_021_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 43: /* STATE 25 - line 22 "test-models/bakery.8.pm" - [(((j<5)&&(choosing[j]==0)))] (0:0:0 - 1) */
		IfNotBlocked
		reached[0][25] = 1;
		if (!(((((int)((P0 *)this)->j)<5)&&(((int)now.choosing[ Index(((int)((P0 *)this)->j), 5) ])==0))))
			continue;
		_m = 3; goto P999; /* 0 */
	case 44: /* STATE 27 - line 24 "test-models/bakery.8.pm" - [((j==5))] (0:0:1 - 1) */
		IfNotBlocked
		reached[0][27] = 1;
		if (!((((int)((P0 *)this)->j)==5)))
			continue;
		/* dead 1: j */  (trpt+1)->bup.oval = ((P0 *)this)->j;
#ifdef HAS_CODE
		if (!readtrail)
#endif
			((P0 *)this)->j = 0;
		_m = 3; goto P999; /* 0 */
	case 45: /* STATE 33 - line 28 "test-models/bakery.8.pm" - [D_STEP] */
		if (!(boq == -1 && (((((int)now.number[ Index(((int)((P0 *)this)->j), 5) ])==0)||(((int)now.number[ Index(((int)((P0 *)this)->j), 5) ])>((int)now.number[0])))||((((int)now.number[ Index(((int)((P0 *)this)->j), 5) ])==((int)now.number[0]))&&(0<=((int)((P0 *)this)->j))))))
			continue;
		sv_save();
		reached[0][33] = 1;
		reached[0][t->st] = 1;
		reached[0][tt] = 1;
S_030_0: /* 2 */
		if (!((((((int)now.number[ Index(((int)((P0 *)this)->j), 5) ])==0)||(((int)now.number[ Index(((int)((P0 *)this)->j), 5) ])>((int)now.number[0])))||((((int)now.number[ Index(((int)((P0 *)this)->j), 5) ])==((int)now.number[0]))&&(0<=((int)((P0 *)this)->j))))))
			Uerror("block in step seq");
S_031_0: /* 2 */
		((P0 *)this)->j = (((int)((P0 *)this)->j)+1);
#ifdef VAR_RANGES
		logval("P_0:j", ((int)((P0 *)this)->j));
#endif
		;
		goto S_033_0;
S_033_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 46: /* STATE 37 - line 32 "test-models/bakery.8.pm" - [number[0] = 0] (0:0:1 - 1) */
		IfNotBlocked
		reached[0][37] = 1;
		(trpt+1)->bup.oval = ((int)now.number[0]);
		now.number[0] = 0;
#ifdef VAR_RANGES
		logval("number[0]", ((int)now.number[0]));
#endif
		;
		_m = 3; goto P999; /* 0 */
	case 47: /* STATE 41 - line 35 "test-models/bakery.8.pm" - [-end-] (0:0:0 - 1) */
		IfNotBlocked
		reached[0][41] = 1;
		if (!delproc(1, II)) continue;
		_m = 3; goto P999; /* 0 */
	case  _T5:	/* np_ */
		if (!((!(trpt->o_pm&4) && !(trpt->tau&128))))
			continue;
		/* else fall through */
	case  _T2:	/* true */
		_m = 3; goto P999;
#undef rand
	}

