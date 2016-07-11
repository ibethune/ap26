/* #######################################################################
    Copyright 2011 Oscar Amoros Huguet, Cristian Garcia Marin

    This file is part of SimpleOpenCL

    SimpleOpenCL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3.

    SimpleOpenCL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SimpleOpenCL. If not, see <http://www.gnu.org/licenses/>.

   ####################################################################### 

   SimpleOpenCL Version 0.010_27_02_2013 

*/

#ifdef __cplusplus
extern "C" {
#endif

#include "simpleCL.h"

void sclPrintErrorFlags( cl_int flag ){
    
	switch (flag){

		case CL_DEVICE_NOT_FOUND:
			printf("\nCL_DEVICE_NOT_FOUND\n");
			break;
		case CL_DEVICE_NOT_AVAILABLE:
			printf("\nCL_DEVICE_NOT_AVAILABLE\n");
			break;
		case CL_COMPILER_NOT_AVAILABLE:
			printf("\nCL_COMPILER_NOT_AVAILABLE\n");
			break;
		case CL_PROFILING_INFO_NOT_AVAILABLE:
			printf("\nCL_PROFILING_INFO_NOT_AVAILABLE\n");
			break;
		case CL_MEM_COPY_OVERLAP:
			printf("\nCL_MEM_COPY_OVERLAP\n");
			break;
		case CL_IMAGE_FORMAT_MISMATCH:
			printf("\nCL_IMAGE_FORMAT_MISMATCH\n");
			break;
		case CL_IMAGE_FORMAT_NOT_SUPPORTED:
			printf("\nCL_IMAGE_FORMAT_NOT_SUPPORTED\n");
			break;
		case CL_INVALID_COMMAND_QUEUE:
			printf("\nCL_INVALID_COMMAND_QUEUE\n");
			break;
		case CL_INVALID_CONTEXT:
			printf("\nCL_INVALID_CONTEXT\n");
			break;
		case CL_INVALID_MEM_OBJECT:
			printf("\nCL_INVALID_MEM_OBJECT\n");
			break;
		case CL_INVALID_VALUE:
			printf("\nCL_INVALID_VALUE\n");
			break;
		case CL_INVALID_EVENT_WAIT_LIST:
			printf("\nCL_INVALID_EVENT_WAIT_LIST\n");
			break;
		case CL_MEM_OBJECT_ALLOCATION_FAILURE:
			printf("\nCL_MEM_OBJECT_ALLOCATION_FAILURE\n");
			break;
		case CL_OUT_OF_HOST_MEMORY:
			printf("\nCL_OUT_OF_HOST_MEMORY\n");
			break;

		case CL_INVALID_PROGRAM_EXECUTABLE:
			printf("\nCL_INVALID_PROGRAM_EXECUTABLE\n");
			break;
		case CL_INVALID_KERNEL:
			printf("\nCL_INVALID_KERNEL\n");
			break;
		case CL_INVALID_KERNEL_ARGS:
			printf("\nCL_INVALID_KERNEL_ARGS\n");
			break;
		case CL_INVALID_WORK_DIMENSION:
			printf("\nCL_INVALID_WORK_DIMENSION\n");
			break;
#ifndef __APPLE__ 
		case CL_INVALID_GLOBAL_WORK_SIZE:
			printf("\nCL_INVALID_GLOBAL_WORK_SIZE\n");
			break;
#endif
		case CL_INVALID_WORK_GROUP_SIZE:
			printf("\nCL_INVALID_WORK_GROUP_SIZE\n");
			break;
		case CL_INVALID_WORK_ITEM_SIZE:
			printf("\nCL_INVALID_WORK_ITEM_SIZE\n");
			break;
		case CL_INVALID_GLOBAL_OFFSET:
			printf("\nCL_INVALID_GLOBAL_OFFSET\n");
			break;
		case CL_OUT_OF_RESOURCES:
			printf("\nCL_OUT_OF_RESOURCES\n");
			break;

		case CL_INVALID_PROGRAM:
			printf("\nCL_INVALID_PROGRAM\n");
			break;
		case CL_INVALID_KERNEL_NAME:
			printf("\nCL_INVALID_KERNEL_NAME\n");
			break;
		case CL_INVALID_KERNEL_DEFINITION:
			printf("\nCL_INVALID_KERNEL_DEFINITION\n");
			break;
		case CL_INVALID_BUFFER_SIZE:
			printf("\nCL_INVALID_BUFFER_SIZE\n");
			break;
		case CL_BUILD_PROGRAM_FAILURE:
			printf("\nCL_BUILD_PROGRAM_FAILURE\n");
			break;
		case CL_INVALID_ARG_INDEX:
			printf("\nCL_INVALID_ARG_INDEX\n");
			break;
		case CL_INVALID_ARG_VALUE:
			printf("\nCL_INVALID_ARG_VALUE\n");
			break;
		case CL_MAP_FAILURE:
			printf("\nCL_MAP_FAILURE\n");
			break;
		case CL_MISALIGNED_SUB_BUFFER_OFFSET:
			printf("\nCL_MISALIGNED_SUB_BUFFER_OFFSET\n");
			break;
		case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
			printf("\nCL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST\n");
			break;
		case CL_INVALID_DEVICE_TYPE:
			printf("\nCL_INVALID_DEVICE_TYPE\n");
			break;
		case CL_INVALID_PLATFORM:
			printf("\nCL_INVALID_PLATFORM\n");
			break;
		case CL_INVALID_DEVICE:
			printf("\nCL_INVALID_DEVICE\n");
			break; 
		case CL_INVALID_QUEUE_PROPERTIES:
			printf("\nCL_INVALID_QUEUE_PROPERTIES\n");
			break; 
		case CL_INVALID_HOST_PTR:
			printf("\nCL_INVALID_HOST_PTR\n");
			break;
		case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
			printf("\nCL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
			break;
		case CL_INVALID_IMAGE_SIZE:
			printf("\nCL_INVALID_IMAGE_SIZE\n");
			break;
		case CL_INVALID_SAMPLER:
			printf("\nCL_INVALID_SAMPLER\n");
			break;
		case CL_INVALID_BINARY:
			printf("\nCL_INVALID_BINARY\n");
			break;
		case CL_INVALID_BUILD_OPTIONS:
			printf("\nCL_INVALID_BUILD_OPTIONS\n");
			break;
		case CL_INVALID_ARG_SIZE:
			printf("\nCL_INVALID_ARG_SIZE\n");
			break;
		case CL_INVALID_EVENT:
			printf("\nCL_INVALID_EVENT\n");
			break;
		case CL_INVALID_OPERATION:
			printf("\nCL_INVALID_OPERATION\n");
			break;
		case CL_INVALID_GL_OBJECT:
			printf("\nCL_INVALID_GL_OBJECT\n");
			break;
		case CL_INVALID_MIP_LEVEL:
			printf("\nCL_INVALID_MIP_LEVEL\n");
			break;
		case CL_INVALID_PROPERTY:
			printf("\nCL_INVALID_PROPERTY\n");
			break;
		default:
			printf("\nUnknown error code: %d\n",flag);    
	}

#ifdef AP26_BOINC
        fprintf(stderr, "OpenCL Error, Code: %d\n", flag);
#endif
	exit(EXIT_FAILURE);
}

char* _sclLoadProgramSource( const char *filename )
{ 
	struct stat statbuf;
	FILE *fh; 
	char *source;
	
	fh = fopen( filename, "r" );
	if ( fh == 0 )
		return 0;
	
	stat( filename, &statbuf );
	source = (char *)malloc( statbuf.st_size + 1 );
	fread( source, statbuf.st_size, 1, fh );
	source[ statbuf.st_size ] = '\0';
	
	fclose( fh );
	
	return source; 
}
 
cl_program _sclCreateProgram( char* program_source, cl_context context )
{
	cl_program program;

	cl_int err;
	
	program = clCreateProgramWithSource( context, 1, (const char**)&program_source, NULL, &err );
	if ( err!=CL_SUCCESS ) {
		printf( "Error on createProgram" );
		sclPrintErrorFlags( err );
	}

	
	return program;
}

void _sclBuildProgram( cl_program program, cl_device_id devices, const char* pName, int opt )
{
	cl_int err;
	char build_c[4096];
	
//	err = clBuildProgram( program, 0, NULL, NULL, NULL, NULL );
	// Bryan Little
	if(opt){
		printf("building with optimizations\n");
		err = clBuildProgram( program, 0, NULL, NULL, NULL, NULL );
	}
	else{
		printf("building withOUT optimizations\n");
		err = clBuildProgram( program, 0, NULL, "-cl-opt-disable", NULL, NULL );
	}


   	if ( err != CL_SUCCESS ) {
		printf( "Error on buildProgram " );
		fprintf( stdout, "\nRequestingInfo\n" );
		clGetProgramBuildInfo( program, devices, CL_PROGRAM_BUILD_LOG, 4096, build_c, NULL );
		printf( "Build Log for %s_program:\n%s\n", pName, build_c );
		sclPrintErrorFlags( err ); 
	}

}

cl_kernel _sclCreateKernel( sclSoft software ) {
	cl_kernel kernel;
	cl_int err;

	kernel = clCreateKernel( software.program, software.kernelName, &err );
	if ( err != CL_SUCCESS ) {
		printf( "Error on createKernel %s ", software.kernelName );
		sclPrintErrorFlags( err );
	}

	return kernel;
}


// Bryan Little - not using events
void sclEnqueueKernel( sclHard hardware, sclSoft software, size_t *global_work_size, size_t *local_work_size) {

	cl_int err;

	err = clEnqueueNDRangeKernel( hardware.queue, software.kernel, 2, NULL, global_work_size, local_work_size, 0, NULL, NULL );
	if ( err != CL_SUCCESS ) {
		printf( "\nError on EnqueueKernel %s", software.kernelName );
		sclPrintErrorFlags(err); 
	}
		
}

/*
cl_event sclEnqueueKernel( sclHard hardware, sclSoft software, size_t *global_work_size, size_t *local_work_size) {
	cl_event myEvent;	
	cl_int err;

	err = clEnqueueNDRangeKernel( hardware.queue, software.kernel, 2, NULL, global_work_size, local_work_size, 0, NULL, &myEvent );
	if ( err != CL_SUCCESS ) {
		printf( "\nError on launchKernel %s", software.kernelName );
		sclPrintErrorFlags(err); }


	return myEvent;
		
}
*/

void sclReleaseClSoft( sclSoft soft ) {
	clReleaseKernel( soft.kernel );
	clReleaseProgram( soft.program );
}


void sclReleaseClHard( sclHard hardware ){
	clReleaseCommandQueue( hardware.queue );
	clReleaseContext( hardware.context );
}


void sclReleaseMemObject( cl_mem object ) {
	cl_int err;

	err = clReleaseMemObject( object );
	if ( err != CL_SUCCESS ) {
		printf( "\nError on sclReleaseMemObject" );
		sclPrintErrorFlags(err); 
	}	

}


int _sclGetMaxComputeUnits( cl_device_id device ) {
	
	cl_uint nCompU;

	clGetDeviceInfo( device, CL_DEVICE_MAX_COMPUTE_UNITS, 4, (void *)&nCompU, NULL );

	return (int)nCompU;	

}

unsigned long int _sclGetMaxMemAllocSize( cl_device_id device ){

	cl_ulong mem;

 	clGetDeviceInfo( device, CL_DEVICE_MAX_MEM_ALLOC_SIZE, 8, (void *)&mem, NULL );

	return (unsigned long int)mem;	

}


unsigned long int _sclGetMaxGlobalMemSize( cl_device_id device ){

	cl_ulong mem;

 	clGetDeviceInfo( device, CL_DEVICE_GLOBAL_MEM_SIZE, 8, (void *)&mem, NULL );

	return (unsigned long int)mem;	

}


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


	retval = boinc_get_opencl_ids(argc, argv, 0, &device, &platform);
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

        // Print out cmd line for diagnostics
        fprintf(stderr, "Command line: ");
        for (i = 0; i < argc; i++)
        	fprintf(stderr, "%s ", argv[i]);
        fprintf(stderr, "\n");

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


// Bryan Little added opt flag to turn on/off optimizations during kernel compile
sclSoft sclGetCLSoftware( char* source, char* name, sclHard hardware, int opt ){

	sclSoft software;

	sprintf( software.kernelName, "%s", name);
	
	/* Create program objects from source
	 ########################################################### */
	software.program = _sclCreateProgram( source, hardware.context );
	/* ########################################################### */
	
	/* Build the program (compile it)
   	 ############################################ */
	// Bryan Little
   	_sclBuildProgram( software.program, hardware.device, name, opt );
   	/* ############################################ */
   	
   	/* Create the kernel object
	 ########################################################################## */
	software.kernel = _sclCreateKernel( software );
	/* ########################################################################## */

	return software;
	
}


/*
cl_mem sclMalloc( sclHard hardware, cl_int mode, size_t size ){
	cl_mem buffer;

	cl_int err;
	
	buffer = clCreateBuffer( hardware.context, mode, size, NULL, &err );
	if ( err != CL_SUCCESS ) {
		printf( "\nclMalloc Error\n" );
		sclPrintErrorFlags( err );
	}

		
	return buffer;
}	
*/

void sclWrite( sclHard hardware, size_t size, cl_mem buffer, void* hostPointer ) {

	cl_int err;

	err = clEnqueueWriteBuffer( hardware.queue, buffer, CL_TRUE, 0, size, hostPointer, 0, NULL, NULL );
	if ( err != CL_SUCCESS ) { 
		printf( "\nclWrite Error\n" );
		sclPrintErrorFlags( err );
	}   

}

void sclRead( sclHard hardware, size_t size, cl_mem buffer, void *hostPointer ) {

	cl_int err;

	err = clEnqueueReadBuffer( hardware.queue, buffer, CL_TRUE, 0, size, hostPointer, 0, NULL, NULL );
	if ( err != CL_SUCCESS ) {
		printf( "\nclRead Error\n" );
		sclPrintErrorFlags( err );
       	}

}

cl_int sclFinish( sclHard hardware ){

	cl_int err;

	err = clFinish( hardware.queue );
	if ( err != CL_SUCCESS ) {
		printf( "\nError clFinish\n" );
		sclPrintErrorFlags( err );
	}


	return err;

}


void sclSetKernelArg( sclSoft software, int argnum, size_t typeSize, void *argument ){

	cl_int err;

	err = clSetKernelArg( software.kernel, argnum, typeSize, argument );
	if ( err != CL_SUCCESS ) {	
		printf( "\nError clSetKernelArg number %d\n", argnum );
		sclPrintErrorFlags( err );
	}


}


#ifdef __cplusplus
}
#endif
