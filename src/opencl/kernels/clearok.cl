/*

	clearOK kernel

*/


__kernel void clearok(__global char *OK, __global long *OKOK){

	int i = get_global_id(0);

	if(i < 23693){
		OK[i] = 1;
		OKOK[i] = 0;
	}

}
