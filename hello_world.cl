#ifndef OPENCL
#define OPENCL
#endif

__kernel void hello_world(__global const float* a, __global const float* b, __global float* result) {
	int idx = get_global_id(0);
	printf("[hello_world] idx:\t%d\n", idx);
	printf("a+b:\t%f, %f\n", a[idx], b[idx]);
	result[idx] = a[idx] + b[idx];
	printf("result:\t%f\n", result[idx]);
}
