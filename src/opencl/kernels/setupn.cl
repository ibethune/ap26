/*

	setup n59s on GPU kernel

	Bryan Little 1-24-2019
	moved from cpu to gpu also greatly reduces host RAM use.  Same VRAM usage.

*/

#define MOD 258559632607830L

__kernel void setupn(__global long *n43_d, __global long *n59_0, __global long *n59_1, long S53, long S47, long S43){


	int i = get_global_id(0);
	long n43, n47, n53;
	int i43, i47, i53;
	int count;

	if(i<10840){

		n43 = n43_d[i];

		if(i<5420){
			count = i * 12673;
		}
		else{
			count = (i-5420) * 12673;
		}

		for(i43=19;i43>0;i43--){
			n47=n43;
			for(i47=23;i47>0;i47--){
				n53=n47;
				for(i53=29;i53>0;i53--){
					//n59=n53;
					if(i<5420){
						n59_0[count] = n53;
					}
					else{
						n59_1[count] = n53;
					}
					count++;

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

}


