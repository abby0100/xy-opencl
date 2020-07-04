#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>

#ifdef DEBUG
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

void test_opencl_nvidia() {
	printf("[-] %s: %d\n", __func__, __LINE__);

	cl_platform_id* platforms;
	cl_uint numOfPlatforms;
	cl_int status;

	status = clGetPlatformIDs(0, NULL, &numOfPlatforms);
	if(status != CL_SUCCESS) {
		perror("Unable to find any OpenCL platforms, exiting...");
		exit(1);
	}
}

int main() {
	printf("[-] %s: %d\n", __func__, __LINE__);
	test_opencl_nvidia();
	return 0;
}
