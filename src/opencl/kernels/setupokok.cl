/*

	setupOKOK kernel

*/
#define MOD 258559632607830L


__kernel void setupokok(long step, int shift, __global char *OK, __global long *OKOK, __global int *offset){


	int i = get_global_id(0);
	int jj;


#define DO_OKOK(_X) \
	if(i<_X) \
		for(jj=0;jj<64;jj++){ \
      			OKOK[ i + offset[_X] ]  |=  (((long)OK[ ((i+(jj+shift)*MOD)%_X) + offset[_X] ])<<jj); \
		}

	DO_OKOK(61);
	DO_OKOK(67);
	DO_OKOK(71);
	DO_OKOK(73);
	DO_OKOK(79);
	DO_OKOK(83);
	DO_OKOK(89);
	DO_OKOK(97);
	DO_OKOK(101);
	DO_OKOK(103);
	DO_OKOK(107);
	DO_OKOK(109);
	DO_OKOK(113);
	DO_OKOK(127);
	DO_OKOK(131);
	DO_OKOK(137);
	DO_OKOK(139);
	DO_OKOK(149);
	DO_OKOK(151);
	DO_OKOK(157);
	DO_OKOK(163);
	DO_OKOK(167);
	DO_OKOK(173);
	DO_OKOK(179);
	DO_OKOK(181);
	DO_OKOK(191);
	DO_OKOK(193);
	DO_OKOK(197);
	DO_OKOK(199);
	DO_OKOK(211);
	DO_OKOK(223);
	DO_OKOK(227);
	DO_OKOK(229);
	DO_OKOK(233);
	DO_OKOK(239);
	DO_OKOK(241);
	DO_OKOK(251);
	DO_OKOK(257);
	DO_OKOK(263);
	DO_OKOK(269);
	DO_OKOK(271);
	DO_OKOK(277);
	DO_OKOK(281);
	DO_OKOK(283);
	DO_OKOK(293);
	DO_OKOK(307);
	DO_OKOK(311);
	DO_OKOK(313);
	DO_OKOK(317);
	DO_OKOK(331);
	DO_OKOK(337);
	DO_OKOK(347);
	DO_OKOK(349);
	DO_OKOK(353);
	DO_OKOK(359);
	DO_OKOK(367);
	DO_OKOK(373);
	DO_OKOK(379);
	DO_OKOK(383);
	DO_OKOK(389);
	DO_OKOK(397);
	DO_OKOK(401);
	DO_OKOK(409);
	DO_OKOK(419);
	DO_OKOK(421);
	DO_OKOK(431);
	DO_OKOK(433);
	DO_OKOK(439);
	DO_OKOK(443);
	DO_OKOK(449);
	DO_OKOK(457);
	DO_OKOK(461);
	DO_OKOK(463);
	DO_OKOK(467);
	DO_OKOK(479);
	DO_OKOK(487);
	DO_OKOK(491);
	DO_OKOK(499);
	DO_OKOK(503);
	DO_OKOK(509);
	DO_OKOK(521);
	DO_OKOK(523);
	DO_OKOK(541);

}




