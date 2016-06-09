/* AP26-boinc.c -- Geoffrey Reynolds, 11 September 2008.

   Modified for SSE2 support 27 September 2009 by Bryan Little
   *NOTE* this source code is for 64-bit only

   Bryan Little 2 June 2016 added result hash and 10-shift search.
---------------------------------------------------------------------

   This file is a replacement for AP26-64.c in Jaroslaw Wroblewski's AP26
   sample implementation.

   See http://www.geocities.com/g_w_reynolds/AP26/AP26-boinc-src.zip for the
   latest version of this file.

   See http://www.math.uni.wroc.pl/~jwr/AP26/AP26.zip for the sample
   implementation containing the rest of the source required.

   See http://www.math.uni.wroc.pl/~jwr/AP26/faster32.zip for a patch to
   dramatically improve performance on 32-bit machines.

   See http://www.math.uni.wroc.pl/~jwr/AP26/AP26v3.pdf for information
   about how the algorithm works and for the copyleft notice.


   Compiling the source:
   ---------------------

   Unpack this archive (AP26-boinc-src.zip) along with the sample
   implementation source (AP26.zip) and patches (faster32.zip, if32.zip)
   from the same directory.
 
   To compile a standalone app for manual testing:

    gcc -O3 -o AP26 AP26-boinc.c

   To compile a BOINC app:

    gcc -O3 -DBOINC -o AP26 AP26-boinc.c -lstdc++ -lpthread -lboinc_api -lboinc

   For non-x86/x86_64/ppc64 architectures, add -lgmp to the gcc command line.


   Testing the executable:
   -----------------------

   To briefly test the AP26 executable, check that the file TEST-366384.txt
   matches the results file SOL-AP26.txt produced by executing:

     ./AP26 366384 366384 0


   Program operation:
   ------------------

   search parameters are given on the command line as

     AP26 [KMIN KMAX SHIFT]

   The search will begin at K=KMIN ITER=0 unless a file AP26-state.txt
   exists containing a checkpoint of the form

     KMIN KMAX SHIFT K HASH

   with KMIN KMAX SHIFT matching the initial search parameters, in which
   case the search will resume from that checkpoint.

   The search will continue up to and including K=KMAX. On completion
   AP26-state.txt will contain a checkpoint of the form

     KMIN KMAX SHIFT KMAX+1 HASH

   Periodic checkpoints will be written to AP26-state.txt.
   All search results and a result hash will be appended to SOL-AP26.txt.
*/

// AP26 application version
#define MAJORV 1
#define MINORV 0


#define __STDC_FORMAT_MACROS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>

// SSE2
#include <emmintrin.h>

// 64bit only
#define __x86_64__


#ifdef BOINC
# include "boinc_api.h"
# include "filesys.h"
#endif

#ifndef EXIT_SUCCESS
# define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
# define EXIT_FAILURE 1
#endif


/***************************
 *
 * Start of compile options.
 *
 ***************************/

/* File names.
 */
#ifndef STATE_FILENAME
# define STATE_FILENAME "AP26-state.txt"
#endif
#ifndef RESULTS_FILENAME
# define RESULTS_FILENAME "SOL-AP26.txt"
#endif


/* Set MINIMUM_AP_LENGTH_TO_REPORT.
 */
#ifndef MINIMUM_AP_LENGTH_TO_REPORT
# define MINIMUM_AP_LENGTH_TO_REPORT 20
#endif



/*************************
 *
 * End of compile options.
 *
 *************************/


/* An alternative to GMP is provided for x86, x86_64 and ppx64. This is
   mainly to avoid the challenge of compiling the GMP library for Windows,
   but is also a little faster.
*/
#include "PrimeQ_x86.h"


/* Global variables */
static int KMIN, KMAX, K_DONE, K_COUNT;
int result_hash;


static FILE *results_file = NULL;



static FILE *my_fopen(const char *filename, const char *mode)
{
#ifdef BOINC
  char resolved_name[512];

  boinc_resolve_filename(filename,resolved_name,sizeof(resolved_name));
  return boinc_fopen(resolved_name,mode);
#else
  return fopen(filename,mode);
#endif
}


// a bit less than 32bit signed int max
#define MAXINTV 2000000000

// Bryan Little 6-9-2016
// BOINC result hash calculation, write to solution file, and close.
// The hash function is a 16 char hexadecimal string used to compare results found by different computers in a BOINC quorum.
// The hash can be used to compare results between GPU and CPU clients.
// It also prevents the server from having to validate every AP10+ reported by clients, which can be in different orders depending on GPU.
// It contains information about the assigned workunit and all APs found of length 10 or larger.
static void write_hash()
{

	int tmp;
	int i = 0;

	// init hash to 0000000000000000
	char hexhash[16] = 	{48, 48, 48, 48,
				 48, 48, 48, 48,
				 48, 48, 48, 48,
				 48, 48, 48, 48};

	// calculate the top 32bits of the hash based on assigned workunit range
	uint64_t minmax = KMIN + KMAX;
	// check to make sure we don't overflow a 32bit signed int with large K values
	while(minmax > MAXINTV){
		minmax -= MAXINTV;
	}

	unsigned int solhash = result_hash;

	// top 32 bits are workunit range... bottom 32 bits are solutions found
	int64_t hash = (int64_t)( (minmax << 32) | solhash);

//	printf("minmax: %llu solhash: %u\ndechash: %lld\n", minmax, solhash, hash);

	// convert to hex
	while(hash!=0){
		tmp = hash % 16;

		if(tmp < 10)
			tmp+=48;
		else
			tmp+=55;
		
		hexhash[i] = tmp;
		i++;
		hash = hash/16;
	}


	if (results_file == NULL)
		results_file = my_fopen(RESULTS_FILENAME,"a");

	if (results_file == NULL){
		fprintf(stderr,"Cannot open %s !!!\n",RESULTS_FILENAME);
		exit(EXIT_FAILURE);
	}

	if (fprintf(results_file,"%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
				hexhash[15],hexhash[14],hexhash[13],hexhash[12],
				hexhash[11],hexhash[10],hexhash[9],hexhash[8],
				hexhash[7],hexhash[6],hexhash[5],hexhash[4],
				hexhash[3],hexhash[2],hexhash[1],hexhash[0])<0){
		fprintf(stderr,"Cannot write to %s !!!\n",RESULTS_FILENAME);
		exit(EXIT_FAILURE);
	}

	fclose(results_file);

}


static void write_state(int KMIN, int KMAX, int SHIFT, int K)
{
	FILE *out;

	if ((out = my_fopen(STATE_FILENAME,"w")) == NULL){
		fprintf(stderr,"Cannot open %s !!!\n",STATE_FILENAME);
		exit(EXIT_FAILURE);
	}

	if (fprintf(out,"%d %d %d %d %d\n",KMIN,KMAX,SHIFT,K,result_hash) < 0){
		fprintf(stderr,"Cannot write to %s !!!\n",STATE_FILENAME);
		exit(EXIT_FAILURE);
	}

	fclose(out);
}

/* Return 1 only if a valid checkpoint can be read.
 */
static int read_state(int KMIN, int KMAX, int SHIFT, int *K)
{
	FILE *in;
	int tmp1, tmp2, tmp3, tmp4;

	if ((in = my_fopen(STATE_FILENAME,"r")) == NULL)
		return 0;

	if (fscanf(in,"%d %d %d %d %d\n",&tmp1,&tmp2,&tmp3,K,&tmp4) != 5){
		fprintf(stderr,"Cannot parse %s !!!\n",STATE_FILENAME);
		exit(EXIT_FAILURE);
	}

	fclose(in);

	/* Check that KMIN KMAX SHIFT all match */
	if (tmp1==KMIN && tmp2==KMAX && tmp3==SHIFT){
		result_hash = tmp4;
		return 1;
	}

	return 0;
}




/* Bryan Little - added to CPU code 6-9-2016
   Returns index j where:
   0<=j<k ==> f+j*d*23# is composite.
   j=k    ==> for all 0<=j<k, f+j*d*23# is a strong probable prime to base 2 only.
*/
static int val_base2_ap26(int k, int d, int64_t f)
{
	int64_t N;
	int j;

	if (f%2==0)
		return 0;

	for (j = 0, N = f; j < k; j++){

		if (N < 0 || N >= (INT64_C(1) << LDBL_MULMOD_LIMIT)){
			fprintf(stderr,__FILE__": modulus out of range.\n");
			exit(EXIT_FAILURE);
		}

		if (!strong_prp(2,N))
			return j;

		N += (int64_t)d*2*3*5*7*11*13*17*19*23;
	}

	return j;
}


/*
   Bryan Little - added to CPU code 6-9-2016
   Returns index j where:
   0<=j<k ==> f+j*d*23# is composite.
   j=k    ==> for all 0<=j<k, f+j*d*23# is a strong probable prime to 9 bases.
*/
static int validate_ap26(int k, int d, int64_t f)
{
  int64_t N;
  int j;

  const int base[] = {2,3,5,7,11,13,17,19,23};

  if (f%2==0)
    return 0;


  for (j = 0, N = f; j < k; j++)
  {

    int i;

    if (N < 0 || N >= (INT64_C(1) << LDBL_MULMOD_LIMIT))
    {
      fprintf(stderr,__FILE__": modulus out of range.\n");
      exit(EXIT_FAILURE);
    }

    for (i = 0; i < sizeof(base)/sizeof(int); i++)
      if (!strong_prp(base[i],N))
        return j;

    N += (int64_t)d*2*3*5*7*11*13*17*19*23;
  }

  return j;
}


// Bryan Little - added to CPU code 6-9-2016
// Changed function to check ALL solutions for validity, not just solutions >= MINIMUM_AP_LENGTH_TO_REPORT
// GPU does a prp base 2 check only. It will sometimes report an AP with a base 2 probable prime.
void ReportSolution(int AP_Length,int difference,int64_t First_Term)
{

	int i;

	i = validate_ap26(AP_Length,difference,First_Term);

	if (i < AP_Length){
#ifdef BOINC
		if (boinc_is_standalone())
#endif
		printf("Non-Solution: %d %d %lld\n",AP_Length,difference,First_Term);

		if (val_base2_ap26(AP_Length,difference,First_Term) < AP_Length){
			// CPU really did calculate something wrong.  It's not a prp base 2 AP
			printf("Error: Computation error, CPU computed invalid AP, exiting...\n");
			fprintf(stderr,"Error: Computation error, CPU computed invalid AP\n");
			exit(EXIT_FAILURE);
		} 

		// Even though this AP is not valid, it may contain an AP that is.
		/* Check leading terms */
		ReportSolution(i,difference,First_Term);

		/* Check trailing terms */
		ReportSolution(AP_Length-(i+1),difference,First_Term+(int64_t)(i+1)*difference*2*3*5*7*11*13*17*19*23);
		return;
	}
	else if (AP_Length >= MINIMUM_AP_LENGTH_TO_REPORT){

		if (results_file == NULL)
			results_file = my_fopen(RESULTS_FILENAME,"a");

#ifdef BOINC
		if (boinc_is_standalone())
#endif
		printf("Solution: %d %d %lld\n",AP_Length,difference,First_Term);

		if (results_file == NULL){
			fprintf(stderr,"Cannot open %s !!!\n",RESULTS_FILENAME);
			exit(EXIT_FAILURE);
		}

		if (fprintf(results_file,"%d %d %lld\n",AP_Length,difference,First_Term)<0){
			fprintf(stderr,"Cannot write to %s !!!\n",RESULTS_FILENAME);
			exit(EXIT_FAILURE);
		}
	}
	
}



/* Checkpoint 
   If force is nonzero then don't ask BOINC for permission.
*/
void checkpoint(int SHIFT, int K, int force)
{
	double d;

	if (K_COUNT > 0)
		d = (double)K_DONE/K_COUNT;
	else
		d = 1.0;

#ifdef BOINC
	if (force)
		boinc_begin_critical_section();
	if (force || boinc_time_to_checkpoint()){
#endif

		if (results_file != NULL)
			fflush(results_file);

		write_state(KMIN,KMAX,SHIFT,K);

		/* It is OK to print this progress message now since other buffers are being flushed now anyway */
		printf("Checkpoint: KMIN=%d KMAX=%d SHIFT=%d K=%d (%.2f%%)\n",KMIN,KMAX,SHIFT,K,d*100.0);

#ifdef BOINC
		if (!force)
			boinc_checkpoint_completed();
	}
	if (force)
		boinc_end_critical_section();

	boinc_fraction_done(d);
#endif
}


#include "AP26.h"


/* Returns 1 iff K will be searched.
 */
static int will_search(int K)
{
  	return (K%PRIME1 && K%PRIME2 && K%PRIME3 && K%PRIME4 &&
          	K%PRIME5 && K%PRIME6 && K%PRIME7 && K%PRIME8);
}


int main(int argc, char *argv[])
{
	int i, K, SHIFT;

	printf("AP26 10-shift search version %d.%d by Bryan Little\n",MAJORV,MINORV);
	printf("Compiled " __DATE__ " with GCC " __VERSION__ "\n");


#ifdef BOINC
	boinc_init();
	fprintf(stderr, "AP26 10-shift search version %d.%d by Bryan Little\n",MAJORV,MINORV);
	fprintf(stderr, "Compiled " __DATE__ " with GCC " __VERSION__ "\n");
#endif

	check_fpu_mode();


	/* Get search parameters from command line */
	if (argc == 4){
		sscanf(argv[1],"%d",&KMIN);
		sscanf(argv[2],"%d",&KMAX);
		sscanf(argv[3],"%d",&SHIFT);
	}
	else{
		fprintf(stderr,"Usage: %s [KMIN KMAX SHIFT]\n",argv[0]);
		exit(EXIT_FAILURE);
	}


	/* Resume from checkpoint if there is one */
	if (read_state(KMIN,KMAX,SHIFT,&K)){
		printf("Resuming search from the checkpoint in %s.\n",STATE_FILENAME);
	}
	else{
		printf("Beginning a new search with parameters from the command line\n");
		K = KMIN;
		// zero result hash for BOINC
		result_hash = 0;
	}

	/* Count the number of K in the range KMIN <= K <= KMAX that will actually
		be searched and (if K > KMIN) those that have already been searched. */
	for (i = KMIN; i <= KMAX; i++){
		if (will_search(i)){
			K_COUNT++;
			if (K > i)
				K_DONE++;
		}
	}

	printf("%d K in this range remaining to be searched (%d skipped, %d done)."
	       "\n",K_COUNT-K_DONE,KMAX+1-KMIN-K_COUNT,K_DONE);




	/* Top-level loop */
	for (; K <= KMAX; ++K){
		if (will_search(K)){

			checkpoint(SHIFT,K,0);

			// start 10 shift change
			int currSHIFT;

			for(currSHIFT=SHIFT; currSHIFT<(SHIFT+640); currSHIFT+=64){
				printf("Starting search at shift: %d reporting APs of size %d and larger\n", currSHIFT, MINIMUM_AP_LENGTH_TO_REPORT);
				SearchAP26(K,currSHIFT);
			}
			// end 10 shift change

		 	K_DONE++;
		}
	}

	/* Force Final checkpoint */
	checkpoint(SHIFT,K,1);


	/* Write BOINC hash to file */
	write_hash();



	/* Done */
#ifdef BOINC
	boinc_finish(0);
#endif


  return EXIT_SUCCESS;
}
