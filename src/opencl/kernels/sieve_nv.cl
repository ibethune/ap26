/*

	NVIDIA sieve kernel
	Using local memory

*/


#define quartern59s 34343830
#define MOD 258559632607830L


__kernel __attribute__((reqd_work_group_size(1024, 1, 1)))
void sieve(__global long *n59, long S59, int shift, __global long *n_result, __global long *OKOK, __global int *ncount, int offset
){

	int i = get_global_id(0);
	__local long localOKOK[6000];

	if(i==0){
		ncount[0]=0;
	}
        barrier(CLK_GLOBAL_MEM_FENCE);

	// this smem copy only works with 1024 threads/block
	int q = get_local_id(0);
	localOKOK[q] = OKOK[q];				// copies 0-1023
	localOKOK[q + 1024] = OKOK[q + 1024];		// copies 1024 - 2047
	localOKOK[q + 2048] = OKOK[q + 2048];		// copies 2048 - 3071
	localOKOK[q + 3072] = OKOK[q + 3072];		// copies 3072 - 4095
	localOKOK[q + 4096] = OKOK[q + 4096];		// copies 4096 - 5119
	if(q < 880){
		localOKOK[q + 5120] = OKOK[q + 5120];	// copies 5120 - 5999
	}
	barrier(CLK_LOCAL_MEM_FENCE); 

	if(i+offset < quartern59s){

		long sito;
		long n59_local = n59[i+offset];

		int i59;
		for(i59=0;i59<35;i59++){
			int n59a=n59_local&((1<<30)-1);
			int n59b=n59_local>>30;

			if(sito  = localOKOK[ (n59a+60*n59b)%61 ]
				& localOKOK[ ((n59a+25*n59b)%67) + 61 ]
				& localOKOK[ ((n59a+20*n59b)%71) + 128 ]
				& localOKOK[ ((n59a+8*n59b)%73) + 199 ]
				& localOKOK[ ((n59a+52*n59b)%79) + 272 ] 
				& localOKOK[ ((n59a+40*n59b)%83) + 351 ]
				& localOKOK[ ((n59a+78*n59b)%89) + 434 ]
				& localOKOK[ ((n59a+33*n59b)%97) + 523 ]
				& localOKOK[ ((n59a+17*n59b)%101) + 620 ]
				& localOKOK[ ((n59a+93*n59b)%103) + 721 ] 
				& localOKOK[ ((n59a+34*n59b)%107) + 824 ]
				& localOKOK[ ((n59a+46*n59b)%109) + 931 ]
				& localOKOK[ ((n59a+4*n59b)%113) + 1040 ]
				& localOKOK[ ((n59a+4*n59b)%127) + 1153 ]
				& localOKOK[ ((n59a+62*n59b)%131) + 1280 ] 
				& localOKOK[ ((n59a+77*n59b)%137) + 1411 ]
				& localOKOK[ ((n59a+45*n59b)%139) + 1548 ]
				& localOKOK[ ((n59a+144*n59b)%149) + 1687 ]
				& localOKOK[ ((n59a+n59b)%151) + 1836 ]
				& localOKOK[ ((n59a+141*n59b)%157) + 1987 ] 
				& localOKOK[ ((n59a+25*n59b)%163) + 2144 ]
				& localOKOK[ ((n59a+127*n59b)%167) + 2307 ]
				& localOKOK[ ((n59a+24*n59b)%173) + 2474 ]
				& localOKOK[ ((n59a+121*n59b)%179) + 2647 ]
				& localOKOK[ ((n59a+49*n59b)%181) + 2826 ] 
				& localOKOK[ ((n59a+180*n59b)%191) + 3007 ]
				& localOKOK[ ((n59a+27*n59b)%193) + 3198 ]
				& localOKOK[ ((n59a+22*n59b)%197) + 3391 ]
				& localOKOK[ ((n59a+111*n59b)%199) + 3588 ]
				& localOKOK[ ((n59a+171*n59b)%211) + 3787 ] 
				& localOKOK[ ((n59a+169*n59b)%223) + 3998 ] 
				& localOKOK[ ((n59a+44*n59b)%227) + 4221 ]
				& localOKOK[ ((n59a+212*n59b)%229) + 4448 ]
				& localOKOK[ ((n59a+2*n59b)%233) + 4677 ]
				& localOKOK[ ((n59a+147*n59b)%239) + 4910 ] 
				& localOKOK[ ((n59a+64*n59b)%241) + 5149 ]
				& localOKOK[ ((n59a+219*n59b)%251) + 5390 ]
				& localOKOK[ ((n59a+193*n59b)%257) + 5641 ] )



			if(sito &= OKOK[ ((n59a+140*n59b)%263) + 5898 ]
				 & OKOK[ ((n59a+79*n59b)%269) + 6161 ]
				 & OKOK[ ((n59a+258*n59b)%271) + 6430 ]
				 & OKOK[ ((n59a+76*n59b)%277) + 6701 ]
				 & OKOK[ ((n59a+79*n59b)%281) + 6978 ]
				 & OKOK[ ((n59a+204*n59b)%283) + 7259 ]
				 & OKOK[ ((n59a+253*n59b)%293) + 7542 ]
				 & OKOK[ ((n59a+114*n59b)%307) + 7835 ])
			if(sito &= OKOK[ ((n59a+18*n59b)%311) + 8142 ]
				 & OKOK[ ((n59a+19*n59b)%313) + 8453 ]
				 & OKOK[ ((n59a+58*n59b)%317) + 8766 ]
				 & OKOK[ ((n59a+n59b)%331) + 9083 ]
				 & OKOK[ ((n59a+175*n59b)%337) + 9414 ]
				 & OKOK[ ((n59a+292*n59b)%347) + 9751 ]
				 & OKOK[ ((n59a+48*n59b)%349) + 10098 ]
				 & OKOK[ ((n59a+191*n59b)%353) + 10447 ])
			if(sito &= OKOK[ ((n59a+108*n59b)%359) + 10800 ]
				 & OKOK[ ((n59a+15*n59b)%367) + 11159 ]
				 & OKOK[ ((n59a+152*n59b)%373) + 11526 ]
				 & OKOK[ ((n59a+335*n59b)%379) + 11899 ]
				 & OKOK[ ((n59a+175*n59b)%383) + 12278 ]
				 & OKOK[ ((n59a+295*n59b)%389) + 12661 ]
				 & OKOK[ ((n59a+141*n59b)%397) + 13050 ]
				 & OKOK[ ((n59a+164*n59b)%401) + 13447 ])
			if(sito &= OKOK[ ((n59a+259*n59b)%409) + 13848 ]
				 & OKOK[ ((n59a+273*n59b)%419) + 14257 ]
				 & OKOK[ ((n59a+269*n59b)%421) + 14676 ]
				 & OKOK[ ((n59a+144*n59b)%431) + 15097 ]
				 & OKOK[ ((n59a+115*n59b)%433) + 15528 ]
				 & OKOK[ ((n59a+65*n59b)%439) + 15961 ]
				 & OKOK[ ((n59a+196*n59b)%443) + 16400 ]
				 & OKOK[ ((n59a+81*n59b)%449) + 16843 ])
			if(sito &= OKOK[ ((n59a+216*n59b)%457) + 17292 ]
				 & OKOK[ ((n59a+447*n59b)%461) + 17749 ]
				 & OKOK[ ((n59a+376*n59b)%463) + 18210 ]
				 & OKOK[ ((n59a+13*n59b)%467) + 18673 ]
				 & OKOK[ ((n59a+96*n59b)%479) + 19140 ]
				 & OKOK[ ((n59a+328*n59b)%487) + 19619 ]
				 & OKOK[ ((n59a+438*n59b)%491) + 20106 ]
				 & OKOK[ ((n59a+111*n59b)%499) + 20597 ])
			if(sito &= OKOK[ ((n59a+299*n59b)%503) + 21096 ]
				 & OKOK[ ((n59a+216*n59b)%509) + 21599 ]
				 & OKOK[ ((n59a+420*n59b)%521) + 22108 ]
				 & OKOK[ ((n59a+335*n59b)%523) + 22629 ]
				 & OKOK[ ((n59a+189*n59b)%541) + 23152 ]){

				int b;
				int bLimit, bStart;

				bLimit = 63 - clz(sito);
				bStart = 63 - clz(sito&-sito);	// this is ctz

				for (b = bStart; b <= bLimit; b++){
					if((sito>>b)&1){

						int na, nb, nc, tmp1, tmp2;
						long n=n59_local+(b+shift)*MOD;

						// na - lowest 29 bits of n
						// nb - next 17 bits
						// nc - the remaining highest bits

						na=((int)n)&((1<<29)-1);
						nb=((int)(n>>29))&((1<<17)-1);
						nc=((int)(n>>46));

						tmp1=na+151*nb+625*nc; // suitable for 7, 17, 23
						tmp2=na+2580*nb+2506*nc; // suitable for 11, 13, 19

						if(tmp1%7 && tmp2%11 && tmp2%13 && tmp1%17 && tmp2%19 && tmp1%23){
							n_result[atomic_add(&ncount[0], 1)] = n;
						}

					}
				}
			}


			n59_local+=S59;
			if(n59_local>= MOD ){
				n59_local-= MOD;
			}

		}

	}

}



