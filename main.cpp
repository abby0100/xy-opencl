#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <iostream>
#include <fstream>
#include <string>

#ifdef APPLE
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#define CHECK_ERROR(ERR, PARAM)	\
	if(ERR != CL_SUCCESS) {	\
		fprintf(stderr, "Unable to obtain OpenCL info for %s, error happened at\t%s: %d\n", PARAM, __FILE__, __LINE__);	\
		exit(1);			\
	}

typedef unsigned char uchar;
typedef unsigned int uint;
using namespace std;

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
	printf("\n[-] %s: %d\n", __func__, __LINE__);

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

void callback(const char* errinfo, const void*, size_t, void*) {
	cout << "Context callback:\t" << errinfo << endl;
}

//void loadProgramSource(const char* file_path, char* buffer, size_t& size) {
//	//ifstream in(_T(file_path), std::ios_base::binary);
//	ifstream in((file_path), std::ios_base::binary);
//	if(!in.good())
//		return;
//
//	// get file length
//	in.seekg(0, std::ios_base::end);
//	size_t length = in.tellg();
//
//	// read program source
//	char data[length + 1];
//	in.seekg(0, std::ios_base::beg);
//	in.read(&data[0], length);
//	data[length] = 0;
//
//	// return
//	size = length;
//	//buffer = &data[0];
//	buffer = data;
//}

void loadProgramSource(const char** filePaths, size_t number, char** buffer, size_t* sizes) {
	for(size_t i=0; i<number; ++i) {
		FILE* file = fopen(filePaths[i], "r");
		if(file == NULL) {
			perror("Couldn't read the program file");
			exit(1);
		}

		fseek(file, 0, SEEK_END);
		sizes[i] = ftell(file);
		rewind(file);
		buffer[i] = new char[sizes[i] + 1];
		fread(buffer[i], sizeof(char), sizes[i], file);
		buffer[i][sizes[i]] = 0;

		//cout << "---> loadProgramSource:\n" << buffer << endl;
		fclose(file);
	}
}

//int ReadSourceFromFile(const char* filePath, char** buffer, size_t& size) {
//	int error = CL_SUCCESS;
//
//	FILE* fp = NULL;
//	fopen_s(&fp, filePath, "rb");
//	if(fp == NULL) {
//		perror("Error: Couldn't find program source file '%s'.", filePath);
//		error = CL_INVALID_VALUE;
//	} else {
//		fseek(fp, 0, SEEK_END);
//		size = ftell(fp);
//		fseek(fp, 0, SEEK_SET);
//		*buffer = new char[size];
//		if(*buffer == NULL) {
//			cout << "Error: Couldn't allocate " << size << " bytes for program source from file " << filePath << endl;
//			error = CL_OUT_OF_HOST_MEMORY;
//		} else {
//			fread(*buffer, 1, size, fp);
//		}
//	}
//	return error;
//}

template <typename T>
void validateData(T* data, size_t size) {
	cout << "\n[-] " << __func__ << ": " << __LINE__ << endl;

	for(size_t i=0; i<size; ++i) {
		if(i % 32 == 0)
			cout << endl;
		cout << data[i] << " ";
	}
	cout << endl;
}

void testGPU() {
	// [2013] https://www.photoneray.com/opencl_01/
	// https://zhuanlan.zhihu.com/p/53772179
	cout << "\n[-] " << __func__ << ": " << __LINE__ << endl;

	cl_int error = CL_SUCCESS;

	// platform
	cl_uint numOfPlatforms = 0;
	error = clGetPlatformIDs(0, NULL, &numOfPlatforms);
	CHECK_ERROR(error, "platform number");
	cout << "Number of available platforms:\t" << numOfPlatforms << endl;

	cl_platform_id* platforms = new cl_platform_id[numOfPlatforms];
	error = clGetPlatformIDs(numOfPlatforms, platforms, NULL);
	CHECK_ERROR(error, "get platforms")

	// device
	cl_uint numOfDevices = 0;
	error = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 0, NULL, &numOfDevices);
	CHECK_ERROR(error, "device number");
	cout << "Number of available devices:\t" << numOfDevices << endl;

	cl_device_id* devices = new cl_device_id[numOfDevices];
	error = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, numOfDevices, devices, NULL);
	CHECK_ERROR(error, "get devices");

	// context
	//cl_context context = clCreateContext(NULL, 1, &devices[0], &callback, NULL, &error);
	cl_context context = clCreateContext(NULL, numOfDevices, devices, &callback, NULL, &error);
	CHECK_ERROR(error, "create context");

	// command queue
	//#pragma warning( disable : 4996 )
	cl_command_queue queue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE, &error);
	CHECK_ERROR(error, "command queue");

	// program
	cl_uint numOfFiles = 1;
	char* buffer[numOfFiles];
	size_t sizes[numOfFiles];
	const char* sources[] = {"hello_world.cl"};

	//loadProgramSource(source, buffer, size);
	//ReadSourceFromFile(source, &buffer, size);
	loadProgramSource(sources, numOfFiles, buffer, sizes);
	//cout << "loadProgramSource:\t" << sources << ", sizes: " << sizes << endl;

	cl_program program = clCreateProgramWithSource(context, numOfFiles, (const char**)buffer, sizes, &error);
	CHECK_ERROR(error, "create program");

	const char options[] = "-cl-finite-math-only -cl-no-signed-zeros";
	//const char options[] = "";
	cout << "Build program successfully!" << endl;
	error = clBuildProgram(program, 1, &devices[0], options, NULL, NULL);
	//error = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	CHECK_ERROR(error, "build program");
	if(error != CL_SUCCESS) {
		size_t log_size = 0;
		clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		char program_log[log_size + 1];
		clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, log_size+1, program_log, NULL);
		program_log[log_size] = 0;
		cout << "\n=== Error === \n\n" << program_log << "\n=============" << endl;
		exit(1);
	}

	// kernel
	cl_kernel kernel = clCreateKernel(program, "hello_world", &error);
	CHECK_ERROR(error, "create kernel");

	// buffer
	size_t bufferSize = 64;
	//size_t bufferSize = 1024;
	//uchar A[bufferSize];
	//uchar B[bufferSize];
	//uchar C[bufferSize];
	float A[bufferSize];
	float B[bufferSize];
	float C[bufferSize];
	for(size_t i=0; i<bufferSize; ++i) {
		A[i] = i + 1;
		B[i] = bufferSize - i;
		C[i] = 0;
	}
	validateData(A, bufferSize);
	validateData(B, bufferSize);
	validateData(C, bufferSize);
	//cl_mem a = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(uchar) * bufferSize, 0, &error);
	//cl_mem a = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(uchar) * bufferSize, A, &error);
	//cl_mem b = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(uchar) * bufferSize, B, &error);
	//cl_mem result = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(uchar) * bufferSize, C, &error);
	cl_mem a = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * bufferSize, A, &error);
	cl_mem b = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * bufferSize, B, &error);
	cl_mem result = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * bufferSize, C, &error);

	// kernel args
	clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*) &a);
	clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*) &b);
	clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*) &result);

	// enqueue way1
	//error = clEnqueueTask(queue, kernel, 0, NULL, NULL);
	//CHECK_ERROR(error, "enqueue task");

	// enqueue way2
	size_t globalThreads[] = {bufferSize,1};
	size_t localThreads[]  = {32,1};
	error = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalThreads, localThreads, 0, NULL, NULL);
	CHECK_ERROR(error, "enqueue NDRange");

	cout << "Enqueue task successfully!" << endl;

	// read result
	//uchar resultData[bufferSize];
	float resultData[bufferSize];
	//error = clEnqueueReadBuffer(queue, result, CL_TRUE, 0, sizeof(uchar) * bufferSize, resultData, 0, NULL, NULL);
	error = clEnqueueReadBuffer(queue, result, CL_TRUE, 0, sizeof(uint) * bufferSize, resultData, 0, NULL, NULL);
	CHECK_ERROR(error, "read buffer");
	cout << "Read buffer successfully!" << endl;

	error = clFinish(queue);
	CHECK_ERROR(error, "finish queue");
	validateData(resultData, bufferSize);
}

int main() {
	printf("\n[-] %s: %d\n", __func__, __LINE__);
	clinfo();

	testGPU();
	return 0;
}
