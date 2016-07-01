/*
	offset kernel
*/



__kernel void offset(__global int *offset){

	int i = get_global_id(0);

	if(i == 61)offset[i] = 0;
	else if(i == 67)offset[i] = 61;
	else if(i == 71)offset[i] = 128;
	else if(i == 73)offset[i] = 199;
	else if(i == 79)offset[i] = 272;
	else if(i == 83)offset[i] = 351;
	else if(i == 89)offset[i] = 434;
	else if(i == 97)offset[i] = 523;
	else if(i == 101)offset[i] = 620;
	else if(i == 103)offset[i] = 721;
	else if(i == 107)offset[i] = 824;
	else if(i == 109)offset[i] = 931;
	else if(i == 113)offset[i] = 1040;
	else if(i == 127)offset[i] = 1153;
	else if(i == 131)offset[i] = 1280;
	else if(i == 137)offset[i] = 1411;
	else if(i == 139)offset[i] = 1548;
	else if(i == 149)offset[i] = 1687;
	else if(i == 151)offset[i] = 1836;
	else if(i == 157)offset[i] = 1987;
	else if(i == 163)offset[i] = 2144;
	else if(i == 167)offset[i] = 2307;
	else if(i == 173)offset[i] = 2474;
	else if(i == 179)offset[i] = 2647;
	else if(i == 181)offset[i] = 2826;
	else if(i == 191)offset[i] = 3007;
	else if(i == 193)offset[i] = 3198;
	else if(i == 197)offset[i] = 3391;
	else if(i == 199)offset[i] = 3588;
	else if(i == 211)offset[i] = 3787;
	else if(i == 223)offset[i] = 3998;
	else if(i == 227)offset[i] = 4221;
	else if(i == 229)offset[i] = 4448;
	else if(i == 233)offset[i] = 4677;
	else if(i == 239)offset[i] = 4910;
	else if(i == 241)offset[i] = 5149;
	else if(i == 251)offset[i] = 5390;
	else if(i == 257)offset[i] = 5641;
	else if(i == 263)offset[i] = 5898;
	else if(i == 269)offset[i] = 6161;
	else if(i == 271)offset[i] = 6430;
	else if(i == 277)offset[i] = 6701;
	else if(i == 281)offset[i] = 6978;
	else if(i == 283)offset[i] = 7259;
	else if(i == 293)offset[i] = 7542;
	else if(i == 307)offset[i] = 7835;
	else if(i == 311)offset[i] = 8142;
	else if(i == 313)offset[i] = 8453;
	else if(i == 317)offset[i] = 8766;
	else if(i == 331)offset[i] = 9083;
	else if(i == 337)offset[i] = 9414;
	else if(i == 347)offset[i] = 9751;
	else if(i == 349)offset[i] = 10098;
	else if(i == 353)offset[i] = 10447;
	else if(i == 359)offset[i] = 10800;
	else if(i == 367)offset[i] = 11159;
	else if(i == 373)offset[i] = 11526;
	else if(i == 379)offset[i] = 11899;
	else if(i == 383)offset[i] = 12278;
	else if(i == 389)offset[i] = 12661;
	else if(i == 397)offset[i] = 13050;
	else if(i == 401)offset[i] = 13447;
	else if(i == 409)offset[i] = 13848;
	else if(i == 419)offset[i] = 14257;
	else if(i == 421)offset[i] = 14676;
	else if(i == 431)offset[i] = 15097;
	else if(i == 433)offset[i] = 15528;
	else if(i == 439)offset[i] = 15961;
	else if(i == 443)offset[i] = 16400;
	else if(i == 449)offset[i] = 16843;
	else if(i == 457)offset[i] = 17292;
	else if(i == 461)offset[i] = 17749;
	else if(i == 463)offset[i] = 18210;
	else if(i == 467)offset[i] = 18673;
	else if(i == 479)offset[i] = 19140;
	else if(i == 487)offset[i] = 19619;
	else if(i == 491)offset[i] = 20106;
	else if(i == 499)offset[i] = 20597;
	else if(i == 503)offset[i] = 21096;
	else if(i == 509)offset[i] = 21599;
	else if(i == 521)offset[i] = 22108;
	else if(i == 523)offset[i] = 22629;
	else if(i == 541)offset[i] = 23152;

}


