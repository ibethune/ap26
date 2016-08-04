// Common code to both CPU and OpenCL versions 

// AP26 application version
#define MAJORV 1
#define MINORV 2
#define SUFFIXV "-dev"

#ifdef AP26_OPENCL
# define TARGET "OpenCL"
#endif
#ifdef AP26_CPU
# define TARGET "CPU"
#endif

// Need both of these???
#define __STDC_FORMAT_MACROS
#define __STDC_CONSTANT_MACROS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>

#include "CONST.H"
#include "PrimeQ_x86.h"

#ifdef AP26_BOINC
# include "boinc_api.h"
# include "version.h"
# include "filesys.h"
# ifdef AP26_OPENCL
#  include "boinc_opencl.h"
# endif
#endif

#ifdef AP26_CPU
  // externs
# include "cpuavx2.h"
# include "cpuavx.h"
# include "cpusse41.h"
# include "cpusse2.h"
#endif

#ifdef AP26_OPENCL
# include "simpleCL.h"
// ocl kernels converted with xxd
# include "checkn.h"
# include "clearok.h"
# include "clearsol.h"
# include "offset.h"
# include "setok.h"
# include "setupokok.h"
# include "sieve.h"
# include "sieve_nv.h"

# define numn59s 	137375320
# define quartern59s 	34343830

// actual numn used is usually < 100,000 we use 1,000,000 here to prevent overflowing the array
# define numn 1000000
# define numOK 23693
# define sol 1024

# define NVIDIA 1
# define GENERIC 0
#endif

#ifndef EXIT_SUCCESS
# define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
# define EXIT_FAILURE 1
#endif

// a bit less than 32bit signed int max
#define MAXINTV 2000000000

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
time_t last_trickle;

#ifdef AP26_OPENCL

sclHard hardware;

sclSoft clearok;
sclSoft clearsol;
sclSoft offset;
sclSoft checkn;
sclSoft setok;
sclSoft setupokok;
sclSoft sieve;

int64_t *n59_h;
int *sol_k_h;
int64_t *sol_val_h;
cl_mem ncount_d, solcount_d, n_result_d, OKOK_d, OK_d, offset_d, sol_k_d, sol_val_d;
cl_mem n59_0_d, n59_1_d, n59_2_d, n59_3_d;

int sieve_ls;

#endif

static FILE *results_file = NULL;



#ifdef AP26_OPENCL
sclHard sclGetBOINCHardware( int argc, char** argv ) {

	sclHard hardware;
	cl_platform_id platform = NULL;
	cl_device_id device;
	cl_context context;
	cl_command_queue queue;
	int retval = 0;
	cl_int status = 0;
	cl_char deviceName[1024];
	cl_char platformVendor[1024];
	cl_char platformName[1024];
	int i;

#if BOINC_MAJOR_VERSION < 7 || (BOINC_MAJOR_VERSION == 7 && BOINC_MINOR_VERSION == 0)
	retval = boinc_get_opencl_ids(&device, &platform);
#else
	retval = boinc_get_opencl_ids(argc, argv, 0, &device, &platform);
#endif
	if (retval) {
		fprintf(stderr, "Error: boinc_get_opencl_ids() failed with error %d\n", retval );
		exit(EXIT_FAILURE);
	}

	cl_context_properties cps[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform, 0 };

	context = clCreateContext(cps, 1, &device, NULL, NULL, &status);
	if (status != CL_SUCCESS) {
        	fprintf(stderr, "Error: clCreateContext() returned %d\n", status);
        	exit(EXIT_FAILURE); 
   	}

	queue = clCreateCommandQueue(context, device, 0, &status);
	if(status != CL_SUCCESS) { 
		fprintf(stderr, "Error: Creating Command Queue. (clCreateCommandQueue) returned %d\n", status );
        	exit(EXIT_FAILURE);
    	}
			
	hardware.platform = platform;
	hardware.device = device;
	hardware.queue = queue;
	hardware.context = context;

	clGetPlatformInfo( hardware.platform, CL_PLATFORM_NAME, sizeof(cl_char)*1024, platformName, NULL );	
	clGetPlatformInfo( hardware.platform, CL_PLATFORM_VENDOR, sizeof(cl_char)*1024, platformVendor, NULL );
	clGetDeviceInfo( hardware.device, CL_DEVICE_NAME, sizeof(cl_char)*1024, deviceName, NULL );
	fprintf(stderr, "GPU Info:\n  Platform name: %s\n  Vendor: %s\n  Device name: %s\n", platformName, platformVendor, deviceName );
        int64_t gmem = (int64_t)_sclGetMaxGlobalMemSize(hardware.device);
        int64_t maxalloc = (int64_t)_sclGetMaxMemAllocSize(hardware.device);
        fprintf(stderr, "  GPU RAM:        %lld\n", gmem);
        fprintf(stderr, "  GPU max malloc: %lld\n", maxalloc);	

	clRetainCommandQueue( hardware.queue );
	clRetainContext( hardware.context );
	
	return hardware;

}



// bryan little
// added boinc restart on gpu malloc error
cl_mem sclMalloc( sclHard hardware, cl_int mode, size_t size ){
        cl_mem buffer;

        cl_int err;

        buffer = clCreateBuffer( hardware.context, mode, size, NULL, &err );
        if ( err != CL_SUCCESS ) {
                printf( "\nclMalloc Error\n" );

#ifdef AP26_BOINC
                fprintf(stderr,"OpenCL memory allocation error, restarting in 1 minute.\n");
                boinc_temporary_exit(60);
#else
                sclPrintErrorFlags( err );
#endif
        }

        return buffer;
}
#endif

static FILE *my_fopen(const char *filename, const char *mode)
{
#ifdef AP26_BOINC
  char resolved_name[512];

  boinc_resolve_filename(filename,resolved_name,sizeof(resolved_name));
  return boinc_fopen(resolved_name,mode);
#else
  return fopen(filename,mode);
#endif
}


void Progress(double prog){

#ifdef AP26_BOINC
	boinc_fraction_done(prog);
#else
	printf("progress: %.2f%%\n",prog*100.0);
#endif

}


// Functions to control trickles to the BOINC server
// Adapted from genefer (http://www.assembla.com/spaces/genefer)

#define TRICKLE_PERIOD 86400.0 // Once per day

#ifdef AP26_BOINC
void handle_trickle_up()
{
    if (boinc_is_standalone()) return; // Only send trickles if we have a real BOINC server to talk to

    time_t now = time(NULL);

    if (difftime(now, last_trickle) > TRICKLE_PERIOD)
    {
        last_trickle = now;

        double progress = boinc_get_fraction_done();
        double cpu;
        boinc_wu_cpu_time(cpu);
        APP_INIT_DATA init_data;
        boinc_get_init_data(init_data);
        double run = boinc_elapsed_time() + init_data.starting_elapsed_time;

        char msg[512];
        sprintf(msg, "<trickle_up>\n"
                    "   <progress>%f</progress>\n"
                    "   <cputime>%f</cputime>\n"
                    "   <runtime>%f</runtime>\n"
                    "</trickle_up>\n",
                     progress, cpu, run  );
        char variety[64];
        sprintf(variety, "ap26_progress");
        boinc_send_trickle_up(variety, msg);
    }

}
#endif

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


static void write_state(int KMIN, int KMAX, int SHIFT, int K, int ITER)
{
	FILE *out;

	if ((out = my_fopen(STATE_FILENAME,"w")) == NULL){
		fprintf(stderr,"Cannot open %s !!!\n",STATE_FILENAME);
		exit(EXIT_FAILURE);
	}

	if (fprintf(out,"%d %d %d %d %d %d %lf\n",KMIN,KMAX,SHIFT,K,ITER,result_hash, (double)last_trickle) < 0){
		fprintf(stderr,"Cannot write to %s !!!\n",STATE_FILENAME);
		exit(EXIT_FAILURE);
	}

	fclose(out);
}

/* Return 1 only if a valid checkpoint can be read.
 */
static int read_state(int KMIN, int KMAX, int SHIFT, int *K, int *ITER)
{
	FILE *in;
	int tmp1, tmp2, tmp3, tmp4;
        double tmp5;

	if ((in = my_fopen(STATE_FILENAME,"r")) == NULL)
		return 0;

	if (fscanf(in,"%d %d %d %d %d %d %lf\n",&tmp1,&tmp2,&tmp3,K,ITER,&tmp4,&tmp5) != 7){
		fprintf(stderr,"Cannot parse %s !!!\n",STATE_FILENAME);
		exit(EXIT_FAILURE);
	}

	fclose(in);

	/* Check that KMIN KMAX SHIFT all match */
	if (tmp1==KMIN && tmp2==KMAX && tmp3==SHIFT){
		result_hash = tmp4;
                last_trickle = (time_t)tmp5;
		return 1;
	}

	return 0;
}

/* Bryan Little 9-28-2015
   Bryan Little - added to CPU code 6-9-2016
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

/*   Bryan Little - added to CPU code 6-9-2016
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

// Bryan Little 9-28-2015
// Bryan Little - added to CPU code 6-9-2016
// Changed function to check ALL solutions for validity, not just solutions >= MINIMUM_AP_LENGTH_TO_REPORT
// GPU does a prp base 2 check only. It will sometimes report an AP with a base 2 probable prime.
void ReportSolution(int AP_Length,int difference,int64_t First_Term)
{

	int i;

	/* 	hash for BOINC quorum
		we create a hash based on each AP10+ result mod 1000
		and that result's AP length  	*/
	result_hash += First_Term % 1000;
	result_hash += AP_Length;
	if(result_hash > MAXINTV){
		result_hash -= MAXINTV;
	}

	i = validate_ap26(AP_Length,difference,First_Term);

	if (i < AP_Length){

		printf("Non-Solution: %d %d %lld\n",AP_Length,difference,First_Term);

		if (val_base2_ap26(AP_Length,difference,First_Term) < AP_Length){
			// GPU really did calculate something wrong.  It's not a prp base 2 AP
			printf("Error: Computation error, found invalid AP, exiting...\n");
			fprintf(stderr,"Error: Computation error, found invalid AP\n");
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
void checkpoint(int SHIFT, int K, int force, int ITER)
{
	double d;

#ifdef AP26_BOINC
	if (force || boinc_time_to_checkpoint()){
#endif

		if (results_file != NULL)
			fflush(results_file);

		write_state(KMIN,KMAX,SHIFT,K,ITER);

		printf("Checkpoint: KMIN:%d KMAX:%d SHIFT:%d K:%d ITER:%d\n",KMIN,KMAX,SHIFT,K,ITER);

#ifdef AP26_BOINC
		handle_trickle_up();

		if (!force)
			boinc_checkpoint_completed();
	}

	if(force){
		if (K_COUNT > 0)
			d = (double)(K_DONE*10+ITER)/(K_COUNT*10);
		else
			d = 1.0;

		boinc_fraction_done(d);
	}
#endif

}

/* Returns 1 iff K will be searched.
 */
static int will_search(int K)
{
  	return (K%PRIME1 && K%PRIME2 && K%PRIME3 && K%PRIME4 &&
          	K%PRIME5 && K%PRIME6 && K%PRIME7 && K%PRIME8);
}

// Definition of SearchAP26()
#ifdef AP26_OPENCL
#include "AP26.h"
#endif

int main(int argc, char *argv[])
{
	int i, K, SHIFT, ITER, GPU;

        // Initialize BOINC
#ifdef AP26_BOINC
        BOINC_OPTIONS options;
        boinc_options_defaults(options);
# ifdef AP26_OPENCL
        options.normal_thread_priority = true;    // Raise thread prio to keep GPU fed
# endif
# if BOINC_MAJOR_VERSION < 7 || (BOINC_MAJOR_VERSION == 7 && BOINC_MINOR_VERSION < 5)
        options.handle_trickle_ups = true;        // We may periodically report status
# endif
        boinc_init_options(&options);
#endif


#ifdef AP26_BOINC
#ifdef __INTEL_COMPILER
	fprintf(stderr, "AP26 %s 10-shift search version %d.%d%s by Bryan Little and Iain Bethune\n", TARGET, MAJORV, MINORV, SUFFIXV);
	fprintf(stderr, "Compiled %s with ICC %d\n", __DATE__, __INTEL_COMPILER);
#else
	fprintf(stderr, "AP26 %s 10-shift search version %d.%d%s by Bryan Little and Iain Bethune\n",TARGET,MAJORV,MINORV,SUFFIXV);
	fprintf(stderr, "Compiled " __DATE__ " with GCC " __VERSION__ "\n");
#endif
#else
#ifdef __INTEL_COMPILER
		printf("AP26 %s 10-shift search version %d.%d%s by Bryan Little and Iain Bethune\n", TARGET, MAJORV, MINORV, SUFFIXV);
		printf("Compiled %s with ICC %d\n", __DATE__, __INTEL_COMPILER);
#else
        printf("AP26 %s 10-shift search version %d.%d%s by Bryan Little and Iain Bethune\n",TARGET,MAJORV,MINORV,SUFFIXV);
        printf("Compiled " __DATE__ " with GCC " __VERSION__ "\n");
#endif
#endif

       // Print out cmd line for diagnostics
        fprintf(stderr, "Command line: ");
        for (i = 0; i < argc; i++)
        	fprintf(stderr, "%s ", argv[i]);
        fprintf(stderr, "\n");

#ifdef AP26_CPU
	int sse41 = __builtin_cpu_supports("sse4.1");
	int avx = __builtin_cpu_supports("avx");
	int avx2 = __builtin_cpu_supports("avx2");

	if(avx2){
		fprintf(stderr, "Detected AVX2 CPU\n");
	}
	else if(avx){
		fprintf(stderr, "Detected AVX CPU\n");
	}
	else if(sse41){
		fprintf(stderr, "Detected SSE4.1 CPU\n");		
	}
	else{
		fprintf(stderr, "Detected SSE2 CPU\n");		
	}

#endif

        /* Check FPU mode and change it to extended precision if necessary. */
	check_fpu_mode();

	/* Get search parameters from command line */
	// ignore argc 5 and 6 (depreciated --device #)
	if (argc == 4 || argc == 6){
		sscanf(argv[1],"%d",&KMIN);
		sscanf(argv[2],"%d",&KMAX);
		sscanf(argv[3],"%d",&SHIFT);
	}
	else{
		fprintf(stderr,"Usage: %s [KMIN KMAX SHIFT]\n",argv[0]);
		exit(EXIT_FAILURE);
	}


	/* Resume from checkpoint if there is one */
	if (read_state(KMIN,KMAX,SHIFT,&K,&ITER)){
		printf("Resuming search from checkpoint.\n");
		fprintf(stderr,"Resuming from checkpoint. K: %d ITER: %d\n",K,ITER);
#ifdef AP26_CPU
		if(avx || avx2){
			if(ITER != 0 && ITER != 4 && ITER != 8){
				printf("Checkpoint was from a different CPU.. Restarting workunit.\n");	
				fprintf(stderr,"Checkpoint was from a different CPU.. Restarting workunit.\n");
				K = KMIN;
				ITER = 0;
				result_hash = 0; // zero result hash for BOINC
				last_trickle = time(NULL); // Start trickle timer
			}
		}
#endif
	}
	else{
		printf("Beginning a new search with parameters from the command line\n");
		K = KMIN;
		ITER = 0;
		result_hash = 0; // zero result hash for BOINC
                last_trickle = time(NULL); // Start trickle timer
	}

#ifdef AP26_OPENCL


# ifdef AP26_BOINC
        hardware = sclGetBOINCHardware( argc, argv );
# endif

        // Check GPU vendor
	cl_int err;
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
                setupokok = sclGetCLSoftware(setupokok_cl,"setupokok",hardware, 0);     // AMD's compiler is slow with optimize on

                printf("compiling checkn\n");
                checkn = sclGetCLSoftware(checkn_cl,"checkn",hardware, 0);              // AMD's compiler breaks the kernel with optimize on
        }

        printf("compiling done\n");
        // End Init

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
                // today's gpus use 1024 max, future may use more?
                if(maxlocal >= 1024){
                        sieve_ls = 1024;
                        printf("local workgroup size for sieve kernel is 1024 threads\n");
                }
                else{
                        printf( "selected NVIDIA GPU does not support 1024 thread local size.  Reverting to generic kernel." );
                        sieve = sclGetCLSoftware(sieve_cl,"sieve",hardware, 1);
                        sieve_ls = 64;
                        printf("local workgroup size for sieve kernel is 64 threads\n");
                }
        }

        else{
                sieve_ls = 64;
                printf("local workgroup size for sieve kernel is 64 threads\n");
        }


        // memory allocation
        // host memory
        n59_h = (int64_t*)malloc(quartern59s * sizeof(int64_t));
        sol_k_h = (int*)malloc(sol * sizeof(int));
        sol_val_h = (int64_t*)malloc(sol * sizeof(int64_t));
        // device memory
        n59_0_d = sclMalloc(hardware, CL_MEM_READ_WRITE, quartern59s * sizeof(int64_t));
        n59_1_d = sclMalloc(hardware, CL_MEM_READ_WRITE, quartern59s * sizeof(int64_t));
        n59_2_d = sclMalloc(hardware, CL_MEM_READ_WRITE, quartern59s * sizeof(int64_t));
        n59_3_d = sclMalloc(hardware, CL_MEM_READ_WRITE, quartern59s * sizeof(int64_t));
        ncount_d = sclMalloc(hardware, CL_MEM_READ_WRITE, sizeof(int));
        solcount_d = sclMalloc(hardware, CL_MEM_READ_WRITE, sizeof(int));
        n_result_d = sclMalloc(hardware, CL_MEM_READ_WRITE, numn * sizeof(int64_t));
        OKOK_d = sclMalloc(hardware, CL_MEM_READ_WRITE, numOK * sizeof(int64_t));
        OK_d = sclMalloc(hardware, CL_MEM_READ_WRITE, numOK * sizeof(char));
        offset_d = sclMalloc(hardware, CL_MEM_READ_WRITE, 542 * sizeof(int));
        sol_k_d = sclMalloc(hardware, CL_MEM_READ_WRITE, sol * sizeof(int));
        sol_val_d = sclMalloc(hardware, CL_MEM_READ_WRITE, sol * sizeof(int64_t));

#endif

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

			checkpoint(SHIFT,K,0,ITER);

#ifdef AP26_CPU
			if(avx2){
				Search_avx2(K, SHIFT, ITER, K_COUNT, K_DONE);
			}
			else if(avx){
				Search_avx(K, SHIFT, ITER, K_COUNT, K_DONE);
			}
			else if(sse41){
				Search_sse41(K, SHIFT, ITER, K_COUNT, K_DONE);
			}
			else{
				Search_sse2(K, SHIFT, ITER, K_COUNT, K_DONE);
			}
#else
                        SearchAP26(K,SHIFT,ITER);
#endif

			ITER = 0;
		 	K_DONE++;
		}
	}


#ifdef AP26_BOINC
	boinc_begin_critical_section();
#endif
	/* Force Final checkpoint */
	checkpoint(SHIFT,K,1,ITER);

	/* Write BOINC hash to file */
	write_hash();

#ifdef AP26_BOINC
	boinc_end_critical_section();
#endif


#ifdef AP26_OPENCL
        // free memory
        // host
        free(n59_h);
        free(sol_k_h);
        free(sol_val_h);
        // device
        sclReleaseMemObject(ncount_d);
        sclReleaseMemObject(solcount_d);
        sclReleaseMemObject(n59_0_d);
        sclReleaseMemObject(n59_1_d);
        sclReleaseMemObject(n59_2_d);
        sclReleaseMemObject(n59_3_d);
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

#endif

	/* Done */
#ifdef AP26_BOINC
	boinc_finish(EXIT_SUCCESS);
#endif


  return EXIT_SUCCESS;
}
