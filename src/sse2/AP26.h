/* AP26-boinc.h --

   Modified for SSE2 support 27 September 2009 by Bryan Little

   This file is a replacement for AP26.h in Jaroslaw Wroblewski's AP26
   sample implementation. Most of my changes are between #if REPORT_FREQUENCY
   #endif clauses, they exist only to allow the program to checkpoint part
   way through the SearchAP26(K,SHIFT) routine. Other minor changes are to
   use int64_t instead of long long for compilers that don't conform to C99.

   See http://www.geocities.com/g_w_reynolds/AP26/AP26-boinc-src.zip for the
   latest version of this file.

   See http://www.math.uni.wroc.pl/~jwr/AP26/AP26.zip for the sample
   implementation containing the rest of the source required.

   See http://www.math.uni.wroc.pl/~jwr/AP26/AP26v3.pdf for information
   about how the algorithm works and for the copyleft notice.
*/


#include "cpuconst.h"
#include "setupoks.h"


// selects elements from two vectors based on a selection mask
#define vec_sel(_X, _Y, _Z) \
	_mm_or_si128( _mm_andnot_si128( _Z, _X ), _mm_and_si128( _Y, _Z ) )



void SearchAP26(int K, int startSHIFT)
{ 
	int64_t n, n43, n47, n53, n59;
	int i43, i47, i53, i59;
	int i3, i5, i31, i37, i41;
	int SHIFT;

	int64_t STEP;
	int64_t n0;
	int64_t sito;
	int64_t S31, S37, S41, S43, S47, S53, S59;

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
		

	// 10 shift
	for(SHIFT=startSHIFT; SHIFT<(startSHIFT+640); SHIFT+=64){

		// init OKOK arrays    
		setupoks(SHIFT,STEP);

		// start searching
		for(i31=0;i31<7;++i31){
		for(i37=0;i37<13;++i37)
		if(i37-i31<=10&&i31-i37<=4)
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
					for(b=0;b<64;b++)
						if((sito>>b)&1)
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
									/* 	hash for BOINC quorum
										we create a hash based on each AP10+ result mod 1000
										and that result's AP length  	*/
									result_hash += first_term % 1000;
									result_hash += k;
									if(result_hash > MAXINTV){
										result_hash -= MAXINTV;
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

		printf("Computation of K: %d SHIFT: %d complete\n", K, SHIFT);

	}


	time(&finish_time);
	printf("total CPU time for K was %d seconds\n", (int)finish_time - (int)start_time);


}
