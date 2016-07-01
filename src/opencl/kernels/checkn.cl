/* PrimeQ_x86.h -- Geoffrey Reynolds, September 11 2008.

   On x86/x86_64 architectures this file can replace PrimeQ32.h/PrimeQ64.h
   in Jaroslaw Wroblewski's AP26 sample implementation.

   See http://www.geocities.com/g_w_reynolds/AP26/AP26-boinc-src.zip for the
   latest version of this file.

   See http://www.math.uni.wroc.pl/~jwr/AP26/AP26.zip for the sample
   implementation containing the rest of the source required.

   See http://www.math.uni.wroc.pl/~jwr/AP26/AP26v3.pdf for information
   about how the algorithm works and for the copyleft notice.


   PrimeQ(N) is limited to N < 2^62. This is adequate for the AP26 search.
   
   debruijn-function for emulating __built_ctzll taken from 
   http://www.pronix.de/comment/site-960/open-2240/site-2.html


	***************
	Bryan Little May 2014
	OpenCL conversion - checkn.cl


*/


ulong mulmod(ulong a, ulong b, ulong p, ulong magic, ulong shift)
{
	ulong ab0, ab1, mab01, mab10, mab11;
	ulong r, s0, s1, t;

	ab1 = mul_hi(a,b);
	ab0 = a*b;
	mab01 = mul_hi(magic,ab0);
	mab10 = magic * ab1;
	mab11 = mul_hi(magic,ab1);

	s0 = mab01 + mab10;
	s1 = mab11 + (s0 < mab01);
	t = (s0 >> shift) | (s1 << (64-shift));

	r = ab0 - t*p;

	if ((long)r < 0)
		r += p;
  
	return r;
}


/* Returns 0 only if N is composite.
   Otherwise N is a strong probable prime to base 2.
   Assumes N odd, 2 < N < 2^LDBL_MULMOD_LIMIT.
   For AP26, N can be assumed to have no prime divisors <= 541?
 */
int strong_prp(long N)
{
	ulong magic, shift;
	long r;
	ulong d;
	uint s, t;
	long a = 2;

	/* getMagic */
	ulong e = (ulong)N;
	ulong two63 = 0x8000000000000000;
	ulong x = two63;
	ulong anc = x -1 - (x-e*(x/e)); 	//ulong anc = t - 1 - (t-d*(t/d));    /* Absolute value of nc. */
	ulong p = 63; 				//ulong p = 63;            /* Init p. */
	ulong q1 = two63/anc; 			//ulong q1 = two63/anc;       /* Init q1 = 2**p/|nc|. */
	ulong r1 = two63 - q1*anc; 		//ulong r1 = two63 - q1*anc;  /* Init r1 = rem(2**p, |nc|). */
	ulong q2 = two63/e; 			//ulong q2 = two63/d;        /* Init q2 = 2**p/|d|. */
	ulong r2 = two63- q2*e; 		//ulong r2 = two63- q2*d;      /* Init r2 = rem(2**p, |d|). */
	ulong delta; 				//, mag;

	do {
		p+=1;
		q1<<=1;
		r1<<=1;
		if (r1 >= anc) {
			q1+=1;
			r1-=anc;
      		}
		q2<<=1;
		r2<<=1;
		if (r2 >= e) {
			q2+=1;
			r2-=e;
      		}
		delta = e - r2;
   	} while (q1 < delta || (q1 == delta && r1 == 0));

	q2+=1;
	shift = p - 64;
	magic = q2;
	/* end getMagic */

	for (d = (ulong)N >> 1, t = 1; !(d & 1); d >>= 1, t++);

  	/* r <-- a^d mod N, assuming d odd */
	for (r = a, d >>= 1; d > 0; d >>= 1)
	{
		a = mulmod(a,a,N,magic,shift);

		if (d & 1)
			r = mulmod(r,a,N,magic,shift);
	}

	/* Clause 1. and s = 0 case for clause 2. */
	if (r == 1 || r == N-1)
		return 1;
	
	/* 0 < s < t cases for clause 2. */
	for (s = 1; s < t; s++)
		if ((r = mulmod(r,r,N,magic,shift)) == N-1)
	    		return 1;
	
	return 0;
}


/*
	main prime checking kernel
*/
__kernel void checkn(__global long *n_result, long STEP, __global int *sol_k, __global long *sol_val, __global int *ncount, __global int *solcount){

	int i = get_global_id(0);

	if(i<ncount[0]){

		long n = n_result[i];

		long m;
		int k;
		int forward, backward;
/*
		k = 0;
		m=n+STEP*5;
		while(strong_prp(m)){
			k++;
			m+=STEP;
		}

		if(k>=10){
			m=n+STEP*4;
			while(m>0&&strong_prp(m)){
				k++;
				m-=STEP;
			}
		}
*/
		
		
		// Bryan Little 5-18-2014
		// New code for OpenCL for one call to strong_prp
		// OpenCL on AMD does not support true function calls.  Functions are substituted like DEFINES
		m=n+STEP*5;
		forward = 1;
		backward = 1;

		for(k=0; forward || backward;){

			if(strong_prp(m)){
				if(forward){
					m+=STEP;
				}
				else{
					m-=STEP;
					if(m<=0){
						backward=0;
					}
				}
				k++;
			}
			else{
				if(forward){
					forward=0;
					if(k>=10){
						m=n+STEP*4;
					}
					else{
						backward=0;
					}
				}
				else{
					backward=0;
				}
			}
		}





		if(k>=10){
			int index = atomic_add(&solcount[0], 1);
			sol_k[index] = k;
			sol_val[index] = m+STEP;
		}


	}
}
