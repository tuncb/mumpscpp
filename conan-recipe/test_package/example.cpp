#include <iostream>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <mumpscpp/mumpscpp.h>
#include <Eigen/core>
#include <shiva/Environment.h>
#include <shiva/Communicator.h>
#include <mumpscpp/UblasCoordinateAdaptor.h>
#include <mumpscpp/UblasVectorAdaptor.h>
#include <mumpscpp/EigenVector.h>

shiva::environment mpi_env;

void assemble_symmetric(mumpscpp::UblasCoordinateSparseMatrix& k)
{
  // lower triangular part
  const size_t matrix_size = 3;
  k.resize(matrix_size, matrix_size, false);
  k.clear();
  k.append_element(0, 0, 2.0);
  k.append_element(1, 0, -1.0);
  k.append_element(1, 1, 2.0);
  k.append_element(2, 0, 0.0);
  k.append_element(2, 1, -1.0);
  k.append_element(2, 2, 2.0);

  k.sort();
}

int main()
{
  shiva::communicator world;
  const size_t matrix_size = 3;

  mumpscpp::UblasCoordinateSparseMatrix k;
  assemble_symmetric(k);
  mumpscpp::EigenVector f = Eigen::VectorXd::Zero(matrix_size);
  f[0] = 1.25;
  f[1] = -2.0;
  f[2] = 1.75;

  mumpscpp::Mumps<double> mumps(mumpscpp::MatrixType::symmetric, mumpscpp::HostParallelism::involved, world.fortran_mpi_communicator());
  mumps.set_output_level(mumpscpp::OutputLevel::error);
  mumps.setDistributedInput(k);

  mumps.analyzeFactorize();
  mumps.solve(f);

  std::cout << f[0] << " should be " << 0.375 << "\n";
  std::cout << f[1] << " should be " << -0.5  << "\n";
  std::cout << f[2] << " should be " << 0.625 << "\n";

  mumps.destroy();
}