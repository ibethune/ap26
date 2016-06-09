/* 
	***************
	Bryan Little 6-9-2016
	BOINC enabled OpenCL Nvidia/AMD GPU AP26 app port
	This is a 10 shift search.  Checks each K with SHIFT and SHIFT+64, SHIFT+128, ..., SHIFT+576
	***************


   AP26-boinc.c -- Geoffrey Reynolds, 11 September 2008.

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


   Program operation:
   ------------------

   search parameters are given on the command line as

     [KMIN KMAX SHIFT --device N] where N is the GPU to use


   The search will begin at K=KMIN unless a file AP26-state.txt
   exists containing a checkpoint of the form

     KMIN KMAX SHIFT K HASH

   with KMIN KMAX SHIFT matching the initial search parameters, in which
   case the search will resume from that checkpoint.

   The search will continue up to and including K=KMAX. On completion
   AP26-state.txt will contain a checkpoint of the form

     KMIN KMAX SHIFT KMAX+1 HASH

   Periodic checkpoints will be written to AP26-state.txt.
   All search results will be appended to SOL-AP26.txt.


*/


// AP26 application version
#define MAJORV 1
#define MINORV 0


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define __STDC_CONSTANT_MACROS
#include <stdint.h>
#include <inttypes.h>

#ifdef BOINC
# include "boinc_api.h"
# include "boinc_opencl.h"
# include "filesys.h"
#endif

#include "CONST.H"
#include <time.h>
#include "simpleCL.h"

// ocl kernels converted with xxd
#include "checkn.h"
#include "clearok.h"
#include "clearsol.h"
#include "offset.h"
#include "setok.h"
#include "setupokok.h"
#include "sieve.h"
#include "sieve_nv.h"




#define numn59s 137375320
#define halfn59s 68687660
// actual numn used is usually < 100,000 we use 1,000,000 here to prevent overflowing the array
#define numn 1000000
#define numOK 23693
#define sol 1024

#define NVIDIA 1
#define GENERIC 0

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



/* Global variables */
static int KMIN, KMAX, K_DONE, K_COUNT;

int result_hash;

sclHard hardware;

sclHard *allhardware;

sclSoft clearok;
sclSoft clearsol;
sclSoft offset;
sclSoft checkn;
sclSoft setok;
sclSoft setupokok;
sclSoft sieve;

int64_t *n59_0_h;
int64_t *n59_1_h;
int *sol_k_h;
int64_t *sol_val_h;
cl_mem ncount_d, solcount_d, n59_0_d, n59_1_d, n_result_d, OKOK_d, OK_d, offset_d, sol_k_d, sol_val_d;


static FILE *results_file = NULL;



/* An alternative to GMP is provided for x86, x86_64 and ppx64. This is
   mainly to avoid the challenge of compiling the GMP library for Windows,
   but is also a little faster.
*/

#include "PrimeQ_x86.h"

// bryan little
// added boinc restart on gpu malloc error
cl_mem sclMalloc( sclHard hardware, cl_int mode, size_t size ){
	cl_mem buffer;

	cl_int err;
	
	buffer = clCreateBuffer( hardware.context, mode, size, NULL, &err );
	if ( err != CL_SUCCESS ) {
		printf( "\nclMalloc Error\n" );
		sclPrintErrorFlags( err );

#ifdef BOINC
		fprintf(stderr,"OpenCL memory allocation error, restarting in 1 minute.\n");
		boinc_temporary_exit(60);
#endif
	}

		
	return buffer;
}	


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
#define MAXINTV 2000000000;

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


/* Bryan Little 9-28-2015
/* Returns index j where:
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

		if (!strong_prp_cpu(2,N))
			return j;

		N += (int64_t)d*2*3*5*7*11*13*17*19*23;
	}

	return j;
}


/* Returns index j where:
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
      if (!strong_prp_cpu(base[i],N))
        return j;

    N += (int64_t)d*2*3*5*7*11*13*17*19*23;
  }

  return j;
}


// Bryan Little 9-28-2015
// Changed function to check ALL solutions reported by GPU for validity using CPU 
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
			// GPU really did calculate something wrong.  It's not a prp base 2 AP
			printf("Error: Computation error, GPU computed invalid AP, exiting...\n");
			fprintf(stderr,"Error: Computation error, GPU computed invalid AP\n");
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


/* Returns 1 iff K will be searched.
 */
static int will_search(int K)
{

  return (K%PRIME1 && K%PRIME2 && K%PRIME3 && K%PRIME4 &&
          K%PRIME5 && K%PRIME6 && K%PRIME7 && K%PRIME8);

}



/*
	***************
	Bryan Little May 2014
	+ Using ~1.1 gigabytes RAM on device and host
	+ This is code tuned for OpenCL devices.
	+ Two 524mb n59 arrays because of OpenCL device memory allocation limits on AMD gpu

*/
void SearchAP26(int K, int SHIFT, int sieve_ls)
{ 

	int64_t STEP;
	int64_t n0;
	int64_t n43, n47, n53;
	int i43, i47, i53;
	int i3, i5, i31, i37, i41;
	int64_t S31, S37, S41, S43, S47, S53, S59;
        size_t global_size[2];
        size_t local_size[2];

	time_t total_start_time, total_finish_time;
	time (&total_start_time);


	STEP=K*PRIM23;
	n0=(N0*(K%17835)+((N0*17835)%MOD)*(K/17835)+N30)%MOD;
	S31=(PRES2*(K%17835)+((PRES2*17835)%MOD)*(K/17835))%MOD;
	S37=(PRES3*(K%17835)+((PRES3*17835)%MOD)*(K/17835))%MOD;
	S41=(PRES4*(K%17835)+((PRES4*17835)%MOD)*(K/17835))%MOD;
	S43=(PRES5*(K%17835)+((PRES5*17835)%MOD)*(K/17835))%MOD;
	S47=(PRES6*(K%17835)+((PRES6*17835)%MOD)*(K/17835))%MOD;
	S53=(PRES7*(K%17835)+((PRES7*17835)%MOD)*(K/17835))%MOD;
	S59=(PRES8*(K%17835)+((PRES8*17835)%MOD)*(K/17835))%MOD;

	// note: n59MAX == 4808136200
	// note: n53MAX == 137375320

	int count=0;
	int count2=0;

	for(i31=0;i31<7;++i31)
	for(i37=0;i37<13;++i37)
	if(i37-i31<=10&&i31-i37<=4)
	for(i41=0;i41<17;++i41)
	if(i41-i31<=14&&i41-i37<=14&&i31-i41<=4&&i37-i41<=10)
	for(i3=0;i3<2;++i3)
	for(i5=0;i5<4;++i5){ 
		n43=(n0+i3*S3+i5*S5+i31*S31+i37*S37+i41*S41)%MOD;
		for(i43=(PRIME5-24);i43>0;i43--){
			n47=n43;
			for(i47=(PRIME6-24);i47>0;i47--){
				n53=n47;
				for(i53=(PRIME7-24);i53>0;i53--){
					//n59=n53;
					if(count < halfn59s){
						n59_0_h[count]=n53;
						count++;
					}
					else{
						n59_1_h[count2]=n53;
						count2++;
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

#ifdef BOINC
	boinc_begin_critical_section();
#endif
	// offload to gpu
	sclWrite(hardware, halfn59s * sizeof(int64_t), n59_0_d, n59_0_h);
	sclWrite(hardware, halfn59s * sizeof(int64_t), n59_1_d, n59_1_h);

	// offset kernel
        global_size[0]=576; global_size[1]=1;
        local_size[0]=64; local_size[1]=1;
	sclSetKernelArg(offset, 0, sizeof(cl_mem), &offset_d);
	sclEnqueueKernel(hardware, offset, global_size, local_size);
	// end offset

	sclFinish(hardware);

#ifdef BOINC
	boinc_end_critical_section();
#endif

	int STARTSHIFT;

	for(STARTSHIFT=SHIFT; SHIFT<(STARTSHIFT+640); SHIFT+=64){

		time_t start_time, finish_time;
		time (&start_time);

#ifdef BOINC
		boinc_begin_critical_section();
#endif

		// clearok kernel
		global_size[0]=23744; global_size[1]=1;
		local_size[0]=64; local_size[1]=1;
		sclSetKernelArg(clearok, 0, sizeof(cl_mem), &OK_d);
		sclSetKernelArg(clearok, 1, sizeof(cl_mem), &OKOK_d);
		sclEnqueueKernel(hardware, clearok, global_size, local_size);
		// end clearok
 

		// setok kernel
		global_size[0]=576; global_size[1]=1;
		local_size[0]=64; local_size[1]=1;
		sclSetKernelArg(setok, 0, sizeof(int64_t), &STEP);
		sclSetKernelArg(setok, 1, sizeof(int), &SHIFT);
		sclSetKernelArg(setok, 2, sizeof(cl_mem), &OK_d);
		sclSetKernelArg(setok, 3, sizeof(cl_mem), &offset_d);
		sclEnqueueKernel(hardware, setok, global_size, local_size);
		// end setok


		// setupokok kernel
		global_size[0]=576; global_size[1]=1;
		local_size[0]=64; local_size[1]=1;
		sclSetKernelArg(setupokok, 0, sizeof(int64_t), &STEP);
		sclSetKernelArg(setupokok, 1, sizeof(int), &SHIFT);
		sclSetKernelArg(setupokok, 2, sizeof(cl_mem), &OK_d);
		sclSetKernelArg(setupokok, 3, sizeof(cl_mem), &OKOK_d);
		sclSetKernelArg(setupokok, 4, sizeof(cl_mem), &offset_d);
		sclEnqueueKernel(hardware, setupokok, global_size, local_size);
		// end setupokok


		// clearsol kernel
		global_size[0]=1024; global_size[1]=1;
		local_size[0]=64; local_size[1]=1;
		sclSetKernelArg(clearsol, 0, sizeof(cl_mem), &sol_k_d);
		sclSetKernelArg(clearsol, 1, sizeof(cl_mem), &solcount_d);
		sclEnqueueKernel(hardware, clearsol, global_size, local_size);
		// end clearsol

		sclFinish(hardware);

#ifdef BOINC
		boinc_end_critical_section();
#endif


		// kernel config for sieve
		const int worksize=1571840;	// must be divisible by 64 and 1024

		int p;
		// n59 array 0 checking
		for( p=0; p<halfn59s; p+=worksize ){

#ifdef BOINC
			boinc_begin_critical_section();
#endif

			// sieve kernel
			sclSetKernelArg(sieve, 0, sizeof(cl_mem), &n59_0_d);
			sclSetKernelArg(sieve, 1, sizeof(int64_t), &S59);
			sclSetKernelArg(sieve, 2, sizeof(int), &SHIFT);
			sclSetKernelArg(sieve, 3, sizeof(cl_mem), &n_result_d);
			sclSetKernelArg(sieve, 4, sizeof(cl_mem), &OKOK_d);
			sclSetKernelArg(sieve, 5, sizeof(cl_mem), &ncount_d);
			sclSetKernelArg(sieve, 6, sizeof(int), &p);
			global_size[0]=worksize; global_size[1]=1;
			local_size[0]=sieve_ls; local_size[1]=1;
			sclEnqueueKernel(hardware, sieve, global_size, local_size);
			// end sieve

/*			int* numbern;
			numbern = (int*)malloc(sizeof(int));
			sclRead(hardware, sizeof(int), ncount_d, numbern);
			printf("narray size: %d of max %d\n",numbern[0],numn);
			free(numbern);
*/
			// checkn kernel
			sclSetKernelArg(checkn, 0, sizeof(cl_mem), &n_result_d);
			sclSetKernelArg(checkn, 1, sizeof(int64_t), &STEP);
			sclSetKernelArg(checkn, 2, sizeof(cl_mem), &sol_k_d);
			sclSetKernelArg(checkn, 3, sizeof(cl_mem), &sol_val_d);
			sclSetKernelArg(checkn, 4, sizeof(cl_mem), &ncount_d);
			sclSetKernelArg(checkn, 5, sizeof(cl_mem), &solcount_d);
			global_size[0]=numn; global_size[1]=1;
			local_size[0]=64; local_size[1]=1;
			sclEnqueueKernel(hardware, checkn, global_size, local_size);
			// end checkn

			// call finish here to fix ctrl-c crash on AMD GPU
			sclFinish(hardware);

#ifdef BOINC
			boinc_end_critical_section();
#endif

		}

		// n59 array 1 checking
		for( p=0; p<halfn59s; p+=worksize ){

#ifdef BOINC
			boinc_begin_critical_section();
#endif

			// sieve kernel
			sclSetKernelArg(sieve, 0, sizeof(cl_mem), &n59_1_d);
			sclSetKernelArg(sieve, 1, sizeof(int64_t), &S59);
			sclSetKernelArg(sieve, 2, sizeof(int), &SHIFT);
			sclSetKernelArg(sieve, 3, sizeof(cl_mem), &n_result_d);
			sclSetKernelArg(sieve, 4, sizeof(cl_mem), &OKOK_d);
			sclSetKernelArg(sieve, 5, sizeof(cl_mem), &ncount_d);
			sclSetKernelArg(sieve, 6, sizeof(int), &p);
			global_size[0]=worksize; global_size[1]=1;
			local_size[0]=sieve_ls; local_size[1]=1;
			sclEnqueueKernel(hardware, sieve, global_size, local_size);
			// end sieve


			// checkn kernel
			sclSetKernelArg(checkn, 0, sizeof(cl_mem), &n_result_d);
			sclSetKernelArg(checkn, 1, sizeof(int64_t), &STEP);
			sclSetKernelArg(checkn, 2, sizeof(cl_mem), &sol_k_d);
			sclSetKernelArg(checkn, 3, sizeof(cl_mem), &sol_val_d);
			sclSetKernelArg(checkn, 4, sizeof(cl_mem), &ncount_d);
			sclSetKernelArg(checkn, 5, sizeof(cl_mem), &solcount_d);
			global_size[0]=numn; global_size[1]=1;
			local_size[0]=64; local_size[1]=1;
			sclEnqueueKernel(hardware, checkn, global_size, local_size);
			// end checkn

			// call finish here to fix ctrl-c crash on AMD GPU
			sclFinish(hardware);

#ifdef BOINC
			boinc_end_critical_section();
#endif

		}


#ifdef BOINC
		boinc_begin_critical_section();
#endif

		// copy n results and n count to local memory
		sclRead(hardware, sol * sizeof(int), sol_k_d, sol_k_h);
		sclRead(hardware, sol * sizeof(int64_t), sol_val_d, sol_val_h);

		sclFinish(hardware);

		int e=0;
		while(sol_k_h[e] != 0){
			ReportSolution(sol_k_h[e],K,sol_val_h[e]);

			/* 	hash for BOINC quorum
				we create a hash based on each AP10+ result mod 1000
				and that result's AP length  	*/
			result_hash += sol_val_h[e] % 1000;
			result_hash += sol_k_h[e];
			if(result_hash > MAXINTV){
				result_hash -= MAXINTV;
			}
	
			e++;
		}

#ifdef BOINC
		boinc_end_critical_section();
#endif

		printf("Computation for K: %d SHIFT: %d complete.\n", K, SHIFT);

		time(&finish_time);
		printf("GPU time was %d seconds\n", (int)finish_time - (int)start_time);
	}

	time(&total_finish_time);
	printf("total GPU time for K was %d seconds\n", (int)total_finish_time - (int)total_start_time);

}




int main(int argc, char *argv[])
{
	int i, K, SHIFT;
	int found=0;
	int GPUNUM, GPU, localsize;
	cl_int err;

	printf("AP26 OpenCL 10-shift search version %d.%d by Bryan Little\n",MAJORV,MINORV);
	printf("Compiled " __DATE__ " with GCC " __VERSION__ "\n");


#ifdef BOINC
	boinc_init();
	fprintf(stderr, "AP26 OpenCL 10-shift search version %d.%d by Bryan Little\n",MAJORV,MINORV);
	fprintf(stderr, "Compiled " __DATE__ " with GCC " __VERSION__ "\n");
#endif


	/* Check FPU mode and change it to extended precision if necessary. */
	check_fpu_mode();

	/* Get search parameters from command line */
	// stand alone or old boinc client
	if (argc == 6){
		sscanf(argv[1],"%d",&KMIN);
		sscanf(argv[2],"%d",&KMAX);
		sscanf(argv[3],"%d",&SHIFT);
		// argv[4] is --device
		sscanf(argv[5],"%d",&GPUNUM);
	}
#ifdef BOINC
	// boinc client >= 7.0.12 
	else if(argc == 4 && !boinc_is_standalone() ){
		sscanf(argv[1],"%d",&KMIN);
		sscanf(argv[2],"%d",&KMAX);
		sscanf(argv[3],"%d",&SHIFT);
	}
#endif
	else{
		printf("Usage: %s [KMIN KMAX SHIFT --device N] where N is the GPU to use.\n",argv[0]);
		exit(EXIT_FAILURE);
	}

  	printf("Search parameters are KMIN: %d KMAX: %d SHIFT: %d\n", KMIN, KMAX, SHIFT);

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


	// opencl init

#ifdef BOINC
	if (boinc_is_standalone()){
		allhardware = sclGetAllHardware(&found);

		hardware = allhardware[GPUNUM];
		printf("\n using device %d\n",GPUNUM);
	}
	else{
		int retval = 0;
		cl_platform_id platform = NULL;
		cl_device_id device;

		retval = boinc_get_opencl_ids(argc, argv, 0, &device, &platform);
		if (retval) {
			fprintf(stderr, "Error: boinc_get_opencl_ids() failed with error %d\n",retval);
			exit(EXIT_FAILURE);
		}

		hardware.platform = platform;
		hardware.device = device;
	}
#else
        allhardware = sclGetAllHardware(&found);

	hardware = allhardware[GPUNUM];
	printf("\n using device %d\n",GPUNUM);
#endif


	// Check GPU vendor
	char vend[1024];
	char vendnv[1024] = "NVIDIA Corporation";
	err = clGetDeviceInfo(hardware.device, CL_DEVICE_VENDOR, sizeof(char)*1024, vend, NULL);
   	if ( err != CL_SUCCESS ) {
		printf( "Error checking vendor of device. " );
		exit(EXIT_FAILURE);
	}

	if(strcmp(vend,vendnv)==0){
		GPU = NVIDIA;
	}
	else{
		GPU = GENERIC;
	}


	printf("compiling clearok\n");
        clearok = sclGetCLSoftware(clearok_cl,"clearok",hardware, 1);

	printf("compiling clearsol\n");
        clearsol = sclGetCLSoftware(clearsol_cl,"clearsol",hardware, 1);

	printf("compiling offset\n");
        offset = sclGetCLSoftware(offset_cl,"offset",hardware, 1);

	printf("compiling setok\n");
        setok = sclGetCLSoftware(setok_cl,"setok",hardware, 1);


	if(GPU == NVIDIA){
		printf("compiling sieve for NVIDIA GPU\n");
		sieve = sclGetCLSoftware(sieve_nv_cl,"sieve",hardware, 1);

		printf("compiling setupokok\n");
		setupokok = sclGetCLSoftware(setupokok_cl,"setupokok",hardware, 1);

		printf("compiling checkn\n");
		checkn = sclGetCLSoftware(checkn_cl,"checkn",hardware, 1);
	}
	else{
		printf("compiling sieve\n");
		sieve = sclGetCLSoftware(sieve_cl,"sieve",hardware, 1);

		printf("compiling setupokok\n");
		setupokok = sclGetCLSoftware(setupokok_cl,"setupokok",hardware, 0);	// AMD's compiler is slow with optimize on

		printf("compiling checkn\n");
		checkn = sclGetCLSoftware(checkn_cl,"checkn",hardware, 0);		// AMD's compiler breaks the kernel with optimize on
	}

	printf("compiling done\n");
	// end init

	// query max local size for kernel on device
	// local memory nvidia kernel only works on devices that can have 1024 thread local size
	size_t local;
	err = clGetKernelWorkGroupInfo(sieve.kernel, hardware.device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
   	if ( err != CL_SUCCESS ) {
		printf( "Error checking local size of kernel on device. " );
		exit(EXIT_FAILURE);
	}
	unsigned int maxlocal = (unsigned int)local;

	if(GPU==NVIDIA){
		// today's gpu's use 1024 max, future may use more?
		if(maxlocal >= 1024){
			localsize = 1024;
			printf("local workgroup size for sieve kernel is 1024 threads\n");
		}
		else{
			printf( "selected NVIDIA GPU does not support 1024 thread local size.  Reverting to generic kernel." );
			sieve = sclGetCLSoftware(sieve_cl,"sieve",hardware, 1);
			localsize = 64;
			printf("local workgroup size for sieve kernel is 64 threads\n");
		}
	}
	else{
		localsize = 64;
		printf("local workgroup size for sieve kernel is 64 threads\n");
	}


#ifdef BOINC
	int64_t gmem = (int64_t)_sclGetMaxGlobalMemSize(hardware.device);
	int64_t maxalloc = (int64_t)_sclGetMaxMemAllocSize(hardware.device);
	fprintf(stderr, "GPU global memory available: %lld\n", gmem);
	fprintf(stderr, "GPU max memory allocation:   %lld\n", maxalloc);
#endif


	// memory allocation
	// host memory
	n59_0_h = (int64_t*)malloc(halfn59s * sizeof(int64_t));
	n59_1_h = (int64_t*)malloc(halfn59s * sizeof(int64_t));
	sol_k_h = (int*)malloc(sol * sizeof(int));
	sol_val_h = (int64_t*)malloc(sol * sizeof(int64_t));
	// device memory
	n59_0_d = sclMalloc(hardware, CL_MEM_READ_WRITE, halfn59s * sizeof(int64_t));
	n59_1_d = sclMalloc(hardware, CL_MEM_READ_WRITE, halfn59s * sizeof(int64_t));
	ncount_d = sclMalloc(hardware, CL_MEM_READ_WRITE, sizeof(int));
	solcount_d = sclMalloc(hardware, CL_MEM_READ_WRITE, sizeof(int));
	n_result_d = sclMalloc(hardware, CL_MEM_READ_WRITE, numn * sizeof(int64_t));
	OKOK_d = sclMalloc(hardware, CL_MEM_READ_WRITE, numOK * sizeof(int64_t));
	OK_d = sclMalloc(hardware, CL_MEM_READ_WRITE, numOK * sizeof(char));
	offset_d = sclMalloc(hardware, CL_MEM_READ_WRITE, 542 * sizeof(int));
	sol_k_d = sclMalloc(hardware, CL_MEM_READ_WRITE, sol * sizeof(int));
	sol_val_d = sclMalloc(hardware, CL_MEM_READ_WRITE, sol * sizeof(int64_t));





	/* Count the number of K in the range KMIN <= K <= KMAX that will actually
		be searched and (if K > KMIN) those that have already been searched. */
	for (i = KMIN; i <= KMAX; i++)
		if (will_search(i)){
			K_COUNT++;
			if (K > i)
				K_DONE++;
		}

	printf("%d K in this range remaining to be searched (%d skipped, %d done).\n",K_COUNT-K_DONE,KMAX+1-KMIN-K_COUNT,K_DONE);


	/* Top-level loop */
	for (; K <= KMAX; ++K){
		if (will_search(K)){
			checkpoint(SHIFT,K,0);
			printf("Starting search... reporting APs of size %d and larger\n\n", MINIMUM_AP_LENGTH_TO_REPORT);
			SearchAP26(K,SHIFT,localsize);

        	 	K_DONE++;
		}
	}

	/* Force Final checkpoint */
	checkpoint(SHIFT,K,1);

	/* Write BOINC hash to result file and close it*/
	write_hash();



	/* Done */

	// free memory
	// host
	free(n59_0_h);
	free(n59_1_h);
	free(sol_k_h);
	free(sol_val_h);
	// device
	sclReleaseMemObject(ncount_d);
	sclReleaseMemObject(solcount_d);
	sclReleaseMemObject(n59_0_d);
	sclReleaseMemObject(n59_1_d);
	sclReleaseMemObject(n_result_d);
	sclReleaseMemObject(OK_d);
	sclReleaseMemObject(OKOK_d);
	sclReleaseMemObject(offset_d);
	sclReleaseMemObject(sol_k_d);
	sclReleaseMemObject(sol_val_d);

	//free scl
	sclReleaseClSoft(clearok);
	sclReleaseClSoft(clearsol);
	sclReleaseClSoft(offset);
	sclReleaseClSoft(checkn);
	sclReleaseClSoft(setok);
	sclReleaseClSoft(setupokok);
	sclReleaseClSoft(sieve);

	sclReleaseClHard(hardware);
	sclReleaseAllHardware(allhardware, found);



#ifdef BOINC
	boinc_finish(0);
#endif

	return EXIT_SUCCESS;
}
