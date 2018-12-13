/*

	clear sol kernel

*/


__kernel void clearsol(__global int *sol_k, __global int *solcount){

	int i = get_global_id(0);

	if(i<10240){
		if (i == 0){
			solcount[0] = 0;
		}
		sol_k[i] = 0;
	}

}


