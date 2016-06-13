# ap26

   Programs for finding large chains of primes in arithmetic progression.

   History
   -------

   This code is developed and maintained by Bryan Little and Iain Bethune,
   based on several versions of the AP26 software developed since 2008.

   AP26-boinc.c -- Geoffrey Reynolds, 11 September 2008.

   Bryan Little 6-9-2016
   BOINC enabled OpenCL Nvidia/AMD GPU AP26 app port

   Modified for SSE2 support 27 September 2009 by Bryan Little
   *NOTE* this source code is for 64-bit only

   Bryan Little 2 June 2016 added result hash and 10-shift search.
   This is a 10 shift search.  Checks each K with SHIFT and SHIFT+64, SHIFT+128, ..., SHIFT+576

   This file is a replacement for AP26-64.c in Jaroslaw Wroblewski's AP26
   sample implementation.

   See http://www.geocities.com/g_w_reynolds/AP26/AP26-boinc-src.zip for the
   an early version of this file.

   See http://www.math.uni.wroc.pl/~jwr/AP26/AP26.zip for the sample
   implementation containing the rest of the source required.

   See http://www.math.uni.wroc.pl/~jwr/AP26/faster32.zip for a patch to
   dramatically improve performance on 32-bit machines.

   See http://www.math.uni.wroc.pl/~jwr/AP26/AP26v3.pdf for information
   about how the algorithm works and for his copyleft notice.

   Compiling
   ---------

   Makefiles are provided in the sse2 and opencl directories for Windows, Linux and Mac OS.

   Compilation is controlled by the following preprocessor #defines:

   -DAP26_BOINC  : Build with the BOINC library
   -DAP26_SSE2   : Build for SSE2 CPUs (64 bit)
   -DAP26_OPENCL : Build for GPUs using OpenCL

   Testing the executable:
   -----------------------

   To briefly test the AP26 executable, check that the file tests/TEST-366384.txt
   matches the results file SOL-AP26.txt produced by executing:

     ./AP26 366384 366384 0

   Program operation:
   ------------------

   search parameters are given on the command line as

     AP26 [KMIN KMAX SHIFT] [--device N] where N is the GPU to use

   The search will begin at K=KMIN unless a file AP26-state.txt
   exists containing a checkpoint of the form

     KMIN KMAX SHIFT K HASH

   with KMIN KMAX SHIFT matching the initial search parameters, in which
   case the search will resume from that checkpoint.

   The search will continue up to and including K=KMAX. On completion
   AP26-state.txt will contain a checkpoint of the form

     KMIN KMAX SHIFT KMAX+1 HASH

   Periodic checkpoints will be written to AP26-state.txt.
   All search results and a result hash will be appended to SOL-AP26.txt.
