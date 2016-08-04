#ifndef _cpusse41_H_
#define _cpusse41_H_

	// externs

	// located in AP26.cpp
	void Progress(double prog);
	void ReportSolution(int AP_Length,int difference,int64_t First_Term);
	int PrimeQ(int64_t N);
	void checkpoint(int SHIFT, int K, int force, int ITER);

	// located in cpusse41.cpp
	void Search_sse41(int K, int startSHIFT, int ITER, int K_COUNT, int K_DONE);



#endif
