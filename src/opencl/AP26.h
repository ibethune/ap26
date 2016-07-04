
/*
	***************
	Bryan Little Jun 30 2016
	+ Using ~1.1GB VRAM on GPU
	+ Using ~415MB RAM on host
	+ This is code tuned for Nvidia and AMD GPU OpenCL devices.
	+ Limit kernel queue depth to .2 sec to improve screen refresh

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
	sleep_time.tv_nsec = 10000000;	// 10ms

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


void SearchAP26(int K, int startSHIFT)
{ 

	int64_t STEP;
	int64_t n0;
	int64_t n43, n47, n53;
	int i43, i47, i53;
	int i3, i5, i31, i37, i41;
	int64_t S31, S37, S41, S43, S47, S53, S59;
        size_t global_size[2];
        size_t local_size[2];
	int profile=1;
	int profileq;
	struct timespec proftime, pstime, petime;

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
	int devicearray=0;

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

					n59_h[count]=n53;
					count++;
					if(count == quartern59s){
						if(devicearray == 0){
							// offload to gpu, blocking
							sclWrite(hardware, quartern59s * sizeof(int64_t), n59_0_d, n59_h);
						}
						else if(devicearray == 1){
							// offload to gpu, blocking
							sclWrite(hardware, quartern59s * sizeof(int64_t), n59_1_d, n59_h);
						}
						else if(devicearray == 2){
							// offload to gpu, blocking
							sclWrite(hardware, quartern59s * sizeof(int64_t), n59_2_d, n59_h);
						}
						else if(devicearray == 3){
							// offload to gpu, blocking
							sclWrite(hardware, quartern59s * sizeof(int64_t), n59_3_d, n59_h);
						}
						devicearray++;
						count=0;
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


	// offset kernel
        global_size[0]=576; global_size[1]=1;
        local_size[0]=64; local_size[1]=1;
	sclSetKernelArg(offset, 0, sizeof(cl_mem), &offset_d);
	sclEnqueueKernel(hardware, offset, global_size, local_size);
	// end offset


	int SHIFT;

	for(SHIFT=startSHIFT; SHIFT<(startSHIFT+640); SHIFT+=64){

		time_t start_time, finish_time;
		time (&start_time);

		int numinq=0;

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


		// kernel config for sieve
		const int worksize=1571840;	// must be divisible by 64 and 1024
		int p;

		for(devicearray=0; devicearray<4; devicearray++){
			for( p=0; p<quartern59s; p+=worksize ){

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
				else if(devicearray == 2){
					sclSetKernelArg(sieve, 0, sizeof(cl_mem), &n59_2_d);
				}
				else if(devicearray == 3){
					sclSetKernelArg(sieve, 0, sizeof(cl_mem), &n59_3_d);
				}
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


				if(profile){
					// kernel profile to limit ocl queue to less than .2 sec
					sclFinish(hardware);
					clock_gettime(CLOCK_MONOTONIC, &petime);
					proftime = diff(pstime,petime);
					fprintf(stderr, "kernel profile (sec): %d, (nanoseconds): %d\n", proftime.tv_sec, proftime.tv_nsec);
					int64_t totalnano = (proftime.tv_sec * 1000000000) + proftime.tv_nsec;
					if(totalnano < 200000000){
						profileq = 200000000 / totalnano;
					}
					else{
						profileq = 1;
					}
					fprintf(stderr, "calculated max kernel queue length: %d\n",profileq);
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

		// sleep CPU thread while GPU is busy
		sleepcpu();

		// copy results to host memory
		// blocking read
		sclRead(hardware, sol * sizeof(int), sol_k_d, sol_k_h);
		sclRead(hardware, sol * sizeof(int64_t), sol_val_d, sol_val_h);



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



		printf("Computation for K: %d SHIFT: %d complete.\n", K, SHIFT);

		time(&finish_time);
		printf("GPU time was %d seconds\n", (int)finish_time - (int)start_time);
	}

	time(&total_finish_time);
	printf("total GPU time for K was %d seconds\n", (int)total_finish_time - (int)total_start_time);

}
