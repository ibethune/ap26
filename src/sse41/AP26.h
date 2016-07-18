/* AP26-boinc.h --

   Modified for SSE2 support 27 September 2009 by Bryan Little
   Modified for SSE41 support 20 March 2012 by Sebastian Jaworowicz

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
#define vec_sel(_X, _Y, _Z) _mm_blendv_epi8(_X, _Y, _Z)

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
	__m128i s53vec1 = _mm_set_epi16(S53 % 61, S53 % 67, S53 % 71, S53 % 73, S53 % 79, S53 % 83, S53 % 89, S53 % 97);
	__m128i s53vec2 = _mm_set_epi16(S53 % 101, S53 % 103, S53 % 107, S53 % 109, S53 % 113, S53 % 127, S53 % 131, S53 % 137);
	__m128i s53vec3 = _mm_set_epi16(S53 % 139, S53 % 149, S53 % 151, S53 % 157, S53 % 163, S53 % 167, S53 % 173, S53 % 179);

	__m128i svec1 = _mm_set_epi16(S59 % 61, S59 % 67, S59 % 71, S59 % 73, S59 % 79, S59 % 83, S59 % 89, S59 % 97);
	__m128i svec2 = _mm_set_epi16(S59 % 101, S59 % 103, S59 % 107, S59 % 109, S59 % 113, S59 % 127, S59 % 131, S59 % 137);
	__m128i svec3 = _mm_set_epi16(S59 % 139, S59 % 149, S59 % 151, S59 % 157, S59 % 163, S59 % 167, S59 % 173, S59 % 179);

	__m128i mvec1 = _mm_set_epi16(MOD % 61, MOD % 67, MOD % 71, MOD % 73, MOD % 79, MOD % 83, MOD % 89, MOD % 97);
	__m128i mvec2 = _mm_set_epi16(MOD % 101, MOD % 103, MOD % 107, MOD % 109, MOD % 113, MOD % 127, MOD % 131, MOD % 137);
	__m128i mvec3 = _mm_set_epi16(MOD % 139, MOD % 149, MOD % 151, MOD % 157, MOD % 163, MOD % 167, MOD % 173, MOD % 179);

	__m128i m59vec1 = _mm_sub_epi16(svec1, mvec1);
	__m128i m59vec2 = _mm_sub_epi16(svec2, mvec2);
	__m128i m59vec3 = _mm_sub_epi16(svec3, mvec3);

	__m128i numvec1_1 = _mm_set_epi16(61, 67, 71, 73, 79, 83, 89, 97);
	__m128i numvec2_1 = _mm_set_epi16(60, 66, 70, 72, 78, 82, 88, 96);

	__m128i numvec1_2 = _mm_set_epi16(101, 103, 107, 109, 113, 127, 131, 137);
	__m128i numvec2_2 = _mm_set_epi16(100, 102, 106, 108, 112, 126, 130, 136);

	__m128i numvec1_3 = _mm_set_epi16(139, 149, 151, 157, 163, 167, 173, 179);
	__m128i numvec2_3 = _mm_set_epi16(138, 148, 150, 156, 162, 166, 172, 178);

	__m128d v3con1 = _mm_set_pd((1.0 / 67), (1.0 / 73));
	__m128d v3con2 = _mm_set_pd((1.0 / 61), (1.0 / 71));
	__m128d v3con3 = _mm_set_pd((1.0 / 83), (1.0 / 97));
	__m128d v3con4 = _mm_set_pd((1.0 / 79), (1.0 / 89));
	__m128d v3con5 = _mm_set_pd((1.0 / 103), (1.0 / 109));
	__m128d v3con6 = _mm_set_pd((1.0 / 101), (1.0 / 107));
	__m128d v3con7 = _mm_set_pd((1.0 / 127), (1.0 / 137));
	__m128d v3con8 = _mm_set_pd((1.0 / 113), (1.0 / 131));

	__m128d v3con9 = _mm_set_pd((1.0 / 149), (1.0 / 157));
	__m128d v3con10 = _mm_set_pd((1.0 / 139), (1.0 / 151));
	__m128d v3con11 = _mm_set_pd((1.0 / 167), (1.0 / 179));
	__m128d v3con12 = _mm_set_pd((1.0 / 163), (1.0 / 173));

	__m128d v3cond1 = _mm_set_pd(67.0, (73.0));
	__m128d v3cond2 = _mm_set_pd((61.0), (71.0));
	__m128d v3cond3 = _mm_set_pd((83.0), (97.0));
	__m128d v3cond4 = _mm_set_pd((79.0), (89.0));
	__m128d v3cond5 = _mm_set_pd((103.0), (109.0));
	__m128d v3cond6 = _mm_set_pd((101.0), (107.0));
	__m128d v3cond7 = _mm_set_pd((127.0), (137.0));
	__m128d v3cond8 = _mm_set_pd((113.0), (131.0));

	__m128d v3cond9 = _mm_set_pd((149.0), (157.0));
	__m128d v3cond10 = _mm_set_pd((139.0), (151.0));
	__m128d v3cond11 = _mm_set_pd((167.0), (179.0));
	__m128d v3cond12 = _mm_set_pd((163.0), (173.0));

	__m128i c59vec1 = _mm_sub_epi16(mvec1, svec1);
	__m128i c59vec2 = _mm_sub_epi16(mvec2, svec2);
	__m128i c59vec3 = _mm_sub_epi16(mvec3, svec3);

	__m128i a59vec1 = _mm_sub_epi16(svec1, mvec1);
	__m128i a59vec2 = _mm_sub_epi16(svec2, mvec2);
	__m128i a59vec3 = _mm_sub_epi16(svec3, mvec3);

	__m128i a59vec1e = _mm_add_epi16(a59vec1, numvec1_1);
	__m128i a59vec2e = _mm_add_epi16(a59vec2, numvec1_2);
	__m128i a59vec3e = _mm_add_epi16(a59vec3, numvec1_3);

	__m128i c53vec1 = _mm_sub_epi16(mvec1, s53vec1);
	__m128i c53vec2 = _mm_sub_epi16(mvec2, s53vec2);
	__m128i c53vec3 = _mm_sub_epi16(mvec3, s53vec3);

	__m128i a53vec1 = _mm_sub_epi16(s53vec1, mvec1);
	__m128i a53vec2 = _mm_sub_epi16(s53vec2, mvec2);
	__m128i a53vec3 = _mm_sub_epi16(s53vec3, mvec3);

	__m128i a53vec1e = _mm_add_epi16(a53vec1, numvec1_1);
	__m128i a53vec2e = _mm_add_epi16(a53vec2, numvec1_2);
	__m128i a53vec3e = _mm_add_epi16(a53vec3, numvec1_3);
		

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

		double da[2];

		da[0] = n53;
		da[1] = n53;

		__m128d vn53 = _mm_loadu_pd(da);

		__m128d v3mul1 = _mm_mul_pd(vn53, v3con1);
		__m128d v3mul2 = _mm_mul_pd(vn53, v3con2);
		__m128d v3mul3 = _mm_mul_pd(vn53, v3con3);
		__m128d v3mul4 = _mm_mul_pd(vn53, v3con4);
		__m128d v3mul5 = _mm_mul_pd(vn53, v3con5);
		__m128d v3mul6 = _mm_mul_pd(vn53, v3con6);
		__m128d v3mul7 = _mm_mul_pd(vn53, v3con7);
		__m128d v3mul8 = _mm_mul_pd(vn53, v3con8);

		__m128d v3mul9 = _mm_mul_pd(vn53, v3con9);
		__m128d v3mul10 = _mm_mul_pd(vn53, v3con10);
		__m128d v3mul11 = _mm_mul_pd(vn53, v3con11);
		__m128d v3mul12 = _mm_mul_pd(vn53, v3con12);

		v3mul1 = _mm_floor_pd(v3mul1);
		v3mul2 = _mm_floor_pd(v3mul2);
		v3mul3 = _mm_floor_pd(v3mul3);
		v3mul4 = _mm_floor_pd(v3mul4);
		v3mul5 = _mm_floor_pd(v3mul5);
		v3mul6 = _mm_floor_pd(v3mul6);
		v3mul7 = _mm_floor_pd(v3mul7);
		v3mul8 = _mm_floor_pd(v3mul8);

		v3mul9 = _mm_floor_pd(v3mul9);
		v3mul10 = _mm_floor_pd(v3mul10);
		v3mul11 = _mm_floor_pd(v3mul11);
		v3mul12 = _mm_floor_pd(v3mul12);

		v3mul1 = _mm_mul_pd(v3mul1, v3cond1);
		v3mul2 = _mm_mul_pd(v3mul2, v3cond2);
		v3mul3 = _mm_mul_pd(v3mul3, v3cond3);
		v3mul4 = _mm_mul_pd(v3mul4, v3cond4);
		v3mul5 = _mm_mul_pd(v3mul5, v3cond5);
		v3mul6 = _mm_mul_pd(v3mul6, v3cond6);
		v3mul7 = _mm_mul_pd(v3mul7, v3cond7);
		v3mul8 = _mm_mul_pd(v3mul8, v3cond8);

		v3mul9 = _mm_mul_pd(v3mul9, v3cond9);
		v3mul10 = _mm_mul_pd(v3mul10, v3cond10);
		v3mul11 = _mm_mul_pd(v3mul11, v3cond11);
		v3mul12 = _mm_mul_pd(v3mul12, v3cond12);

		v3mul1 = _mm_sub_pd(vn53, v3mul1);
		v3mul2 = _mm_sub_pd(vn53, v3mul2);
		v3mul3 = _mm_sub_pd(vn53, v3mul3);
		v3mul4 = _mm_sub_pd(vn53, v3mul4);
		v3mul5 = _mm_sub_pd(vn53, v3mul5);
		v3mul6 = _mm_sub_pd(vn53, v3mul6);
		v3mul7 = _mm_sub_pd(vn53, v3mul7);
		v3mul8 = _mm_sub_pd(vn53, v3mul8);

		v3mul9 = _mm_sub_pd(vn53, v3mul9);
		v3mul10 = _mm_sub_pd(vn53, v3mul10);
		v3mul11 = _mm_sub_pd(vn53, v3mul11);
		v3mul12 = _mm_sub_pd(vn53, v3mul12);

		__m128i v3num1 = _mm_cvttpd_epi32(v3mul1);
		__m128i v3num2 = _mm_cvttpd_epi32(v3mul2);
		__m128i v3num3 = _mm_cvttpd_epi32(v3mul3);
		__m128i v3num4 = _mm_cvttpd_epi32(v3mul4);
		__m128i v3num5 = _mm_cvttpd_epi32(v3mul5);
		__m128i v3num6 = _mm_cvttpd_epi32(v3mul6);
		__m128i v3num7 = _mm_cvttpd_epi32(v3mul7);
		__m128i v3num8 = _mm_cvttpd_epi32(v3mul8);

		__m128i v3num9 = _mm_cvttpd_epi32(v3mul9);
		__m128i v3num10 = _mm_cvttpd_epi32(v3mul10);
		__m128i v3num11 = _mm_cvttpd_epi32(v3mul11);
		__m128i v3num12 = _mm_cvttpd_epi32(v3mul12);

		__m128i v3shift1 = _mm_unpacklo_epi32(v3num1, v3num2);
		__m128i v3shift2 = _mm_unpacklo_epi32(v3num3, v3num4);
		__m128i v3shift3 = _mm_unpacklo_epi32(v3num5, v3num6);
		__m128i v3shift4 = _mm_unpacklo_epi32(v3num7, v3num8);

		__m128i v3shift5 = _mm_unpacklo_epi32(v3num9, v3num10);
		__m128i v3shift6 = _mm_unpacklo_epi32(v3num11, v3num12);

		__m128i r_numvec1a = _mm_packus_epi32(v3shift2, v3shift1);
		__m128i r_numvec2a = _mm_packus_epi32(v3shift4, v3shift3);
		__m128i r_numvec3a = _mm_packus_epi32(v3shift6, v3shift5);

		for(i53=(PRIME7-24);i53>0;i53--)
		{
		n59=n53;

		__m128i r_numvec1 = _mm_load_si128(&r_numvec1a);
		__m128i r_numvec2 = _mm_load_si128(&r_numvec2a);
		__m128i r_numvec3 = _mm_load_si128(&r_numvec3a);

			for(i59=(PRIME8-24);i59>0;i59--)
			{
				__m128i l1, l2, l3, l4, l5, l6, l7, l8, wy;

				l1 = _mm_loadl_epi64((__m128i const*)&OKOK61[_mm_extract_epi16(r_numvec1, 7)]);
				l2 = _mm_loadl_epi64((__m128i const*)&OKOK67[_mm_extract_epi16(r_numvec1, 6)]);
				l3 = _mm_loadl_epi64((__m128i const*)&OKOK71[_mm_extract_epi16(r_numvec1, 5)]);
				l4 = _mm_loadl_epi64((__m128i const*)&OKOK73[_mm_extract_epi16(r_numvec1, 4)]);
				l5 = _mm_loadl_epi64((__m128i const*)&OKOK79[_mm_extract_epi16(r_numvec1, 3)]);
				l6 = _mm_loadl_epi64((__m128i const*)&OKOK83[_mm_extract_epi16(r_numvec1, 2)]);
				l7 = _mm_loadl_epi64((__m128i const*)&OKOK89[_mm_extract_epi16(r_numvec1, 1)]);
				l8 = _mm_loadl_epi64((__m128i const*)&OKOK97[_mm_extract_epi16(r_numvec1, 0)]);

				wy = _mm_and_si128(l1, l2);
				wy = _mm_and_si128(wy, l3);
				wy = _mm_and_si128(wy, l4);
				wy = _mm_and_si128(wy, l5);
				wy = _mm_and_si128(wy, l6);
				wy = _mm_and_si128(wy, l7);
				wy = _mm_and_si128(wy, l8);

				l1 = _mm_loadl_epi64((__m128i const*)&OKOK101[_mm_extract_epi16(r_numvec2, 7)]);
				l2 = _mm_loadl_epi64((__m128i const*)&OKOK103[_mm_extract_epi16(r_numvec2, 6)]);
				l3 = _mm_loadl_epi64((__m128i const*)&OKOK107[_mm_extract_epi16(r_numvec2, 5)]);
				l4 = _mm_loadl_epi64((__m128i const*)&OKOK109[_mm_extract_epi16(r_numvec2, 4)]);
				l5 = _mm_loadl_epi64((__m128i const*)&OKOK113[_mm_extract_epi16(r_numvec2, 3)]);
				l6 = _mm_loadl_epi64((__m128i const*)&OKOK127[_mm_extract_epi16(r_numvec2, 2)]);
				l7 = _mm_loadl_epi64((__m128i const*)&OKOK131[_mm_extract_epi16(r_numvec2, 1)]);
				l8 = _mm_loadl_epi64((__m128i const*)&OKOK137[_mm_extract_epi16(r_numvec2, 0)]);

				wy = _mm_and_si128(wy, l1);
				wy = _mm_and_si128(wy, l2);
				wy = _mm_and_si128(wy, l3);
				wy = _mm_and_si128(wy, l4);
				wy = _mm_and_si128(wy, l5);
				wy = _mm_and_si128(wy, l6);
				wy = _mm_and_si128(wy, l7);
				wy = _mm_and_si128(wy, l8);

				l1 = _mm_loadl_epi64((__m128i const*)&OKOK139[_mm_extract_epi16(r_numvec3, 7)]);
				l2 = _mm_loadl_epi64((__m128i const*)&OKOK149[_mm_extract_epi16(r_numvec3, 6)]);
				l3 = _mm_loadl_epi64((__m128i const*)&OKOK151[_mm_extract_epi16(r_numvec3, 5)]);
				l4 = _mm_loadl_epi64((__m128i const*)&OKOK157[_mm_extract_epi16(r_numvec3, 4)]);
				l5 = _mm_loadl_epi64((__m128i const*)&OKOK163[_mm_extract_epi16(r_numvec3, 3)]);
				l6 = _mm_loadl_epi64((__m128i const*)&OKOK167[_mm_extract_epi16(r_numvec3, 2)]);
				l7 = _mm_loadl_epi64((__m128i const*)&OKOK173[_mm_extract_epi16(r_numvec3, 1)]);
				l8 = _mm_loadl_epi64((__m128i const*)&OKOK179[_mm_extract_epi16(r_numvec3, 0)]);

				wy = _mm_and_si128(wy, l1);
				wy = _mm_and_si128(wy, l2);
				wy = _mm_and_si128(wy, l3);
				wy = _mm_and_si128(wy, l4);
				wy = _mm_and_si128(wy, l5);
				wy = _mm_and_si128(wy, l6);
				wy = _mm_and_si128(wy, l7);
				wy = _mm_and_si128(wy, l8);

				if (!_mm_testz_si128(wy, wy))
				{
							//_mm_storel_epi64((__m128i *)&sito, wy);
					sito = _mm_extract_epi64(wy, 0);

				if (sito &= (OKOK181[REM(n59, 181, 8)]
						& OKOK191[REM(n59,191,8)]
						& OKOK193[REM(n59,193,8)]
						& OKOK197[REM(n59,197,8)]
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
#if defined(_MSC_VER) && defined(__x86_64__)
					unsigned long bLimit, bStart;

					_BitScanReverse64(&bLimit, sito);
					_BitScanForward64(&bStart, sito);
					for (b = bStart; b <= bLimit; b++)
						if(_bittest64(&sito, b))
#else
#warning TODO gcc bit scan
					for (b = 0; b<64; b++)
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

				} } 


				n59+=S59;

				if (n59 >= MOD){
					n59 -= MOD;

					__m128i cmpb1 = _mm_cmpgt_epi16(c59vec1, r_numvec1);
					__m128i cmpb2 = _mm_cmpgt_epi16(c59vec2, r_numvec2);
					__m128i cmpb3 = _mm_cmpgt_epi16(c59vec3, r_numvec3);

					__m128i add1 = vec_sel(a59vec1, a59vec1e, cmpb1);
					__m128i add2 = vec_sel(a59vec2, a59vec2e, cmpb2);
					__m128i add3 = vec_sel(a59vec3, a59vec3e, cmpb3);

					r_numvec1 = _mm_add_epi16(r_numvec1, add1);
					r_numvec2 = _mm_add_epi16(r_numvec2, add2);
					r_numvec3 = _mm_add_epi16(r_numvec3, add3);
				}
				else
				{
					r_numvec1 = _mm_add_epi16(r_numvec1, svec1);
					r_numvec2 = _mm_add_epi16(r_numvec2, svec2);
					r_numvec3 = _mm_add_epi16(r_numvec3, svec3);
				}

				__m128i cmpa1 = _mm_cmpgt_epi16(r_numvec1, numvec2_1);
				__m128i cmpa2 = _mm_cmpgt_epi16(r_numvec2, numvec2_2);
				__m128i cmpa3 = _mm_cmpgt_epi16(r_numvec3, numvec2_3);

				__m128i sub1 = _mm_sub_epi16(r_numvec1, numvec1_1);
				__m128i sub2 = _mm_sub_epi16(r_numvec2, numvec1_2);
				__m128i sub3 = _mm_sub_epi16(r_numvec3, numvec1_3);

				r_numvec1 = vec_sel(r_numvec1, sub1, cmpa1);
				r_numvec2 = vec_sel(r_numvec2, sub2, cmpa2);
				r_numvec3 = vec_sel(r_numvec3, sub3, cmpa3);
			   
			}     

		n53+=S53;
		if (n53 >= MOD)
		{
			n53 -= MOD;

			__m128i cmpb1a = _mm_cmpgt_epi16(c53vec1, r_numvec1a);
			__m128i cmpb2a = _mm_cmpgt_epi16(c53vec2, r_numvec2a);
			__m128i cmpb3a = _mm_cmpgt_epi16(c53vec3, r_numvec3a);

			__m128i add1a = vec_sel(a53vec1, a53vec1e, cmpb1a);
			__m128i add2a = vec_sel(a53vec2, a53vec2e, cmpb2a);
			__m128i add3a = vec_sel(a53vec3, a53vec3e, cmpb3a);

			r_numvec1a = _mm_add_epi16(r_numvec1a, add1a);
			r_numvec2a = _mm_add_epi16(r_numvec2a, add2a);
			r_numvec3a = _mm_add_epi16(r_numvec3a, add3a);
		}
		else
		{
			r_numvec1a = _mm_add_epi16(r_numvec1a, s53vec1);
			r_numvec2a = _mm_add_epi16(r_numvec2a, s53vec2);
			r_numvec3a = _mm_add_epi16(r_numvec3a, s53vec3);
		}

		__m128i cmpa1a = _mm_cmpgt_epi16(r_numvec1a, numvec2_1);
		__m128i cmpa2a = _mm_cmpgt_epi16(r_numvec2a, numvec2_2);
		__m128i cmpa3a = _mm_cmpgt_epi16(r_numvec3a, numvec2_3);

		__m128i sub1a = _mm_sub_epi16(r_numvec1a, numvec1_1);
		__m128i sub2a = _mm_sub_epi16(r_numvec2a, numvec1_2);
		__m128i sub3a = _mm_sub_epi16(r_numvec3a, numvec1_3);

		r_numvec1a = vec_sel(r_numvec1a, sub1a, cmpa1a);
		r_numvec2a = vec_sel(r_numvec2a, sub2a, cmpa2a);
		r_numvec3a = vec_sel(r_numvec3a, sub3a, cmpa3a);
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
