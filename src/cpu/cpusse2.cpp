/* cpusse2.cpp

   SSE2 support 27 September 2009 by Bryan Little
   Sieve arrays sized to fit in 256kbyte L2 cache

   Count zeros by Sebastian Jaworowicz

   See http://www.math.uni.wroc.pl/~jwr/AP26/AP26v3.pdf for information
   about how the algorithm works and for the copyleft notice.
*/

#define __STDC_FORMAT_MACROS
#define __STDC_CONSTANT_MACROS

#include <x86intrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>

#include "cpuconst.h"
#include "cpusse2.h"

// 850 progress per k

// selects elements from two vectors based on a selection mask
#define vec_sel(_X, _Y, _Z) \
	_mm_xor_si128(_X, _mm_and_si128(_Z, _mm_xor_si128(_Y, _X)))

#define MAKE_OK(_X) \
  for(j=0;j<_X;j++) \
    OK##_X[j]=1; \
  for(j=(_X-23);j<=_X;j++) \
    OK##_X[(j*(STEP%_X))%_X]=0;

#define MAKE_OKOK(_X) \
  for(j=0;j<_X;j++) \
    OKOK##_X[j]=0; \
  for(j=0;j<_X;j++) \
    for(jj=0;jj<64;jj++) \
      OKOK##_X[j]|=(((int64_t)OK##_X[(j+(jj+SHIFT)*MOD)%_X])<<jj);


void Search_sse2(int K, int startSHIFT, int ITER, int K_COUNT, int K_DONE)
{ 
	int64_t n, n43, n47, n53, n59;
	int i43, i47, i53, i59;
	int i3, i5, i31, i37, i41;
	int SHIFT;
	int64_t STEP;
	int64_t n0;
	int64_t sito;
	int64_t S31, S37, S41, S43, S47, S53, S59;
	double d = (double)1.0 / (K_COUNT*850);
	double dd;
	int iter = ITER;
	int progress = iter * 85;

	// char arrays total 23693 bytes
	static char OK61[61];
	static char OK67[67];
	static char OK71[71];
	static char OK73[73];
	static char OK79[79];
	static char OK83[83];
	static char OK89[89];
	static char OK97[97];
	static char OK101[101];
	static char OK103[103];
	static char OK107[107];
	static char OK109[109];
	static char OK113[113];
	static char OK127[127];
	static char OK131[131];
	static char OK137[137];
	static char OK139[139];
	static char OK149[149];
	static char OK151[151];
	static char OK157[157];
	static char OK163[163];
	static char OK167[167];
	static char OK173[173];
	static char OK179[179];
	static char OK181[181];
	static char OK191[191];
	static char OK193[193];
	static char OK197[197];
	static char OK199[199];
	static char OK211[211];
	static char OK223[223];
	static char OK227[227];
	static char OK229[229];
	static char OK233[233];
	static char OK239[239];
	static char OK241[241];
	static char OK251[251];
	static char OK257[257];
	static char OK263[263];
	static char OK269[269];
	static char OK271[271];
	static char OK277[277];
	static char OK281[281];
	static char OK283[283];
	static char OK293[293];
	static char OK307[307];
	static char OK311[311];
	static char OK313[313];
	static char OK317[317];
	static char OK331[331];
	static char OK337[337];
	static char OK347[347];
	static char OK349[349];
	static char OK353[353];
	static char OK359[359];
	static char OK367[367];
	static char OK373[373];
	static char OK379[379];
	static char OK383[383];
	static char OK389[389];
	static char OK397[397];
	static char OK401[401];
	static char OK409[409];
	static char OK419[419];
	static char OK421[421];
	static char OK431[431];
	static char OK433[433];
	static char OK439[439];
	static char OK443[443];
	static char OK449[449];
	static char OK457[457];
	static char OK461[461];
	static char OK463[463];
	static char OK467[467];
	static char OK479[479];
	static char OK487[487];
	static char OK491[491];
	static char OK499[499];
	static char OK503[503];
	static char OK509[509];
	static char OK521[521];
	static char OK523[523];
	static char OK541[541];
	// int64 arrays total 189544 bytes
	static int64_t OKOK61[61];
	static int64_t OKOK67[67];
	static int64_t OKOK71[71];
	static int64_t OKOK73[73];
	static int64_t OKOK79[79];
	static int64_t OKOK83[83];
	static int64_t OKOK89[89];
	static int64_t OKOK97[97];
	static int64_t OKOK101[101];
	static int64_t OKOK103[103];
	static int64_t OKOK107[107];
	static int64_t OKOK109[109];
	static int64_t OKOK113[113];
	static int64_t OKOK127[127];
	static int64_t OKOK131[131];
	static int64_t OKOK137[137];
	static int64_t OKOK139[139];
	static int64_t OKOK149[149];
	static int64_t OKOK151[151];
	static int64_t OKOK157[157];
	static int64_t OKOK163[163];
	static int64_t OKOK167[167];
	static int64_t OKOK173[173];
	static int64_t OKOK179[179];
	static int64_t OKOK181[181];
	static int64_t OKOK191[191];
	static int64_t OKOK193[193];
	static int64_t OKOK197[197];
	static int64_t OKOK199[199];
	static int64_t OKOK211[211];
	static int64_t OKOK223[223];
	static int64_t OKOK227[227];
	static int64_t OKOK229[229];
	static int64_t OKOK233[233];
	static int64_t OKOK239[239];
	static int64_t OKOK241[241];
	static int64_t OKOK251[251];
	static int64_t OKOK257[257];
	static int64_t OKOK263[263];
	static int64_t OKOK269[269];
	static int64_t OKOK271[271];
	static int64_t OKOK277[277];
	static int64_t OKOK281[281];
	static int64_t OKOK283[283];
	static int64_t OKOK293[293];
	static int64_t OKOK307[307];
	static int64_t OKOK311[311];
	static int64_t OKOK313[313];
	static int64_t OKOK317[317];
	static int64_t OKOK331[331];
	static int64_t OKOK337[337];
	static int64_t OKOK347[347];
	static int64_t OKOK349[349];
	static int64_t OKOK353[353];
	static int64_t OKOK359[359];
	static int64_t OKOK367[367];
	static int64_t OKOK373[373];
	static int64_t OKOK379[379];
	static int64_t OKOK383[383];
	static int64_t OKOK389[389];
	static int64_t OKOK397[397];
	static int64_t OKOK401[401];
	static int64_t OKOK409[409];
	static int64_t OKOK419[419];
	static int64_t OKOK421[421];
	static int64_t OKOK431[431];
	static int64_t OKOK433[433];
	static int64_t OKOK439[439];
	static int64_t OKOK443[443];
	static int64_t OKOK449[449];
	static int64_t OKOK457[457];
	static int64_t OKOK461[461];
	static int64_t OKOK463[463];
	static int64_t OKOK467[467];
	static int64_t OKOK479[479];
	static int64_t OKOK487[487];
	static int64_t OKOK491[491];
	static int64_t OKOK499[499];
	static int64_t OKOK503[503];
	static int64_t OKOK509[509];
	static int64_t OKOK521[521];
	static int64_t OKOK523[523];
	static int64_t OKOK541[541];

	time_t start_time, finish_time;
	time (&start_time);

	STEP=K*PRIM23;
	n0=(N0*(K%17835)+((N0*17835)%MOD)*(K/17835)+N30)%MOD;

	S31=(PRES2*(K%17835)+((PRES2*17835)%MOD)*(K/17835))%MOD;
	S37=(PRES3*(K%17835)+((PRES3*17835)%MOD)*(K/17835))%MOD;
	S41=(PRES4*(K%17835)+((PRES4*17835)%MOD)*(K/17835))%MOD;
	S43=(PRES5*(K%17835)+((PRES5*17835)%MOD)*(K/17835))%MOD;
	S47=(PRES6*(K%17835)+((PRES6*17835)%MOD)*(K/17835))%MOD;
	S53=(PRES7*(K%17835)+((PRES7*17835)%MOD)*(K/17835))%MOD;
	S59=(PRES8*(K%17835)+((PRES8*17835)%MOD)*(K/17835))%MOD;

	//quick loop vectors
	__m128i svec1		= _mm_set_epi16(S59%61, S59%67, S59%71, S59%73, S59%79, S59%83, S59%89, S59%97);
	__m128i svec2		= _mm_set_epi16(S59%101, S59%103, S59%107, S59%109, S59%113, S59%127, S59%131, S59%137);

	__m128i mvec1		= _mm_set_epi16(MOD%61, MOD%67, MOD%71, MOD%73, MOD%79, MOD%83, MOD%89, MOD%97);
	__m128i mvec2		= _mm_set_epi16(MOD%101, MOD%103, MOD%107, MOD%109, MOD%113, MOD%127, MOD%131, MOD%137);

	__m128i numvec1_1	= _mm_set_epi16(61, 67, 71, 73, 79, 83, 89, 97);
	__m128i numvec2_1	= _mm_set_epi16(60, 66, 70, 72, 78, 82, 88, 96);

	__m128i numvec1_2	= _mm_set_epi16(101, 103, 107, 109, 113, 127, 131, 137);
	__m128i numvec2_2	= _mm_set_epi16(100, 102, 106, 108, 112, 126, 130, 136);

	__m128i zerovec		= _mm_set_epi16(0, 0, 0, 0, 0, 0, 0, 0);

	int j,jj;

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
		
	// 10 shift
	for(SHIFT=startSHIFT+(iter*64); SHIFT<(startSHIFT+640); SHIFT+=64){

		// init OKOK arrays    
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

		// start searching
		for(i31=0;i31<7;++i31){
		for(i37=0;i37<13;++i37)
		if(i37-i31<=10&&i31-i37<=4)
		{
			// 85x
			progress++;
    			dd = (double)(K_DONE*850+progress) * d;
			Progress(dd);

		for(i41=0;i41<17;++i41)
		if(i41-i31<=14&&i41-i37<=14&&i31-i41<=4&&i37-i41<=10)
		for(i3=0;i3<2;++i3)
		for(i5=0;i5<4;++i5)
		{ 
		n43=(n0+i3*S3+i5*S5+i31*S31+i37*S37+i41*S41)%MOD;
		{
		for(i43=(PRIME5-24);i43>0;i43--)
		{
		n47=n43;
		for(i47=(PRIME6-24);i47>0;i47--)
		{
		n53=n47;
		for(i53=(PRIME7-24);i53>0;i53--)
		{
		n59=n53;

		// Bryan Little, new fast vectors for loop
		// elements in vector arrays are stored in reverse order
		__m128i r_numvec1 = _mm_set_epi16(REM(n59,61,6), REM(n59,67,7), REM(n59,71,7), REM(n59,73,7), REM(n59,79,7), REM(n59,83,7), REM(n59,89,7), REM(n59,97,7));
		__m128i r_numvec2 = _mm_set_epi16(REM(n59,101,7), REM(n59,103,7), REM(n59,107,7), REM(n59,109,7), REM(n59,113,7), REM(n59,127,7), REM(n59,131,8), REM(n59,137,8));

			for(i59=(PRIME8-24);i59>0;i59--)
			{
				if(sito = OKOK61[_mm_extract_epi16(r_numvec1, 7)]
						& OKOK67[_mm_extract_epi16(r_numvec1, 6)]
						& OKOK71[_mm_extract_epi16(r_numvec1, 5)]
						& OKOK73[_mm_extract_epi16(r_numvec1, 4)]
						& OKOK79[_mm_extract_epi16(r_numvec1, 3)]
						& OKOK83[_mm_extract_epi16(r_numvec1, 2)]
						& OKOK89[_mm_extract_epi16(r_numvec1, 1)]
						& OKOK97[_mm_extract_epi16(r_numvec1, 0)])
				if(sito&=(OKOK101[_mm_extract_epi16(r_numvec2, 7)]
						& OKOK103[_mm_extract_epi16(r_numvec2, 6)]
						& OKOK107[_mm_extract_epi16(r_numvec2, 5)]
						& OKOK109[_mm_extract_epi16(r_numvec2, 4)]
						& OKOK113[_mm_extract_epi16(r_numvec2, 3)]
						& OKOK127[_mm_extract_epi16(r_numvec2, 2)]
						& OKOK131[_mm_extract_epi16(r_numvec2, 1)]
						& OKOK137[_mm_extract_epi16(r_numvec2, 0)]))
				if(sito&=(OKOK139[REM(n59,139,8)]
						& OKOK149[REM(n59,149,8)]
						& OKOK151[REM(n59,151,8)]
						& OKOK157[REM(n59,157,8)]
						& OKOK163[REM(n59,163,8)]
						& OKOK167[REM(n59,167,8)]
						& OKOK173[REM(n59,173,8)]
						& OKOK179[REM(n59,179,8)]
						& OKOK181[REM(n59,181,8)]
						& OKOK191[REM(n59,191,8)]
						& OKOK193[REM(n59,193,8)]))
				if(sito&=(OKOK197[REM(n59,197,8)]
						& OKOK199[REM(n59,199,8)]
						& OKOK211[REM(n59,211,8)]
						& OKOK223[REM(n59,223,8)]
						& OKOK227[REM(n59,227,8)]
						& OKOK229[REM(n59,229,8)]
						& OKOK233[REM(n59,233,8)]
						& OKOK239[REM(n59,239,8)]
						& OKOK241[REM(n59,241,8)]
						& OKOK251[REM(n59,251,8)]
						& OKOK257[REM(n59,257,9)]))
				if(sito&=(OKOK263[REM(n59,263,9)]
						& OKOK269[REM(n59,269,9)]
						& OKOK271[REM(n59,271,9)]
						& OKOK277[REM(n59,277,9)]
						& OKOK281[REM(n59,281,9)]
						& OKOK283[REM(n59,283,9)]
						& OKOK293[REM(n59,293,9)]
						& OKOK307[REM(n59,307,9)]
						& OKOK311[REM(n59,311,9)]
						& OKOK313[REM(n59,313,9)]
						& OKOK317[REM(n59,317,9)]))
				if(sito&=(OKOK331[REM(n59,331,9)]
						& OKOK337[REM(n59,337,9)]
						& OKOK347[REM(n59,347,9)]
						& OKOK349[REM(n59,349,9)]
						& OKOK353[REM(n59,353,9)]
						& OKOK359[REM(n59,359,9)]
						& OKOK367[REM(n59,367,9)]
						& OKOK373[REM(n59,373,9)]
						& OKOK379[REM(n59,379,9)]
						& OKOK383[REM(n59,383,9)]
						& OKOK389[REM(n59,389,9)]))
				if(sito&=(OKOK397[REM(n59,397,9)]
						& OKOK401[REM(n59,401,9)]
						& OKOK409[REM(n59,409,9)]
						& OKOK419[REM(n59,419,9)]
						& OKOK421[REM(n59,421,9)]
						& OKOK431[REM(n59,431,9)]
						& OKOK433[REM(n59,433,9)]
						& OKOK439[REM(n59,439,9)]
						& OKOK443[REM(n59,443,9)]
						& OKOK449[REM(n59,449,9)]
						& OKOK457[REM(n59,457,9)]))
				if(sito&=(OKOK461[REM(n59,461,9)]
						& OKOK463[REM(n59,463,9)]
						& OKOK467[REM(n59,467,9)]
						& OKOK479[REM(n59,479,9)]
						& OKOK487[REM(n59,487,9)]
						& OKOK491[REM(n59,491,9)]
						& OKOK499[REM(n59,499,9)]
						& OKOK503[REM(n59,503,9)]
						& OKOK509[REM(n59,509,9)]
						& OKOK521[REM(n59,521,10)]
						& OKOK523[REM(n59,523,10)]
						& OKOK541[REM(n59,541,10)]))

				{
					int b;
					int bLimit, bStart;

					bLimit = 63 - __builtin_clzll(sito);
					bStart = __builtin_ctzll(sito);

					for (b = bStart; b <= bLimit; b++){
						if ((sito >> b) & 1)
						{
							n=n59+(b+SHIFT)*MOD;

							if(n%7)
							if(n%11)
							if(n%13)
							if(n%17)
							if(n%19)
							if(n%23)
							{
								int64_t m;
								int k;
								k=0; m=n+STEP*5;
								while(PrimeQ(m)){
									k++;
									m+=STEP;
								}

								if(k>=10){
									m=n+STEP*4;
									while(m>0&&PrimeQ(m)){
										k++;
										m-=STEP;
									}
								}

								if(k>=10)
								{
									int64_t first_term = m+STEP;
									ReportSolution(k,K,first_term);
								}
							}
						}
					}
				}


				n59+=S59;

				__m128i subvec;

				r_numvec1 = _mm_add_epi16(r_numvec1, svec1);
				r_numvec2 = _mm_add_epi16(r_numvec2, svec2);

				if(n59>=MOD){
					n59-=MOD;

					__m128i addvec;

					r_numvec1 = _mm_sub_epi16(r_numvec1, mvec1);
					addvec = _mm_add_epi16(r_numvec1, numvec1_1);
					r_numvec1 = vec_sel( r_numvec1, addvec, _mm_cmpgt_epi16( zerovec, r_numvec1 ) );

					r_numvec2 = _mm_sub_epi16(r_numvec2, mvec2);
					addvec = _mm_add_epi16(r_numvec2, numvec1_2);
					r_numvec2 = vec_sel(r_numvec2, addvec, _mm_cmpgt_epi16( zerovec, r_numvec2 ) );
				}
				subvec = _mm_sub_epi16(r_numvec1, numvec1_1);
				r_numvec1 = vec_sel(r_numvec1, subvec, _mm_cmpgt_epi16(r_numvec1, numvec2_1) );

				subvec = _mm_sub_epi16(r_numvec2, numvec1_2);
				r_numvec2 = vec_sel(r_numvec2, subvec, _mm_cmpgt_epi16(r_numvec2, numvec2_2) );
			   
			}     

		n53+=S53;
		if(n53>=MOD)n53-=MOD;
		}
		n47+=S47;
		if(n47>=MOD)n47-=MOD;
		}
		n43+=S43;
		if(n43>=MOD)n43-=MOD;
		}

		}
		}
		}
		}

		iter++;
		if(iter<10){
			checkpoint(startSHIFT,K,0,iter);
		}		

	}

	time(&finish_time);
	printf("Computation of K: %d complete in %d seconds\n", K, (int)finish_time - (int)start_time);



}
