
/*
	***************
	Bryan Little Jun 30 2016
	+ Using ~1.1GB VRAM on GPU
	+ This is code tuned for Nvidia and AMD GPU OpenCL devices.
	+ Limit kernel queue depth to .1 sec to improve screen refresh

*/

// Code snippet from https://gist.github.com/jbenet/1087739
#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
#  include <mach/clock.h>
#  include <mach/mach.h>
#  define CLOCK_MONOTONIC 0
void clock_gettime(int ignored, struct timespec *ts){
  clock_serv_t cclock;
  mach_timespec_t mts;
  host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
  ts->tv_sec = mts.tv_sec;
  ts->tv_nsec = mts.tv_nsec;
}
#endif

// sleep CPU thread while GPU is busy
void sleepcpu(){

	cl_event kernelsDone;
	cl_int err;
	cl_int info;
	struct timespec sleep_time;
	sleep_time.tv_sec = 0;
	sleep_time.tv_nsec = 1000000;	// 1ms

	err = clEnqueueMarker( hardware.queue, &kernelsDone);
	if ( err != CL_SUCCESS ) {
		printf( "\nError on Finish Marker");
		sclPrintErrorFlags(err); 
	}

	clFlush(hardware.queue);

	err = clGetEventInfo(kernelsDone, CL_EVENT_COMMAND_EXECUTION_STATUS, sizeof(cl_int), &info, NULL);
	if ( err != CL_SUCCESS ) {
		printf( "\nclGetEventInfo Error\n" );
		sclPrintErrorFlags( err );
       	}

	// sleep until event complete
	while(info >= 0 && info != CL_COMPLETE){
		nanosleep(&sleep_time,NULL);
		err = clGetEventInfo(kernelsDone, CL_EVENT_COMMAND_EXECUTION_STATUS, sizeof(cl_int), &info, NULL);
		if ( err != CL_SUCCESS ) {
			printf( "\nclGetEventInfo Error\n" );
			sclPrintErrorFlags( err );
	       	}
	}
	clReleaseEvent(kernelsDone);
}


struct timespec diff(struct timespec start, struct timespec end)
{
	struct timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}


void SearchAP26(int K, int startSHIFT, int s_local, int compute)
{ 

	int64_t STEP;
	int64_t n0;

	int i3, i5, i31, i37, i41;
	int64_t S31, S37, S41, S43, S47, S53, S59;
        size_t global_size[3];
        size_t local_size[3];
	int profile=1;
	int profileq;
	int numinq=0;
	double d = (double)1.0 / (K_COUNT*880);
	double dd;
	int progress = 0;

//	int64_t totaln=0;

	// only using 1D workgroup sizes
	global_size[1]=1;
	local_size[1]=1;
	global_size[2]=1;
	local_size[2]=1;

	struct timespec proftime, pstime, petime;

	time_t total_start_time, total_finish_time;
	time_t last_time, curr_time;

	time (&total_start_time);
	time (&last_time);


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

	for(i31=0;i31<7;++i31)
	for(i37=0;i37<13;++i37)
	if(i37-i31<=10&&i31-i37<=4)
	for(i41=0;i41<17;++i41)
	if(i41-i31<=14&&i41-i37<=14&&i31-i41<=4&&i37-i41<=10)
	for(i3=0;i3<2;++i3)
	for(i5=0;i5<4;++i5){ 
		n43_h[count]=(n0+i3*S3+i5*S5+i31*S31+i37*S37+i41*S41)%MOD;  //10840 of these  12673 n53 per
		count++;
	}

	// offload to gpu, blocking
	sclWrite(hardware, numn43s * sizeof(int64_t), n43_d, n43_h);
	printf( "n43 array copy to GPU complete\n");		

	// setup n59s kernel
        global_size[0]=10880;
        local_size[0]=64;
	sclSetKernelArg(setupn, 0, sizeof(cl_mem), &n43_d);
	sclSetKernelArg(setupn, 1, sizeof(cl_mem), &n59_0_d);
	sclSetKernelArg(setupn, 2, sizeof(cl_mem), &n59_1_d);
	sclSetKernelArg(setupn, 3, sizeof(int64_t), &S53);
	sclSetKernelArg(setupn, 4, sizeof(int64_t), &S47);
	sclSetKernelArg(setupn, 5, sizeof(int64_t), &S43);
	sclEnqueueKernel(hardware, setupn, global_size, local_size);
	// end setup n59s

	// offset kernel
        global_size[0]=576;
        local_size[0]=64;
	sclSetKernelArg(offset, 0, sizeof(cl_mem), &offset_d);
	sclEnqueueKernel(hardware, offset, global_size, local_size);
	// end offset

	// clearsol kernel
	global_size[0]=10240;  
	local_size[0]=64; 
	sclSetKernelArg(clearsol, 0, sizeof(cl_mem), &sol_k_d);
	sclSetKernelArg(clearsol, 1, sizeof(cl_mem), &solcount_d);
	sclEnqueueKernel(hardware, clearsol, global_size, local_size);
	// end clearsol


	int SHIFT;

	for(SHIFT=startSHIFT; SHIFT<(startSHIFT+640); SHIFT+=64){


		// clearok kernel
		global_size[0]=23744; 
		local_size[0]=64; 
		sclSetKernelArg(clearok, 0, sizeof(cl_mem), &OK_d);
		sclSetKernelArg(clearok, 1, sizeof(cl_mem), &OKOK_d);
		sclEnqueueKernel(hardware, clearok, global_size, local_size);
		// end clearok
 

		// setok kernel
		global_size[0]=576; 
		local_size[0]=64; 
		sclSetKernelArg(setok, 0, sizeof(int64_t), &STEP);
		sclSetKernelArg(setok, 1, sizeof(int), &SHIFT);
		sclSetKernelArg(setok, 2, sizeof(cl_mem), &OK_d);
		sclSetKernelArg(setok, 3, sizeof(cl_mem), &offset_d);
		sclEnqueueKernel(hardware, setok, global_size, local_size);
		// end setok


		// setupokok kernel
		global_size[0]=576; 
		local_size[0]=64; 
		sclSetKernelArg(setupokok, 0, sizeof(int64_t), &STEP);
		sclSetKernelArg(setupokok, 1, sizeof(int), &SHIFT);
		sclSetKernelArg(setupokok, 2, sizeof(cl_mem), &OK_d);
		sclSetKernelArg(setupokok, 3, sizeof(cl_mem), &OKOK_d);
		sclSetKernelArg(setupokok, 4, sizeof(cl_mem), &offset_d);
		sclEnqueueKernel(hardware, setupokok, global_size, local_size);
		// end setupokok


		// kernel config for sieve
		const int worksize=1571840;	// must be divisible by 64 and 1024
		int p;

		for(int devicearray=0; devicearray<2; devicearray++){
			for( p=0; p<halfn59s; p+=worksize ){

				// update BOINC progress every 2 sec
				// 88x
				progress++;
				time (&curr_time);
				if( ((int)curr_time - (int)last_time) > 1 ){
		    			dd = (double)(K_DONE*880+progress) * d;
					Progress(dd);
					last_time = curr_time;
				}

				if(profile){
					sleepcpu();  // clear queue
					clock_gettime(CLOCK_MONOTONIC, &pstime);
				}

				// sieve kernel
				if(devicearray == 0){
					sclSetKernelArg(sieve, 0, sizeof(cl_mem), &n59_0_d);
				}
				else if(devicearray == 1){
					sclSetKernelArg(sieve, 0, sizeof(cl_mem), &n59_1_d);
				}
				sclSetKernelArg(sieve, 1, sizeof(int64_t), &S59);
				sclSetKernelArg(sieve, 2, sizeof(int), &SHIFT);
				sclSetKernelArg(sieve, 3, sizeof(cl_mem), &n_result_d);
				sclSetKernelArg(sieve, 4, sizeof(cl_mem), &OKOK_d);
				sclSetKernelArg(sieve, 5, sizeof(cl_mem), &ncount_d);
				sclSetKernelArg(sieve, 6, sizeof(int), &p);
				global_size[0]=worksize; 
				local_size[0]=s_local; 
				sclEnqueueKernel(hardware, sieve, global_size, local_size);
				// end sieve



/*
				int* numbern;
				numbern = (int*)malloc(sizeof(int));
				sclRead(hardware, sizeof(int), ncount_d, numbern);
				totaln += (int64_t)numbern[0];
				printf("adding %d to totaln: %" INT64_FORMAT "\n",numbern[0], totaln);
				//printf("narray size: %d of max %d\n",numbern[0],numn);
				free(numbern);
*/



				// checkn kernel
				sclSetKernelArg(checkn, 0, sizeof(cl_mem), &n_result_d);
				sclSetKernelArg(checkn, 1, sizeof(int64_t), &STEP);
				sclSetKernelArg(checkn, 2, sizeof(cl_mem), &sol_k_d);
				sclSetKernelArg(checkn, 3, sizeof(cl_mem), &sol_val_d);
				sclSetKernelArg(checkn, 4, sizeof(cl_mem), &ncount_d);
				sclSetKernelArg(checkn, 5, sizeof(cl_mem), &solcount_d);
				global_size[0]=numn;  
				local_size[0]=64; 
				sclEnqueueKernel(hardware, checkn, global_size, local_size);
				// end checkn


				if(profile){
					// kernel profile
					sclFinish(hardware);
					clock_gettime(CLOCK_MONOTONIC, &petime);
					proftime = diff(pstime,petime);
					int64_t totalnano = (proftime.tv_sec * 1000000000) + proftime.tv_nsec;

					if(compute){
						// compute mode will queue up kernels to run for up to 1 second
						if(totalnano < 1000000000){
							profileq = 1000000000 / totalnano;
						}
						else{
							profileq = 1;
						}
					}
					else{
						// limit ocl queue to less than .1 sec
						if(totalnano < 100000000){
							profileq = 100000000 / totalnano;
						}
						else{
							profileq = 1;
						}
					}

					profile=0;
				}				
				else{
					numinq++;
				}

				if(numinq == profileq){
					// sleep CPU thread while GPU is busy
					sleepcpu();
					numinq = 0;
				}

			}

		}

	}


	// sleep CPU thread while GPU is busy
	sleepcpu();

	// copy results to host memory
	// blocking read
	sclRead(hardware, sol * sizeof(int), sol_k_d, sol_k_h);
	sclRead(hardware, sol * sizeof(int64_t), sol_val_d, sol_val_h);


	int e=0;
	while(sol_k_h[e] != 0){
		ReportSolution(sol_k_h[e],K,sol_val_h[e]);
		e++;
	}

	time(&total_finish_time);

	if(compute){
		fprintf(stderr, "K %d done in %d sec. Kernel queue: %d (compute mode)\n", K, (int)total_finish_time - (int)total_start_time, profileq);
	}
	else{
		fprintf(stderr, "K %d done in %d sec. Kernel queue: %d\n", K, (int)total_finish_time - (int)total_start_time, profileq);
	}

//	printf("total n for K: %" INT64_FORMAT "\n",totaln);  // for K 366384 this should be 38838420


}
