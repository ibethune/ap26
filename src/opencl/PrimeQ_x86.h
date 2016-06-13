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
*/


#include <stdio.h>
#include <stdlib.h>
#include <float.h>

/* Some compilers or compiler options use 64-bit instead of 80-bit long
   doubles. This code will still work but the modulus will be limited to
   2^52 instead of 2^62. It might be better just to stop with a compile-time
   error in that case.
*/
#if (defined(LDBL_MANT_DIG) && LDBL_MANT_DIG >= 64)
# define LDBL_MULMOD_LIMIT 62
#else
# define LDBL_MULMOD_LIMIT 52
# warning long double mantissa is < 64 bits wide,
# if (defined(__i386__) || defined(__x86_64__))
#  warning try building with -DNO_LONG_DOUBLE instead.
# else
#  warning try building with -DGMP -lgmp instead.
# endif
#endif


/* Even if a compiler uses 80-bit long doubles, it appears possible that it
   might still perform double-precision operations on those types. This
   function checks and ensures that the FPU is in extended precision mode.
*/
#if (defined(__i386__) || defined(__x86_64__))
static void check_fpu_mode(void)
{
  unsigned short fp_mode;

  if (sizeof(long double) < 10)
  {
    fprintf(stderr,__FILE__": long double type is too narrow.\n");
    exit(EXIT_FAILURE);
  }

# ifdef _MSC_VER
  __asm { fnstcw word ptr fp_mode }
# else
  asm ("fnstcw %0" : "=m" (fp_mode) );
# endif

  /* Bits 8-9 determine precision: 00=single, 10=double, 11=extended */
  if ((fp_mode & 0x0300) != 0x0300)
  {
    printf("Found the FPU in %s precision mode, "
           "switching to extended precision mode.\n",
           (fp_mode & 0x0200) ? "double" : "single");

    fp_mode |= 0x0300;
# ifdef _MSC_VER
    __asm { fldcw word ptr fp_mode }
# else
    asm volatile ("fldcw %0" : : "m" (fp_mode) );
# endif
  }
}
#endif


/* Returns r = a*b (mod p) in the range 0 <= r < p.
   Assumes 0 <= a,b < p < 2^LDBL_MULMOD_LIMIT.
   Assumes inv = 1.0L/p.
*/
static int64_t mulmod(int64_t a, int64_t b, int64_t p, long double inv)
{
  int64_t q, r;
  long double x, y;

  /* ab (mod p) = ab-pq, where q = floor(ab/p) */

  x = a;
  y = b;
  q = x*y*inv;
  r = a*b - q*p;

  /* Because the default FPU rounding mode is round-to-nearest the result
     could be out by +/-p. One of these branches could be eliminated by
     using a non-default rounding mode. (Round-to-zero is ideal I think).
  */
  if (r < 0)
    r += p;
  else if (r >= p)
    r -= p;

  return r;
}


/* Returns 0 only if N is composite.
   Otherwise N is a strong probable prime to base a.
   Assumes N odd, a < N < 2^LDBL_MULMOD_LIMIT.
 */
static int strong_prp(int64_t a, int64_t N)
{
  long double inv;
  int64_t r;
  uint64_t d;
  unsigned int s, t;

  /* If N is prime and N = d*2^t+1, where d is odd, then either
     1.  a^d = 1 (mod N), or
     2.  a^(d*2^s) = -1 (mod N) for some s in 0 <= s < t */

  inv = 1.0L/N;

#ifdef __GNUC__
  t = __builtin_ctzll(N-1);
  d = (uint64_t)N >> t;
#else
  for (d = (uint64_t)N >> 1, t = 1; !(d & 1); d >>= 1, t++)
    ;
#endif

  /* r <-- a^d mod N, assuming d odd */
  for (r = a, d >>= 1; d > 0; d >>= 1)
  {
    a = mulmod(a,a,N,inv);
    if (d & 1)
      r = mulmod(r,a,N,inv);
  }

  /* Clause 1. and s = 0 case for clause 2. */
  if (r == 1 || r == N-1)
    return 1;

  /* 0 < s < t cases for clause 2. */
  for (s = 1; s < t; s++)
    if ((r = mulmod(r,r,N,inv)) == N-1)
      return 1;

  return 0;
}


/* Returns 0 only if N is composite.
   Otherwise N is a strong probable prime to base 2.
   For AP26, N can be assumed to have no prime divisors <= 541?
 */
/*
int PrimeQ(int64_t N)
{
  if ((uint64_t)N >= (UINT64_C(1) << LDBL_MULMOD_LIMIT))
  {
    fprintf(stderr,__FILE__": modulus out of range.\n");
    exit(EXIT_FAILURE);
  }

  return strong_prp(2,N);
}
*/
