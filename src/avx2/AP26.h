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

   Instruction latencies, throughputs for optimization
   http://www.agner.org/optimize/
   http://users.atw.hu/instlatx64/
*/


#include "cpuconst.h"
#include "setupoks.h"

#if _MSC_VER
#include <intrin.h>
#endif

// selects elements from two vectors based on a selection mask
#define vec_sel(_X, _Y, _Z) _mm256_blendv_epi8(_X, _Y, _Z)

// define this macro for compilers (e.g. GCC) that don't provide it
#ifndef _mm256_set_m128i
#define _mm256_set_m128i(v0, v1)  _mm256_insertf128_si256(_mm256_castsi128_si256(v1), (v0), 1)
#endif


void SearchAP26(int K, int startSHIFT, int ITER)
{ 
	int64_t n, n43, n47, n53, n59;
	int i43, i47, i53, i59;
	int i3, i5, i31, i37, i41;
	int SHIFT;
	int iter = ITER;
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
	__m256i s53vec1 = _mm256_set_epi16(S53%101, S53%103, S53%107, S53%109, S53%113, S53%127, S53%131, S53%137, S53%61, S53%67, S53%71, S53%73, S53%79, S53%83, S53%89, S53%97);

	__m256i svec1		= _mm256_set_epi16(S59%101, S59%103, S59%107, S59%109, S59%113, S59%127, S59%131, S59%137, S59%61, S59%67, S59%71, S59%73, S59%79, S59%83, S59%89, S59%97);
	//__m256i svec2 = _mm256_set_epi16(S59%227, S59%223, S59%211, S59%199, S59%197, S59%193, S59%191, S59%181, S59%179, 
	//	S59%173, S59%167, S59%163, S59%157, S59%151, S59%149, S59%139);
	
    __m256i mvec1		= _mm256_set_epi16(MOD%101, MOD%103, MOD%107, MOD%109, MOD%113, MOD%127, MOD%131, MOD%137,MOD%61, MOD%67, MOD%71, MOD%73, MOD%79, MOD%83, MOD%89, MOD%97);
	//__m256i mvec2 = _mm256_set_epi16(MOD%227, MOD%223, MOD%211, MOD%199, MOD%197, MOD%193, MOD%191, MOD%181, MOD%179,
	//	MOD%173, MOD%167, MOD%163, MOD%157, MOD%151, MOD%149, MOD%139);

	__m256i m59vec1 = _mm256_sub_epi16(svec1, mvec1);

    __m256i numvec11	= _mm256_set_epi16(101, 103, 107, 109, 113, 127, 131, 137, 61, 67, 71, 73, 79, 83, 89, 97);
		//__m256i numvec12 = _mm256_set_epi16(227, 223, 211, 199, 197, 193, 191, 181, 179, 173, 167, 163, 157, 151, 149, 139);

    __m256i numvec21		= _mm256_set_epi16(100, 102, 106, 108, 112, 126, 130, 136, 60, 66, 70, 72, 78, 82, 88, 96);
		//__m256i numvec22 = _mm256_set_epi16(226, 222, 210, 198, 196, 192, 190, 180, 178, 172, 166, 162, 156, 150, 148, 138);

     __m256i zerovec	= _mm256_set_epi16(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	 __m256d v3con1 = _mm256_set_pd((1.0 / 61.0), (1.0 / 67.0), (1.0 / 71.0), (1.0 / 73.0)); 
	 __m256d v3con2 = _mm256_set_pd((1.0 / 79.0), (1.0 / 83.0), (1.0 / 89.0), (1.0 / 97.0));
	 __m256d v3con3 = _mm256_set_pd((1.0 / 101.0), (1.0 / 103.0), (1.0 / 107.0), (1.0 / 109.0));
	 __m256d v3con4 = _mm256_set_pd((1.0 / 113.0), (1.0 / 127.0), (1.0 / 131.0), (1.0 / 137.0));
		 

	 __m256d v3cond1 = _mm256_set_pd((61.0), (67.0), (71.0), (73.0));
	 __m256d v3cond2 = _mm256_set_pd((79.0), (83.0), (89.0), (97.0));
	 __m256d v3cond3 = _mm256_set_pd((101.0), (103.0), (107.0), (109.0));
	 __m256d v3cond4 = _mm256_set_pd((113.0), (127.0), (131.0), (137.0));

	 __m256i c59vec1 = _mm256_sub_epi16(mvec1, svec1);

	 __m256i a59vec1 = _mm256_sub_epi16(svec1, mvec1);

	 __m256i a59vec1e = _mm256_add_epi16(a59vec1, numvec11);

	 __m256i c53vec1 = _mm256_sub_epi16(mvec1, s53vec1);

	 __m256i a53vec1 = _mm256_sub_epi16(s53vec1, mvec1);

	 __m256i a53vec1e = _mm256_add_epi16(a53vec1, numvec11);

	// 10 shift
	for(SHIFT=startSHIFT+(iter*64); SHIFT<(startSHIFT+640); SHIFT+=64){

		time(&start_time);

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

		__m256d vn53 = _mm256_set1_pd(double(n53));

		__m256d v3mul1 = _mm256_mul_pd(vn53, v3con1);
		__m256d v3mul2 = _mm256_mul_pd(vn53, v3con2);
		__m256d v3mul3 = _mm256_mul_pd(vn53, v3con3);
		__m256d v3mul4 = _mm256_mul_pd(vn53, v3con4);

		v3mul1 = _mm256_floor_pd(v3mul1);
		v3mul2 = _mm256_floor_pd(v3mul2);
		v3mul3 = _mm256_floor_pd(v3mul3);
		v3mul4 = _mm256_floor_pd(v3mul4);

		v3mul1 = _mm256_fnmadd_pd(v3mul1, v3cond1, vn53);
		v3mul2 = _mm256_fnmadd_pd(v3mul2, v3cond2, vn53);
		v3mul3 = _mm256_fnmadd_pd(v3mul3, v3cond3, vn53);
		v3mul4 = _mm256_fnmadd_pd(v3mul4, v3cond4, vn53);

		__m128i v3num1 = _mm256_cvttpd_epi32(v3mul1);
		__m128i v3num2 = _mm256_cvttpd_epi32(v3mul2);
		__m128i v3num3 = _mm256_cvttpd_epi32(v3mul3);
		__m128i v3num4 = _mm256_cvttpd_epi32(v3mul4);

		__m128i r_numvec1a = _mm_packus_epi32(v3num2, v3num1);
		__m128i r_numvec2a = _mm_packus_epi32(v3num4, v3num3);

		__m256i r_numvec1t = _mm256_set_m128i(r_numvec2a, r_numvec1a);

		for(i53=(PRIME7-24);i53>0;i53--)
		{
		n59=n53;

		__m256i r_numvec1 = r_numvec1t;

			for(i59=(PRIME8-24);i59>0;i59--)
			{
                                if(sito = OKOK61[_mm256_extract_epi16(r_numvec1, 7)]
                                                & OKOK67[_mm256_extract_epi16(r_numvec1, 6)]
                                                & OKOK71[_mm256_extract_epi16(r_numvec1, 5)]
                                                & OKOK73[_mm256_extract_epi16(r_numvec1, 4)]
                                                & OKOK79[_mm256_extract_epi16(r_numvec1, 3)]
                                                & OKOK83[_mm256_extract_epi16(r_numvec1, 2)]
                                                & OKOK89[_mm256_extract_epi16(r_numvec1, 1)]
                                                & OKOK97[_mm256_extract_epi16(r_numvec1, 0)])
                                if(sito&=(OKOK101[_mm256_extract_epi16(r_numvec1, 15)]
                                                & OKOK103[_mm256_extract_epi16(r_numvec1, 14)]
                                                & OKOK107[_mm256_extract_epi16(r_numvec1, 13)]
                                                & OKOK109[_mm256_extract_epi16(r_numvec1, 12)]
                                                & OKOK113[_mm256_extract_epi16(r_numvec1, 11)]
                                                & OKOK127[_mm256_extract_epi16(r_numvec1, 10)]
                                                & OKOK131[_mm256_extract_epi16(r_numvec1, 9)]
                                                & OKOK137[_mm256_extract_epi16(r_numvec1, 8)]))
												//if (sito &= (OKOK139[_mm256_extract_epi16(r_numvec2, 0)]
												//	& OKOK149[_mm256_extract_epi16(r_numvec2, 1)]
												//	& OKOK151[_mm256_extract_epi16(r_numvec2, 2)]
												//	& OKOK157[_mm256_extract_epi16(r_numvec2, 3)]
												//	& OKOK163[_mm256_extract_epi16(r_numvec2, 4)]
												//	& OKOK167[_mm256_extract_epi16(r_numvec2, 5)]
												//	& OKOK173[_mm256_extract_epi16(r_numvec2, 6)]
												//	& OKOK179[_mm256_extract_epi16(r_numvec2, 7)]))
												//	if (sito &= (OKOK181[_mm256_extract_epi16(r_numvec2, 8)]
												//		& OKOK191[_mm256_extract_epi16(r_numvec2, 9)]
												//		& OKOK193[_mm256_extract_epi16(r_numvec2, 10)]
												//		& OKOK197[_mm256_extract_epi16(r_numvec2, 11)]
												//		& OKOK199[_mm256_extract_epi16(r_numvec2, 12)]
												//		& OKOK211[_mm256_extract_epi16(r_numvec2, 13)]
												//		& OKOK223[_mm256_extract_epi16(r_numvec2, 14)]
												//		& OKOK227[_mm256_extract_epi16(r_numvec2, 15)]))
				//if (sito &= (OKOK229[REM(n59, 229, 8)]

				if (sito &= (OKOK139[REM(n59, 139, 8)]
					& OKOK149[REM(n59, 149, 8)]
					& OKOK151[REM(n59, 151, 8)]
					& OKOK157[REM(n59, 157, 8)]
					& OKOK163[REM(n59, 163, 8)]
					& OKOK167[REM(n59, 167, 8)]
					& OKOK173[REM(n59, 173, 8)]
					& OKOK179[REM(n59, 179, 8)]
					& OKOK181[REM(n59, 181, 8)]
					& OKOK191[REM(n59, 191, 8)]
					& OKOK193[REM(n59, 193, 8)]))
					if (sito &= (OKOK197[REM(n59, 197, 8)]
						& OKOK199[REM(n59, 199, 8)]
						& OKOK211[REM(n59, 211, 8)]
						& OKOK223[REM(n59, 223, 8)]
						& OKOK227[REM(n59, 227, 8)]
						& OKOK229[REM(n59, 229, 8)]
						& OKOK233[REM(n59, 233, 8)]
						& OKOK239[REM(n59, 239, 8)]
						& OKOK241[REM(n59, 241, 8)]
						& OKOK251[REM(n59, 251, 8)]
						& OKOK257[REM(n59, 257, 9)]))
						if (sito &= (OKOK263[REM(n59, 263, 9)]

					/*	& OKOK233[REM(n59,233,8)]
						& OKOK239[REM(n59,239,8)]
						& OKOK241[REM(n59,241,8)]
						& OKOK251[REM(n59,251,8)]
						& OKOK257[REM(n59,257,9)]*/
						& OKOK263[REM(n59,263,9)]
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
					unsigned long bLimit, bStart;
#if defined(_MSC_VER) && defined(__x86_64__)
					_BitScanReverse64(&bLimit, sito);
					_BitScanForward64(&bStart, sito);
					for (b = bStart; b <= bLimit; b++)
						if (_bittest64(&sito, b))
#else
					bLimit = __builtin_clzll(sito);
					bStart = __builtin_ctzll(sito);
					for (b = bStart; b <= bLimit; b++)
						if ((sito >> b) & 1)
#endif
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

				if (n59 >= MOD){
					n59 -= MOD;

					__m256i cmpb1 = _mm256_cmpgt_epi16(c59vec1, r_numvec1);

					__m256i add1 = vec_sel(a59vec1, a59vec1e, cmpb1);

					r_numvec1 = _mm256_add_epi16(r_numvec1, add1);
				}
				else
				{
					r_numvec1 = _mm256_add_epi16(r_numvec1, svec1);
				}

				__m256i cmpa1 = _mm256_cmpgt_epi16(r_numvec1, numvec21);

				__m256i sub1 = _mm256_sub_epi16(r_numvec1, numvec11);

				r_numvec1 = vec_sel(r_numvec1, sub1, cmpa1);
			}     

			n53 += S53;
			if (n53 >= MOD)
			{
				n53 -= MOD;

				__m256i cmpb1a = _mm256_cmpgt_epi16(c53vec1, r_numvec1t);

				__m256i add1a = vec_sel(a53vec1, a53vec1e, cmpb1a);

				r_numvec1t = _mm256_add_epi16(r_numvec1t, add1a);
			}
			else
			{
				r_numvec1t = _mm256_add_epi16(r_numvec1t, s53vec1);
			}

			__m256i cmpa1a = _mm256_cmpgt_epi16(r_numvec1t, numvec21);

			__m256i sub1a = _mm256_sub_epi16(r_numvec1t, numvec11);

			r_numvec1t = vec_sel(r_numvec1t, sub1a, cmpa1a);
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

		time(&finish_time);
		printf("Computation of K: %d SHIFT: %d complete in %d seconds\n", K, SHIFT, (int)finish_time - (int)start_time);
		iter++;
		checkpoint(startSHIFT, K, 0, iter);

	}



}
