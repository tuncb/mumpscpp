#mumpscpp
## Introduction
mumpscpp is a c++ wrapper for [MUMPS](http://mumps.enseeiht.fr/) library. 
The Current supported version of MUMPS is 5.0.1.
Note that currently, only the double version of the mumps library was provided.
In the later versions this might be fixed!

## How to use
Include the main header file.

	#include <mumpscpp/mumpscpp.h>
In order to link to MUMPS libraries automatically.	
	
	#include <mumpscpp/mumpsLibraries.h>
You can automatically link to MSMPI and/or MKL libraries as well

	#define MUMPSCPP_LIB_MSMPI
	#define MUMPSCPP_LIB_MKL_THREAD
	// or #define MUMPSCPP_LIB_MKL_SEQUENTIAL
	#include <mumpscpp/mumpsLibraries.h>

A simple analysis with mumpscpp looks like this:

	mumpscpp::Mumps<double> mumps(
	           mumpscpp::MatrixType::symmetric, 
	           mumpscpp::HostParallelism::involved, 
	           world.fortran_mpi_communicator()
	          );
	mumps.set_output_level(mumpscpp::OutputLevel::error);
	mumps.setDistributedInput(k);
	mumps.analyzeFactorize();
	mumps.solve(f);

You can write the problem to a text file using:

	mumps.write_problem("d:\\test.txt");

You can gather schur compliments:

	// only on the host computer
	mumps.set_host_shur_complement(dofs, mumpscpp::SchurReturnType::host);
    // on all computers
    mumps.analyzeFactorize();
    auto&& schur = mumps.get_host_schur();
	

