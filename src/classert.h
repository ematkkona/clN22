#define cl(...)    cl_assert((cl##__VA_ARGS__), __FILE__, __LINE__, true);
#define cl_ok(err) cl_assert(err, __FILE__, __LINE__, true);
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#if CL_TARGET_OPENCL_VERSION == 120
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#elif CL_TARGET_OPENCL_VERSION == 110
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#elif CL_TARGET_OPENCL_VERSION == 100
#define CL_USE_DEPRECATED_OPENCL_1_0_APIS
#endif
cl_int cl_assert(cl_int const code, char const* const file, int const line, bool const abort);
