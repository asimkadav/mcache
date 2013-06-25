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

		 /* PROC CtoB */
	case 3: /* STATE 6 - line 64 "hanoi.4.pm" - [D_STEP] */
		if (!(boq == -1 && (((int)now.c[ Index((((int)now.c_act)-1), 18) ])<((int)now.b[ Index((((int)now.b_act)-1), 18) ]))))
			continue;
		sv_save();
		reached[6][6] = 1;
		reached[6][t->st] = 1;
		reached[6][tt] = 1;
S_113_0: /* 2 */
		if (!((((int)now.c[ Index((((int)now.c_act)-1), 18) ])<((int)now.b[ Index((((int)now.b_act)-1), 18) ]))))
			Uerror("block in step seq");
S_114_0: /* 2 */
		now.b[ Index(now.b_act, 18) ] = ((int)now.c[ Index((((int)now.c_act)-1), 18) ]);
#ifdef VAR_RANGES
		logval("b[b_act]", ((int)now.b[ Index(((int)now.b_act), 18) ]));
#endif
		;
S_115_0: /* 2 */
		now.b_act = (((int)now.b_act)+1);
#ifdef VAR_RANGES
		logval("b_act", ((int)now.b_act));
#endif
		;
S_116_0: /* 2 */
		now.c[ Index((now.c_act-1), 18) ] = 0;
#ifdef VAR_RANGES
		logval("c[(c_act-1)]", ((int)now.c[ Index((((int)now.c_act)-1), 18) ]));
#endif
		;
S_117_0: /* 2 */
		now.c_act = (((int)now.c_act)-1);
#ifdef VAR_RANGES
		logval("c_act", ((int)now.c_act));
#endif
		;
		goto S_119_0;
S_119_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 4: /* STATE 10 - line 67 "hanoi.4.pm" - [-end-] (0:0:0 - 1) */
		IfNotBlocked
		reached[6][10] = 1;
		if (!delproc(1, II)) continue;
		_m = 3; goto P999; /* 0 */

		 /* PROC CtoA */
	case 5: /* STATE 6 - line 56 "hanoi.4.pm" - [D_STEP] */
		if (!(boq == -1 && (((int)now.c[ Index((((int)now.c_act)-1), 18) ])<((int)now.a[ Index((((int)now.a_act)-1), 18) ]))))
			continue;
		sv_save();
		reached[5][6] = 1;
		reached[5][t->st] = 1;
		reached[5][tt] = 1;
S_103_0: /* 2 */
		if (!((((int)now.c[ Index((((int)now.c_act)-1), 18) ])<((int)now.a[ Index((((int)now.a_act)-1), 18) ]))))
			Uerror("block in step seq");
S_104_0: /* 2 */
		now.a[ Index(now.a_act, 18) ] = ((int)now.c[ Index((((int)now.c_act)-1), 18) ]);
#ifdef VAR_RANGES
		logval("a[a_act]", ((int)now.a[ Index(((int)now.a_act), 18) ]));
#endif
		;
S_105_0: /* 2 */
		now.a_act = (((int)now.a_act)+1);
#ifdef VAR_RANGES
		logval("a_act", ((int)now.a_act));
#endif
		;
S_106_0: /* 2 */
		now.c[ Index((now.c_act-1), 18) ] = 0;
#ifdef VAR_RANGES
		logval("c[(c_act-1)]", ((int)now.c[ Index((((int)now.c_act)-1), 18) ]));
#endif
		;
S_107_0: /* 2 */
		now.c_act = (((int)now.c_act)-1);
#ifdef VAR_RANGES
		logval("c_act", ((int)now.c_act));
#endif
		;
		goto S_109_0;
S_109_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 6: /* STATE 10 - line 59 "hanoi.4.pm" - [-end-] (0:0:0 - 1) */
		IfNotBlocked
		reached[5][10] = 1;
		if (!delproc(1, II)) continue;
		_m = 3; goto P999; /* 0 */

		 /* PROC BtoC */
	case 7: /* STATE 6 - line 48 "hanoi.4.pm" - [D_STEP] */
		if (!(boq == -1 && (((int)now.b[ Index((((int)now.b_act)-1), 18) ])<((int)now.c[ Index((((int)now.c_act)-1), 18) ]))))
			continue;
		sv_save();
		reached[4][6] = 1;
		reached[4][t->st] = 1;
		reached[4][tt] = 1;
S_093_0: /* 2 */
		if (!((((int)now.b[ Index((((int)now.b_act)-1), 18) ])<((int)now.c[ Index((((int)now.c_act)-1), 18) ]))))
			Uerror("block in step seq");
S_094_0: /* 2 */
		now.c[ Index(now.c_act, 18) ] = ((int)now.b[ Index((((int)now.b_act)-1), 18) ]);
#ifdef VAR_RANGES
		logval("c[c_act]", ((int)now.c[ Index(((int)now.c_act), 18) ]));
#endif
		;
S_095_0: /* 2 */
		now.c_act = (((int)now.c_act)+1);
#ifdef VAR_RANGES
		logval("c_act", ((int)now.c_act));
#endif
		;
S_096_0: /* 2 */
		now.b[ Index((now.b_act-1), 18) ] = 0;
#ifdef VAR_RANGES
		logval("b[(b_act-1)]", ((int)now.b[ Index((((int)now.b_act)-1), 18) ]));
#endif
		;
S_097_0: /* 2 */
		now.b_act = (((int)now.b_act)-1);
#ifdef VAR_RANGES
		logval("b_act", ((int)now.b_act));
#endif
		;
		goto S_099_0;
S_099_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 8: /* STATE 10 - line 51 "hanoi.4.pm" - [-end-] (0:0:0 - 1) */
		IfNotBlocked
		reached[4][10] = 1;
		if (!delproc(1, II)) continue;
		_m = 3; goto P999; /* 0 */

		 /* PROC BtoA */
	case 9: /* STATE 6 - line 40 "hanoi.4.pm" - [D_STEP] */
		if (!(boq == -1 && (((int)now.b[ Index((((int)now.b_act)-1), 18) ])<((int)now.a[ Index((((int)now.a_act)-1), 18) ]))))
			continue;
		sv_save();
		reached[3][6] = 1;
		reached[3][t->st] = 1;
		reached[3][tt] = 1;
S_083_0: /* 2 */
		if (!((((int)now.b[ Index((((int)now.b_act)-1), 18) ])<((int)now.a[ Index((((int)now.a_act)-1), 18) ]))))
			Uerror("block in step seq");
S_084_0: /* 2 */
		now.a[ Index(now.a_act, 18) ] = ((int)now.b[ Index((((int)now.b_act)-1), 18) ]);
#ifdef VAR_RANGES
		logval("a[a_act]", ((int)now.a[ Index(((int)now.a_act), 18) ]));
#endif
		;
S_085_0: /* 2 */
		now.a_act = (((int)now.a_act)+1);
#ifdef VAR_RANGES
		logval("a_act", ((int)now.a_act));
#endif
		;
S_086_0: /* 2 */
		now.b[ Index((now.b_act-1), 18) ] = 0;
#ifdef VAR_RANGES
		logval("b[(b_act-1)]", ((int)now.b[ Index((((int)now.b_act)-1), 18) ]));
#endif
		;
S_087_0: /* 2 */
		now.b_act = (((int)now.b_act)-1);
#ifdef VAR_RANGES
		logval("b_act", ((int)now.b_act));
#endif
		;
		goto S_089_0;
S_089_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 10: /* STATE 10 - line 43 "hanoi.4.pm" - [-end-] (0:0:0 - 1) */
		IfNotBlocked
		reached[3][10] = 1;
		if (!delproc(1, II)) continue;
		_m = 3; goto P999; /* 0 */

		 /* PROC AtoC */
	case 11: /* STATE 6 - line 32 "hanoi.4.pm" - [D_STEP] */
		if (!(boq == -1 && (((int)now.a[ Index((((int)now.a_act)-1), 18) ])<((int)now.c[ Index((((int)now.c_act)-1), 18) ]))))
			continue;
		sv_save();
		reached[2][6] = 1;
		reached[2][t->st] = 1;
		reached[2][tt] = 1;
S_073_0: /* 2 */
		if (!((((int)now.a[ Index((((int)now.a_act)-1), 18) ])<((int)now.c[ Index((((int)now.c_act)-1), 18) ]))))
			Uerror("block in step seq");
S_074_0: /* 2 */
		now.c[ Index(now.c_act, 18) ] = ((int)now.a[ Index((((int)now.a_act)-1), 18) ]);
#ifdef VAR_RANGES
		logval("c[c_act]", ((int)now.c[ Index(((int)now.c_act), 18) ]));
#endif
		;
S_075_0: /* 2 */
		now.c_act = (((int)now.c_act)+1);
#ifdef VAR_RANGES
		logval("c_act", ((int)now.c_act));
#endif
		;
S_076_0: /* 2 */
		now.a[ Index((now.a_act-1), 18) ] = 0;
#ifdef VAR_RANGES
		logval("a[(a_act-1)]", ((int)now.a[ Index((((int)now.a_act)-1), 18) ]));
#endif
		;
S_077_0: /* 2 */
		now.a_act = (((int)now.a_act)-1);
#ifdef VAR_RANGES
		logval("a_act", ((int)now.a_act));
#endif
		;
		goto S_079_0;
S_079_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 12: /* STATE 10 - line 35 "hanoi.4.pm" - [-end-] (0:0:0 - 1) */
		IfNotBlocked
		reached[2][10] = 1;
		if (!delproc(1, II)) continue;
		_m = 3; goto P999; /* 0 */

		 /* PROC AtoB */
	case 13: /* STATE 6 - line 24 "hanoi.4.pm" - [D_STEP] */
		if (!(boq == -1 && (((int)now.a[ Index((((int)now.a_act)-1), 18) ])<((int)now.b[ Index((((int)now.b_act)-1), 18) ]))))
			continue;
		sv_save();
		reached[1][6] = 1;
		reached[1][t->st] = 1;
		reached[1][tt] = 1;
S_063_0: /* 2 */
		if (!((((int)now.a[ Index((((int)now.a_act)-1), 18) ])<((int)now.b[ Index((((int)now.b_act)-1), 18) ]))))
			Uerror("block in step seq");
S_064_0: /* 2 */
		now.b[ Index(now.b_act, 18) ] = ((int)now.a[ Index((((int)now.a_act)-1), 18) ]);
#ifdef VAR_RANGES
		logval("b[b_act]", ((int)now.b[ Index(((int)now.b_act), 18) ]));
#endif
		;
S_065_0: /* 2 */
		now.b_act = (((int)now.b_act)+1);
#ifdef VAR_RANGES
		logval("b_act", ((int)now.b_act));
#endif
		;
S_066_0: /* 2 */
		now.a[ Index((now.a_act-1), 18) ] = 0;
#ifdef VAR_RANGES
		logval("a[(a_act-1)]", ((int)now.a[ Index((((int)now.a_act)-1), 18) ]));
#endif
		;
S_067_0: /* 2 */
		now.a_act = (((int)now.a_act)-1);
#ifdef VAR_RANGES
		logval("a_act", ((int)now.a_act));
#endif
		;
		goto S_069_0;
S_069_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 14: /* STATE 10 - line 27 "hanoi.4.pm" - [-end-] (0:0:0 - 1) */
		IfNotBlocked
		reached[1][10] = 1;
		if (!delproc(1, II)) continue;
		_m = 3; goto P999; /* 0 */

		 /* PROC :init: */
	case 15: /* STATE 55 - line 10 "hanoi.4.pm" - [D_STEP] */
		IfNotBlocked
		sv_save();
		reached[0][55] = 1;
		reached[0][t->st] = 1;
		reached[0][tt] = 1;
S_000_0: /* 2 */
		now.a[0] = 100;
#ifdef VAR_RANGES
		logval("a[0]", ((int)now.a[0]));
#endif
		;
S_001_0: /* 2 */
		now.a[1] = (17-0);
#ifdef VAR_RANGES
		logval("a[1]", ((int)now.a[1]));
#endif
		;
S_002_0: /* 2 */
		now.a[2] = (17-1);
#ifdef VAR_RANGES
		logval("a[2]", ((int)now.a[2]));
#endif
		;
S_003_0: /* 2 */
		now.a[3] = (17-2);
#ifdef VAR_RANGES
		logval("a[3]", ((int)now.a[3]));
#endif
		;
S_004_0: /* 2 */
		now.a[4] = (17-3);
#ifdef VAR_RANGES
		logval("a[4]", ((int)now.a[4]));
#endif
		;
S_005_0: /* 2 */
		now.a[5] = (17-4);
#ifdef VAR_RANGES
		logval("a[5]", ((int)now.a[5]));
#endif
		;
S_006_0: /* 2 */
		now.a[6] = (17-5);
#ifdef VAR_RANGES
		logval("a[6]", ((int)now.a[6]));
#endif
		;
S_007_0: /* 2 */
		now.a[7] = (17-6);
#ifdef VAR_RANGES
		logval("a[7]", ((int)now.a[7]));
#endif
		;
S_008_0: /* 2 */
		now.a[8] = (17-7);
#ifdef VAR_RANGES
		logval("a[8]", ((int)now.a[8]));
#endif
		;
S_009_0: /* 2 */
		now.a[9] = (17-8);
#ifdef VAR_RANGES
		logval("a[9]", ((int)now.a[9]));
#endif
		;
S_010_0: /* 2 */
		now.a[10] = (17-9);
#ifdef VAR_RANGES
		logval("a[10]", ((int)now.a[10]));
#endif
		;
S_011_0: /* 2 */
		now.a[11] = (17-10);
#ifdef VAR_RANGES
		logval("a[11]", ((int)now.a[11]));
#endif
		;
S_012_0: /* 2 */
		now.a[12] = (17-11);
#ifdef VAR_RANGES
		logval("a[12]", ((int)now.a[12]));
#endif
		;
S_013_0: /* 2 */
		now.a[13] = (17-12);
#ifdef VAR_RANGES
		logval("a[13]", ((int)now.a[13]));
#endif
		;
S_014_0: /* 2 */
		now.a[14] = (17-13);
#ifdef VAR_RANGES
		logval("a[14]", ((int)now.a[14]));
#endif
		;
S_015_0: /* 2 */
		now.a[15] = (17-14);
#ifdef VAR_RANGES
		logval("a[15]", ((int)now.a[15]));
#endif
		;
S_016_0: /* 2 */
		now.a[16] = (17-15);
#ifdef VAR_RANGES
		logval("a[16]", ((int)now.a[16]));
#endif
		;
S_017_0: /* 2 */
		now.a[17] = (17-16);
#ifdef VAR_RANGES
		logval("a[17]", ((int)now.a[17]));
#endif
		;
S_018_0: /* 2 */
		now.b[0] = 100;
#ifdef VAR_RANGES
		logval("b[0]", ((int)now.b[0]));
#endif
		;
S_019_0: /* 2 */
		now.b[1] = 0;
#ifdef VAR_RANGES
		logval("b[1]", ((int)now.b[1]));
#endif
		;
S_020_0: /* 2 */
		now.b[2] = 0;
#ifdef VAR_RANGES
		logval("b[2]", ((int)now.b[2]));
#endif
		;
S_021_0: /* 2 */
		now.b[3] = 0;
#ifdef VAR_RANGES
		logval("b[3]", ((int)now.b[3]));
#endif
		;
S_022_0: /* 2 */
		now.b[4] = 0;
#ifdef VAR_RANGES
		logval("b[4]", ((int)now.b[4]));
#endif
		;
S_023_0: /* 2 */
		now.b[5] = 0;
#ifdef VAR_RANGES
		logval("b[5]", ((int)now.b[5]));
#endif
		;
S_024_0: /* 2 */
		now.b[6] = 0;
#ifdef VAR_RANGES
		logval("b[6]", ((int)now.b[6]));
#endif
		;
S_025_0: /* 2 */
		now.b[7] = 0;
#ifdef VAR_RANGES
		logval("b[7]", ((int)now.b[7]));
#endif
		;
S_026_0: /* 2 */
		now.b[8] = 0;
#ifdef VAR_RANGES
		logval("b[8]", ((int)now.b[8]));
#endif
		;
S_027_0: /* 2 */
		now.b[9] = 0;
#ifdef VAR_RANGES
		logval("b[9]", ((int)now.b[9]));
#endif
		;
S_028_0: /* 2 */
		now.b[10] = 0;
#ifdef VAR_RANGES
		logval("b[10]", ((int)now.b[10]));
#endif
		;
S_029_0: /* 2 */
		now.b[11] = 0;
#ifdef VAR_RANGES
		logval("b[11]", ((int)now.b[11]));
#endif
		;
S_030_0: /* 2 */
		now.b[12] = 0;
#ifdef VAR_RANGES
		logval("b[12]", ((int)now.b[12]));
#endif
		;
S_031_0: /* 2 */
		now.b[13] = 0;
#ifdef VAR_RANGES
		logval("b[13]", ((int)now.b[13]));
#endif
		;
S_032_0: /* 2 */
		now.b[14] = 0;
#ifdef VAR_RANGES
		logval("b[14]", ((int)now.b[14]));
#endif
		;
S_033_0: /* 2 */
		now.b[15] = 0;
#ifdef VAR_RANGES
		logval("b[15]", ((int)now.b[15]));
#endif
		;
S_034_0: /* 2 */
		now.b[16] = 0;
#ifdef VAR_RANGES
		logval("b[16]", ((int)now.b[16]));
#endif
		;
S_035_0: /* 2 */
		now.b[17] = 0;
#ifdef VAR_RANGES
		logval("b[17]", ((int)now.b[17]));
#endif
		;
S_036_0: /* 2 */
		now.c[0] = 100;
#ifdef VAR_RANGES
		logval("c[0]", ((int)now.c[0]));
#endif
		;
S_037_0: /* 2 */
		now.c[1] = 0;
#ifdef VAR_RANGES
		logval("c[1]", ((int)now.c[1]));
#endif
		;
S_038_0: /* 2 */
		now.c[2] = 0;
#ifdef VAR_RANGES
		logval("c[2]", ((int)now.c[2]));
#endif
		;
S_039_0: /* 2 */
		now.c[3] = 0;
#ifdef VAR_RANGES
		logval("c[3]", ((int)now.c[3]));
#endif
		;
S_040_0: /* 2 */
		now.c[4] = 0;
#ifdef VAR_RANGES
		logval("c[4]", ((int)now.c[4]));
#endif
		;
S_041_0: /* 2 */
		now.c[5] = 0;
#ifdef VAR_RANGES
		logval("c[5]", ((int)now.c[5]));
#endif
		;
S_042_0: /* 2 */
		now.c[6] = 0;
#ifdef VAR_RANGES
		logval("c[6]", ((int)now.c[6]));
#endif
		;
S_043_0: /* 2 */
		now.c[7] = 0;
#ifdef VAR_RANGES
		logval("c[7]", ((int)now.c[7]));
#endif
		;
S_044_0: /* 2 */
		now.c[8] = 0;
#ifdef VAR_RANGES
		logval("c[8]", ((int)now.c[8]));
#endif
		;
S_045_0: /* 2 */
		now.c[9] = 0;
#ifdef VAR_RANGES
		logval("c[9]", ((int)now.c[9]));
#endif
		;
S_046_0: /* 2 */
		now.c[10] = 0;
#ifdef VAR_RANGES
		logval("c[10]", ((int)now.c[10]));
#endif
		;
S_047_0: /* 2 */
		now.c[11] = 0;
#ifdef VAR_RANGES
		logval("c[11]", ((int)now.c[11]));
#endif
		;
S_048_0: /* 2 */
		now.c[12] = 0;
#ifdef VAR_RANGES
		logval("c[12]", ((int)now.c[12]));
#endif
		;
S_049_0: /* 2 */
		now.c[13] = 0;
#ifdef VAR_RANGES
		logval("c[13]", ((int)now.c[13]));
#endif
		;
S_050_0: /* 2 */
		now.c[14] = 0;
#ifdef VAR_RANGES
		logval("c[14]", ((int)now.c[14]));
#endif
		;
S_051_0: /* 2 */
		now.c[15] = 0;
#ifdef VAR_RANGES
		logval("c[15]", ((int)now.c[15]));
#endif
		;
S_052_0: /* 2 */
		now.c[16] = 0;
#ifdef VAR_RANGES
		logval("c[16]", ((int)now.c[16]));
#endif
		;
S_053_0: /* 2 */
		now.c[17] = 0;
#ifdef VAR_RANGES
		logval("c[17]", ((int)now.c[17]));
#endif
		;
		goto S_061_0;
S_061_0: /* 1 */

#if defined(C_States) && (HAS_TRACK==1)
		c_update((uchar *) &(now.c_state[0]));
#endif
		_m = 3; goto P999;

	case 16: /* STATE 56 - line 13 "hanoi.4.pm" - [(run AtoB())] (0:0:0 - 1) */
		IfNotBlocked
		reached[0][56] = 1;
		if (!(addproc(1)))
			continue;
		_m = 3; goto P999; /* 0 */
	case 17: /* STATE 57 - line 14 "hanoi.4.pm" - [(run AtoC())] (0:0:0 - 1) */
		IfNotBlocked
		reached[0][57] = 1;
		if (!(addproc(2)))
			continue;
		_m = 3; goto P999; /* 0 */
	case 18: /* STATE 58 - line 15 "hanoi.4.pm" - [(run BtoA())] (0:0:0 - 1) */
		IfNotBlocked
		reached[0][58] = 1;
		if (!(addproc(3)))
			continue;
		_m = 3; goto P999; /* 0 */
	case 19: /* STATE 59 - line 16 "hanoi.4.pm" - [(run BtoC())] (0:0:0 - 1) */
		IfNotBlocked
		reached[0][59] = 1;
		if (!(addproc(4)))
			continue;
		_m = 3; goto P999; /* 0 */
	case 20: /* STATE 60 - line 17 "hanoi.4.pm" - [(run CtoA())] (0:0:0 - 1) */
		IfNotBlocked
		reached[0][60] = 1;
		if (!(addproc(5)))
			continue;
		_m = 3; goto P999; /* 0 */
	case 21: /* STATE 61 - line 18 "hanoi.4.pm" - [(run CtoB())] (0:0:0 - 1) */
		IfNotBlocked
		reached[0][61] = 1;
		if (!(addproc(6)))
			continue;
		_m = 3; goto P999; /* 0 */
	case 22: /* STATE 63 - line 19 "hanoi.4.pm" - [-end-] (0:0:0 - 1) */
		IfNotBlocked
		reached[0][63] = 1;
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

