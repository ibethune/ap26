/* cpuavx.cpp --

   AVX support 29 July 2016 by Bryan Little
   Sieve arrays sized to fit in 256kbyte L2 cache

   Blend and count zeros by Sebastian Jaworowicz

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
#include "cpuavx.h"

//  255 progress per K

// selects elements from two vectors based on a selection mask
#define vec_sel(_X, _Y, _Z) _mm_blendv_epi8(_X, _Y, _Z)


#define MAKE_OK(_X) \
  for(j=0;j<_X;j++) \
    OK##_X[j]=1; \
  for(j=(_X-23);j<=_X;j++) \
    OK##_X[(j*(STEP%_X))%_X]=0;


#define MAKE_OKOKx(_X) \
  for(j=0;j<_X;j++){ \
    aOKOK=0; \
    bOKOK=0; \
    cOKOK=0; \
    dOKOK=0; \
    for(jj=0;jj<64;jj++){ \
      if(SHIFT < maxshift) \
        aOKOK|=(((int64_t)OK##_X[(j+(jj+SHIFT)*MOD)%_X])<<jj); \
      if(SHIFT+64 < maxshift) \
        bOKOK|=(((int64_t)OK##_X[(j+(jj+SHIFT+64)*MOD)%_X])<<jj); \
      if(SHIFT+128 < maxshift) \
        cOKOK|=(((int64_t)OK##_X[(j+(jj+SHIFT+128)*MOD)%_X])<<jj); \
      if(SHIFT+192 < maxshift) \
        dOKOK|=(((int64_t)OK##_X[(j+(jj+SHIFT+192)*MOD)%_X])<<jj); \
    } \
    xOKOK##_X[j] = _mm256_castsi256_pd ( _mm256_set_epi64x( aOKOK, bOKOK, cOKOK, dOKOK) ); \
  }


int continuesito_avx(__m256d dsito){

	if( _mm256_testz_si256(_mm256_castpd_si256(dsito), _mm256_castpd_si256(dsito)) == 0 )
		return 1;

	return 0;

}


void Search_avx(int K, int startSHIFT, int ITER, int K_COUNT, int K_DONE)
{ 
	int64_t n, n43, n47, n53, n59;
	int i43, i47, i53, i59;
	int i3, i5, i31, i37, i41;
	int SHIFT;
	int maxshift = startSHIFT+640;
	int64_t STEP;
	int64_t n0;
	int64_t S31, S37, S41, S43, S47, S53, S59;
	double d = (double)1.0 / (K_COUNT*255);
	double dd;
	int iter = ITER;
	int progress = iter * 21;
	int j,jj;

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
	// m256 arrays total 223296 bytes
	static __m256d xOKOK61[61];
	static __m256d xOKOK67[67];
	static __m256d xOKOK71[71];
	static __m256d xOKOK73[73];
	static __m256d xOKOK79[79];
	static __m256d xOKOK83[83];
	static __m256d xOKOK89[89];
	static __m256d xOKOK97[97];
	static __m256d xOKOK101[101];
	static __m256d xOKOK103[103];
	static __m256d xOKOK107[107];
	static __m256d xOKOK109[109];
	static __m256d xOKOK113[113];
	static __m256d xOKOK127[127];
	static __m256d xOKOK131[131];
	static __m256d xOKOK137[137];
	static __m256d xOKOK139[139];
	static __m256d xOKOK149[149];
	static __m256d xOKOK151[151];
	static __m256d xOKOK157[157];
	static __m256d xOKOK163[163];
	static __m256d xOKOK167[167];
	static __m256d xOKOK173[173];
	static __m256d xOKOK179[179];
	static __m256d xOKOK181[181];
	static __m256d xOKOK191[191];
	static __m256d xOKOK193[193];
	static __m256d xOKOK197[197];
	static __m256d xOKOK199[199];
	static __m256d xOKOK211[211];
	static __m256d xOKOK223[223];
	static __m256d xOKOK227[227];
	static __m256d xOKOK229[229];
	static __m256d xOKOK233[233];
	static __m256d xOKOK239[239];
	static __m256d xOKOK241[241];
	static __m256d xOKOK251[251];
	static __m256d xOKOK257[257];
	static __m256d xOKOK263[263];
	static __m256d xOKOK269[269];
	static __m256d xOKOK271[271];
	static __m256d xOKOK277[277];

	int64_t aOKOK,bOKOK,cOKOK,dOKOK;


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

	// init OK arrays    
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
	for(SHIFT=startSHIFT+(iter*64); SHIFT<maxshift; SHIFT+=256){

		MAKE_OKOKx(61);
		MAKE_OKOKx(67);
		MAKE_OKOKx(71);
		MAKE_OKOKx(73);
		MAKE_OKOKx(79);
		MAKE_OKOKx(83);
		MAKE_OKOKx(89);
		MAKE_OKOKx(97);
		MAKE_OKOKx(101);
		MAKE_OKOKx(103);
		MAKE_OKOKx(107);
		MAKE_OKOKx(109);
		MAKE_OKOKx(113);
		MAKE_OKOKx(127);
		MAKE_OKOKx(131);
		MAKE_OKOKx(137);
		MAKE_OKOKx(139);
		MAKE_OKOKx(149);
		MAKE_OKOKx(151);
		MAKE_OKOKx(157);
		MAKE_OKOKx(163);
		MAKE_OKOKx(167);
		MAKE_OKOKx(173);
		MAKE_OKOKx(179);
		MAKE_OKOKx(181);
		MAKE_OKOKx(191);
		MAKE_OKOKx(193);
		MAKE_OKOKx(197);
		MAKE_OKOKx(199);
		MAKE_OKOKx(211);
		MAKE_OKOKx(223);
		MAKE_OKOKx(227);
		MAKE_OKOKx(229);
		MAKE_OKOKx(233);
		MAKE_OKOKx(239);
		MAKE_OKOKx(241);
		MAKE_OKOKx(251);
		MAKE_OKOKx(257);
		MAKE_OKOKx(263);
		MAKE_OKOKx(269);
		MAKE_OKOKx(271);
		MAKE_OKOKx(277);

		// start searching
		for(i31=0;i31<7;++i31)
		{
		for(i37=0;i37<13;++i37)
		if(i37-i31<=10&&i31-i37<=4)
		{
			// 85x
			progress++;
    			dd = (double)(K_DONE*255+progress) * d;
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

		__m128i r_numvec1 = _mm_set_epi16(REM(n59,61,6), REM(n59,67,7), REM(n59,71,7), REM(n59,73,7), REM(n59,79,7), REM(n59,83,7), REM(n59,89,7), REM(n59,97,7));
		__m128i r_numvec2 = _mm_set_epi16(REM(n59,101,7), REM(n59,103,7), REM(n59,107,7), REM(n59,109,7), REM(n59,113,7), REM(n59,127,7), REM(n59,131,8), REM(n59,137,8));

			for(i59=(PRIME8-24);i59>0;i59--)
			{

				__m256d dsito;

				dsito = _mm256_and_pd( xOKOK61[_mm_extract_epi16(r_numvec1, 7)], xOKOK67[_mm_extract_epi16(r_numvec1, 6)] );
				dsito = _mm256_and_pd( dsito, xOKOK71[_mm_extract_epi16(r_numvec1, 5)] );
				dsito = _mm256_and_pd( dsito, xOKOK73[_mm_extract_epi16(r_numvec1, 4)] );
				dsito = _mm256_and_pd( dsito, xOKOK79[_mm_extract_epi16(r_numvec1, 3)] );
				dsito = _mm256_and_pd( dsito, xOKOK83[_mm_extract_epi16(r_numvec1, 2)] );
				dsito = _mm256_and_pd( dsito, xOKOK89[_mm_extract_epi16(r_numvec1, 1)] );
				dsito = _mm256_and_pd( dsito, xOKOK97[_mm_extract_epi16(r_numvec1, 0)] );
				dsito = _mm256_and_pd( dsito, xOKOK101[_mm_extract_epi16(r_numvec2, 7)] );
				dsito = _mm256_and_pd( dsito, xOKOK103[_mm_extract_epi16(r_numvec2, 6)] );
				dsito = _mm256_and_pd( dsito, xOKOK107[_mm_extract_epi16(r_numvec2, 5)] );
				dsito = _mm256_and_pd( dsito, xOKOK109[_mm_extract_epi16(r_numvec2, 4)] );
				dsito = _mm256_and_pd( dsito, xOKOK113[_mm_extract_epi16(r_numvec2, 3)] );
				dsito = _mm256_and_pd( dsito, xOKOK127[_mm_extract_epi16(r_numvec2, 2)] );
				dsito = _mm256_and_pd( dsito, xOKOK131[_mm_extract_epi16(r_numvec2, 1)] );
				dsito = _mm256_and_pd( dsito, xOKOK137[_mm_extract_epi16(r_numvec2, 0)] );
				if( continuesito_avx(dsito) ){
					dsito = _mm256_and_pd( dsito, xOKOK139[REM(n59,139,8)] );
					dsito = _mm256_and_pd( dsito, xOKOK149[REM(n59,149,8)] );
					dsito = _mm256_and_pd( dsito, xOKOK151[REM(n59,151,8)] );
					dsito = _mm256_and_pd( dsito, xOKOK157[REM(n59,157,8)] );
					dsito = _mm256_and_pd( dsito, xOKOK163[REM(n59,163,8)] );
					dsito = _mm256_and_pd( dsito, xOKOK167[REM(n59,167,8)] );
					dsito = _mm256_and_pd( dsito, xOKOK173[REM(n59,173,8)] );
					dsito = _mm256_and_pd( dsito, xOKOK179[REM(n59,179,8)] );
					dsito = _mm256_and_pd( dsito, xOKOK181[REM(n59,181,8)] );
					dsito = _mm256_and_pd( dsito, xOKOK191[REM(n59,191,8)] );
					dsito = _mm256_and_pd( dsito, xOKOK193[REM(n59,193,8)] );
					dsito = _mm256_and_pd( dsito, xOKOK197[REM(n59,197,8)] );
					dsito = _mm256_and_pd( dsito, xOKOK199[REM(n59,199,8)] );
				if( continuesito_avx(dsito) ){
					dsito = _mm256_and_pd( dsito, xOKOK211[REM(n59,211,8)] );
					dsito = _mm256_and_pd( dsito, xOKOK223[REM(n59,223,8)] );
					dsito = _mm256_and_pd( dsito, xOKOK227[REM(n59,227,8)] );
					dsito = _mm256_and_pd( dsito, xOKOK229[REM(n59,229,8)] );
					dsito = _mm256_and_pd( dsito, xOKOK233[REM(n59,233,8)] );
					dsito = _mm256_and_pd( dsito, xOKOK239[REM(n59,239,8)] );
					dsito = _mm256_and_pd( dsito, xOKOK241[REM(n59,241,8)] );
					dsito = _mm256_and_pd( dsito, xOKOK251[REM(n59,251,8)] );
					dsito = _mm256_and_pd( dsito, xOKOK257[REM(n59,257,9)] );
					dsito = _mm256_and_pd( dsito, xOKOK263[REM(n59,263,9)] );
					dsito = _mm256_and_pd( dsito, xOKOK269[REM(n59,269,9)] );
					dsito = _mm256_and_pd( dsito, xOKOK271[REM(n59,271,9)] );
					dsito = _mm256_and_pd( dsito, xOKOK277[REM(n59,277,9)] );
				if( continuesito_avx(dsito) ){
					__m256i isito;
					int64_t sito0;
					int64_t sito1;
					int64_t sito2;
					int64_t sito3; 

					isito = _mm256_castpd_si256( dsito );
					sito0 = _mm256_extract_epi64( isito, 3 );
					sito1 = _mm256_extract_epi64( isito, 2 );
					sito2 = _mm256_extract_epi64( isito, 1 );
					sito3 = _mm256_extract_epi64( isito, 0 );

					if(sito0){
						int b;
						int64_t n;
						int bLimit, bStart;

						bLimit = 63 - __builtin_clzll(sito0);
						bStart = __builtin_ctzll(sito0);

						for (b = bStart; b <= bLimit; b++){
							if ((sito0 >> b) & 1)
							{
								n=n59+(b+SHIFT)*MOD;

								if(n%7)
								if(n%11)
								if(n%13)
								if(n%17)
								if(n%19)
								if(n%23)
								if(OK281[n%281])
								if(OK283[n%283])
								if(OK293[n%293])
								if(OK307[n%307])
								if(OK311[n%311])
								if(OK313[n%313])
								if(OK317[n%317])
								if(OK331[n%331])
								if(OK337[n%337])
								if(OK347[n%347])
								if(OK349[n%349])
								if(OK353[n%353])
								if(OK359[n%359])
								if(OK367[n%367])
								if(OK373[n%373])
								if(OK379[n%379])
								if(OK383[n%383])
								if(OK389[n%389])
								if(OK397[n%397])
								if(OK401[n%401])
								if(OK409[n%409])
								if(OK419[n%419])
								if(OK421[n%421])
								if(OK431[n%431])
								if(OK433[n%433])
								if(OK439[n%439])
								if(OK443[n%443])
								if(OK449[n%449])
								if(OK457[n%457])
								if(OK461[n%461])
								if(OK463[n%463])
								if(OK467[n%467])
								if(OK479[n%479])
								if(OK487[n%487])
								if(OK491[n%491])
								if(OK499[n%499])
								if(OK503[n%503])
								if(OK509[n%509])
								if(OK521[n%521])
								if(OK523[n%523])
								if(OK541[n%541])
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

					if(sito1){
						int b;
						int64_t n;
						int bLimit, bStart;

						bLimit = 63 - __builtin_clzll(sito1);
						bStart = __builtin_ctzll(sito1);

						for (b = bStart; b <= bLimit; b++){
							if ((sito1 >> b) & 1)
							{
								n=n59+(b+SHIFT+64)*MOD;

								if(n%7)
								if(n%11)
								if(n%13)
								if(n%17)
								if(n%19)
								if(n%23)
								if(OK281[n%281])
								if(OK283[n%283])
								if(OK293[n%293])
								if(OK307[n%307])
								if(OK311[n%311])
								if(OK313[n%313])
								if(OK317[n%317])
								if(OK331[n%331])
								if(OK337[n%337])
								if(OK347[n%347])
								if(OK349[n%349])
								if(OK353[n%353])
								if(OK359[n%359])
								if(OK367[n%367])
								if(OK373[n%373])
								if(OK379[n%379])
								if(OK383[n%383])
								if(OK389[n%389])
								if(OK397[n%397])
								if(OK401[n%401])
								if(OK409[n%409])
								if(OK419[n%419])
								if(OK421[n%421])
								if(OK431[n%431])
								if(OK433[n%433])
								if(OK439[n%439])
								if(OK443[n%443])
								if(OK449[n%449])
								if(OK457[n%457])
								if(OK461[n%461])
								if(OK463[n%463])
								if(OK467[n%467])
								if(OK479[n%479])
								if(OK487[n%487])
								if(OK491[n%491])
								if(OK499[n%499])
								if(OK503[n%503])
								if(OK509[n%509])
								if(OK521[n%521])
								if(OK523[n%523])
								if(OK541[n%541])
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

					if(sito2){
						int b;
						int64_t n;
						int bLimit, bStart;

						bLimit = 63 - __builtin_clzll(sito2);
						bStart = __builtin_ctzll(sito2);

						for (b = bStart; b <= bLimit; b++){
							if ((sito2 >> b) & 1)
							{
								n=n59+(b+SHIFT+128)*MOD;

								if(n%7)
								if(n%11)
								if(n%13)
								if(n%17)
								if(n%19)
								if(n%23)
								if(OK281[n%281])
								if(OK283[n%283])
								if(OK293[n%293])
								if(OK307[n%307])
								if(OK311[n%311])
								if(OK313[n%313])
								if(OK317[n%317])
								if(OK331[n%331])
								if(OK337[n%337])
								if(OK347[n%347])
								if(OK349[n%349])
								if(OK353[n%353])
								if(OK359[n%359])
								if(OK367[n%367])
								if(OK373[n%373])
								if(OK379[n%379])
								if(OK383[n%383])
								if(OK389[n%389])
								if(OK397[n%397])
								if(OK401[n%401])
								if(OK409[n%409])
								if(OK419[n%419])
								if(OK421[n%421])
								if(OK431[n%431])
								if(OK433[n%433])
								if(OK439[n%439])
								if(OK443[n%443])
								if(OK449[n%449])
								if(OK457[n%457])
								if(OK461[n%461])
								if(OK463[n%463])
								if(OK467[n%467])
								if(OK479[n%479])
								if(OK487[n%487])
								if(OK491[n%491])
								if(OK499[n%499])
								if(OK503[n%503])
								if(OK509[n%509])
								if(OK521[n%521])
								if(OK523[n%523])
								if(OK541[n%541])
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

					if(sito3){
						int b;
						int64_t n;
						int bLimit, bStart;

						bLimit = 63 - __builtin_clzll(sito3);
						bStart = __builtin_ctzll(sito3);

						for (b = bStart; b <= bLimit; b++){
							if ((sito3 >> b) & 1)
							{
								n=n59+(b+SHIFT+192)*MOD;

								if(n%7)
								if(n%11)
								if(n%13)
								if(n%17)
								if(n%19)
								if(n%23)
								if(OK281[n%281])
								if(OK283[n%283])
								if(OK293[n%293])
								if(OK307[n%307])
								if(OK311[n%311])
								if(OK313[n%313])
								if(OK317[n%317])
								if(OK331[n%331])
								if(OK337[n%337])
								if(OK347[n%347])
								if(OK349[n%349])
								if(OK353[n%353])
								if(OK359[n%359])
								if(OK367[n%367])
								if(OK373[n%373])
								if(OK379[n%379])
								if(OK383[n%383])
								if(OK389[n%389])
								if(OK397[n%397])
								if(OK401[n%401])
								if(OK409[n%409])
								if(OK419[n%419])
								if(OK421[n%421])
								if(OK431[n%431])
								if(OK433[n%433])
								if(OK439[n%439])
								if(OK443[n%443])
								if(OK449[n%449])
								if(OK457[n%457])
								if(OK461[n%461])
								if(OK463[n%463])
								if(OK467[n%467])
								if(OK479[n%479])
								if(OK487[n%487])
								if(OK491[n%491])
								if(OK499[n%499])
								if(OK503[n%503])
								if(OK509[n%509])
								if(OK521[n%521])
								if(OK523[n%523])
								if(OK541[n%541])
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


				}}}

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
		
		iter+=4;
		if(iter<10){		
			checkpoint(startSHIFT,K,0,iter);
		}

	}

	time(&finish_time);
	printf("Computation of K: %d complete in %d seconds\n", K, (int)finish_time - (int)start_time);



}
