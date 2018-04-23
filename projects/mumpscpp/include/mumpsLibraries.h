#pragma once


// mumps libraries
#pragma comment (lib,"dmumps_c.lib")
#pragma comment (lib,"dmumps_fortran.lib")
#pragma comment (lib,"mumps_common_c.lib")
#pragma comment (lib,"pord_c.lib")

#ifdef MUMPSCPP_LIB_MSMPI
// msmpi libraries
#pragma comment (lib,"msmpi.lib")
#pragma comment (lib,"msmpifec.lib")
#endif

#ifdef MUMPSCPP_LIB_MKL_SEQUENTIAL
// intel mkl
#pragma comment(lib,"mkl_sequential.lib")
#pragma comment (lib,"mkl_intel_lp64.lib")
#pragma comment(lib,"mkl_blacs_msmpi_lp64.lib")
#pragma comment(lib,"mkl_scalapack_lp64.lib")
#pragma comment(lib,"mkl_core.lib")
#endif

#ifdef MUMPSCPP_LIB_MKL_THREAD
// intel mkl
#pragma comment(lib,"libiomp5md.lib")
#pragma comment(lib,"mkl_intel_thread.lib")
#pragma comment (lib,"mkl_intel_lp64.lib")
#pragma comment(lib,"mkl_blacs_msmpi_lp64.lib")
#pragma comment(lib,"mkl_scalapack_lp64.lib")
#pragma comment(lib,"mkl_core.lib")
#endif

