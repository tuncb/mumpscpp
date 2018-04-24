#pragma once
#include <vector>
#include <exception>
#include <mpi.h>
#include <mumpscpp/dmumps_c.h>

namespace mumpscpp {

class MumpsError : public std::exception
{
public:
	MumpsError(int info1, int info2, int infog1, int infog2) throw() 
		: info1(info1), info2(info2), infog1(infog1), infog2(infog2)
	{}

	int info1;
	int info2;
	int infog1;
	int infog2;
};

enum class MatrixType : int {
  unsymmetric = 0,
  symmetric_positive_definite = 1,
  symmetric = 2
};

enum class HostParallelism : int {
  not_involved = 0,
  involved = 1
};

enum class SchurReturnType : int {
  none = 0,
  host = 1,
  slaves_lower = 2,
  slaves_full = 3
};

enum class OutputLevel : int {
  none = 0, 
  error = 1, 
  warnings_stats = 2, 
  warnings_diag = 3, 
  all = 4
};


namespace detail {

template <typename MumpsStructureType>
void checkMumpsError(MumpsStructureType* structure)
{
	if (structure->info[0] < 0) {
		throw mumpscpp::MumpsError(structure->info[0], structure->info[1], structure->infog[0], structure->infog[1]);
	}
}
	
template <typename T>
struct MumpsStructureHelper {};

template <>
struct MumpsStructureHelper<double> 
{
	typedef  DMUMPS_STRUC_C Structure;

	void call(DMUMPS_STRUC_C* structure) 
	{
		dmumps_c(structure);
		checkMumpsError(structure);
	}
};

}

namespace traits {

template <class T> struct LhsAdaptor {};
template <class T> struct RhsAdaptor {};

}

template <typename T>
class Mumps
{
public:
	Mumps() : is_initialized(false) {}
	Mumps(MatrixType matrix_type, HostParallelism host_parallelism, int mpiCommunicator)
	{
		this->initialize(matrix_type, host_parallelism, mpiCommunicator);
	}

	~Mumps()
	{
		if (this->is_initialized) {
			this->callMumps(-2);
			this->is_initialized = false;
		}
	}

	typename detail::MumpsStructureHelper<T>::Structure mumps_structure;
	
	int& finfo (const size_t n) {return mumps_structure.info[n - 1];}
	int& finfog(const size_t n) {return mumps_structure.infog[n - 1];}
	int& ficntl(const size_t n) {return mumps_structure.icntl[n - 1];}
	T& fcntl (const size_t n) {return mumps_structure.cntl[n - 1];}

	void initialize(MatrixType matrix_type, HostParallelism host_parallelism, int mpiCommunicator)
	{
		mumps_structure.comm_fortran = mpiCommunicator;
		mumps_structure.par = (int)host_parallelism;
		mumps_structure.sym = (int)matrix_type;
		this->callMumps(-1);
		this->is_initialized = true;
	}
	void destroy()
	{
		this->callMumps(-2);
		this->is_initialized = false;
	}

  void write_problem(char* str)
  {
    strcpy(mumps_structure.write_problem, str);
  }

  void set_host_shur_complement(std::vector<int>& dofs, SchurReturnType shur_type)
  {
    auto c_comm = MPI_Comm_f2c(mumps_structure.comm_fortran);
    int rank = 0;
    MPI_Comm_rank(c_comm, &rank);
    if (rank == 0) {
      this->ficntl(19) = (int)shur_type;

      size_t n = dofs.size();
      mumps_structure.size_schur = n;
      mumps_structure.listvar_schur = dofs.data();

      mumps_structure.nprow = 1;
      mumps_structure.npcol = 1;
      mumps_structure.mblock = 100;
      mumps_structure.nblock = 100;
      mumps_structure.schur_lld = n;

      schur.resize(n*n);
      mumps_structure.schur = schur.data();
    }
  }

  std::vector<T>& get_host_schur()
  {
    return schur;
  }

	template<class Mat>
	void setDistributedInput(Mat& mat)
	{
		this->ficntl(18) = 3;
		traits::LhsAdaptor<Mat> adaptor;
		mumps_structure.n   = adaptor.getOrder(mat);
		mumps_structure.nz_loc 	= adaptor.getNumEntry(mat);
		mumps_structure.irn_loc = adaptor.getIndex1Data(mat);
		mumps_structure.jcn_loc = adaptor.getIndex2Data(mat);
		mumps_structure.a_loc 	= adaptor.getValueData(mat);
	}
	void analyzeFactorize()
	{
		this->callMumps(4);
	}
	
	template<class Mat>
	void solve(Mat& mat)
	{
		traits::RhsAdaptor<Mat> adaptor;
		mumps_structure.rhs  = adaptor.getValueData(mat);
		mumps_structure.nrhs = adaptor.getNumRhs(mat);
		mumps_structure.lrhs = adaptor.getLeadingDim(mat);
		
		this->callMumps(3);
	}

	void callMumps(const int job)
	{
		this->mumps_structure.job = job;
		this->helper.call(&mumps_structure);
	}

  void set_output_level(OutputLevel level)
  {
    this->ficntl(4) = (int)level;
  }

private:
	bool is_initialized;

	detail::MumpsStructureHelper<T> helper;
  std::vector<T> schur;
};

}