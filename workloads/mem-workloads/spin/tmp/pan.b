	switch (t->back) {
	default: Uerror("bad return move");
	case  0: goto R999; /* nothing to undo */

		 /* PROC P_4 */
	case 3: /* STATE 4 */
		sv_restor();
		goto R999;
	case 4: /* STATE 11 */
		sv_restor();
		goto R999;
	case 5: /* STATE 15 */
		sv_restor();
		goto R999;
	case 6: /* STATE 21 */
		sv_restor();
		goto R999;
;
		;
		
	case 8: /* STATE 27 */
		;
	/* 0 */	((P4 *)this)->j = trpt->bup.oval;
		;
		;
		goto R999;
	case 9: /* STATE 33 */
		sv_restor();
		goto R999;

	case 10: /* STATE 37 */
		;
		now.number[4] = trpt->bup.oval;
		;
		goto R999;

	case 11: /* STATE 41 */
		;
		p_restor(II);
		;
		;
		goto R999;

		 /* PROC P_3 */
	case 12: /* STATE 4 */
		sv_restor();
		goto R999;
	case 13: /* STATE 11 */
		sv_restor();
		goto R999;
	case 14: /* STATE 15 */
		sv_restor();
		goto R999;
	case 15: /* STATE 21 */
		sv_restor();
		goto R999;
;
		;
		
	case 17: /* STATE 27 */
		;
	/* 0 */	((P3 *)this)->j = trpt->bup.oval;
		;
		;
		goto R999;
	case 18: /* STATE 33 */
		sv_restor();
		goto R999;

	case 19: /* STATE 37 */
		;
		now.number[3] = trpt->bup.oval;
		;
		goto R999;

	case 20: /* STATE 41 */
		;
		p_restor(II);
		;
		;
		goto R999;

		 /* PROC P_2 */
	case 21: /* STATE 4 */
		sv_restor();
		goto R999;
	case 22: /* STATE 11 */
		sv_restor();
		goto R999;
	case 23: /* STATE 15 */
		sv_restor();
		goto R999;
	case 24: /* STATE 21 */
		sv_restor();
		goto R999;
;
		;
		
	case 26: /* STATE 27 */
		;
	/* 0 */	((P2 *)this)->j = trpt->bup.oval;
		;
		;
		goto R999;
	case 27: /* STATE 33 */
		sv_restor();
		goto R999;

	case 28: /* STATE 37 */
		;
		now.number[2] = trpt->bup.oval;
		;
		goto R999;

	case 29: /* STATE 41 */
		;
		p_restor(II);
		;
		;
		goto R999;

		 /* PROC P_1 */
	case 30: /* STATE 4 */
		sv_restor();
		goto R999;
	case 31: /* STATE 11 */
		sv_restor();
		goto R999;
	case 32: /* STATE 15 */
		sv_restor();
		goto R999;
	case 33: /* STATE 21 */
		sv_restor();
		goto R999;
;
		;
		
	case 35: /* STATE 27 */
		;
	/* 0 */	((P1 *)this)->j = trpt->bup.oval;
		;
		;
		goto R999;
	case 36: /* STATE 33 */
		sv_restor();
		goto R999;

	case 37: /* STATE 37 */
		;
		now.number[1] = trpt->bup.oval;
		;
		goto R999;

	case 38: /* STATE 41 */
		;
		p_restor(II);
		;
		;
		goto R999;

		 /* PROC P_0 */
	case 39: /* STATE 4 */
		sv_restor();
		goto R999;
	case 40: /* STATE 11 */
		sv_restor();
		goto R999;
	case 41: /* STATE 15 */
		sv_restor();
		goto R999;
	case 42: /* STATE 21 */
		sv_restor();
		goto R999;
;
		;
		
	case 44: /* STATE 27 */
		;
	/* 0 */	((P0 *)this)->j = trpt->bup.oval;
		;
		;
		goto R999;
	case 45: /* STATE 33 */
		sv_restor();
		goto R999;

	case 46: /* STATE 37 */
		;
		now.number[0] = trpt->bup.oval;
		;
		goto R999;

	case 47: /* STATE 41 */
		;
		p_restor(II);
		;
		;
		goto R999;
	}

