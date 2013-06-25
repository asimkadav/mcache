	switch (t->back) {
	default: Uerror("bad return move");
	case  0: goto R999; /* nothing to undo */

		 /* PROC CtoB */
	case 3: /* STATE 6 */
		sv_restor();
		goto R999;

	case 4: /* STATE 10 */
		;
		p_restor(II);
		;
		;
		goto R999;

		 /* PROC CtoA */
	case 5: /* STATE 6 */
		sv_restor();
		goto R999;

	case 6: /* STATE 10 */
		;
		p_restor(II);
		;
		;
		goto R999;

		 /* PROC BtoC */
	case 7: /* STATE 6 */
		sv_restor();
		goto R999;

	case 8: /* STATE 10 */
		;
		p_restor(II);
		;
		;
		goto R999;

		 /* PROC BtoA */
	case 9: /* STATE 6 */
		sv_restor();
		goto R999;

	case 10: /* STATE 10 */
		;
		p_restor(II);
		;
		;
		goto R999;

		 /* PROC AtoC */
	case 11: /* STATE 6 */
		sv_restor();
		goto R999;

	case 12: /* STATE 10 */
		;
		p_restor(II);
		;
		;
		goto R999;

		 /* PROC AtoB */
	case 13: /* STATE 6 */
		sv_restor();
		goto R999;

	case 14: /* STATE 10 */
		;
		p_restor(II);
		;
		;
		goto R999;

		 /* PROC :init: */
	case 15: /* STATE 55 */
		sv_restor();
		goto R999;

	case 16: /* STATE 56 */
		;
		;
		delproc(0, now._nr_pr-1);
		;
		goto R999;

	case 17: /* STATE 57 */
		;
		;
		delproc(0, now._nr_pr-1);
		;
		goto R999;

	case 18: /* STATE 58 */
		;
		;
		delproc(0, now._nr_pr-1);
		;
		goto R999;

	case 19: /* STATE 59 */
		;
		;
		delproc(0, now._nr_pr-1);
		;
		goto R999;

	case 20: /* STATE 60 */
		;
		;
		delproc(0, now._nr_pr-1);
		;
		goto R999;

	case 21: /* STATE 61 */
		;
		;
		delproc(0, now._nr_pr-1);
		;
		goto R999;

	case 22: /* STATE 63 */
		;
		p_restor(II);
		;
		;
		goto R999;
	}

