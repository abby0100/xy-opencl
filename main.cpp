#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>

#ifdef APPLE
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

void displayPlatformInfo(cl_platform_id id, cl_platform_info param_name, const char* paramNameAsStr) {

	cl_int status = 0;
	size_t paramSize = 0;

	status = clGetPlatformInfo(id, param_name, 0, NULL, &paramSize);
	char* moreInfo = (char*) alloca(sizeof(char) * paramSize);

	status = clGetPlatformInfo(id, param_name, paramSize, moreInfo, NULL);
	if(status != CL_SUCCESS) {
		perror("Unable to find any OpenCL platform information, exiting...");
		exit(1);
	}
	printf("%s:\t%s\n", paramNameAsStr, moreInfo);
}

void test_opencl_nvidia() {
	printf("[-] %s: %d\n", __func__, __LINE__);

	// platform
	cl_platform_id* platforms;
	cl_uint numOfPlatforms;
	cl_int status;

	status = clGetPlatformIDs(0, NULL, &numOfPlatforms);
	if(status != CL_SUCCESS) {
		perror("Unable to get OpenCL platforms number, exiting...");
		exit(1);
	}

	platforms = (cl_platform_id*) alloca(sizeof(cl_platform_id) * numOfPlatforms);
	printf("Number of OpenCL platforms found:\t%d\n", numOfPlatforms);

	status = clGetPlatformIDs(numOfPlatforms, platforms, NULL);
	if(status != CL_SUCCESS) {
		perror("Unable to find and OpenCL platforms, exiting...");
		exit(1);
	}

	// display
	for(cl_uint i=0; i<numOfPlatforms; ++i) {
		displayPlatformInfo(platforms[i], CL_PLATFORM_VERSION,	"CL_PLATFORM_VERSION");
		displayPlatformInfo(platforms[i], CL_PLATFORM_VENDOR,	"CL_PLATFORM_VENDOR");
		displayPlatformInfo(platforms[i], CL_PLATFORM_NAME, 	"CL_PLATFORM_NAME");
		displayPlatformInfo(platforms[i], CL_PLATFORM_PROFILE,	"CL_PLATFORM_PROFILE");
		displayPlatformInfo(platforms[i], CL_PLATFORM_EXTENSIONS,	"CL_PLATFORM_EXTENSIONS");
	}
}

int main() {
	printf("[-] %s: %d\n", __func__, __LINE__);
	test_opencl_nvidia();
	return 0;
}
