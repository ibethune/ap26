/* This file replaces MAKES.H from Jaroslaw Wroblewski's AP26 sample
   implementation. The only changes are to use int64_t instead of long long,
   and to replace expressions (j*STEP)%X with (j*(STEP%X))%X.

	Bryan Little  
	extended OKOK to 541
	compress all OKOK# arrays into one contiguous array
	access array using offset
*/


#define MAKE_OK(_X) \
  for(j=(_X-23);j<=_X;j++) \
    OK[ ((j*(STEP%_X))%_X) + offset[_X] ]=0;

#define MAKE_OKOK(_X) \
  for(j=0;j<_X;j++) \
    for(jj=0;jj<64;jj++) \
      OKOK[ j + offset[_X] ]|=(((int64_t)OK[ ((j+(jj+SHIFT)*MOD)%_X) + offset[_X]  ])<<jj);

void setupoks(int SHIFT, int64_t STEP, int64_t *OKOK){

	int j,jj;
	char *OK;
	int *offset;

        OK = (char*)malloc(numOK * sizeof(char));
        offset = (int*)malloc(542 * sizeof(int));

	// clear arrays
	for(j=0;j<23693;j++){
		OK[j] = 1;
		OKOK[j] = 0;
	}

	// setup offset
	for(j=0;j<=541;j++){
		if(j == 61)offset[j] = 0;
		else if(j == 67)offset[j] = 61;
		else if(j == 71)offset[j] = 128;
		else if(j == 73)offset[j] = 199;
		else if(j == 79)offset[j] = 272;
		else if(j == 83)offset[j] = 351;
		else if(j == 89)offset[j] = 434;
		else if(j == 97)offset[j] = 523;
		else if(j == 101)offset[j] = 620;
		else if(j == 103)offset[j] = 721;
		else if(j == 107)offset[j] = 824;
		else if(j == 109)offset[j] = 931;
		else if(j == 113)offset[j] = 1040;
		else if(j == 127)offset[j] = 1153;
		else if(j == 131)offset[j] = 1280;
		else if(j == 137)offset[j] = 1411;
		else if(j == 139)offset[j] = 1548;
		else if(j == 149)offset[j] = 1687;
		else if(j == 151)offset[j] = 1836;
		else if(j == 157)offset[j] = 1987;
		else if(j == 163)offset[j] = 2144;
		else if(j == 167)offset[j] = 2307;
		else if(j == 173)offset[j] = 2474;
		else if(j == 179)offset[j] = 2647;
		else if(j == 181)offset[j] = 2826;
		else if(j == 191)offset[j] = 3007;
		else if(j == 193)offset[j] = 3198;
		else if(j == 197)offset[j] = 3391;
		else if(j == 199)offset[j] = 3588;
		else if(j == 211)offset[j] = 3787;
		else if(j == 223)offset[j] = 3998;
		else if(j == 227)offset[j] = 4221;
		else if(j == 229)offset[j] = 4448;
		else if(j == 233)offset[j] = 4677;
		else if(j == 239)offset[j] = 4910;
		else if(j == 241)offset[j] = 5149;
		else if(j == 251)offset[j] = 5390;
		else if(j == 257)offset[j] = 5641;
		else if(j == 263)offset[j] = 5898;
		else if(j == 269)offset[j] = 6161;
		else if(j == 271)offset[j] = 6430;
		else if(j == 277)offset[j] = 6701;
		else if(j == 281)offset[j] = 6978;
		else if(j == 283)offset[j] = 7259;
		else if(j == 293)offset[j] = 7542;
		else if(j == 307)offset[j] = 7835;
		else if(j == 311)offset[j] = 8142;
		else if(j == 313)offset[j] = 8453;
		else if(j == 317)offset[j] = 8766;
		else if(j == 331)offset[j] = 9083;
		else if(j == 337)offset[j] = 9414;
		else if(j == 347)offset[j] = 9751;
		else if(j == 349)offset[j] = 10098;
		else if(j == 353)offset[j] = 10447;
		else if(j == 359)offset[j] = 10800;
		else if(j == 367)offset[j] = 11159;
		else if(j == 373)offset[j] = 11526;
		else if(j == 379)offset[j] = 11899;
		else if(j == 383)offset[j] = 12278;
		else if(j == 389)offset[j] = 12661;
		else if(j == 397)offset[j] = 13050;
		else if(j == 401)offset[j] = 13447;
		else if(j == 409)offset[j] = 13848;
		else if(j == 419)offset[j] = 14257;
		else if(j == 421)offset[j] = 14676;
		else if(j == 431)offset[j] = 15097;
		else if(j == 433)offset[j] = 15528;
		else if(j == 439)offset[j] = 15961;
		else if(j == 443)offset[j] = 16400;
		else if(j == 449)offset[j] = 16843;
		else if(j == 457)offset[j] = 17292;
		else if(j == 461)offset[j] = 17749;
		else if(j == 463)offset[j] = 18210;
		else if(j == 467)offset[j] = 18673;
		else if(j == 479)offset[j] = 19140;
		else if(j == 487)offset[j] = 19619;
		else if(j == 491)offset[j] = 20106;
		else if(j == 499)offset[j] = 20597;
		else if(j == 503)offset[j] = 21096;
		else if(j == 509)offset[j] = 21599;
		else if(j == 521)offset[j] = 22108;
		else if(j == 523)offset[j] = 22629;
		else if(j == 541)offset[j] = 23152;
	}

	// setup
	MAKE_OK(61);
	MAKE_OK(67);
	MAKE_OK(71);
	MAKE_OK(73);
	MAKE_OK(79);
	MAKE_OK(83);
	MAKE_OK(89);
	MAKE_OK(97);
	MAKE_OK(101);
	MAKE_OK(103);
	MAKE_OK(107);
	MAKE_OK(109);
	MAKE_OK(113);
	MAKE_OK(127);
	MAKE_OK(131);
	MAKE_OK(137);
	MAKE_OK(139);
	MAKE_OK(149);
	MAKE_OK(151);
	MAKE_OK(157);
	MAKE_OK(163);
	MAKE_OK(167);
	MAKE_OK(173);
	MAKE_OK(179);
	MAKE_OK(181);
	MAKE_OK(191);
	MAKE_OK(193);
	MAKE_OK(197);
	MAKE_OK(199);
	MAKE_OK(211);
	MAKE_OK(223);
	MAKE_OK(227);
	MAKE_OK(229);
	MAKE_OK(233);
	MAKE_OK(239);
	MAKE_OK(241);
	MAKE_OK(251);
	MAKE_OK(257);
	MAKE_OK(263);
	MAKE_OK(269);
	MAKE_OK(271);
	MAKE_OK(277);
	MAKE_OK(281);
	MAKE_OK(283);
	MAKE_OK(293);
	MAKE_OK(307);
	MAKE_OK(311);
	MAKE_OK(313);
	MAKE_OK(317);
	MAKE_OK(331);
	MAKE_OK(337);
	MAKE_OK(347);
	MAKE_OK(349);
	MAKE_OK(353);
	MAKE_OK(359);
	MAKE_OK(367);
	MAKE_OK(373);
	MAKE_OK(379);
	MAKE_OK(383);
	MAKE_OK(389);
	MAKE_OK(397);
	MAKE_OK(401);
	MAKE_OK(409);
	MAKE_OK(419);
	MAKE_OK(421);
	MAKE_OK(431);
	MAKE_OK(433);
	MAKE_OK(439);
	MAKE_OK(443);
	MAKE_OK(449);
	MAKE_OK(457);
	MAKE_OK(461);
	MAKE_OK(463);
	MAKE_OK(467);
	MAKE_OK(479);
	MAKE_OK(487);
	MAKE_OK(491);
	MAKE_OK(499);
	MAKE_OK(503);
	MAKE_OK(509);
	MAKE_OK(521);
	MAKE_OK(523);
	MAKE_OK(541);

	MAKE_OKOK(61);
	MAKE_OKOK(67);
	MAKE_OKOK(71);
	MAKE_OKOK(73);
	MAKE_OKOK(79);
	MAKE_OKOK(83);
	MAKE_OKOK(89);
	MAKE_OKOK(97);
	MAKE_OKOK(101);
	MAKE_OKOK(103);
	MAKE_OKOK(107);
	MAKE_OKOK(109);
	MAKE_OKOK(113);
	MAKE_OKOK(127);
	MAKE_OKOK(131);
	MAKE_OKOK(137);
	MAKE_OKOK(139);
	MAKE_OKOK(149);
	MAKE_OKOK(151);
	MAKE_OKOK(157);
	MAKE_OKOK(163);
	MAKE_OKOK(167);
	MAKE_OKOK(173);
	MAKE_OKOK(179);
	MAKE_OKOK(181);
	MAKE_OKOK(191);
	MAKE_OKOK(193);
	MAKE_OKOK(197);
	MAKE_OKOK(199);
	MAKE_OKOK(211);
	MAKE_OKOK(223);
	MAKE_OKOK(227);
	MAKE_OKOK(229);
	MAKE_OKOK(233);
	MAKE_OKOK(239);
	MAKE_OKOK(241);
	MAKE_OKOK(251);
	MAKE_OKOK(257);
	MAKE_OKOK(263);
	MAKE_OKOK(269);
	MAKE_OKOK(271);
	MAKE_OKOK(277);
	MAKE_OKOK(281);
	MAKE_OKOK(283);
	MAKE_OKOK(293);
	MAKE_OKOK(307);
	MAKE_OKOK(311);
	MAKE_OKOK(313);
	MAKE_OKOK(317);
	MAKE_OKOK(331);
	MAKE_OKOK(337);
	MAKE_OKOK(347);
	MAKE_OKOK(349);
	MAKE_OKOK(353);
	MAKE_OKOK(359);
	MAKE_OKOK(367);
	MAKE_OKOK(373);
	MAKE_OKOK(379);
	MAKE_OKOK(383);
	MAKE_OKOK(389);
	MAKE_OKOK(397);
	MAKE_OKOK(401);
	MAKE_OKOK(409);
	MAKE_OKOK(419);
	MAKE_OKOK(421);
	MAKE_OKOK(431);
	MAKE_OKOK(433);
	MAKE_OKOK(439);
	MAKE_OKOK(443);
	MAKE_OKOK(449);
	MAKE_OKOK(457);
	MAKE_OKOK(461);
	MAKE_OKOK(463);
	MAKE_OKOK(467);
	MAKE_OKOK(479);
	MAKE_OKOK(487);
	MAKE_OKOK(491);
	MAKE_OKOK(499);
	MAKE_OKOK(503);
	MAKE_OKOK(509);
	MAKE_OKOK(521);
	MAKE_OKOK(523);
	MAKE_OKOK(541);

	free(OK);
	free(offset);

}
