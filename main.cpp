#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>

#ifdef APPLE
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

void displayDeviceDetails(cl_device_id id, cl_device_info param_name, const char* paramNameAsStr) {
	cl_int error = 0;
	size_t paramSize = 0;

	error = clGetDeviceInfo(id, param_name, 0, NULL, &paramSize);
	if(error != CL_SUCCESS) {
		perror("Unable to obtain device info for param, exiting...");
		exit(1);
	}

	switch(param_name) {
		case CL_DEVICE_TYPE: {
			cl_device_type* dev_type = (cl_device_type*) alloca (sizeof(cl_device_type) * paramSize);

			error = clGetDeviceInfo(id, param_name, paramSize, dev_type, NULL);
			if(error != CL_SUCCESS) {
				perror("Unable to obtain device info for dev_type, exiting...");
				exit(1);
			}

			switch(*dev_type) {
			case CL_DEVICE_TYPE_CPU:
				printf("CPU detected\n");
				break;
			case CL_DEVICE_TYPE_GPU:
				printf("GPU detected\n");
				break;
			case CL_DEVICE_TYPE_DEFAULT:
				printf("default device detected\n");
				break;
			}
		}
		break;
		case CL_DEVICE_VENDOR_ID:
		case CL_DEVICE_MAX_COMPUTE_UNITS:
		case CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: {
			cl_uint* ret = (cl_uint*) alloca (sizeof(cl_uint) * paramSize);
			error = clGetDeviceInfo(id, param_name, paramSize, ret, NULL);
			if(error != CL_SUCCESS) {
				perror("Unable to obtain device info for param, exiting...");
				exit(1);
			}

			switch(param_name) {
			case CL_DEVICE_VENDOR_ID:
				printf("\tVENDOR ID:\t\t\t\t0x%x\n", *ret);
				break;
			case CL_DEVICE_MAX_COMPUTE_UNITS:
				printf("\tMaximum number of parallel compute units:\t%d\n", *ret);
				break;
			case CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS:
				printf("\tMaximu dimensions for global/local work-item IDs:\t%d\n", *ret);
				break;
			}
		}
		break;
		case CL_DEVICE_MAX_WORK_ITEM_SIZES: {
			cl_uint maxWIDimensions;
			size_t* ret = (size_t*) alloca (sizeof(size_t) * paramSize);
			error = clGetDeviceInfo(id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &maxWIDimensions, NULL);
			if(error != CL_SUCCESS) {
				perror("Unable to obtain device info for param, exiting...");
				exit(1);
			}
			printf("\tMaximum number of work-items in each dimension: ( ");
			for (cl_int i=0; i<maxWIDimensions; ++i) {
				printf("%d ", (int)ret[i]);
			}
			printf(" )\n");
		}
		break;
		case CL_DEVICE_MAX_WORK_GROUP_SIZE: {
			size_t* ret = (size_t*) alloca (sizeof(size_t) * paramSize);
			error = clGetDeviceInfo(id, param_name, paramSize, ret, NULL);
			if(error != CL_SUCCESS) {
				perror("Unable to obtain device info for param, exiting...");
				exit(1);
			}
			printf("\tMaximun number of work-items in a work-group:\t%d\n", (int)*ret);
		}
		break;
		case CL_DEVICE_GLOBAL_MEM_SIZE: {
			size_t* ret = (size_t*) alloca (sizeof(size_t) * paramSize);
			error = clGetDeviceInfo(id, param_name, paramSize, ret, NULL);
			if(error != CL_SUCCESS) {
				perror("Unable to obtain device info for param, exiting...");
				exit(1);
			}
			printf("\tGlobal memory of Device (MB):\t\t\t%d\n", (int)(*ret/1024/1024));
		}
		break;
		case CL_DEVICE_EXTENSIONS: {
			char extensionInfo[4096];
			error = clGetDeviceInfo(id, param_name, paramSize, extensionInfo, NULL);
			printf("\tSupported extensions: %s\n", extensionInfo);
		}
		break;
	}
}

void displayDeviceInfo(cl_platform_id id, cl_device_type dev_type) {
	cl_int error = 0;
	cl_uint numOfDevices = 0;

	error = clGetDeviceIDs(id, dev_type, 0, NULL, &numOfDevices);
	if(error != CL_SUCCESS) {
		perror("Unable to find any OpenCL compliant devices, exiting...");
		exit(1);
	}

	cl_device_id* devices = (cl_device_id*) alloca(sizeof(cl_device_id) * numOfDevices);
	error = clGetDeviceIDs(id, dev_type, numOfDevices, devices, NULL);
	if(error != CL_SUCCESS) {
		perror("Unable to find any OpenCL compliant devices info, exiting...");
		exit(1);
	}

	printf("Number of detected OpenCL devices:\t\t%d\n", numOfDevices);
	for(cl_uint i=0; i<numOfDevices; ++i) {
		displayDeviceDetails(devices[i], CL_DEVICE_TYPE,		"CL_DEVICE_TYPE");
		displayDeviceDetails(devices[i], CL_DEVICE_VENDOR_ID,	"CL_DEVICE_VENDOR_ID");
		displayDeviceDetails(devices[i], CL_DEVICE_MAX_COMPUTE_UNITS,	"CL_DEVICE_MAX_COMPUTE_UNITS");
		displayDeviceDetails(devices[i], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,	"CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS");
		displayDeviceDetails(devices[i], CL_DEVICE_MAX_WORK_GROUP_SIZE,	"CL_DEVICE_MAX_WORK_GROUP_SIZE");
		displayDeviceDetails(devices[i], CL_DEVICE_GLOBAL_MEM_SIZE,	"CL_DEVICE_GLOBAL_MEM_SIZE");
		displayDeviceDetails(devices[i], CL_DEVICE_EXTENSIONS,	"CL_DEVICE_EXTENSIONS");
	}	
}

void displayPlatformInfo(cl_platform_id id, cl_platform_info param_name, const char* paramNameAsStr) {

	cl_int error = 0;
	size_t paramSize = 0;

	error = clGetPlatformInfo(id, param_name, 0, NULL, &paramSize);
	char* moreInfo = (char*) alloca(sizeof(char) * paramSize);

	error = clGetPlatformInfo(id, param_name, paramSize, moreInfo, NULL);
	if(error != CL_SUCCESS) {
		perror("Unable to find any OpenCL platform information, exiting...");
		exit(1);
	}
	printf("%s:\t%s\n", paramNameAsStr, moreInfo);
}

void clinfo() {
	printf("[-] %s: %d\n", __func__, __LINE__);

	// platform
	cl_platform_id* platforms;
	cl_uint numOfPlatforms;
	cl_int error;

	error = clGetPlatformIDs(0, NULL, &numOfPlatforms);
	if(error != CL_SUCCESS) {
		perror("Unable to get OpenCL platforms number, exiting...");
		exit(1);
	}

	platforms = (cl_platform_id*) alloca(sizeof(cl_platform_id) * numOfPlatforms);
	printf("\n");
	printf("Number of OpenCL platforms found:\t\t%d\n", numOfPlatforms);

	error = clGetPlatformIDs(numOfPlatforms, platforms, NULL);
	if(error != CL_SUCCESS) {
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

		printf("\n");
		displayDeviceInfo(platforms[i], CL_DEVICE_TYPE_GPU);
	}
}

int main() {
	printf("[-] %s: %d\n", __func__, __LINE__);
	clinfo();
	return 0;
}
