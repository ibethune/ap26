/*
	setok kernel

*/





__kernel void setok(long step, int shift, __global char *OK, __global int *offset){

	int i = get_global_id(0);

/*
#define SET_OK(_X) \
	if(i>=(_X-23)  &&  i<=_X) \
		OK##_X[(i*(step%_X))%_X]=0;
*/

#define SET_OK(_X) \
	if(i>=(_X-23)  &&  i<=_X) \
		OK[ ((i*(step%_X))%_X) + offset[_X] ]=0;

	SET_OK(61);
	SET_OK(67);
	SET_OK(71);
	SET_OK(73);
	SET_OK(79);
	SET_OK(83);
	SET_OK(89);
	SET_OK(97);
	SET_OK(101);
	SET_OK(103);
	SET_OK(107);
	SET_OK(109);
	SET_OK(113);
	SET_OK(127);
	SET_OK(131);
	SET_OK(137);
	SET_OK(139);
	SET_OK(149);
	SET_OK(151);
	SET_OK(157);
	SET_OK(163);
	SET_OK(167);
	SET_OK(173);
	SET_OK(179);
	SET_OK(181);
	SET_OK(191);
	SET_OK(193);
	SET_OK(197);
	SET_OK(199);
	SET_OK(211);
	SET_OK(223);
	SET_OK(227);
	SET_OK(229);
	SET_OK(233);
	SET_OK(239);
	SET_OK(241);
	SET_OK(251);
	SET_OK(257);
	SET_OK(263);
	SET_OK(269);
	SET_OK(271);
	SET_OK(277);
	SET_OK(281);
	SET_OK(283);
	SET_OK(293);
	SET_OK(307);
	SET_OK(311);
	SET_OK(313);
	SET_OK(317);
	SET_OK(331);
	SET_OK(337);
	SET_OK(347);
	SET_OK(349);
	SET_OK(353);
	SET_OK(359);
	SET_OK(367);
	SET_OK(373);
	SET_OK(379);
	SET_OK(383);
	SET_OK(389);
	SET_OK(397);
	SET_OK(401);
	SET_OK(409);
	SET_OK(419);
	SET_OK(421);
	SET_OK(431);
	SET_OK(433);
	SET_OK(439);
	SET_OK(443);
	SET_OK(449);
	SET_OK(457);
	SET_OK(461);
	SET_OK(463);
	SET_OK(467);
	SET_OK(479);
	SET_OK(487);
	SET_OK(491);
	SET_OK(499);
	SET_OK(503);
	SET_OK(509);
	SET_OK(521);
	SET_OK(523);
	SET_OK(541);

}


